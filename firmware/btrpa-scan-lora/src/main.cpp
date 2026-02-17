/**
 * btrpa-scan-lora - Distributed BLE Detection Mesh Firmware
 *
 * Extends btrpa-scan BLE detection onto LoRa mesh networks for
 * time-critical search and rescue operations.
 *
 * Hardware: Heltec WiFi LoRa 32 V3/V4
 * Based on: btrpa-scan by David Kennedy (@HackingDave)
 */

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <TinyGPSPlus.h>
#include <RadioLib.h>
#include "config.h"

// Heltec V3 Display Support - Using U8g2
#if defined(HELTEC_V3)
    #include <U8g2lib.h>
    #include <Wire.h>

    // Heltec V3: SSD1306 128X64 OLED
    // I2C pins: SDA=17, SCL=18, RST=21
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 21);

    // Display lock to prevent scanning screen from overwriting alerts
    volatile bool displayLocked = false;
    unsigned long displayLockUntil = 0;
#endif

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

void initDisplay() {
    #if defined(HELTEC_V3)
    // Enable Vext power for OLED (GPIO 36)
    pinMode(36, OUTPUT);
    digitalWrite(36, LOW);  // LOW = power ON for Vext
    delay(100);
    Serial.println("Display: Vext power enabled");

    // Initialize I2C with Heltec V3 pins: SDA=17, SCL=18
    Wire.begin(17, 18);
    delay(100);

    Serial.println("Display: Initializing OLED...");
    u8g2.begin();
    Serial.println("Display: OLED initialized");

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "btrpa-scan-lora");
    u8g2.drawStr(0, 25, "Initializing...");
    u8g2.sendBuffer();
    Serial.println("Display: Startup screen displayed");
    #endif
}

void displayStatus(const char* line1, const char* line2 = "", const char* line3 = "", const char* line4 = "") {
    #if defined(HELTEC_V3)
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    if (line1[0]) u8g2.drawStr(0, 10, line1);
    if (line2[0]) u8g2.drawStr(0, 24, line2);
    if (line3[0]) u8g2.drawStr(0, 38, line3);
    if (line4[0]) u8g2.drawStr(0, 52, line4);
    u8g2.sendBuffer();
    #endif
}

void displayTrueHit(const char* mac, int rssi) {
    #if defined(HELTEC_V3)
    u8g2.clearBuffer();

    // Large alert text
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(5, 18, "TRUE HIT!");

    // MAC and RSSI
    u8g2.setFont(u8g2_font_ncenB08_tr);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s", mac);
    u8g2.drawStr(0, 35, buffer);
    snprintf(buffer, sizeof(buffer), "RSSI: %d dBm", rssi);
    u8g2.drawStr(0, 48, buffer);
    snprintf(buffer, sizeof(buffer), "Time: %lus", millis() / 1000);
    u8g2.drawStr(0, 61, buffer);

    u8g2.sendBuffer();

    // Lock display for 3 seconds using timer (non-blocking)
    displayLocked = true;
    displayLockUntil = millis() + 3000;
    #endif
}

void displayPossibleHit(const char* mac, int rssi, const char* deviceType) {
    #if defined(HELTEC_V3)
    u8g2.clearBuffer();

    // Alert text
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 14, "POSSIBLE HIT");

    // Device info
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 28, deviceType);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s", mac);
    u8g2.drawStr(0, 42, buffer);
    snprintf(buffer, sizeof(buffer), "RSSI: %d dBm", rssi);
    u8g2.drawStr(0, 56, buffer);

    u8g2.sendBuffer();

    // Lock display for 2 seconds using timer (non-blocking)
    displayLocked = true;
    displayLockUntil = millis() + 2000;
    #endif
}

