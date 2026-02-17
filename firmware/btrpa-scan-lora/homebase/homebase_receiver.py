#!/usr/bin/env python3
"""
btrpa-scan-lora Homebase Receiver
Monitors LoRa mesh messages from field nodes and provides command center view
"""

import serial
import serial.tools.list_ports
import json
import time
import csv
from datetime import datetime
from pathlib import Path
import re

class HomebaseReceiver:
    def __init__(self, port=None, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.serial_conn = None
        self.detections_log = []
        self.node_status = {}

        # Create logs directory
        self.log_dir = Path("logs")
        self.log_dir.mkdir(exist_ok=True)

        # Create session log file
        self.session_id = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.log_file = self.log_dir / f"homebase_session_{self.session_id}.csv"
        self.init_log_file()

    def init_log_file(self):
        """Initialize CSV log file with headers"""
        with open(self.log_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([
                'Timestamp', 'Type', 'Source Node', 'Target MAC',
                'RSSI', 'Latitude', 'Longitude', 'Device Type', 'Notes'
            ])

    def find_heltec_port(self):
        """Auto-detect Heltec device serial port"""
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # Look for common Heltec USB identifiers
            if 'usbserial' in port.device.lower() or 'ttyUSB' in port.device:
                return port.device
        return None

    def connect(self):
        """Connect to Heltec homebase device"""
        if not self.port:
            self.port = self.find_heltec_port()
            if not self.port:
                print("❌ No Heltec device found. Please specify port manually.")
                return False

        try:
            self.serial_conn = serial.Serial(
                self.port,
                self.baudrate,
                timeout=1,
                exclusive=True
            )
            print(f"✅ Connected to homebase at {self.port}")
            return True
        except Exception as e:
            print(f"❌ Failed to connect: {e}")
            return False

    def parse_true_hit(self, line):
        """Parse TRUE HIT message from serial output"""
        patterns = {
            'node': r'Node:\s*(\S+)',
            'mac': r'Target MAC:\s*([0-9a-f:]+)',
            'rssi': r'RSSI:\s*(-?\d+)',
            'gps': r'GPS:\s*(-?\d+\.\d+),\s*(-?\d+\.\d+)',
        }

        data = {'type': 'TRUE_HIT'}

        for key, pattern in patterns.items():
            match = re.search(pattern, line, re.IGNORECASE)
            if match:
                if key == 'gps':
                    data['latitude'] = float(match.group(1))
                    data['longitude'] = float(match.group(2))
                elif key == 'rssi':
                    data['rssi'] = int(match.group(1))
                else:
                    data[key] = match.group(1)

        return data if 'node' in data else None

    def parse_lora_message(self, line):
        """Parse LoRa mesh message from serial output"""
        if '📩 LoRa message received:' in line:
            return {'is_lora_message': True}

        patterns = {
            'from': r'From:\s*(\S+)',
            'type': r'Type:\s*(\d+)',
            'mac': r'MAC:\s*([0-9a-f:]+)',
            'rssi': r'RSSI:\s*(-?\d+)',
            'gps': r'GPS:\s*(-?\d+\.\d+),\s*(-?\d+\.\d+)',
            'device': r'Device:\s*(.+)',
        }

        data = {}
        for key, pattern in patterns.items():
            match = re.search(pattern, line, re.IGNORECASE)
            if match:
                if key == 'gps':
                    data['latitude'] = float(match.group(1))
                    data['longitude'] = float(match.group(2))
                elif key in ['type', 'rssi']:
                    data[key] = int(match.group(1))
                elif key == 'from':
                    data['node'] = match.group(1)
                else:
                    data[key] = match.group(1).strip()

        return data if data else None

    def log_detection(self, detection):
        """Log detection to CSV file and console"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # Console output with color and formatting
        if detection.get('type') in ['TRUE_HIT', 1]:
            print("\n" + "="*70)
            print("🚨 TRUE HIT DETECTED! 🚨")
            print("="*70)
        else:
            print("\n" + "-"*70)
            print("⚠️  POSSIBLE HIT")
            print("-"*70)

        print(f"⏰ Time: {timestamp}")
        print(f"📍 Node: {detection.get('node', 'Unknown')}")
        if detection.get('mac'):
            print(f"📱 MAC: {detection['mac']}")
        if detection.get('rssi'):
            print(f"📡 RSSI: {detection['rssi']} dBm")
        if detection.get('latitude') and detection.get('longitude'):
            lat, lon = detection['latitude'], detection['longitude']
            print(f"🗺️  GPS: {lat:.6f}, {lon:.6f}")
            # Google Maps link
            print(f"   https://maps.google.com/?q={lat},{lon}")
        if detection.get('device'):
            print(f"🏥 Device: {detection['device']}")
        print("-"*70 + "\n")

        # Write to CSV
        with open(self.log_file, 'a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([
                timestamp,
                detection.get('type', 'UNKNOWN'),
                detection.get('node', ''),
                detection.get('mac', ''),
                detection.get('rssi', ''),
                detection.get('latitude', ''),
                detection.get('longitude', ''),
                detection.get('device', ''),
                detection.get('notes', '')
            ])

        # Store in memory
        detection['timestamp'] = timestamp
        self.detections_log.append(detection)

    def update_node_status(self, node_id):
        """Update last seen timestamp for a node"""
        self.node_status[node_id] = {
            'last_seen': datetime.now(),
            'status': 'online'
        }

    def print_status(self):
        """Print current status of all nodes"""
        print("\n" + "="*70)
        print("📊 NETWORK STATUS")
        print("="*70)
        print(f"Active Nodes: {len(self.node_status)}")
        print(f"Total Detections: {len(self.detections_log)}")

        if self.node_status:
            print("\nNode List:")
            for node_id, status in self.node_status.items():
                last_seen = status['last_seen'].strftime("%H:%M:%S")
                print(f"  • {node_id}: {status['status']} (last seen {last_seen})")

        print("="*70 + "\n")

    def run(self):
        """Main receiver loop"""
        if not self.connect():
            return

        print("\n" + "="*70)
        print("🏠 HOMEBASE RECEIVER ACTIVE")
        print("="*70)
        print(f"📁 Logging to: {self.log_file}")
        print(f"🔌 Port: {self.port}")
        print("⌨️  Press Ctrl+C to stop")
        print("="*70 + "\n")

        try:
            buffer = ""
            current_detection = {}
            last_status_time = time.time()

            while True:
                if self.serial_conn.in_waiting:
                    try:
                        line = self.serial_conn.readline().decode('utf-8', errors='ignore').strip()

                        if not line:
                            continue

                        # Check for TRUE HIT (direct or via LoRa)
                        if '🚨' in line or 'TRUE HIT' in line:
                            current_detection = self.parse_true_hit(line)
                            if current_detection and current_detection.get('node'):
                                self.update_node_status(current_detection['node'])

                        # Check for LoRa message
                        elif '📩 LoRa message received:' in line:
                            current_detection = {'is_lora_message': True}

                        # Parse LoRa message details
                        elif current_detection.get('is_lora_message'):
                            parsed = self.parse_lora_message(line)
                            if parsed:
                                current_detection.update(parsed)
                                if 'node' in current_detection:
                                    self.update_node_status(current_detection['node'])

                        # Check if detection is complete
                        if current_detection and \
                           'node' in current_detection and \
                           ('mac' in current_detection or 'device' in current_detection):
                            self.log_detection(current_detection)
                            current_detection = {}

                        # Print status every 60 seconds
                        if time.time() - last_status_time > 60:
                            self.print_status()
                            last_status_time = time.time()

                    except UnicodeDecodeError:
                        continue

                time.sleep(0.01)  # Small delay to prevent CPU spinning

        except KeyboardInterrupt:
            print("\n\n🛑 Stopping homebase receiver...")
            self.print_status()
            print(f"\n📁 Session log saved to: {self.log_file}")

        finally:
            if self.serial_conn:
                self.serial_conn.close()


def main():
    import argparse

    parser = argparse.ArgumentParser(
        description='btrpa-scan-lora Homebase Receiver',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 homebase_receiver.py                    # Auto-detect port
  python3 homebase_receiver.py -p /dev/ttyUSB0    # Specify port
  python3 homebase_receiver.py -p COM3             # Windows port
        """
    )

    parser.add_argument('-p', '--port', help='Serial port (auto-detect if not specified)')
    parser.add_argument('-b', '--baudrate', type=int, default=115200, help='Baudrate (default: 115200)')

    args = parser.parse_args()

    receiver = HomebaseReceiver(port=args.port, baudrate=args.baudrate)
    receiver.run()


if __name__ == '__main__':
    main()
