---
stepsCompleted: [1, 2, 3, 4, 5]
inputDocuments:
  - /Users/msi/GitHub-Local/btrpa-scan-main/README.md
  - /Users/msi/GitHub-Local/btrpa-scan-main/lorawan-ble-search-rescue-prd.md
date: 2026-02-16
author: grayson
---

# Product Brief: btrpa-scan-main

<!-- Content will be appended sequentially through collaborative workflow steps -->

## Executive Summary

btrpa-scan-lora extends David Kennedy's proven BLE detection technology onto distributed LoRa mesh networks, enabling wide-area Bluetooth device detection for time-critical search operations. When someone with a known Bluetooth device (pacemaker, phone, medical device) goes missing, search teams need simultaneous coverage across square kilometers—not sequential scanning with single devices. This firmware turns commodity Heltec LoRa hardware ($18/unit) into auto-meshing detection nodes that alert a central coordinator the moment a target device is detected anywhere in the network. The solution prioritizes rapid deployment over perfect features: web-based flashing, simple configuration, and reliable detection using battle-tested algorithms.

**Target Impact:** Enable any SAR team or volunteer group to deploy distributed BLE detection networks in hours, not weeks, using readily available hardware.

---

## Core Vision

### Problem Statement

Time-critical searches for missing persons with Bluetooth devices (medical implants, phones, wearables) face a fundamental coverage limitation: single-point BLE scanners require physically traversing every area sequentially. When a person with a pacemaker goes missing, searchers can detect their device's Bluetooth advertisements within 10-100 meters—but only if a scanner is physically present in that location at that moment. This creates an impossible trade-off in large search areas: either deploy many expensive specialized systems, or accept that vast areas go unmonitored while searchers slowly move through the grid.

### Problem Impact

**Operational Reality:**
- Search areas span square kilometers; BLE detection ranges span meters
- Every minute without coverage = areas where victim could be but won't be detected
- SAR teams lack affordable, deployable technology to monitor wide areas simultaneously
- Existing solutions require significant technical expertise or expensive equipment

**Human Cost:**
In active searches, time is measured in survival probability. A missing person with a cardiac pacemaker represents both a detectable signal and a medical vulnerability. Current detection methods force searchers to choose between speed (covering ground quickly but missing signals) and thoroughness (detecting signals but covering minimal area).

### Why Existing Solutions Fall Short

| Approach | Limitation |
|----------|------------|
| **Single btrpa-scan deployments** | Sequential coverage only; requires carrying laptops through search area |
| **Mobile phone apps** | Solve different use cases; require smartphones; limited range |
| **Commercial SAR systems** | Expensive ($1000s per unit); require specialized training and procurement |
| **DIY cobbling together** | No standard firmware; requires embedded systems expertise; weeks to deploy |

**The Gap:** Cheap LoRa hardware exists. BLE scanning technology exists. Mesh networking exists. But no one has packaged these into flashable firmware optimized for search operations that a non-technical person can deploy in minutes.

### Proposed Solution

**btrpa-scan-lora: Flashable firmware that transforms commodity LoRa hardware into distributed BLE detection mesh networks.**

**Core Capabilities:**
- **Web-based flashing:** No command line, no dependencies—flash firmware through browser
- **Target configuration:** Input target MAC address via simple interface (web UI or serial)
- **Auto-mesh networking:** Nodes discover each other and form mesh automatically using Meshtastic
- **Priority alerting:** Detection of target MAC triggers immediate high-priority LoRa message to all nodes
- **GPS tracking:** Every detection tagged with precise coordinates
- **Homebase correlation:** Central receiver (laptop + LoRa node) displays detections in real-time

**Hardware Support (Phase 1):**
- Heltec WiFi LoRa 32 V3 and V4 (user has both)
- Future: RAK, LilyGO, and other ESP32+LoRa platforms

**Deployment Model:**
1. Flash Heltec devices with web flasher (5 minutes per device)
2. Configure target MAC address
3. Distribute to searchers (mobile nodes) or place in fixed locations
4. Turn on—nodes auto-mesh and begin detection
5. Homebase receives priority alerts when target detected

### Key Differentiators

