# Product Requirements Document: MeshBeacon Search and Rescue System

**Document Version:** 1.0  
**Date:** February 16, 2026  
**Author:** Grayson Estes  
**Project Repository:** https://github.com/EstesIO/btrpa-scan  
**Classification:** Open Source

---

## Executive Summary

MeshBeacon is a low-cost, field-deployable search and rescue system that uses LoRaWAN mesh networks to detect survivors via their Bluetooth medical devices and personal electronics. By combining the btrpa-scan BLE detection technology with inexpensive Heltec LoRa nodes (~$18 each), emergency responders can rapidly deploy a wide-area detection grid capable of locating people with pacemakers, insulin pumps, fitness trackers, and smartphones in disaster zones, collapsed structures, and wilderness areas.

**Core Innovation:** Transform medical device Bluetooth identifiers from privacy concerns into lifesaving beacons during emergencies.

---

## Problem Statement

### Current Search and Rescue Challenges

In disaster scenarios—earthquakes, building collapses, avalanches, floods, wildfires—locating survivors quickly is critical but difficult:

- **Debris and Rubble:** Traditional visual and acoustic searches fail when victims are buried
- **Time Sensitivity:** Survival rates drop dramatically after 72 hours (the "golden window")
- **Large Search Areas:** Wilderness and disaster zones can span hundreds of square kilometers
- **Resource Constraints:** SAR teams are limited by personnel, equipment, and communication infrastructure
- **Infrastructure Collapse:** Cellular towers, Wi-Fi, and power grids fail in disasters

### The Bluetooth Opportunity

Nearly every person today carries or wears Bluetooth devices:
- **Medical Devices:** Pacemakers, insulin pumps, CGMs, hearing aids (always transmitting for telemetry)
- **Wearables:** Fitness trackers, smartwatches (frequent BLE advertisements)
- **Smartphones:** Constant BLE beacon scanning for devices/services
- **Wireless Earbuds:** Persistent BLE advertising when active

These devices create a unique Bluetooth "signature" that can be detected at range (10-100+ meters depending on conditions) even when victims are:
- Unconscious or unable to call for help
- Trapped under debris where voice/visual search fails
- In remote wilderness areas
- Without functioning cellular service

### Why Existing Solutions Fall Short

| Technology | Limitation |
|------------|------------|
| **Cellular Triangulation** | Requires functioning towers; dies within hours of disaster |
| **Thermal Imaging** | Expensive equipment; limited by debris/water; only detects body heat |
| **Search Dogs** | Amazing but limited by handler fatigue, weather, scent contamination |
| **Voice/Acoustic** | Requires conscious victim; fails under heavy debris |
| **Drones** | Short flight time; visual only; struggles in smoke/darkness/heavy foliage |

---

## Solution Overview

### System Architecture

MeshBeacon consists of three components:

**1. Detection Nodes (Heltec WiFi LoRa 32 V4)**
- Scan for BLE devices (medical devices, phones, wearables)
- GPS-tag detections with precise coordinates
- Transmit findings via LoRaWAN mesh to coordinator
- Solar-powered for extended deployment (optional)
- Weatherproof enclosure for field conditions

**2. LoRaWAN Mesh Network**
- Long-range radio communication (10-15km line-of-sight, 2-5km urban)
- Self-healing mesh topology (nodes relay messages automatically)
- Low power consumption (days/weeks on battery)
- Operates without cellular/internet infrastructure
- Meshtastic-compatible for community SAR networks

**3. Command Center Application**
- Real-time map of all BLE detections
- Prioritized victim list (medical devices = highest priority)
- Signal strength heatmaps for triangulation
- Integration with existing SAR coordination tools
- Export to KML/GPX for field teams

### How It Works

```
[Victim with Pacemaker]
        ↓ BLE Advertisement (70:B3:D5:B3:4:XX:X)
[Detection Node A] ← 15m away, RSSI -65dBm
        ↓ LoRa transmission
[Detection Node B] ← Mesh relay
        ↓
[Detection Node C] ← Mesh relay
        ↓
[Command Center] ← "Medical device detected at GPS 34.0522°N, 118.2437°W"
        ↓
[SAR Team Dispatch] → Prioritize rescue at that location
```

**Key Advantage:** Multiple nodes detecting the same device enable triangulation for sub-10-meter accuracy.

---

## Hardware Specifications

### Primary Hardware: Heltec WiFi LoRa 32 V4

**Why This Hardware:**
- **Cost:** $17.90-$19.90 per unit (affordable for mass deployment)
- **Dual Radio:** ESP32-S3 Bluetooth + SX1262 LoRa in single package
- **Built-in GPS Interface:** 1.25×8P GNSS connector
- **Solar Ready:** 1.25×2P solar input (4.7-6V)
- **Long Range:** LoRa TX power up to 28dBm (~15km line-of-sight)
- **Low Power:** <20µA sleep current (weeks on battery)
- **Meshtastic Compatible:** Drop-in support for existing mesh networks

