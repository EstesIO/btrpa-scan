#!/usr/bin/env python3
#
# btrpa-scan - Bluetooth Low Energy (BLE) Scanner with RPA Resolution
#
# Written by: David Kennedy (@HackingDave)
# Company:    TrustedSec
# Website:    https://www.trustedsec.com
#
# A BLE scanner that discovers broadcasting devices, searches for specific
# targets by MAC address, and resolves Resolvable Private Addresses (RPAs)
# using Identity Resolving Keys (IRKs) per the Bluetooth Core Specification.
#

"""Bluetooth LE scanner - scan all devices or search for a specific one."""

import argparse
import asyncio
import platform
import signal
import sys
import time
from typing import Dict, Optional

from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes


class BLEScanner:
    def __init__(self, target_mac: Optional[str], timeout: float,
                 irk: Optional[bytes] = None):
        self.target_mac = target_mac.upper() if target_mac else None
        self.targeted = target_mac is not None
        self.timeout = timeout
        self.seen_count = 0
        self.unique_devices: Dict[str, int] = {}
        self.running = True
        # IRK mode
        self.irk = irk
        self.irk_mode = irk is not None
        self.resolved_devices: Dict[str, int] = {}  # addr -> detection count
        self.rpa_count = 0
        self.non_rpa_warned: set = set()

    def _print_device(self, device: BLEDevice, adv: AdvertisementData, label: str,
                      resolved: Optional[bool] = None):
        rssi = adv.rssi
        tx_power = adv.tx_power
        dist = _estimate_distance(rssi, tx_power)

        print(f"\n{'='*60}")
        print(f"  {label}")
        print(f"{'='*60}")
        addr_line = f"  Address      : {device.address}"
        if resolved is True:
            addr_line += "  << IRK MATCH >>"
        elif resolved is False:
            addr_line += "  (no match)"
        print(addr_line)
        print(f"  Name         : {device.name or 'Unknown'}")
        print(f"  RSSI         : {rssi} dBm")
        print(f"  TX Power     : {tx_power if tx_power is not None else 'N/A'} dBm")
        if dist is not None:
            print(f"  Est. Distance: ~{dist:.1f} m")
        if adv.local_name and adv.local_name != device.name:
            print(f"  Local Name   : {adv.local_name}")
        if adv.manufacturer_data:
            for mfr_id, data in adv.manufacturer_data.items():
                print(f"  Manufacturer : 0x{mfr_id:04X} -> {data.hex()}")
        if adv.service_uuids:
            print(f"  Services     : {', '.join(adv.service_uuids)}")
        if adv.service_data:
            for uuid, data in adv.service_data.items():
                print(f"  Service Data : {uuid} -> {data.hex()}")
        if adv.platform_data:
            for item in adv.platform_data:
                print(f"  Platform Data: {item}")
        print(f"  Timestamp    : {time.strftime('%H:%M:%S')}")
        print(f"{'='*60}")

    def detection_callback(self, device: BLEDevice, adv: AdvertisementData):
        addr = (device.address or "").upper()

        if self.irk_mode:
            self._irk_detection(device, adv, addr)
            return

        if self.targeted:
            if self.target_mac not in addr:
                return
            self.seen_count += 1
            self._print_device(device, adv, f"TARGET FOUND  —  detection #{self.seen_count}")
        else:
            times_seen = self.unique_devices.get(addr, 0) + 1
            self.unique_devices[addr] = times_seen
            self.seen_count += 1
            self._print_device(device, adv, f"DEVICE #{len(self.unique_devices)}  —  seen {times_seen}x")

    def _irk_detection(self, device: BLEDevice, adv: AdvertisementData, addr: str):
        """Handle a detection in IRK resolution mode."""
        self.seen_count += 1

        # CoreBluetooth UUIDs look like 8-4-4-4-12 hex (no colons in MAC sense)
        # Real MACs are XX:XX:XX:XX:XX:XX (6 colon-separated octets)
        is_uuid = len(addr.replace("-", "")) == 32 and ":" not in addr
        if is_uuid:
            if addr not in self.non_rpa_warned:
                self.non_rpa_warned.add(addr)
                print(f"  [!] UUID address {addr} — cannot resolve (need real MAC)")
            return

        # Track unique devices
        times_seen = self.unique_devices.get(addr, 0) + 1
        self.unique_devices[addr] = times_seen

        # Try to resolve
        resolved = _resolve_rpa(self.irk, addr)

        if resolved:
            self.rpa_count += 1
            det_count = self.resolved_devices.get(addr, 0) + 1
            self.resolved_devices[addr] = det_count
            self._print_device(
                device, adv,
                f"IRK RESOLVED  —  match #{det_count} (addr seen {times_seen}x)",
                resolved=True,
            )
        else:
            pass  # silent — only print IRK matches

    async def scan(self):
        if self.irk_mode:
            print("Mode: IRK RESOLUTION — resolving RPAs against provided IRK")
            print(f"  IRK: {self.irk.hex()}")
            _os = platform.system()
            if _os == "Darwin":
                print("  Note: using undocumented macOS API to retrieve real BT addresses")
            elif _os == "Linux":
                print("  Note: Linux/BlueZ — may require root or CAP_NET_ADMIN")
            elif _os == "Windows":
                print("  Note: Windows/WinRT — real MAC addresses available natively")
        elif self.targeted:
            print(f"Mode: TARGETED — searching for {self.target_mac}")
        else:
            print("Mode: DISCOVER ALL — showing every broadcasting device")
        if self.timeout == float('inf'):
            print("Running continuously  |  Press Ctrl+C to stop")
        else:
            print(f"Timeout: {self.timeout}s  |  Press Ctrl+C to stop")
        print(f"{'—'*60}")

        scanner_kwargs: dict = {"detection_callback": self.detection_callback}
        if self.irk_mode and platform.system() == "Darwin":
            scanner_kwargs["cb"] = {"use_bdaddr": True}

        scanner = BleakScanner(**scanner_kwargs)
        await scanner.start()

        start = time.time()
        try:
            if self.timeout == float('inf'):
                while self.running:
                    await asyncio.sleep(0.5)
            else:
                while self.running and (time.time() - start) < self.timeout:
                    await asyncio.sleep(0.1)
        except asyncio.CancelledError:
            pass
        finally:
            await scanner.stop()

        elapsed = time.time() - start
        print(f"\n{'—'*60}")
        print(f"Scan complete — {elapsed:.1f}s elapsed")
        print(f"  Total detections : {self.seen_count}")
        if self.irk_mode:
            print(f"  Unique addresses : {len(self.unique_devices)}")
            print(f"  IRK matches      : {self.rpa_count} detections across {len(self.resolved_devices)} address(es)")
            if self.resolved_devices:
                print(f"\n  Resolved addresses:")
                print(f"  {'Address':<20} {'Detections':>11}")
                print(f"  {'—'*20} {'—'*11}")
                for addr, count in sorted(self.resolved_devices.items(),
                                          key=lambda x: x[1], reverse=True):
                    print(f"  {addr:<20} {count:>10}x")
            if not self.resolved_devices:
                print("\n  No addresses resolved — the device may not be broadcasting,")
                print("  or the IRK may be incorrect.")
        elif not self.targeted:
            print(f"  Unique devices   : {len(self.unique_devices)}")
            if self.unique_devices:
                print(f"\n  {'Address':<40} {'Seen':>6}")
                print(f"  {'—'*40} {'—'*6}")
                for addr, count in sorted(self.unique_devices.items(), key=lambda x: x[1], reverse=True):
                    print(f"  {addr:<40} {count:>5}x")

    def stop(self):
        self.running = False