void displayScanning() {
    #if defined(HELTEC_V3)
    // Check if lock timer expired
    if (displayLocked && millis() >= displayLockUntil) {
        displayLocked = false;
    }

    // Don't update if display is still locked (showing alert)
    if (displayLocked) return;

    static unsigned long lastUpdate = 0;
    static int dotCount = 0;

    // Update display every 500ms
    if (millis() - lastUpdate > 500) {
        lastUpdate = millis();

        extern uint32_t totalScans;
        extern uint32_t trueHits;
        extern uint32_t possibleHits;

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.drawStr(20, 16, "SCANNING");

        // Animated dots
        char dots[5] = "";
        for (int i = 0; i < dotCount; i++) {
            dots[i] = '.';
        }
        dots[dotCount] = '\0';
        u8g2.drawStr(50, 32, dots);

        dotCount = (dotCount + 1) % 4;

        // Stats
        u8g2.setFont(u8g2_font_ncenB08_tr);
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Scans: %u", totalScans);
        u8g2.drawStr(0, 50, buffer);
        snprintf(buffer, sizeof(buffer), "Hits: %u/%u", trueHits, possibleHits);
        u8g2.drawStr(0, 62, buffer);

        u8g2.sendBuffer();
    }
    #endif
}

// ============================================================================
// LORA MESH COMMUNICATION
// ============================================================================

// Heltec V3 LoRa SX1262 pin definitions
#if defined(HELTEC_V3)
    #define LORA_NSS 8
    #define LORA_DIO1 14
    #define LORA_BUSY 13
    #define LORA_RST 12
    #define LORA_MISO 11
    #define LORA_MOSI 10
    #define LORA_SCK 9
#elif defined(HELTEC_V2)
    #define LORA_NSS 18
    #define LORA_DIO1 26
    #define LORA_BUSY 32
    #define LORA_RST 14
    #define LORA_MISO 19
    #define LORA_MOSI 27
    #define LORA_SCK 5
#endif

// Create LoRa radio instance
SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);

// LoRa message types
enum MessageType {
    MSG_TRUE_HIT = 1,      // Critical: Exact MAC match detected
    MSG_POSSIBLE_HIT = 2,  // Lower priority: Medical device prefix
    MSG_POSITION = 3,      // Position beacon (movement-based)
    MSG_STATUS = 4         // Node status update
};

// Mesh message structure (max 255 bytes for LoRa)
struct MeshMessage {
    uint8_t type;           // MessageType
    char nodeId[17];        // Node ID (max 16 chars + null)
    char mac[18];           // MAC address (xx:xx:xx:xx:xx:xx + null)
    int16_t rssi;           // RSSI in dBm
    float lat;              // Latitude
    float lon;              // Longitude
    uint32_t timestamp;     // Milliseconds since boot
    char deviceType[32];    // For POSSIBLE_HIT messages
};

bool loraInitialized = false;

void initLoRa() {
    Serial.println("Initializing LoRa...");

    // Initialize SPI for LoRa with custom pins
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

    // Initialize SX1262 with frequency from config
    Serial.printf("LoRa: Configuring SX1262 at %d MHz\n", LORA_FREQUENCY);
    int state = radio.begin(LORA_FREQUENCY);

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("LoRa: SX1262 initialized successfully");
    } else {
        Serial.printf("LoRa: Init failed, code %d\n", state);
        return;
    }

    // Set LoRa parameters for long range
    radio.setSpreadingFactor(10);        // SF10 for good range
    radio.setBandwidth(125.0);           // 125 kHz
    radio.setCodingRate(8);              // CR 4/8 for error correction
    radio.setOutputPower(LORA_TX_POWER); // TX power from config
    radio.setPreambleLength(16);

    // Set sync word for private network
    radio.setSyncWord(0x12);

    // Start in receive mode
    radio.startReceive();

    loraInitialized = true;
    Serial.println("LoRa: Mesh ready");
}

void sendLoRaMessage(const MeshMessage& msg) {
    if (!loraInitialized) return;

    // Serialize message to byte array
    uint8_t buffer[sizeof(MeshMessage)];
    memcpy(buffer, &msg, sizeof(MeshMessage));

    // Transmit via LoRa (blocking but feeds watchdog)
    Serial.printf("LoRa: Sending message (type %d, %d bytes)\n", msg.type, sizeof(MeshMessage));
    int state = radio.transmit(buffer, sizeof(MeshMessage));

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("LoRa: Message sent successfully");
        // Return to receive mode
        radio.startReceive();
    } else {
        Serial.printf("LoRa: Send failed, code %d\n", state);
        radio.startReceive();
    }
}