**Technical Specifications:**

| Parameter | Value |
|-----------|-------|
| **MCU** | ESP32-S3R2 (dual-core, 2MB PSRAM, 16MB Flash) |
| **LoRa Chip** | Semtech SX1262 |
| **LoRa Frequency** | 902-928MHz (US), 863-870MHz (EU) |
| **LoRa TX Power** | 28±1 dBm (high-power version) |
| **LoRa RX Sensitivity** | -137dBm @ SF12, BW=125kHz |
| **Bluetooth** | BLE 5.0, Bluetooth Mesh |
| **Wi-Fi** | 802.11 b/g/n (for configuration only) |
| **GPS** | External module via 1.25×8P (L76K recommended) |
| **Battery** | 3.3-4.4V lithium (3.7V 18650 typical) |
| **Solar Input** | 4.7-6V via 1.25×2P |
| **Operating Temp** | -20°C to +70°C |
| **Dimensions** | 51.7 × 25.4 × 10.7 mm |

### Recommended Configuration

**Standard Detection Node:**
- Heltec WiFi LoRa 32 V4: $18
- L76K GNSS module: $8
- 18650 battery (3000mAh): $3
- Weatherproof enclosure: $5
- 868/915MHz antenna: $2
- **Total: ~$36 per node**

**Solar-Powered Extended Deployment Node:**
- Add 5W 6V solar panel: $10
- Larger weatherproof enclosure: $8
- **Total: ~$54 per node**

### Alternative/Compatible Hardware

| Device | Cost | Pros | Cons |
|--------|------|------|------|
| **Heltec CubeCell** | $12-15 | Ultra low power, smaller | Less memory, no built-in display |
| **LilyGO T-Beam** | $30-40 | Integrated GPS | More expensive, larger |
| **RAK WisBlock** | $25-50 | Modular, ruggedized | Higher cost, complex assembly |

---

## Software Architecture

### Detection Node Firmware

**Base:** Modified btrpa-scan + Meshtastic

```
┌─────────────────────────────────────┐
│     MeshBeacon Node Firmware        │
├─────────────────────────────────────┤
│ BLE Scanner (btrpa-scan core)       │
│  - Passive scanning (low power)     │
│  - MAC address collection           │
│  - RSSI measurement                 │
│  - Medical device prioritization    │
├─────────────────────────────────────┤
│ GPS Tagging (via gpsd)              │
│  - Location stamping per detection  │
│  - Time synchronization             │
├─────────────────────────────────────┤
│ LoRa Mesh Networking (Meshtastic)   │
│  - Message routing                  │
│  - Automatic retries                │
│  - Collision avoidance              │
├─────────────────────────────────────┤
│ Power Management                    │
│  - Sleep scheduling                 │
│  - Solar MPPT control               │
│  - Battery monitoring               │
└─────────────────────────────────────┘
```

**Key Software Features:**

1. **Medical Device Priority Detection:**
   - Hardcoded MAC prefixes for critical devices (Medtronic `70:B3:D5:B3:4`, etc.)
   - Instant transmission of medical device detections (no batching delay)
   - Continuous re-scanning of medical device locations

2. **Adaptive Scanning:**
   - Active search mode: 100% duty cycle, continuous BLE scanning
   - Conservation mode: 10% duty cycle, periodic scanning (for long deployments)
   - Triggered by: solar power availability, battery level, or manual command

3. **Mesh Resilience:**
   - Store-and-forward for out-of-range nodes
   - Duplicate detection filtering (same MAC, same location)
   - Automatic mesh reformation if nodes fail

4. **Data Payload (LoRa Transmission):**
```json
{
  "node_id": "MESH-A3F2",
  "timestamp": "2026-02-16T14:32:07Z",
  "gps": {"lat": 34.0522, "lon": -118.2437, "alt": 123},
  "detection": {
    "mac": "70:B3:D5:B3:4:1A:2B",
    "rssi": -65,
    "type": "medical_device",
    "vendor": "Medtronic",
    "est_distance": 15.2
  }
}
```

### Command Center Application

**Platform:** Web-based (works on laptop, tablet, phone)

**Core Features:**

1. **Live Detection Map**
   - Real-time plotting of all BLE detections on map (OpenStreetMap base)
   - Color-coded markers: Red = medical devices, Yellow = phones/wearables, Gray = unknown
   - Signal strength rings around detection nodes (for triangulation visualization)
   - Heat map overlay showing high-probability survivor zones

2. **Victim Priority Queue**
   - Ranked list of detected devices:
     - Priority 1: Medical devices (pacemakers, insulin pumps)
     - Priority 2: Wearables/phones (active Bluetooth = likely alive)
     - Priority 3: Weak/intermittent signals (buried/low battery)
   - Estimated survival time based on detection type and time since disaster