def _estimate_distance(rssi: int, tx_power: Optional[int]) -> Optional[float]:
    """Estimate distance in meters using the log-distance path loss model."""
    if tx_power is None:
        return None
    if rssi == 0:
        return None
    n = 2.0
    return 10 ** ((tx_power - rssi) / (10 * n))


def _bt_ah(irk: bytes, prand: bytes) -> bytes:
    """Bluetooth Core Spec ah() function (Vol 3, Part H, Section 2.2.2).

    AES-128-ECB(IRK, padding || prand) → return last 3 bytes.
    """
    plaintext = b'\x00' * 13 + prand  # 16 bytes: 13 zero-pad + 3-byte prand
    cipher = Cipher(algorithms.AES(irk), modes.ECB())
    enc = cipher.encryptor()
    ct = enc.update(plaintext) + enc.finalize()
    return ct[-3:]  # last 3 bytes = hash


def _is_rpa(addr_bytes: bytes) -> bool:
    """Check if a 6-byte address is a Resolvable Private Address.

    RPA has top two bits of the most-significant byte set to 01.
    """
    return len(addr_bytes) == 6 and (addr_bytes[0] >> 6) == 0b01


def _resolve_rpa(irk: bytes, address: str) -> bool:
    """Resolve a MAC address string against an IRK.

    MAC format: AA:BB:CC:DD:EE:FF
    prand = first 3 octets (AA:BB:CC), hash = last 3 octets (DD:EE:FF).
    Returns True if ah(IRK, prand) == hash.
    """
    parts = address.replace("-", ":").split(":")
    if len(parts) != 6:
        return False
    try:
        addr_bytes = bytes(int(b, 16) for b in parts)
    except ValueError:
        return False
    if not _is_rpa(addr_bytes):
        return False
    prand = addr_bytes[:3]
    expected_hash = addr_bytes[3:]
    return _bt_ah(irk, prand) == expected_hash