**Built on Proven Technology:**
Not starting from zero—btrpa-scan's detection algorithms are field-tested by TrustedSec. The firmware ports these proven methods to embedded systems.

**Optimized for Speed, Not Perfection:**
KISS principle—get working firmware that handles the 80% use case (target MAC detection + mesh alert) before adding sophisticated features.

**Commodity Hardware Focus:**
Works on $18 Heltec devices anyone can order online. No specialized procurement, no vendor lock-in.

**Non-Technical Friendly:**
SAR volunteers shouldn't need embedded systems expertise. Web flasher + simple config = operational in minutes.

**Open Source from Day One:**
Community can contribute hardware support, improve detection algorithms, and adapt for their specific needs. Built on btrpa-scan's open foundation.

---

## Target Users

### Primary Users: Field Personnel (SAR/Law Enforcement)

**Who they are:**
- Search and Rescue team members
- Law enforcement officers conducting searches
- Volunteers coordinating search efforts

**What they need:**
- Plug in device, turn it on, start searching
- Audible alert (beep) when target device detected
- Zero configuration in the field
- Durable enough to carry while searching

**What they DON'T need:**
- Technical knowledge
- Screen interaction while searching
- Complex setup or troubleshooting
- Battery management concerns (external batteries available)

**Success moment:** Device beeps during search, they radio coordinates to homebase

### Secondary Users: Homebase Coordinator

**Who they are:**
- Incident commander or search coordinator
- Person with laptop receiving LoRa mesh data

**What they need:**
- Real-time alerts when any node gets a hit
- GPS coordinates of detection
- RSSI/distance estimation
- Ability to dispatch teams to detection location

**What they DON'T need:**
- Complex dashboards
- Historical analytics (not during active search)
- Multi-search management

**Success moment:** Alert comes in, they dispatch nearest team to exact coordinates

### User Journey

**Pre-Search:**
1. Homebase coordinator flashes devices via web browser (5 min per device)
2. Configure target MAC address
3. Distribute to field personnel

**During Search:**
1. Field personnel power on device, attach to belt/vest
2. Device auto-joins mesh
3. Personnel conduct normal search patterns
4. **IF HIT:** Device beeps + logs GPS + sends to mesh → homebase receives alert
5. Homebase dispatches response

**Post-Search:**
1. Devices collected
2. Local logs retrieved if needed for analysis

---

## Success Metrics

### User Success Metrics

**Field Personnel:**
- Device powers on and joins mesh automatically (100% success rate)
- Detection alert (audible beep) occurs within 5 seconds of target device in range
- Device runs for full search duration (8+ hours with external battery)
- Zero configuration required in field

**Homebase Coordinator:**
- See all active nodes on map in real-time
- Receive detection alerts within 60 seconds with GPS coordinates
- Visualize coverage trails showing where mobile nodes have searched
- Identify coverage gaps and dispatch resources accordingly
- Know when specific areas were last scanned

### Technical Performance Metrics

**Detection Performance:**
- BLE detection range: 30-100m (environment dependent)
- Detection latency: <5 seconds from proximity to local alert
- Alert delivery: <60 seconds from detection to homebase
- False negative rate: <5% (if target in range, should detect)

**Mesh Performance:**
- Position beacon trigger: Movement >25 meters from last beacon
- Mesh connectivity: 95%+ uptime during operations
- Multi-hop support: 3-5 hops maximum for reliability

**Deployment Metrics:**
- Web flash time: <10 minutes per device
- Configuration time: <2 minutes (input target MAC)
- Time to operational: <15 minutes total (unpack to field-ready)

### Business/Adoption Metrics

**POC Success (Weeks 1-4):**
- 10 devices successfully flashed and deployed in test environment
- Detection validated: Known device detected at 30+ meters
- Mesh validated: 3+ hop communication successful
- Coverage mapping functional: Movement-based trails visible on homebase map
- Real-world test: Deployed in actual search operation (current search)

**Community Adoption (Months 3-12):**
- 5+ SAR teams or law enforcement agencies using firmware
- 50+ devices deployed across real search operations
- 1+ documented successful detection contributing to rescue
- 10+ community contributors (code, hardware support, documentation)
- Hardware support expanded: 3+ different LoRa device types supported