3. **Triangulation Engine**
   - Automatic triangulation when 3+ nodes detect same MAC
   - Confidence ellipse showing probable location (accuracy: 5-50m depending on geometry)
   - Direction arrows for single-node detections

4. **SAR Team Coordination**
   - Assign teams to specific detections
   - Mark locations as "searched," "rescued," or "deceased"
   - Export waypoints to GPS devices (GPX format)
   - Integration with FEMA/USAR coordination systems

5. **Historical Playback**
   - Replay detection timeline to track victim movement (e.g., someone walking after earthquake)
   - Identify patterns (multiple devices moving together = group of survivors)

---

## Target Medical Device Identifiers

### Known MAC Prefixes (Organizational Unique Identifiers)

**Priority 1: Cardiac Devices**

| Vendor | MAC Prefix | Device Types | Notes |
|--------|-----------|--------------|-------|
| **Medtronic** | `70:B3:D5:B3:4` | Pacemakers, ICDs, CRT | Always-on telemetry; 4,096 address block |
| Abbott | *Research required* | Pacemakers, ICDs | |
| Boston Scientific | *Research required* | Pacemakers, ICDs | |
| Biotronik | *Research required* | Pacemakers, ICDs | |

**Priority 2: Diabetes Management**

| Vendor | MAC Prefix | Device Types | Notes |
|--------|-----------|--------------|-------|
| Dexcom | *Research required* | CGM sensors | Every 5 minutes transmission |
| Abbott FreeStyle | *Research required* | CGM sensors | NFC + BLE |
| Medtronic MiniMed | *Research required* | Insulin pumps | |
| Insulet OmniPod | *Research required* | Insulin pumps | |

**Priority 3: Other Medical Devices**

| Vendor | Device Type | Notes |
|--------|-------------|-------|
| Cochlear | Hearing implants | BLE-enabled models |
| Sonova/Phonak | Hearing aids | High BLE advertisement rate |
| Medtronic Neurostimulators | Pain management | Always-on telemetry |

**Note:** MAC prefix research must be expanded to cover major medical device manufacturers globally.

### Bluetooth Service UUIDs (for identification)

Many medical devices advertise specific BLE service UUIDs:

| Service UUID | Device Type | Standard |
|--------------|-------------|----------|
| `0x180D` | Heart Rate Monitor | Bluetooth SIG standard |
| `0x1808` | Glucose Meter | Bluetooth SIG standard |
| `0x181F` | Continuous Glucose Monitoring | Bluetooth SIG standard |
| Custom UUIDs | Manufacturer-specific | Requires reverse engineering |

**Implementation:** Node firmware should log all detected service UUIDs for later analysis and correlation with medical device types.

---

## Deployment Scenarios

### Scenario 1: Urban Building Collapse (Earthquake/Explosion)

**Context:** 
- Multi-story building collapsed
- 50-200 potential victims
- Search area: 1 city block (~100m × 100m)
- Urban RF environment (lots of Bluetooth noise)

**Deployment:**
- **12 detection nodes** placed in grid pattern around perimeter (25m spacing)
- **4 elevated nodes** on nearby structures for better line-of-sight
- **1 command center** in mobile SAR vehicle
- **Power:** Battery-only (mission duration: 24-48 hours)

**Expected Performance:**
- Detection range: 20-50m (reduced by concrete/rebar)
- Triangulation accuracy: 5-15m
- Mesh range: 1-2km (dense urban)
- Detection latency: <30 seconds from victim to command center

**Victim Triage:**
1. Medical device detections → Immediate priority (critical life support)
2. Phone/wearable detections → Secondary priority (likely conscious)
3. Weak/intermittent signals → Tertiary (buried deep or low battery)

### Scenario 2: Wilderness Search and Rescue

**Context:**
- Hiker missing in national park
- Search area: 25 square kilometers
- Mountainous terrain with heavy forest
- Victim likely has: smartphone, fitness tracker (possibly medical devices)

**Deployment:**
- **20 solar nodes** placed along trails and high points (500m spacing)
- **4 mobile nodes** carried by SAR teams
- **1 base station** at trailhead parking
- **Power:** Solar + battery (mission duration: 1-2 weeks)

**Expected Performance:**
- Detection range: 50-100m (open terrain), 20-40m (dense forest)
- Mesh range: 5-15km (line-of-sight mountaintop nodes)
- Detection latency: 1-5 minutes (depending on mesh hops)
- Coverage: ~5-10% of total area (focused on probable paths)

**Search Strategy:**
- Deploy nodes along "most likely" routes first (trails, water sources)
- Move nodes based on detection patterns
- Use mobile nodes to "sweep" between fixed nodes

### Scenario 3: Natural Disaster (Hurricane/Flood)