void sendTrueHitAlert(const char* mac, int rssi, double lat, double lon) {
    MeshMessage msg;
    msg.type = MSG_TRUE_HIT;
    strncpy(msg.nodeId, NODE_ID_CONFIG, sizeof(msg.nodeId) - 1);
    strncpy(msg.mac, mac, sizeof(msg.mac) - 1);
    msg.rssi = rssi;
    msg.lat = lat;
    msg.lon = lon;
    msg.timestamp = millis();
    msg.deviceType[0] = '\0';

    Serial.println("📡 Sending TRUE HIT via LoRa mesh...");
    sendLoRaMessage(msg);
}

void sendPossibleHitAlert(const char* mac, int rssi, double lat, double lon, const char* deviceType) {
    MeshMessage msg;
    msg.type = MSG_POSSIBLE_HIT;
    strncpy(msg.nodeId, NODE_ID_CONFIG, sizeof(msg.nodeId) - 1);
    strncpy(msg.mac, mac, sizeof(msg.mac) - 1);
    msg.rssi = rssi;
    msg.lat = lat;
    msg.lon = lon;
    msg.timestamp = millis();
    strncpy(msg.deviceType, deviceType, sizeof(msg.deviceType) - 1);

    Serial.println("📡 Sending POSSIBLE HIT via LoRa mesh...");
    sendLoRaMessage(msg);
}

void sendPositionBeaconLoRa(double lat, double lon) {
    MeshMessage msg;
    msg.type = MSG_POSITION;
    strncpy(msg.nodeId, NODE_ID_CONFIG, sizeof(msg.nodeId) - 1);
    msg.mac[0] = '\0';
    msg.rssi = 0;
    msg.lat = lat;
    msg.lon = lon;
    msg.timestamp = millis();
    msg.deviceType[0] = '\0';

    Serial.println("📡 Sending position beacon via LoRa...");
    sendLoRaMessage(msg);
}

void checkLoRaMessages() {
    if (!loraInitialized) return;

    // Check if there's a message available (non-blocking)
    if (radio.available()) {
        uint8_t buffer[sizeof(MeshMessage)];
        int state = radio.readData(buffer, sizeof(MeshMessage));

        if (state == RADIOLIB_ERR_NONE) {
            // Message received successfully
            MeshMessage* msg = (MeshMessage*)buffer;

            Serial.println("\n📩 LoRa message received:");
            Serial.printf("  From: %s\n", msg->nodeId);
            Serial.printf("  Type: %d\n", msg->type);

            if (msg->type == MSG_TRUE_HIT) {
                Serial.println("  🚨 TRUE HIT ALERT from mesh!");
                Serial.printf("  MAC: %s\n", msg->mac);
                Serial.printf("  RSSI: %d dBm\n", msg->rssi);
                if (msg->lat != 0.0 && msg->lon != 0.0) {
                    Serial.printf("  GPS: %.6f, %.6f\n", msg->lat, msg->lon);
                }

                // Display alert on local OLED
                displayTrueHit(msg->mac, msg->rssi);

                // TODO: Forward to homebase if this is a relay node
            } else if (msg->type == MSG_POSSIBLE_HIT) {
                Serial.println("  ⚠️  POSSIBLE HIT from mesh");
                Serial.printf("  MAC: %s\n", msg->mac);
                Serial.printf("  Device: %s\n", msg->deviceType);
            } else if (msg->type == MSG_POSITION) {
                Serial.printf("  📍 Position beacon: %.6f, %.6f\n", msg->lat, msg->lon);
            }

            // Return to receive mode
            radio.startReceive();
        }
    }
}

// ============================================================================
// CONFIGURATION
// ============================================================================

#define NODE_ID NODE_ID_CONFIG
#define SCAN_INTERVAL BLE_SCAN_INTERVAL
#define SCAN_WINDOW BLE_SCAN_WINDOW
#define SCAN_DURATION BLE_SCAN_DURATION