def _parse_irk(irk_string: str) -> bytes:
    """Parse an IRK from hex string (plain, colon-separated, or 0x-prefixed).

    Returns 16 bytes or raises ValueError.
    """
    s = irk_string.strip()
    if s.lower().startswith("0x"):
        s = s[2:]
    s = s.replace(":", "").replace("-", "")
    if len(s) != 32:
        raise ValueError(f"IRK must be exactly 16 bytes (32 hex chars), got {len(s)} hex chars")
    try:
        return bytes.fromhex(s)
    except ValueError:
        raise ValueError(f"IRK contains invalid hex characters: {irk_string}")


def main():
    parser = argparse.ArgumentParser(
        description="BLE Scanner — discover all devices or hunt for a specific one"
    )
    parser.add_argument(
        "mac",
        nargs="?",
        default=None,
        help="Target MAC address to search for (omit to scan all)"
    )
    parser.add_argument(
        "-a", "--all",
        action="store_true",
        help="Scan for all broadcasting devices"
    )
    parser.add_argument(
        "--irk",
        type=str,
        default=None,
        metavar="HEX",
        help="Resolve RPAs using this Identity Resolving Key (32 hex chars)"
    )
    parser.add_argument(
        "-t", "--timeout",
        type=float,
        default=None,
        help="Scan timeout in seconds (default: 30, or infinite for --irk)"
    )
    args = parser.parse_args()

    # Mutual exclusivity
    if args.irk and args.all:
        parser.error("Cannot use --irk with --all")
    if args.irk and args.mac:
        parser.error("Cannot use --irk with a specific MAC address")
    if args.mac and args.all:
        parser.error("Cannot use --all with a specific MAC address")
    if not args.mac and not args.all and not args.irk:
        parser.print_help()
        sys.exit(0)

    # Parse and validate IRK
    irk = None
    if args.irk:
        try:
            irk = _parse_irk(args.irk)
        except ValueError as e:
            parser.error(str(e))

    # Default timeout: infinite for IRK mode, 30s otherwise
    if args.timeout is not None:
        timeout = args.timeout
    elif args.irk:
        timeout = float('inf')
    else:
        timeout = 30.0

    target = args.mac if not args.all and not args.irk else None
    scanner = BLEScanner(target, timeout, irk=irk)

    def handle_signal(*_):
        print("\nStopping scan...")
        scanner.stop()

    signal.signal(signal.SIGINT, handle_signal)
    # SIGTERM is not reliably catchable on Windows, but harmless to register
    if platform.system() != "Windows":
        signal.signal(signal.SIGTERM, handle_signal)

    try:
        asyncio.run(scanner.scan())
    except KeyboardInterrupt:
        pass  # fallback if signal handler didn't fire (Windows edge case)


if __name__ == "__main__":
    main()