**Context:**
- Widespread flooding after hurricane
- Thousands of displaced persons
- Infrastructure destroyed (no power, cell towers down)
- Search area: entire county (~1000 square km)

**Deployment:**
- **50 solar nodes** deployed on elevated structures (cell towers, tall buildings, bridges)
- **10 mobile nodes** on boats/vehicles for dynamic search
- **3 command centers** at different emergency operations centers
- **Power:** Solar + large battery banks (mission duration: weeks to months)

**Expected Performance:**
- Detection range: 30-60m (variable due to water attenuation)
- Mesh range: 10-20km (elevated nodes over water)
- Coverage: ~2-5% of total area (focused on populated zones)
- System lifespan: 30+ days with solar

**Unique Challenges:**
- Water attenuates RF signals significantly
- Nodes may need waterproof deployment on floating platforms
- High Bluetooth noise from many survivors in shelters

---

## Implementation Roadmap

### Phase 1: Proof of Concept (Q1 2026) - 8 weeks

**Goal:** Validate core concept with lab/field testing

**Deliverables:**
- [ ] btrpa-scan integration with Heltec V4 hardware
- [ ] Basic LoRa mesh messaging (Meshtastic integration)
- [ ] GPS tagging of BLE detections
- [ ] Medtronic pacemaker MAC detection (70:B3:D5:B3:4)
- [ ] Simple command center web app (map + detection list)
- [ ] Lab testing: 5 nodes, controlled environment
- [ ] Field test: Simulated "victim" with pacemaker in debris pile

**Success Criteria:**
- Detect Medtronic pacemaker at 30+ meters
- Mesh message delivery <5 seconds over 3 hops
- GPS accuracy within 10 meters
- Battery life >24 hours on continuous scanning

### Phase 2: Expanded Device Support (Q2 2026) - 12 weeks

**Goal:** Broaden medical device coverage and improve detection accuracy

**Deliverables:**
- [ ] Research and catalog MAC prefixes for major medical device vendors (Abbott, Boston Scientific, Dexcom, etc.)
- [ ] BLE service UUID identification and logging
- [ ] IRK (Identity Resolving Key) support for Apple/Android random MAC addresses
- [ ] Multi-node triangulation algorithm
- [ ] Power management: active/conservation modes
- [ ] Solar charging integration and testing

**Success Criteria:**
- Detect 5+ different medical device types
- Triangulation accuracy <10 meters with 3+ nodes
- Solar-powered node operates 7+ days without external charging
- Conservation mode: 7 days battery life on single 18650

### Phase 3: Field Deployable System (Q3 2026) - 16 weeks

**Goal:** Create ruggedized, user-friendly system for SAR teams

**Deliverables:**
- [ ] Weatherproof enclosures for all node types (IP67 rated)
- [ ] Quick-deploy mounting hardware (magnets, straps, tripods)
- [ ] Command center mobile app (iOS/Android) + web dashboard
- [ ] Automatic mesh network configuration (zero-config deployment)
- [ ] Data export: GPX, KML, SAROPS integration
- [ ] Training materials and deployment SOPs
- [ ] Partnership with SAR organizations for beta testing

**Success Criteria:**
- 50-node deployment by non-technical user in <60 minutes
- System operates in rain, snow, extreme temperatures (-20°C to +50°C)
- Command center app usable on smartphone with offline maps
- Successful detection in 3+ real SAR training exercises

### Phase 4: Production & Scale (Q4 2026) - Ongoing

**Goal:** Large-scale manufacturing and adoption

**Deliverables:**
- [ ] Open-source hardware design files (schematic, PCB, BOM, enclosure CAD)
- [ ] Open-source firmware repository (GPL or Apache 2.0)
- [ ] Manufacturing partnerships for volume production
- [ ] Deployment kits: 10-node, 25-node, 50-node configurations
- [ ] Integration with FEMA, Red Cross, national SAR organizations
- [ ] Global medical device MAC database (crowdsourced)

**Success Criteria:**
- 100+ organizations using MeshBeacon
- Cost <$40 per node in volume (100+ unit orders)
- Detect 20+ different medical device types
- Save at least one life (documented rescue attributed to MeshBeacon)

---

## Use Cases & User Stories

### UC-1: First Responder - Building Collapse

**Actor:** Urban Search and Rescue Team Leader  
**Context:** Earthquake collapses apartment building; 30+ residents potentially trapped

**Scenario:**
1. Team arrives on scene 2 hours after collapse
2. Deploy 12 MeshBeacon nodes around perimeter (15 minutes)
3. Command center app shows 8 BLE detections within first 5 minutes
4. **Critical Alert:** Medtronic pacemaker detected at -72dBm, triangulated to northwest corner, 2nd floor debris
5. Team prioritizes rescue at that location
6. Victim (elderly male with pacemaker) extracted alive 4 hours after detection
7. Continue systematic search using other BLE detections as waypoints