// GPS Serial (adjust for your Heltec version)
HardwareSerial GPSSerial(1);

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

TinyGPSPlus gps;
bool gpsAvailable = false;

// Target MAC addresses (TRUE HIT) - loaded from config.h
std::vector<std::string> targetMACs;

// Medical device MAC prefixes (POSSIBLE HIT) - loaded from config.h
struct MedicalDevicePrefix {
    std::string prefix;
    std::string deviceType;
    std::string manufacturer;
};

std::vector<MedicalDevicePrefix> medicalPrefixes;

// Detection statistics
uint32_t totalScans = 0;
uint32_t trueHits = 0;
uint32_t possibleHits = 0;

// GPS tracking for movement-based beaconing
double lastBeaconLat = 0.0;
double lastBeaconLon = 0.0;
#define MOVEMENT_THRESHOLD_METERS 25.0

// ============================================================================
// BLE SCANNING CALLBACKS
// ============================================================================

class BLEScanCallbacks : public NimBLEAdvertisedDeviceCallbacks {

    bool isTrueHit(const std::string& macAddr) {
        for (const auto& targetMAC : targetMACs) {
            if (strcasecmp(macAddr.c_str(), targetMAC.c_str()) == 0) {
                return true;
            }
        }
        return false;
    }

    MedicalDevicePrefix* isPossibleHit(const std::string& macAddr) {
        for (auto& prefix : medicalPrefixes) {
            if (macAddr.length() >= prefix.prefix.length()) {
                std::string devicePrefix = macAddr.substr(0, prefix.prefix.length());
                if (strcasecmp(devicePrefix.c_str(), prefix.prefix.c_str()) == 0) {
                    return &prefix;
                }
            }
        }
        return nullptr;
    }

    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        totalScans++;

        std::string macAddr = advertisedDevice->getAddress().toString();
        int rssi = advertisedDevice->getRSSI();

        // DEBUG: Show all detected devices
        #if DEBUG_SHOW_ALL_DEVICES
        Serial.printf("[BLE] Device: %s | RSSI: %d dBm", macAddr.c_str(), rssi);
        if (advertisedDevice->haveName()) {
            Serial.printf(" | Name: %s", advertisedDevice->getName().c_str());
        }
        Serial.println();
        #endif

        // Get GPS coordinates if available
        double lat = 0.0, lon = 0.0;
        if (gpsAvailable && gps.location.isValid()) {
            lat = gps.location.lat();
            lon = gps.location.lng();
        }

        // Check for TRUE HIT (exact MAC match)
        if (isTrueHit(macAddr)) {
            trueHits++;
            handleTrueHit(macAddr, rssi, lat, lon);
            return;
        }

        // Check for POSSIBLE HIT (medical device prefix match)
        MedicalDevicePrefix* medical = isPossibleHit(macAddr);
        if (medical != nullptr) {
            possibleHits++;
            handlePossibleHit(macAddr, rssi, lat, lon, medical);
            return;
        }
    }

    void handleTrueHit(const std::string& mac, int rssi, double lat, double lon) {
        Serial.println("\n🚨 ========== TRUE HIT ==========");
        Serial.printf("Node: %s\n", NODE_ID);
        Serial.printf("Target MAC: %s\n", mac.c_str());
        Serial.printf("RSSI: %d dBm\n", rssi);

        if (gpsAvailable && lat != 0.0 && lon != 0.0) {
            Serial.printf("GPS: %.6f, %.6f\n", lat, lon);
        } else {
            Serial.println("GPS: N/A");
        }

        Serial.printf("Time: %lu ms\n", millis());
        Serial.println("================================\n");

        // Display alert on OLED screen
        displayTrueHit(mac.c_str(), rssi);

        // Send priority LoRa mesh message
        sendTrueHitAlert(mac.c_str(), rssi, lat, lon);
    }

    void handlePossibleHit(const std::string& mac, int rssi, double lat, double lon,
                          MedicalDevicePrefix* medical) {
        Serial.println("\n⚠️  ======== POSSIBLE HIT ========");
        Serial.printf("Node: %s\n", NODE_ID);
        Serial.printf("MAC: %s\n", mac.c_str());
        Serial.printf("Device: %s (%s)\n", medical->deviceType.c_str(), medical->manufacturer.c_str());
        Serial.printf("RSSI: %d dBm\n", rssi);

        if (gpsAvailable && lat != 0.0 && lon != 0.0) {
            Serial.printf("GPS: %.6f, %.6f\n", lat, lon);
        } else {
            Serial.println("GPS: N/A");
        }

        Serial.printf("Time: %lu ms\n", millis());
        Serial.println("================================\n");

        // Display alert on OLED screen
        displayPossibleHit(mac.c_str(), rssi, medical->deviceType.c_str());

        // Send LoRa mesh message
        sendPossibleHitAlert(mac.c_str(), rssi, lat, lon, medical->deviceType.c_str());
    }

    // Note: Buzzer functions removed - Heltec V3 uses OLED display for alerts
    // If external buzzer is added, buzzer alert functions can be re-enabled
};