---

## MVP Scope

### Core Features

**1. Multi-Target BLE Detection Engine:**
- Scan for multiple configured target MAC addresses (TRUE HIT)
- Scan against medical device MAC prefix database (POSSIBLE HIT)
- Different local alert tones for each priority level
- Log all detections with timestamp, RSSI, GPS coordinates, priority level

**2. LoRa Mesh Communication:**
- Auto-join Meshtastic mesh on boot
- Send priority alert when detection occurs (TRUE HIT immediate, POSSIBLE HIT batched)
- Send position beacon when movement >25m threshold
- Message format includes: Node ID, MAC, priority level, GPS, RSSI, timestamp

**3. Web-Based Flasher & Configuration:**
- Simple web interface to flash firmware to Heltec V3/V4
- Configuration interface for:
  - Multiple target MAC addresses (comma-separated or list)
  - Enable/disable medical device prefix scanning
  - Custom alert tones (optional)

**4. Homebase Receiver:**
- Python script receives LoRa messages via USB-connected Heltec device
- Console alerts with priority distinction:
  - TRUE HIT: `🚨 TRUE HIT - Node 7 - Target MAC: 70:B3:D5:B3:4:1A:2B - GPS: 34.0522,-118.2437 - RSSI: -65dBm`
  - POSSIBLE HIT: `⚠️ POSSIBLE HIT - Node 3 - Medical Device (Medtronic) - GPS: 34.0520,-118.2435 - RSSI: -72dBm`
- Basic web map showing:
  - Node positions (blue dots with trails)
  - TRUE HITs (red markers)
  - POSSIBLE HITs (yellow markers)

**5. Medical Device Database (Built-in):**
- Pre-loaded MAC prefix database for known medical device manufacturers:
  - Medtronic cardiac: `70:B3:D5:B3:4`
  - [Others from research - Dexcom, Abbott, Boston Scientific, etc.]
- Firmware updates can expand database

### Out of Scope for MVP

**Explicitly NOT in MVP (defer to v2+):**
- IRK resolution for randomized MAC addresses (too complex for POC)
- Multi-search management (multiple concurrent searches)
- Historical analytics and replay
- Advanced triangulation algorithms (RSSI from multiple nodes)
- Solar power management (use external batteries for now)
- RSSI-based distance estimation refinement (use raw RSSI for now)
- Web-based configuration (use serial/USB configuration for MVP)
- Encrypted mesh communication (use Meshtastic defaults)
- Support for non-Heltec hardware (V3/V4 only for MVP)

### MVP Success Criteria

**Technical Validation:**
- TRUE HIT detection confirmed at 30+ meters in controlled test
- POSSIBLE HIT correctly identifies medical device vendor
- Mesh message delivery <60 seconds across 3+ hops
- Movement-based position beaconing works reliably
- Web flasher successfully flashes 10+ devices without issues

**Operational Validation:**
- Deployed in real search operation (current or next search)
- Field personnel successfully use devices with <5 minutes training
- Homebase receives and acts on alerts
- Coverage trails visible on homebase map
- Zero critical failures during 8-hour deployment

**Decision Gate:**
- If both technical and operational validation pass → Proceed to v2 development
- If TRUE HIT detection works but other features need refinement → Iterate on MVP
- If core detection fails → Re-evaluate BLE scanning approach

### Future Vision

**v2 Features (Months 3-6):**
- Support for additional hardware (RAK, LilyGO, T-Beam)
- Advanced triangulation using RSSI from multiple nodes
- IRK resolution for smartphone random MAC addresses
- Improved web UI for configuration (no serial cable needed)
- Mobile app for homebase (iOS/Android)
- Offline map support
- Historical search replay and analysis

**v3 Features (Months 6-12):**
- Solar power management and extended deployment modes
- Drone attachment optimization (weight/mounting)
- Integration with existing SAR coordination systems (SAROPS, etc.)
- Multi-search management (track multiple missing persons simultaneously)
- Community-contributed medical device database
- Global medical device MAC database
- Advanced analytics (coverage heatmaps, optimal search patterns)

---