**Value:** Reduced search time by 60% compared to random grid search; prioritized victim with critical medical device

### UC-2: Wilderness SAR - Lost Hiker

**Actor:** County Sheriff SAR Coordinator  
**Context:** Day-hiker overdue 24 hours; last known position: trailhead 15km from road

**Scenario:**
1. Deploy 20 solar nodes along trails at 500m intervals (deployed by helicopter in 2 hours)
2. 4 ground teams carry mobile nodes for active search
3. After 18 hours: weak smartphone BLE signal detected by Node 14, ~3km from trailhead
4. Ground team dispatched to Node 14 location
5. Mobile node carried by team provides stronger signal and direction
6. Hiker located 200m from Node 14, injured but alive
7. Medical evacuation via helicopter to hospital

**Value:** Located hiker in 20 hours vs. estimated 3-5 days for traditional search; coverage of 25 sq km with 20 nodes vs. 100+ searchers

### UC-3: Hurricane Aftermath - Welfare Checks

**Actor:** Emergency Management Agency  
**Context:** Hurricane destroyed neighborhood; 500+ homes damaged; welfare checks needed for at-risk residents

**Scenario:**
1. 30 solar nodes deployed on utility poles and elevated structures (survives storm)
2. Database of at-risk residents pre-loaded (elderly, medical conditions) with known device MAC addresses (opt-in program)
3. Post-storm: system detects 80% of registered medical devices within 48 hours
4. Command center generates priority list of addresses with no detections
5. Door-to-door checks focused on "no signal" addresses
6. 3 residents found in need of immediate medical evacuation

**Value:** Optimized limited rescue resources; ensured high-risk residents checked within 72 hours

### UC-4: Training Exercise - SAR Team Certification

**Actor:** SAR Training Coordinator  
**Context:** Annual certification exercise for volunteer SAR team

**Scenario:**
1. Place 5 "victims" in wilderness area with medical devices (borrowed pacemaker simulator, insulin pump)
2. Trainee team deploys 15 MeshBeacon nodes
3. Command center shows real-time detections
4. Trainees practice triangulation, navigation to signals, and rescue procedures
5. System logs all detections for after-action review and scoring

**Value:** Realistic training with measurable outcomes; builds team proficiency with new technology; documents training for certification

---

## Technical Challenges & Mitigations

### Challenge 1: Bluetooth Range Limitations

**Problem:** BLE typically ranges 10-30m; detection range inadequate for large search areas

**Mitigations:**
- **High-gain antennas:** Upgrade from PCB antenna to external 2.4GHz whip (+3-5dB gain)
- **Active scanning:** Use SCAN_REQ to elicit SCAN_RSP from devices (increases effective range)
- **Multiple nodes:** Dense deployment (50m spacing) ensures overlapping coverage
- **Elevated placement:** Height advantage improves line-of-sight (inverse square law)
- **Directionality:** Use directional antennas on mobile nodes for "sweep" searching

**Expected Improvement:** Range 30m → 60-100m in optimal conditions

### Challenge 2: Bluetooth Privacy - Random MAC Addresses

**Problem:** Modern smartphones randomize MAC addresses (Apple iOS 14+, Android 10+) to prevent tracking

**Mitigations:**
- **IRK resolution:** Extract Identity Resolving Keys from smartphone backups (requires victim consent/pre-registration)
- **Fingerprinting:** Even with random MACs, service UUIDs and manufacturer data remain constant
- **Behavioral patterns:** Detect "clusters" of devices that move together (likely single person)
- **Medical devices:** Many medical devices do NOT use random MACs (legacy Bluetooth implementations)

**Effectiveness:** Can still detect 60-80% of smartphones via non-randomized fields; medical devices remain 100% detectable

### Challenge 3: RF Interference in Urban Environments

