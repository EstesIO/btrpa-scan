# Homebase Receiver

Command center application for monitoring btrpa-scan-lora mesh network during search operations.

## Quick Start

```bash
# Make executable
chmod +x homebase_receiver.py

# Run with auto-detected port
python3 homebase_receiver.py

# Or specify port explicitly
python3 homebase_receiver.py -p /dev/ttyUSB0  # Linux/Mac
python3 homebase_receiver.py -p COM3           # Windows
```

## Requirements

```bash
pip3 install pyserial
```

## Features

- **Real-time Monitoring**: Displays all TRUE HIT and POSSIBLE HIT detections from mesh network
- **GPS Mapping**: Shows coordinates with Google Maps links for immediate navigation
- **Session Logging**: All detections saved to timestamped CSV files
- **Node Status**: Tracks which field nodes are active and when last seen
- **Auto-Discovery**: Automatically finds Heltec device if connected

## Setup

### 1. Hardware Setup

- Connect one Heltec device to your laptop/command center via USB
- This device acts as the homebase receiver
- Flash it with the same firmware as field nodes
- It will receive LoRa messages from all field nodes

### 2. Run Homebase Software

```bash
cd homebase
python3 homebase_receiver.py
```

### 3. Deploy Field Nodes

- Power on field nodes (battery, USB power bank, or vehicle power)
- Each node will automatically scan and transmit detections via LoRa
- Homebase will display all detections with coordinates

## Output Format

### Console Display

```
======================================================================
🚨 TRUE HIT DETECTED! 🚨
======================================================================
⏰ Time: 2026-02-16 17:30:45
📍 Node: NODE-002
📱 MAC: 28:34:ff:74:aa:99
📡 RSSI: -66 dBm
🗺️  GPS: 37.774929, -122.419418
   https://maps.google.com/?q=37.774929,-122.419418
----------------------------------------------------------------------
```

### CSV Log (logs/homebase_session_YYYYMMDD_HHMMSS.csv)

| Timestamp | Type | Source Node | Target MAC | RSSI | Latitude | Longitude | Device Type | Notes |
|-----------|------|-------------|------------|------|----------|-----------|-------------|-------|
| 2026-02-16 17:30:45 | TRUE_HIT | NODE-002 | 28:34:ff:74:aa:99 | -66 | 37.774929 | -122.419418 | | |

## Network Status

The homebase displays periodic status updates showing:
- Number of active field nodes
- Total detections received
- Last seen time for each node

## Use Case: Multi-Node SAR Operation

1. **Command Center**: Run homebase receiver on laptop at command post
2. **Field Teams**: Each team carries 1-2 Heltec nodes
3. **Coverage**: Nodes scan while teams move through search area
4. **Coordination**: All detections appear at homebase with GPS coordinates
5. **Response**: Command center dispatches nearest team to detection location

## Logs Directory

All session logs are saved to `logs/` directory:
- `homebase_session_20260216_173045.csv` - Session log
- Import into Excel/Google Sheets for analysis
- GPS coordinates can be batch imported into mapping software

## Advanced Usage

### Filter by Node

```python
# Modify homebase_receiver.py to filter specific nodes
MONITORED_NODES = ['NODE-001', 'NODE-003']  # Only show these nodes
```

### Export to KML (for Google Earth)

```bash
# Convert CSV to KML for visualization
python3 csv_to_kml.py logs/homebase_session_20260216_173045.csv
```

### Alert Notifications

Add desktop notifications when TRUE HIT detected:

```python
# macOS
os.system('say "True hit detected"')
os.system('osascript -e "display notification \\"Check homebase\\" with title \\"TRUE HIT\\""')

# Linux
os.system('notify-send "TRUE HIT" "Check homebase for details"')
```

## Troubleshooting

### No Port Found

```bash
# List available serial ports
python3 -c "import serial.tools.list_ports; print([p.device for p in serial.tools.list_ports.comports()])"

# Specify port manually
python3 homebase_receiver.py -p /dev/cu.usbserial-0001
```

### Permission Denied (Linux)

```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER
# Log out and log back in
```

### No LoRa Messages Received

1. Check both homebase and field nodes are on same LoRa frequency (915 MHz US)
2. Verify field nodes are powered and showing "LoRa: Mesh ready" on boot
3. Check range - LoRa has ~2km range in urban, 10+ km rural
4. Ensure antennas are properly connected

## Future Enhancements

- Web dashboard with live map
- SMS/email alerts for TRUE HITs
- Integration with existing SAR dispatch systems
- Historical heatmap of search coverage
- Mobile app for field commanders
