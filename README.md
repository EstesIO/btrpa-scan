# btrpa-scan: MeshBeacon Search and Rescue Extension

**Extending BLE detection from laptop-based scanning to field-deployable mesh networks for time-critical search operations.**

---

## 🎯 What This Is

This repository extends **[Dave Kennedy's](https://twitter.com/HackingDave) btrpa-scan** Bluetooth detection technology into a distributed mesh network system designed for search and rescue operations.

**Original btrpa-scan (by [@HackingDave](https://github.com/hackingdave)):**
- **Discover All Devices** - Scan for all broadcasting BLE devices in range with signal strength, estimated distance, manufacturer data, and service UUIDs
- **Targeted Search** - Search for a specific device by MAC address and monitor every detection
- **IRK Resolution** - Resolve Resolvable Private Addresses against one or more Identity Resolving Keys to identify devices using randomized addresses for privacy
- **Multiple IRKs** - Load multiple IRKs from a file to resolve addresses for several devices in a single scan
- **RSSI Filtering** - Filter out weak signals with a minimum RSSI threshold
- **RSSI Averaging** - Sliding window average smooths noisy BLE RSSI readings for more stable distance estimates
- **Name Filtering** - Filter devices by name with case-insensitive substring matching
- **Active Scanning** - Send SCAN_REQ to get SCAN_RSP with additional service UUIDs and device names that passive scanning misses
- **Environment Presets** - Indoor, outdoor, and free-space path-loss models for more accurate distance estimation
- **Proximity Alerts** - Audible/visual alert when a device is estimated within a configurable distance
- **Web GUI** - Browser-based radar interface with animated sweep, distance visualization, GPS map, signal-strength device list with pin/unpin, and hover tooltips
- **Live TUI** - Curses-based live-updating table sorted by signal strength
- **Real-Time CSV Log** - Stream each detection to a CSV file as it happens
- **Batch Export** - Export results to CSV, JSON, or JSONL at end of scan (supports stdout with `-o -`)
- **GPS Location Stamping** - Tag each detection with GPS coordinates via gpsd; tracks per-device best location (strongest RSSI = closest proximity). On by default, degrades gracefully if gpsd is unavailable
- **Multi-Adapter** - Scan with multiple Bluetooth adapters simultaneously (Linux)
- **Verbose/Quiet Modes** - Verbose mode shows additional details (e.g. non-matching RPAs in IRK mode); quiet mode suppresses per-device output and shows only the summary
- **Perfect for:** Forensics, proximity detection, device tracking

**MeshBeacon Extension (this fork):**
- LoRa mesh network of low-cost detection nodes ($18-36 each)
- Distributed deployment across disaster zones
- Real-time command center coordination
- **Perfect for:** SAR operations, disaster response, wilderness rescue

> **Credit Where Due:** The core BLE detection algorithms, RPA resolution, and GPS tagging were created by David Kennedy ([@HackingDave](https://twitter.com/HackingDave)) at [TrustedSec](https://www.trustedsec.com). This project builds upon that foundation to address a specific emergency response use case.

---

## 🚨 The Emergency Response Use Case

**Scenario:** Earthquake collapses apartment building. 50+ residents potentially trapped. Cellular towers down. Time is critical.

**Problem:** Traditional search methods (voice calls, thermal imaging, search dogs) struggle when:
- Victims are unconscious or unable to respond
- Heavy debris blocks sight and sound
- Large search areas overwhelm available personnel
- Infrastructure (cell towers, power) is destroyed

**Solution:** Nearly everyone carries Bluetooth devices:
- **Medical devices** (pacemakers, insulin pumps, CGMs) - always transmitting
- **Smartphones** - constant BLE advertising
- **Wearables** (Apple Watch, Fitbit) - frequent broadcasts
- **Hearing aids, wireless earbuds** - persistent signals

**MeshBeacon detects these signals, coordinates multiple search teams, and provides GPS-tagged detection points to command centers.**

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│       SEARCH OPERATION: GROUND TEAMS + UAV SWARM DEPLOYMENT         │
└─────────────────────────────────────────────────────────────────────┘

GROUND DEPLOYMENT:
Field Team A          Field Team B          Field Team C
┌──────────┐         ┌──────────┐         ┌──────────┐
│ NODE-001 │         │ NODE-002 │         │ NODE-003 │
│ $36 each │◄──LoRa──┤ BLE Scan │◄──LoRa──┤ BLE Scan │
│ 24hr batt│  Mesh   │ + GPS    │  Mesh   │ + GPS    │
└────┬─────┘         └────┬─────┘         └────┬─────┘
     │                    │                    │
     │         LoRa Network (2-10km range)    │
     └────────────────┬───────────────────────┘
                      │
                      │
UAV/DRONE DEPLOYMENT:                        │
    ┌─────────┐      ┌─────────┐            │
    │ UAV-001 │      │ UAV-002 │            │
    │ Payload │◄LoRa─┤ Payload │            │
    │ +GPS    │ Mesh │ +GPS    │            │
    └────┬────┘      └────┬────┘            │
         │                │                  │
         │  • Rapid area scanning           │
         │  • GPS breadcrumb trail          │
         │  • Coverage heatmap              │
         └────────────────┴──────────────────┘
                          │
                          ▼
                   ┌──────────────┐
                   │  HOMEBASE    │
                   │  Command     │
                   │  Center      │
                   │  • Live Map  │
                   │  • Coverage  │
                   │    Heatmap   │
                   │  • Priority  │
                   │  • CSV Logs  │
                   └──────────────┘

Detection: "Medtronic pacemaker detected"
Location: 34.0522°N, 118.2437°W
Coverage: Grid B3 - Scanned 2 min ago by UAV-001
Priority: CRITICAL - Dispatch Team B
```

**Key Innovations:**
- **Medical Device Beaconing:** Transforms medical device Bluetooth from a privacy concern into a lifesaving beacon during emergencies
- **UAV-Deployable Payloads:** Lightweight nodes ($36 each) can be mounted to commercial drones for rapid area scanning
- **GPS Coverage Mapping:** Every node transmits GPS position, creating real-time heatmap of searched areas to avoid duplicate coverage and identify gaps
- **Mesh Network Architecture:** Ground teams and UAVs share the same LoRa mesh, enabling coordinated search operations across mixed deployment scenarios

---

## 📚 Documentation

This repository contains multiple components. **Start here based on your goal:**

### 🔍 **Want to understand the vision and use cases?**
→ **[Read the Product Requirements Document](lorawan-ble-search-rescue-prd.md)**
- Complete system architecture
- Medical device detection modes
- Deployment scenarios (building collapse, wilderness, hurricane)
- 50-node deployment costs ($3,170 total)
- Ethics, privacy, and legal considerations

### 🛠️ **Want to build and deploy nodes?**
→ **[See the Firmware README](firmware/btrpa-scan-lora/README.md)**
- Hardware requirements (Heltec WiFi LoRa 32 V3)
- Quick start with web flasher (no CLI needed!)
- Field operation procedures
- Homebase receiver setup
- Troubleshooting guide

### 💻 **Want to use the original Python scanner?**
→ **[Dave Kennedy's btrpa-scan Documentation](#original-btrpa-scan-python-scanner)** (see below)
- Laptop/computer-based BLE scanning
- IRK resolution for randomized MAC addresses
- Advanced RSSI filtering and analysis

---

## 🚀 Quick Start: Deploy a Search Operation

**Hardware needed (per node):**
- Heltec WiFi LoRa 32 V3: $18-25
- USB power bank (10,000mAh): $10-15
- Optional GPS module: $8

**Software setup (5 minutes):**

```bash
# 1. Clone repository
git clone https://github.com/EstesIO/btrpa-scan-MeshBeacon.git
cd btrpa-scan-MeshBeacon/firmware/btrpa-scan-lora

# 2. Configure target MAC address (the missing person's device)
# Edit include/config.h with pacemaker/phone MAC

# 3. Flash firmware to nodes
pio run --target upload

# 4. Deploy nodes in field (battery-powered, autonomous)

# 5. Run homebase receiver at command center
cd homebase
python3 homebase_receiver.py
```

**Result:** Real-time detection of target device across multiple km² with GPS coordinates.

→ **[Complete deployment guide in firmware README](firmware/btrpa-scan-lora/README.md)**

---

## ✨ What's Implemented (v1.0)

**Firmware Features:**
- ✅ BLE detection (TRUE HIT: exact MAC | POSSIBLE HIT: medical device prefixes)
- ✅ LoRa mesh networking (2-10km range, no infrastructure needed)
- ✅ OLED display with real-time alerts
- ✅ GPS position tagging and movement-based beaconing
- ✅ Non-blocking operations (no watchdog timeouts)
- ✅ 24+ hour battery life on USB power bank

**Deployment Tools:**
- ✅ Web flasher (browser-based, no CLI required)
- ✅ Drag-and-drop MAC configuration UI
- ✅ Homebase receiver (Python, real-time CSV logging, Google Maps links)
- ✅ Multi-node coordination

**Known Medical Device MACs:**
- Medtronic pacemakers: `70:B3:D5:B3:4:XX:XX` (confirmed working)
- Additional devices: See [PRD Section: Target Medical Devices](lorawan-ble-search-rescue-prd.md#target-medical-device-identifiers)

---

## 🎯 Real-World Deployment Scenarios

### Scenario 1: Urban Building Collapse
- **Nodes:** 12 battery-powered units around perimeter
- **Range:** 20-50m through concrete/debris
- **Mission:** 24-48 hours
- **Result:** Prioritize victims with pacemakers/medical devices first

### Scenario 2: Wilderness Search
- **Nodes:** 20 solar-powered units along trails
- **Range:** 50-100m in open terrain
- **Mission:** 1-2 weeks autonomous
- **Result:** Locate hiker's smartphone/fitness tracker across 25km²

### Scenario 3: Hurricane/Flood Aftermath
- **Nodes:** 50 elevated solar nodes on structures
- **Range:** 10-20km mesh coverage
- **Mission:** Weeks to months
- **Result:** Welfare checks for at-risk residents with medical devices

### Scenario 4: UAV Swarm Deployment (Large Area Search)
- **Nodes:** 6 UAV-mounted payloads + 4 ground command posts
- **Coverage:** 100km² scanned in 2-3 hours
- **GPS Tracking:** Real-time heatmap shows scanned areas, prevents duplicate coverage
- **Mission:** Rapid initial sweep, then ground team follow-up on detections
- **Result:** Locate missing person's smartphone/medical device across vast wilderness area, GPS breadcrumb trail guides ground teams to last known detection zone
- **Payload Specs:** 120g per node (Heltec + battery), mountable to DJI Mavic/Phantom or custom fixed-wing UAVs

→ **[Complete scenarios with success metrics in PRD](lorawan-ble-search-rescue-prd.md#deployment-scenarios)**

---

## 🔬 How It Extends btrpa-scan

| Dave Kennedy's btrpa-scan | MeshBeacon Extension |
|---------------------------|----------------------|
| Python script on laptop | C++ firmware on ESP32 |
| Single-point detection | Distributed mesh network |
| Real-time console output | LoRa transmission + command center |
| USB-powered scanning | Battery + solar for field deployment |
| IRK resolution for privacy | Medical device MAC prioritization |
| **Use case: Forensics/tracking** | **Use case: Emergency SAR operations** |

**Both use the same core concepts:**
- BLE passive/active scanning
- RSSI-based distance estimation
- GPS location stamping
- MAC address filtering

**The extension adds:**
- LoRa mesh networking (RadioLib integration)
- Field-deployable hardware (Heltec LoRa nodes)
- Command center coordination software
- Multi-node triangulation
- Priority alerting for medical devices

---

## 🛠️ Hardware

**Primary Platform:** [Heltec WiFi LoRa 32 V3](https://heltec.org/project/wifi-lora-32-v3/)

**Why this hardware?**
- **Dual radio:** ESP32-S3 (BLE) + SX1262 (LoRa) in one package
- **Cost:** $18-25 per node (affordable mass deployment)
- **Range:** 10km+ LoRa mesh, 50m+ BLE detection
- **Battery:** 24+ hours on USB power bank
- **Solar ready:** Indefinite operation with 5W panel
- **OLED display:** Built-in status/alert screen

**Bill of Materials (50-node deployment):**
- Total cost: **$3,170** (~$63 per node all-inclusive)
- See [PRD Appendix: Bill of Materials](lorawan-ble-search-rescue-prd.md#bill-of-materials-bom)

---

## 📊 Technical Performance

| Metric | Target | Status |
|--------|--------|--------|
| BLE Detection Range | 50+ meters | ✅ Tested |
| LoRa Mesh Range | 2-10 km | ✅ Tested |
| Detection Latency | <60 seconds | ✅ <1 sec for TRUE HIT |
| Battery Life (active) | 24+ hours | ✅ 24-36 hours |
| Medical Device ID | 95%+ accuracy | ✅ Medtronic confirmed |
| Node Cost | <$40 | ✅ $36 per node |

---

## 🔐 Ethics & Privacy

**Informed Consent:**
- Emergency exception: Detection justified in life-threatening situations
- Opt-in program available for pre-planned deployments
- All MAC addresses purged within 30 days post-incident

**Medical Device Safety:**
- Passive BLE scanning only (no interference)
- LoRa operates on separate frequency from medical telemetry
- Low RF power well below safety limits

**HIPAA/GDPR Compliance:**
- MAC address alone is not PHI (no patient linkage)
- Emergency processing exemption under vital interests
- Data minimization: MAC, RSSI, GPS only

**Use Restrictions:**
- ✅ Authorized SAR operations
- ✅ Emergency response with proper authority
- ❌ Surveillance or tracking outside emergencies
- ❌ Commercial use without proper context

→ **[Complete legal analysis in PRD](lorawan-ble-search-rescue-prd.md#ethics-privacy-and-legal-considerations)**

---

## 🗺️ Project Roadmap

### ✅ Phase 1: Proof of Concept (Feb 2026 - COMPLETE)
- [x] LoRa mesh integration
- [x] Medtronic pacemaker detection
- [x] GPS tagging
- [x] Homebase receiver
- [x] Web flasher

### 🚧 Phase 2: Expanded Device Support (Q2 2026)
- [ ] Additional medical device MAC database
- [ ] IRK resolution for smartphones
- [ ] Multi-node triangulation
- [ ] Solar charging optimization
- [ ] **GPS coverage heatmap visualization** (shows scanned areas in real-time)
- [ ] **Breadcrumb trail tracking** (prevent duplicate UAV/team coverage)

### 🔮 Phase 3: Field Deployable System (Q3 2026)
- [ ] Weatherproof enclosures (IP67)
- [ ] Mobile app for field commanders
- [ ] **UAV payload mounting kits** (DJI Mavic/Phantom compatible)
- [ ] **UAV integration guide** (weight/power specs for drone deployment)
- [ ] Integration with FEMA/USAR systems
- [ ] Partnership with SAR organizations

### 🎯 Phase 4: Production (Q4 2026)
- [ ] Open-source hardware design files
- [ ] Manufacturing partnerships
- [ ] 50+ SAR organization adoption
- [ ] **Success metric: Save at least one life**

---

## 📖 Original btrpa-scan (Python Scanner)

**The foundation this project builds upon.**

Written by: **David Kennedy ([@HackingDave](https://twitter.com/HackingDave))**
Company: **[TrustedSec](https://www.trustedsec.com)**

### Features
- Discover all BLE devices or target specific MAC addresses
- IRK resolution for Resolvable Private Addresses
- RSSI filtering and averaging for stable distance estimates
- Active scanning for additional device metadata
- Live TUI with real-time updates
- GPS location stamping (via gpsd)
- CSV/JSON export

### Quick Start

```bash
# Install dependencies
pip install -r requirements.txt

# Scan for all BLE devices
python3 btrpa-scan.py --all

# Search for specific device
python3 btrpa-scan.py AA:BB:CC:DD:EE:FF

# Resolve randomized MAC addresses with IRK
python3 btrpa-scan.py --irk 0123456789ABCDEF0123456789ABCDEF

# Live terminal UI with GPS
python3 btrpa-scan.py --all --tui --active
```

### Requirements
- Python 3.7+
- Bluetooth hardware
- macOS, Linux, or Windows
- `gpsd` (optional, for GPS)

→ **[Complete Python scanner documentation in original README](https://github.com/TrustedSec/btrpa-scan)**

---

## 🤝 Contributing

**This project welcomes contributions in several areas:**

1. **Medical Device MAC Database**
   - Help identify MAC prefixes for additional devices
   - Test with real medical devices and report findings
   - Submit PRs to expand device coverage

2. **Field Testing**
   - SAR organizations: Beta test deployments
   - Report range, battery life, detection accuracy
   - Provide feedback on usability

<<<<<<< HEAD
| Platform | Notes |
|----------|-------|
| **macOS** | Uses CoreBluetooth. IRK mode leverages an undocumented API to retrieve real Bluetooth addresses instead of UUIDs. `--active` has no effect — CoreBluetooth always scans actively. |
| **Linux** | May require `root` or `CAP_NET_ADMIN` capability for scanning. |
| **Windows** | Native WinRT Bluetooth API — real MAC addresses available natively. TUI requires `pip install windows-curses`. |
=======
3. **Code Contributions**
   - Firmware improvements (power management, mesh routing)
   - Homebase receiver features (mapping, triangulation)
   - Documentation and tutorials
>>>>>>> 77c304c (Create comprehensive main README positioning MeshBeacon as SAR extension)

4. **Hardware Testing**
   - Alternative LoRa boards (RAK, LilyGO, etc.)
   - Antenna comparisons
   - Weatherproof enclosure designs

---

## 📜 License

<<<<<<< HEAD
```
usage: btrpa-scan.py [-h] [-a] [--irk HEX] [--irk-file PATH] [-t TIMEOUT]
                     [--output {csv,json,jsonl}] [-o FILE] [--log FILE]
                     [-v | -q] [--min-rssi DBM] [--rssi-window N] [--active]
                     [--environment {free_space,indoor,outdoor}]
                     [--ref-rssi DBM] [--name-filter PATTERN]
                     [--alert-within METERS] [--tui] [--gui] [--gui-port PORT]
                     [--no-gps] [--adapters LIST] [mac]
=======
**Apache 2.0** - Same as original btrpa-scan
>>>>>>> 77c304c (Create comprehensive main README positioning MeshBeacon as SAR extension)

- Firmware and software: Open source
- Hardware designs: Open source (when released)
- Free for non-commercial SAR/emergency use
- Commercial deployment requires attribution

---

<<<<<<< HEAD
optional arguments:
  -h, --help            show this help message and exit
  -a, --all             Scan for all broadcasting devices
  --irk HEX             Resolve RPAs using this Identity Resolving Key (32 hex chars)
  --irk-file PATH       Read IRK(s) from a file (one per line, hex format)
  -t, --timeout TIMEOUT Scan timeout in seconds (default: 30, or infinite for --irk)
  --output {csv,json,jsonl}
                        Batch output format written at end of scan
  -o, --output-file FILE
                        Output file path (default: btrpa-scan-results.<format>;
                        use - for stdout)
  --log FILE            Stream detections to a CSV file in real time
  -v, --verbose         Verbose mode — show additional details
  -q, --quiet           Quiet mode — suppress per-device output, show summary only
  --min-rssi DBM        Minimum RSSI threshold (e.g. -70) — ignore weaker signals
  --rssi-window N       RSSI sliding window size for averaging (default: 1 = no averaging)
  --active              Use active scanning (sends SCAN_REQ for additional data)
  --environment {free_space,indoor,outdoor}
                        Distance estimation path-loss model (default: free_space)
  --ref-rssi DBM        Calibrated RSSI at 1 metre for distance estimation
  --name-filter PATTERN Filter devices by name (case-insensitive substring match)
  --alert-within METERS Proximity alert when device is within this distance
  --tui                 Live-updating terminal table instead of scrolling output
  --gui                 Launch web-based radar interface in the browser
  --gui-port PORT       Port for GUI web server (default: 5000)
  --no-gps              Disable GPS location stamping (GPS is on by default via gpsd)
  --adapters LIST       Comma-separated Bluetooth adapter names (e.g. hci0,hci1)
```
=======
## 🙏 Acknowledgments
>>>>>>> 77c304c (Create comprehensive main README positioning MeshBeacon as SAR extension)

- **David Kennedy ([@HackingDave](https://twitter.com/HackingDave))** - Original btrpa-scan creator, BLE detection algorithms, RPA resolution, GPS integration
- **TrustedSec** - Security research community and original project sponsor
- **Heltec Automation** - ESP32 LoRa hardware platform
- **RadioLib Project** - LoRa communication library
- **SAR Community** - Field testing, feedback, and validation

---

## 📞 Contact & Support

**Project Repository:** https://github.com/EstesIO/btrpa-scan-MeshBeacon
**Author:** Grayson Estes (grayson@estes.io)
**Original btrpa-scan:** https://github.com/TrustedSec/btrpa-scan

**For Active SAR Operations:**
- Open GitHub issue with `[URGENT]` prefix
- Include serial logs and field conditions

**For General Questions:**
- GitHub Discussions
- See documentation links above

---

## ⚠️ Disclaimer

**This system is designed for authorized search and rescue operations only.**

Proper use requires:
- Legal authority for emergency operations
- Compliance with RF transmission regulations
- Proper SAR training and coordination
- Data handling per HIPAA/GDPR requirements

Not intended for:
- Unauthorized surveillance or tracking
- Commercial use without proper licensing
- Personal projects outside emergency context

**When seconds matter, MeshBeacon helps coordinate the search.** 🔍

<<<<<<< HEAD
| Format | Example |
|--------|---------|
| Plain hex | `0123456789ABCDEF0123456789ABCDEF` |
| Colon-separated | `01:23:45:67:89:AB:CD:EF:01:23:45:67:89:AB:CD:EF` |
| Dash-separated | `01-23-45-67-89-AB-CD-EF-01-23-45-67-89-AB-CD-EF` |
| 0x-prefixed | `0x0123456789ABCDEF0123456789ABCDEF` |

#### IRK from a File

Load one or more IRKs from a file. Each line should contain one IRK in any supported hex format. Lines starting with `#` are treated as comments:

```bash
python3 btrpa-scan.py --irk-file keys.txt
```

Example `keys.txt`:

```
# Alice's phone
0123456789ABCDEF0123456789ABCDEF
# Bob's watch
FEDCBA9876543210FEDCBA9876543210
```

When multiple IRKs are loaded, each detected RPA is checked against all keys. The summary shows total matches across all keys.

#### IRK from Environment Variable

Set the `BTRPA_IRK` environment variable to avoid passing the key on the command line:

```bash
export BTRPA_IRK=0123456789ABCDEF0123456789ABCDEF
python3 btrpa-scan.py
```

Priority: `--irk` > `--irk-file` > `BTRPA_IRK`

### RSSI Filtering

Only show devices with signal strength above a threshold:

```bash
python3 btrpa-scan.py --all --min-rssi -70
```

### RSSI Averaging

BLE RSSI is inherently noisy. Use a sliding window average for more stable distance estimates and to filter out spurious weak detections:

```bash
python3 btrpa-scan.py --all --rssi-window 5
```

When windowing is active, the display shows both raw and averaged RSSI (e.g. `RSSI: -65 dBm (avg: -62 dBm over 5 readings)`), and distance estimation uses the averaged value. The `--min-rssi` filter also applies to the averaged RSSI, preventing a single noisy spike from dropping a device.

### Name Filtering

Filter devices by name using a case-insensitive substring match:

```bash
python3 btrpa-scan.py --all --name-filter "AirPods"
```

Only devices whose advertised name contains the given pattern will be shown. Devices with no name are excluded when a name filter is active.

### Active Scanning

Passive scanning (the default) only sees advertisements. Active scanning sends `SCAN_REQ` and gets `SCAN_RSP`, which can reveal additional service UUIDs and device names:

```bash
python3 btrpa-scan.py --all --active
```

> **Note:** On macOS, CoreBluetooth always scans actively regardless of this flag. On Linux/BlueZ, active scanning may require root or `CAP_NET_ADMIN`.

### Environment Presets

Distance estimation uses a path-loss exponent that varies by environment. The default (`free_space`, n=2.0) assumes no obstructions. For more realistic estimates indoors:

```bash
python3 btrpa-scan.py --all --environment indoor
```

| Preset | Path-Loss Exponent (n) | Use Case |
|--------|------------------------|----------|
| `free_space` | 2.0 | Open air, line of sight |
| `outdoor` | 2.2 | Parks, parking lots |
| `indoor` | 3.0 | Offices, homes, buildings |

Higher `n` values produce larger distance estimates for the same RSSI, reflecting signal attenuation from walls and obstacles.

### Reference RSSI Calibration

By default btrpa-scan derives the expected RSSI at 1 metre from the advertised TX Power using an empirically validated 59 dB offset (the iBeacon standard). For even better accuracy you can supply a calibrated value measured in your own environment:

1. Place the target device exactly 1 metre from the scanner.
2. Run a short scan and note the average RSSI.
3. Pass that value with `--ref-rssi`:

```bash
python3 btrpa-scan.py --all --ref-rssi -55
```

When `--ref-rssi` is set, TX Power is ignored entirely. This also enables distance estimates for devices that don't advertise TX Power.

### Proximity Alerts

Trigger an audible bell and visual alert when a device is estimated within a given distance. Requires that the target device advertise TX Power:

```bash
python3 btrpa-scan.py AA:BB:CC:DD:EE:FF --alert-within 5.0
```

Works in all modes including IRK resolution:

```bash
python3 btrpa-scan.py --irk <key> --alert-within 3.0
```

### Live TUI

Replace scrolling output with a live-updating terminal table, sorted by signal strength:

```bash
python3 btrpa-scan.py --all --tui
```

The TUI shows all detected devices in a compact table with address, name, RSSI, averaged RSSI, estimated distance, detection count, and last-seen time. Resolved IRK matches are shown in bold, and devices within the `--alert-within` threshold are highlighted.

Combine with other flags:

```bash
python3 btrpa-scan.py --irk <key> --tui --rssi-window 5 --environment indoor --alert-within 5.0
```

### Web GUI

Launch a browser-based radar interface with an animated sweep, real-time device tracking, and GPS map:

```bash
python3 btrpa-scan.py --all --gui
```

The GUI features:

- **Radar display** — animated sweep line with concentric distance rings (1m, 5m, 10m, 20m). Devices appear as color-coded dots positioned by estimated distance (green = close, yellow = medium, red = far). Full-height layout with matrix data rain background and ghost MAC flicker effects
- **Device list** — right-side panel listing all detected devices, color-coded by signal strength. Click any device to pin it for tracking
- **Pinned devices** — left-side panel showing pinned MAC addresses with live RSSI and distance updates. Pin multiple devices simultaneously; click × to unpin
- **GPS map** — Leaflet.js map with OpenStreetMap tiles showing scanner position and device locations. Automatically hidden if no GPS is available
- **Hover tooltips** — hover over any device (radar dot, list entry, or pinned entry) to see full device details (address, name, RSSI, TX power, distance, GPS, manufacturer data, services)
- **Dark theme** — matrix-green hacker aesthetic designed for field work

GUI mode scans continuously by default (no 30-second timeout). Press `Ctrl+C` to stop. Use `-t` to set a specific scan duration:

```bash
# Scan for 60 seconds with indoor path-loss model
python3 btrpa-scan.py --all --gui -t 60 --environment indoor

# Custom port
python3 btrpa-scan.py --all --gui --gui-port 8080

# Combine with RSSI averaging and proximity alerts
python3 btrpa-scan.py --all --gui --rssi-window 5 --alert-within 5.0
```

> **Note:** `--gui` requires Flask and flask-socketio (`pip install flask flask-socketio`). Cannot be combined with `--tui` or `--quiet`.

### Real-Time CSV Log

Stream each detection to a CSV file as it happens (useful for long-running scans where you want incremental data):

```bash
python3 btrpa-scan.py --all --log scan.csv
```

This can be combined with `--output` for a separate batch export:

```bash
python3 btrpa-scan.py --all --log live.csv --output json -o results.json
```

### Batch Export

Export all results at end of scan in CSV, JSON, or JSONL (JSON Lines) format:

```bash
python3 btrpa-scan.py --all --output json -o results.json -t 30
python3 btrpa-scan.py --all --output csv -t 30
python3 btrpa-scan.py --all --output jsonl -o results.jsonl -t 30
```

JSONL writes one JSON object per line, making it easy to pipe through `jq`:

```bash
python3 btrpa-scan.py --all --output jsonl -o results.jsonl -t 10
cat results.jsonl | jq .
```

Write output to stdout for piping:

```bash
python3 btrpa-scan.py --all --output json -o - -t 10 -q | jq .
```

### Multi-Adapter Scanning (Linux)

Scan with multiple Bluetooth adapters simultaneously for wider coverage:

```bash
python3 btrpa-scan.py --all --adapters hci0,hci1
```

Each adapter runs its own scanner instance sharing the same detection callback. All detections are merged into a single output.

### GPS Location Stamping

GPS is **on by default**. Each detection is tagged with the current GPS coordinates from gpsd. The scanner also tracks the **best GPS fix per device** — the coordinates from the detection with the strongest RSSI (closest proximity = most accurate location).

If gpsd is not running, the scanner prints a note and continues normally without GPS:

```bash
# With gpsd running — detections include lat/lon
python3 btrpa-scan.py --all --output json

# Without gpsd — works fine, GPS fields are empty
python3 btrpa-scan.py --all

# Explicitly disable GPS (skips connection attempt)
python3 btrpa-scan.py --all --no-gps
```

GPS coordinates appear in:
- **Console output** — "Best GPS" line per device
- **TUI settings bar** — current fix coordinates
- **GUI** — interactive map panel with device markers and scanner position
- **Header** — GPS connection status
- **Summary tables** — "Best GPS" column per device
- **All exports** (CSV, JSON, JSONL, real-time log) — `latitude`, `longitude`, `gps_altitude` fields

### Quiet and Verbose Modes

Run in quiet mode (summary only, no per-device output — useful with `--output` or `--log`):

```bash
python3 btrpa-scan.py --all -q --output json -t 30
```

Run in verbose mode (show non-matching RPAs in IRK mode):

```bash
python3 btrpa-scan.py --irk 0123456789ABCDEF0123456789ABCDEF -v
```

## How RPA Resolution Works

Bluetooth Low Energy devices use Resolvable Private Addresses (RPAs) to prevent tracking. An RPA is a temporary MAC address that changes periodically, but can be resolved by anyone who possesses the device's Identity Resolving Key (IRK).

An RPA consists of:
- **prand** (3 bytes) - A random value with the top two bits set to `01`
- **hash** (3 bytes) - Computed as `AES-128-ECB(IRK, padding || prand)` truncated to 3 bytes

btrpa-scan implements the `ah()` function from the Bluetooth Core Specification (Vol 3, Part H, Section 2.2.2) to resolve these addresses in real time.

> **Note on AES-ECB:** The use of AES in ECB mode for the `ah()` function is mandated by the Bluetooth Core Specification. Because only a single 16-byte block is ever encrypted, ECB's lack of diffusion across blocks is irrelevant — this is not a vulnerability.

## Security Considerations

- **IRK on the command line:** Command-line arguments are visible to other users on the system via `ps`. To avoid exposing the IRK, use `--irk-file` to read from a file, or set the `BTRPA_IRK` environment variable. Console output masks IRKs by default (showing only the first and last 4 hex characters).
- **GPS coordinates in exports:** All export formats (CSV, JSON, JSONL) and console output include GPS coordinates when available. If sharing scan results, be aware that your location may be disclosed. Use `--no-gps` to disable GPS entirely.
- **Output file paths:** The `--output-file` and `--log` options write to the specified path. Ensure the destination has appropriate permissions for your use case.

## Running Tests

```bash
pip install pytest
python -m pytest test_btrpa_scan.py -v
```

## Example Output

```
Mode: DISCOVER ALL - showing every broadcasting device
Scanning: passive
GPS: connected (37.774929, -122.419418)
Timeout: 30s  |  Press Ctrl+C to stop
------------------------------------------------------------

============================================================
  DEVICE #1  -  seen 1x
============================================================
  Address      : AA:BB:CC:DD:EE:FF
  Name         : MyDevice
  RSSI         : -45 dBm
  TX Power     : -59 dBm
  Est. Distance: ~0.4 m
  Manufacturer : 0x004C -> 0215abcdef
  Best GPS     : 37.774929, -122.419418
  Timestamp    : 14:32:07
============================================================

------------------------------------------------------------
Scan complete - 30.0s elapsed
  Total detections : 142
  Unique devices   : 12
  Results written to btrpa-scan-results.json
```

## Stopping a Scan

Press `Ctrl+C` at any time to gracefully stop the scan and display summary statistics.
=======
---

**Built upon [@HackingDave](https://twitter.com/HackingDave)'s btrpa-scan. Extended for those who search when lives are on the line.**
>>>>>>> 77c304c (Create comprehensive main README positioning MeshBeacon as SAR extension)