**Problem:** Urban areas saturated with Bluetooth devices (cars, appliances, other survivors' phones)

**Mitigations:**
- **Medical device prioritization:** Flag known medical MAC prefixes immediately (no false positives)
- **RSSI filtering:** Ignore weak signals (<-85dBm) to focus on nearby devices
- **Time-series analysis:** Track signal strength over time (trapped victim's signal should be static, not moving)
- **Duplicate suppression:** Mesh network deduplicates same MAC from multiple nodes

**Expected Result:** ~10% false positive rate in dense urban; <1% for medical devices

### Challenge 4: LoRa Mesh Scalability

**Problem:** LoRa has limited bandwidth (~250 bps); mesh network may saturate with 50+ nodes

**Mitigations:**
- **Message prioritization:** Medical device detections sent immediately; other devices batched
- **Adaptive duty cycle:** Nodes reduce transmission rate if channel busy (CSMA/CA)
- **Edge processing:** Nodes filter and aggregate detections before transmission (send summaries, not raw scans)
- **Multi-channel:** Use multiple LoRa frequencies (if hardware supports) for parallel communication

**Capacity:** Estimated 50 nodes maximum per mesh with 5-minute latency; 100 nodes with 15-minute latency

### Challenge 5: Power Management for Extended Deployments

**Problem:** Battery-only nodes last 24-48 hours; long disasters (hurricanes, wildfires) require weeks

**Mitigations:**
- **Solar panels:** 5W panel + 10,000mAh battery = indefinite operation in sunny conditions
- **Conservation mode:** Reduce scan rate from 100% to 10% duty cycle (extends life 10x)
- **Hierarchical mesh:** Battery-powered mobile nodes, solar-powered fixed nodes (fixed nodes relay)
- **Swappable batteries:** Quick-change battery packs for field replacement

**Target:** 7+ days on battery; indefinite with solar (at reduced scan rate)

---

## Ethics, Privacy, and Legal Considerations

### Informed Consent

**Question:** Is it ethical to detect medical devices without patient consent?

**Position:** 
- **Emergency Exception:** In life-threatening emergencies, detection without consent is justified (similar to medical triage)
- **Opt-In Program:** For pre-planned deployments (hurricanes, wildfires), offer opt-in device registration program
- **Data Deletion:** All MAC addresses purged within 30 days post-incident; no long-term storage

**Best Practice:** Partner with medical device manufacturers to add "emergency beacon mode" toggle in device apps

### Medical Device Safety

**Question:** Could RF emissions from LoRa nodes interfere with medical devices?

**Position:**
- **Passive BLE scanning only:** Nodes do not transmit on 2.4GHz (no interference with medical devices)
- **LoRa frequency:** 900MHz (US) / 868MHz (EU) - separate from medical device telemetry (2.4GHz)
- **Low power:** 28dBm LoRa TX power well below FCC limits; medical devices tested for EM immunity

**Validation Required:** Test with actual pacemakers/insulin pumps in lab environment before field deployment

### HIPAA Compliance (US) / GDPR (EU)

**Question:** Does detecting MAC addresses constitute collection of protected health information?

**Legal Analysis:**
- **HIPAA:** MAC address alone is NOT PHI (not individually identifiable); becomes PHI only when linked to name/identity
- **GDPR:** MAC address is "personal data"; emergency processing permitted under Article 6(1)(d) (vital interests)
- **Data Minimization:** Collect only MAC, RSSI, GPS; do NOT collect device serial numbers, patient names, or medical data

**Compliance Strategy:**
- No linkage of MAC addresses to patient identities during operations
- Emergency use exemption documentation
- Data retention policy: 30 days maximum
- Incident commander is data controller; SAR organization responsible

### Law Enforcement Concerns

**Question:** Could this technology be misused for surveillance/tracking?

**Mitigation:**
- **Open Source:** All code public on GitHub; community audit for backdoors/tracking features
- **Emergency-Only Design:** System requires manual deployment and activation (not covert)
- **Short-Range Detection:** 50-100m range prevents wide-area surveillance
- **Encryption:** LoRa mesh messages encrypted (prevents third-party eavesdropping)

**Policy Position:** MeshBeacon is SAR-only technology; use for law enforcement surveillance violates project ethics and should be prohibited by license terms

---

## Success Metrics

### Technical Performance Metrics

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| BLE Detection Range | ≥50 meters | Field test with known devices at measured distances |
| Triangulation Accuracy | ≤10 meters | GPS ground truth vs. computed location |
| LoRa Mesh Range | ≥10 km line-of-sight | Link testing between nodes with GPS |
| Detection Latency | ≤60 seconds | Timestamp at victim vs. timestamp at command center |
| Medical Device ID Rate | ≥95% true positive | Lab test with known medical devices |
| Battery Life (Active) | ≥24 hours | Continuous scanning test |
| Battery Life (Conservation) | ≥7 days | 10% duty cycle test |
| Solar Self-Sufficiency | Indefinite in sun | 30-day deployment test |

### Operational Metrics

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Deployment Time | ≤5 minutes per node | Timed training exercises |
| User Training Time | ≤2 hours to proficiency | Training program evaluation |
| System Reliability | ≥95% uptime | Field deployment logs |
| False Positive Rate | <5% | Post-incident analysis |
| Adoption by SAR Teams | 50+ organizations by EOY 2027 | Partnership tracking |

### Humanitarian Impact Metrics

| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Lives Saved | ≥1 documented rescue | Incident reports from SAR teams |
| Search Time Reduction | 50% vs. baseline | Comparison to historical SAR operations |
| Coverage Area per Node | 5,000 sq meters | Calculated from detection range |
| Cost per Deployment | <$2,000 (50-node system) | Hardware BOM + labor |

---

## Bill of Materials (BOM)

### 50-Node Standard Deployment Kit

| Item | Quantity | Unit Cost | Total Cost |
|------|----------|-----------|------------|
| Heltec WiFi LoRa 32 V4 | 50 | $18.00 | $900 |
| L76K GNSS module | 50 | $8.00 | $400 |
| 18650 Li-ion battery (3000mAh) | 50 | $3.00 | $150 |
| Weatherproof enclosure (IP65) | 50 | $5.00 | $250 |
| 868/915MHz LoRa antenna | 50 | $2.00 | $100 |
| 2.4GHz BLE high-gain antenna | 50 | $4.00 | $200 |
| Mounting hardware (magnets/straps) | 50 | $3.00 | $150 |
| USB-C charging cables | 10 | $2.00 | $20 |
| Pelican case (for transport) | 2 | $100.00 | $200 |
| Command center laptop + GPS | 1 | $800.00 | $800 |
| **TOTAL** | | | **$3,170** |

**Per-Node Cost:** $63.40 (all-inclusive)

### Optional: Solar-Powered Upgrade (10 nodes)

| Item | Quantity | Unit Cost | Total Cost |
|------|----------|-----------|------------|
| 5W 6V solar panel | 10 | $10.00 | $100 |
| Larger weatherproof box | 10 | $8.00 | $80 |
| 10,000mAh battery bank | 10 | $12.00 | $120 |
| **ADDITIONAL COST** | | | **$300** |

**Solar Node Cost:** $93.40 per node

---

## Open Questions

### Technical

1. **IRK Extraction:** Can we develop legal, ethical method to extract IRK from smartphones for pre-registration programs?
   - **Approach:** Work with phone manufacturers (Apple, Google) on "Emergency Beacon" API
   - **Timeline:** 2027-2028 (requires industry partnerships)

2. **Medical Device Database:** Who maintains the authoritative list of medical device MAC prefixes?
   - **Proposed Solution:** Crowdsourced GitHub repository; contributions from SAR teams, medical device users
   - **Validation:** Cross-reference with FDA device listings, manufacturer docs

3. **Mesh Protocol:** Should we use Meshtastic (established, large community) or custom protocol (optimized for SAR)?
   - **Recommendation:** Start with Meshtastic for Phase 1-2; evaluate custom protocol in Phase 3 if performance insufficient

### Operational

4. **Certification:** Do SAR teams need special training/certification to deploy MeshBeacon?
   - **Recommendation:** 2-hour training module; no formal certification required (similar to thermal imaging cameras)

5. **Liability:** Who is liable if system fails to detect victim or provides false location?
   - **Legal Consultation Required:** Likely covered under Good Samaritan laws; need formal legal opinion
   - **Mitigation:** Clear disclaimers; position as "aid" to search, not replacement for human judgment

6. **International Deployment:** How do we handle different LoRa frequencies (US 915MHz vs EU 868MHz vs Asia 923MHz)?
   - **Solution:** Hardware SKUs for each region; firmware auto-detects frequency on boot

### Strategic

7. **Sustainability:** How do we fund ongoing development after initial launch?
   - **Options:** (a) Grants from DHS/FEMA, (b) Hardware sales via contract manufacturing, (c) SaaS command center hosting
   - **Preference:** Open hardware + SaaS model (hardware at cost, command center $50/month subscription for large organizations)

8. **Partnerships:** Which organizations should we partner with for validation and adoption?
   - **Priority 1:** FEMA US&R Task Forces (28 teams nationwide)
   - **Priority 2:** National Association for Search and Rescue (NASAR)
   - **Priority 3:** International Search and Rescue Advisory Group (INSARAG)
   - **Priority 4:** Medical device manufacturers (Medtronic, Abbott, Dexcom) for MAC prefix database

---

## Appendix A: Medical Device Detection Modes

### Mode 1: Known MAC Prefix Detection

**How it works:** Node firmware contains hardcoded list of medical device MAC prefixes (OUI database)

**Detection Example:**
```
[Node detects BLE advertisement]
MAC: 70:B3:D5:B3:4:1A:2B
RSSI: -72 dBm

[Firmware checks OUI]
Prefix: 70:B3:D5:B3:4 → MATCH: Medtronic cardiac device

[Immediate priority alert sent via LoRa]
Priority: CRITICAL
Device Type: Pacemaker/ICD
Location: 34.0522°N, 118.2437°W
Est. Distance: 18 meters
```

### Mode 2: BLE Service UUID Analysis

**How it works:** Node logs advertised BLE service UUIDs; command center correlates with medical device database

**Detection Example:**
```
[Node detects BLE advertisement]
MAC: A4:B2:C3:D4:E5:F6 (unknown vendor)
Service UUIDs: [0x180D, 0x180F, Custom:0xFE12]

[Command center lookup]
0x180D → Heart Rate Service (Bluetooth SIG standard)
0x180F → Battery Service
Custom:0xFE12 → Possibly proprietary health device

[Alert generated]
Priority: MEDIUM
Device Type: Wearable health monitor (suspected)
Confidence: 70%
```

### Mode 3: RPA (Resolvable Private Address) Resolution

**How it works:** Pre-registered users provide IRK; node firmware resolves random MAC to true identity

**Detection Example:**
```
[Node detects BLE advertisement]
MAC: 4C:AB:12:34:56:78 (random address, changes every 15 min)

[Firmware attempts IRK resolution using btrpa-scan algorithm]
IRK Database: 50 pre-registered users (opt-in disaster preparedness program)
→ IRK #23 resolves to TRUE IDENTITY: John Doe, iPhone 15 Pro

[Alert generated]
Priority: HIGH (pre-registered user)
Device Type: Smartphone (iPhone)
Owner: John Doe (Age 67, medical conditions: diabetes, hypertension)
Location: 34.0522°N, 118.2437°W
```

---

## Appendix B: LoRa Mesh Network Design

### Frequency Plans

| Region | Frequency Band | Channels | TX Power Limit |
|--------|----------------|----------|----------------|
| **North America** | 902-928 MHz | 64 channels (125kHz BW) | 30 dBm EIRP |
| **Europe** | 863-870 MHz | 8 channels (125kHz BW) | 14 dBm EIRP (27 dBm with LBT) |
| **Asia (CN)** | 470-510 MHz | 96 channels (125kHz BW) | 19.15 dBm EIRP |
| **Australia** | 915-928 MHz | 64 channels (125kHz BW) | 30 dBm EIRP |

### Mesh Topology Example (20-node deployment)

```
                    [Command Center]
                           |
         +-----------------+-----------------+
         |                 |                 |
    [Node 1]          [Node 2]          [Node 3]
    (Elevated)        (Elevated)        (Elevated)
       / \               / \               / \
      /   \             /   \             /   \
  [N4]   [N5]      [N6]   [N7]      [N8]   [N9]
  (Field) (Field)  (Field) (Field)  (Field) (Field)
    |       |         |       |         |       |
  [N10]  [N11]     [N12]  [N13]     [N14]  [N15]
  (Ground)(Ground) (Ground)(Ground) (Ground)(Ground)
```

**Key Principles:**
- **Hierarchical:** Elevated nodes act as "supernodes" with longer range
- **Redundant:** Multiple paths between any two nodes (self-healing)
- **Dynamic:** Mesh automatically reconfigures if node fails

### Message Routing Example

```
Scenario: Node 10 detects medical device; needs to reach Command Center

Path 1 (Primary): Node 10 → Node 4 → Node 1 → Command Center (3 hops, 8 sec)
Path 2 (Backup):  Node 10 → Node 5 → Node 2 → Command Center (3 hops, 9 sec)
Path 3 (If Node 4 fails): Node 10 → Node 11 → Node 5 → Node 2 → Command Center (4 hops, 12 sec)
```

---

## Appendix C: Reference Materials

### Hardware Datasheets
- [Heltec WiFi LoRa 32 V4 Documentation](https://wiki.heltec.org/docs/devices/open-source-hardware/esp32-series/lora-32/wifi-lora-32-v4/)
- [Semtech SX1262 Datasheet](https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R000000HT76/7Nka9W5WgugoZe_.7SFxb8H3wF5eGiUPQcxaXwKs)
- [ESP32-S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)

### Software & Firmware
- [btrpa-scan GitHub Repository](https://github.com/EstesIO/btrpa-scan)
- [Meshtastic Firmware](https://meshtastic.org/)
- [Heltec ESP32 Arduino Framework](https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series)

### Academic Research
- Bluetooth LE in Disaster Response: [IEEE Paper](https://ieeexplore.ieee.org/document/8835567)
- LoRa for Emergency Networks: [Springer Journal](https://link.springer.com/article/10.1007/s11277-021-08647-4)
- Medical Device MAC Address Privacy: [ACM CCS 2019](https://dl.acm.org/doi/10.1145/3319535.3354240)

### SAR Standards & Protocols
- [NFPA 1670: Standard on Operations and Training for Technical Search and Rescue](https://www.nfpa.org/codes-and-standards/all-codes-and-standards/list-of-codes-and-standards/detail?code=1670)
- [INSARAG Guidelines (UN)](https://www.insarag.org/methodology/guidelines/)
- [FEMA US&R Response System](https://www.fema.gov/emergency-managers/national-preparedness/frameworks/urban-search-rescue)

---

## Document Change Log

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | Feb 16, 2026 | Grayson Estes | Initial release |

---

**Project Status:** Proof of Concept  
**Next Milestone:** Phase 1 Hardware Integration (March 2026)  
**License:** Apache 2.0 (Hardware + Software)  
**Contact:** grayson@estes.io
