# btrpa-scan-lora Firmware

Flashable firmware for Heltec LoRa devices that provides distributed BLE detection with mesh networking for search and rescue operations.

## Quick Start

### 1. Install PlatformIO

**VS Code (Recommended):**
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install PlatformIO IDE extension
3. Restart VS Code

**Command Line:**
```bash
pip install platformio
```

### 2. Configure Your Node

Edit `include/config.h`:

```cpp
// Set unique node ID
#define NODE_ID_CONFIG "NODE-001"  // Change for each device

// Add your target MAC addresses
const char* TARGET_MACS[] = {
    "70:b3:d5:b3:4:1a:2b",  // Known pacemaker MAC
    "a4:b2:c3:d4:e5:f6",     // Known Apple Watch MAC
};
```

### 3. Build and Flash

**VS Code:**
1. Open `firmware/btrpa-scan-lora` folder
2. Click PlatformIO icon in sidebar
3. Select your board (heltec_wifi_lora_32_v3 or v2)
4. Click "Upload"

**Command Line:**
```bash
cd firmware/btrpa-scan-lora

# For Heltec V3
pio run -e heltec_wifi_lora_32_v3 --target upload

# For Heltec V2
pio run -e heltec_wifi_lora_32_v2 --target upload
```

### 4. Monitor Serial Output

```bash
pio device monitor
```

## Features (Current Build)

✅ **Multi-Target BLE Detection**
- Scan for specific target MAC addresses (TRUE HIT)
- Scan for medical device prefixes (POSSIBLE HIT)
- Active BLE scanning for extended range

✅ **GPS Integration**
- Automatic GPS detection
- Position tagging on all detections
- Movement-based position beaconing (>25m threshold)

✅ **Audible Alerts**
- TRUE HIT: 3 rapid beeps (2000 Hz)
- POSSIBLE HIT: 2 slower beeps (1500 Hz)

✅ **Detection Logging**
- MAC address, RSSI, GPS coordinates
- Timestamp, priority level
- Statistics tracking

🚧 **Coming Next**
- LoRa mesh communication (Meshtastic integration)
- Homebase receiver script
- Web flasher interface

## Hardware Support

- ✅ Heltec WiFi LoRa 32 V3
- ✅ Heltec WiFi LoRa 32 V2
- 🚧 RAK WisBlock (future)
- 🚧 LilyGO T-Beam (future)

## Configuration

### Target MAC Addresses (TRUE HIT)

Add exact MAC addresses to `config.h`:
```cpp
const char* TARGET_MACS[] = {
    "70:b3:d5:b3:4:1a:2b",  // Exact match
};
```

### Medical Device Prefixes (POSSIBLE HIT)

Pre-loaded database in `config.h`:
```cpp
const MedicalDevicePrefixConfig MEDICAL_DEVICE_PREFIXES[] = {
    {"70:b3:d5:b3:4", "Pacemaker/ICD", "Medtronic"},
    // Add more...
};
```

### BLE Scanning Parameters

Adjust in `config.h`:
```cpp
#define BLE_SCAN_INTERVAL 500   // ms between scans
#define BLE_SCAN_WINDOW 450     // ms scanning duration
#define BLE_ACTIVE_SCAN true    // Active scanning for range
```

### GPS Movement Threshold

```cpp
#define GPS_MOVEMENT_THRESHOLD 25.0  // meters
```

## Serial Output Examples

### TRUE HIT
```
🚨 ========== TRUE HIT ==========
Node: NODE-001
Target MAC: 70:b3:d5:b3:4:1a:2b
RSSI: -65 dBm
GPS: 34.052200, -118.243700
Time: 123456 ms
================================
```

### POSSIBLE HIT
```
⚠️  ======== POSSIBLE HIT ========
Node: NODE-001
MAC: 70:b3:d5:b3:4:XX:XX
Device: Pacemaker/ICD (Medtronic)
RSSI: -72 dBm
GPS: 34.052000, -118.243500
Time: 234567 ms
================================
```

### Statistics (every 30 seconds)
```
--- Statistics ---
Uptime: 1800 seconds
Total scans: 45234
TRUE HITs: 0
POSSIBLE HITs: 3
GPS: 34.052200, -118.243700
------------------
```

## Troubleshooting

### GPS Not Detected
- Check GPS module connection (RX/TX pins)
- Verify GPS module power
- Node will continue without GPS (coordinates shown as N/A)

### BLE Scanning Issues
- Ensure sufficient power supply (USB or external battery)
- Check antenna connection
- Monitor serial output for errors

### Build Errors
```bash
# Clean and rebuild
pio run --target clean
pio run
```

## Next Steps

1. **Configure target MACs** in `config.h`
2. **Flash to your Heltec device**
3. **Test detection** with known BLE device
4. **Monitor serial output** to verify operation
5. **Deploy in field** once validated

## Development Status

This is **MVP firmware** - focused on core detection functionality. LoRa mesh communication coming next.

See `../../_bmad-output/planning-artifacts/product-brief-btrpa-scan-main-2026-02-16.md` for full project roadmap.

## License

Apache 2.0 - Same as btrpa-scan

## Based On

[btrpa-scan](https://github.com/hackingdave/btrpa-scan) by David Kennedy (@HackingDave)