// ============================================================================
// GPS FUNCTIONS
// ============================================================================

void initGPS() {
    Serial.println("Initializing GPS...");
    GPSSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    delay(1000);

    // Wait briefly for GPS data
    unsigned long startTime = millis();
    while (millis() - startTime < 2000) {
        while (GPSSerial.available() > 0) {
            gps.encode(GPSSerial.read());
        }
    }

    if (gps.charsProcessed() > 10) {
        gpsAvailable = true;
        Serial.println("GPS: Connected");
        if (gps.location.isValid()) {
            Serial.printf("GPS: Initial fix - %.6f, %.6f\n",
                         gps.location.lat(), gps.location.lng());
        } else {
            Serial.println("GPS: Waiting for fix...");
        }
    } else {
        gpsAvailable = false;
        Serial.println("GPS: No module detected (continuing without GPS)");
    }
}

void updateGPS() {
    if (!gpsAvailable) return;

    while (GPSSerial.available() > 0) {
        gps.encode(GPSSerial.read());
    }
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Haversine formula for distance in meters
    double R = 6371000; // Earth radius in meters
    double phi1 = lat1 * DEG_TO_RAD;
    double phi2 = lat2 * DEG_TO_RAD;
    double deltaPhi = (lat2 - lat1) * DEG_TO_RAD;
    double deltaLambda = (lon2 - lon1) * DEG_TO_RAD;

    double a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
               cos(phi1) * cos(phi2) *
               sin(deltaLambda / 2) * sin(deltaLambda / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

bool shouldSendPositionBeacon() {
    if (!gpsAvailable || !gps.location.isValid()) return false;
    if (lastBeaconLat == 0.0 && lastBeaconLon == 0.0) return true; // First beacon

    double distance = calculateDistance(
        lastBeaconLat, lastBeaconLon,
        gps.location.lat(), gps.location.lng()
    );

    return distance >= MOVEMENT_THRESHOLD_METERS;
}

void sendPositionBeacon() {
    if (!gpsAvailable || !gps.location.isValid()) return;

    lastBeaconLat = gps.location.lat();
    lastBeaconLon = gps.location.lng();

    Serial.printf("📍 Position Beacon: %.6f, %.6f\n", lastBeaconLat, lastBeaconLon);

    // Send position beacon via LoRa mesh
    sendPositionBeaconLoRa(lastBeaconLat, lastBeaconLon);
}

// ============================================================================
// BLE SCANNING
// ============================================================================

void initBLE() {
    Serial.println("Initializing BLE...");

    NimBLEDevice::init(NODE_ID);
    NimBLEScan* pBLEScan = NimBLEDevice::getScan();

    pBLEScan->setAdvertisedDeviceCallbacks(new BLEScanCallbacks(), false);
    pBLEScan->setActiveScan(true); // Active scanning for better range
    pBLEScan->setInterval(SCAN_INTERVAL);
    pBLEScan->setWindow(SCAN_WINDOW);

    Serial.println("BLE: Scanner initialized");
}

void startBLEScan() {
    Serial.println("BLE: Starting continuous scan...");
    NimBLEScan* pBLEScan = NimBLEDevice::getScan();
    pBLEScan->start(SCAN_DURATION, nullptr, false); // Continuous scan
}

// ============================================================================
// MAIN SETUP AND LOOP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Initialize OLED display
    initDisplay();

    Serial.println("\n\n");
    Serial.println("================================");
    Serial.println("btrpa-scan-lora");
    Serial.println("BLE Detection Mesh Node");
    Serial.println("================================");
    Serial.printf("Node ID: %s\n", NODE_ID);
    Serial.printf("Firmware Build: %s %s\n", __DATE__, __TIME__);
    Serial.println("================================\n");

    // Load configuration from config.h
    Serial.println("Loading configuration...");

    // Load target MACs
    for (int i = 0; i < NUM_TARGET_MACS; i++) {
        targetMACs.push_back(TARGET_MACS[i]);
        Serial.printf("  Target MAC: %s\n", TARGET_MACS[i]);
    }

    // Load medical device prefixes
    if (ENABLE_MEDICAL_DEVICE_SCANNING) {
        for (int i = 0; i < NUM_MEDICAL_PREFIXES; i++) {
            MedicalDevicePrefix prefix;
            prefix.prefix = MEDICAL_DEVICE_PREFIXES[i].prefix;
            prefix.deviceType = MEDICAL_DEVICE_PREFIXES[i].deviceType;
            prefix.manufacturer = MEDICAL_DEVICE_PREFIXES[i].manufacturer;
            medicalPrefixes.push_back(prefix);
            Serial.printf("  Medical prefix: %s (%s - %s)\n",
                         prefix.prefix.c_str(),
                         prefix.deviceType.c_str(),
                         prefix.manufacturer.c_str());
        }
    }
    Serial.println();

    // Note: Heltec V3 doesn't have a built-in buzzer
    // External buzzer can be added later if needed
    // pinMode(BUZZER_PIN, OUTPUT);
    // tone(BUZZER_PIN, 1000, 200);

    displayStatus("btrpa-scan-lora", "Loading...", "", "");
    delay(500);

    // Initialize GPS
    initGPS();

    // Initialize BLE scanning
    initBLE();

    // Initialize LoRa mesh
    initLoRa();

    // Start BLE scanning
    startBLEScan();

    Serial.println("\n🔍 Node operational - scanning for targets...\n");

    // Show scanning screen on OLED
    displayStatus("READY", "Scanning for", "targets...", "");

    // Print configuration summary
    Serial.printf("Target MACs: %d configured\n", targetMACs.size());
    Serial.printf("Medical prefixes: %d configured\n", medicalPrefixes.size());

    #if DEBUG_SHOW_ALL_DEVICES
    Serial.println("");
    Serial.println("⚠️  DEBUG MODE: Showing ALL detected BLE devices");
    Serial.println("This will be very verbose! Looking for:");
    for (const auto& mac : targetMACs) {
        Serial.printf("   → Target: %s\n", mac.c_str());
    }
    #endif

    Serial.println();
}

void loop() {
    // Check for incoming LoRa messages
    checkLoRaMessages();

    // Update GPS data
    updateGPS();

    // Check if we should send position beacon
    if (shouldSendPositionBeacon()) {
        sendPositionBeacon();
    }

    // Print statistics every 30 seconds
    static unsigned long lastStatsTime = 0;
    if (millis() - lastStatsTime > 30000) {
        lastStatsTime = millis();
        Serial.println("\n--- Statistics ---");
        Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
        Serial.printf("Total scans: %u\n", totalScans);
        Serial.printf("TRUE HITs: %u\n", trueHits);
        Serial.printf("POSSIBLE HITs: %u\n", possibleHits);

        if (gpsAvailable && gps.location.isValid()) {
            Serial.printf("GPS: %.6f, %.6f\n", gps.location.lat(), gps.location.lng());
        } else {
            Serial.println("GPS: No fix");
        }
        Serial.println("------------------\n");
    }

    // Update OLED display with scanning animation
    displayScanning();

    delay(100); // Small delay to prevent watchdog issues
}
