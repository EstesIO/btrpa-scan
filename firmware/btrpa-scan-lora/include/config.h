/**
 * btrpa-scan-lora Configuration
 *
 * Edit this file to configure your node before flashing
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// NODE CONFIGURATION
// ============================================================================

// Unique node identifier (max 16 chars)
// Change this for each node: NODE-001, NODE-002, etc.
#define NODE_ID_CONFIG "NODE-001"

// ============================================================================
// TARGET MAC ADDRESSES (TRUE HIT)
// ============================================================================

// Add your target MAC addresses here (exact matches)
// Format: "aa:bb:cc:dd:ee:ff" (lowercase, colon-separated)
//
// Example scenarios:
// - Known pacemaker MAC from phone app disconnect
// - Known Apple Watch MAC
// - Known smartphone MAC
//
// To add multiple targets, uncomment and fill in:

const char* TARGET_MACS[] = {
    // INSTRUCTIONS: Add target MAC addresses below (one per line)
    // Format: "aa:bb:cc:dd:ee:ff" (lowercase, colon-separated)
    // Example: "70:b3:d5:b3:4a:bc",  // Medtronic pacemaker from phone app

    "28:34:ff:74:aa:99",  // Test device (replace with real target MAC)
    // Add more target MACs here...
};

const int NUM_TARGET_MACS = sizeof(TARGET_MACS) / sizeof(TARGET_MACS[0]);

// ============================================================================
// MEDICAL DEVICE PREFIXES (POSSIBLE HIT)
// ============================================================================

// Enable medical device prefix scanning
#define ENABLE_MEDICAL_DEVICE_SCANNING true

// Medical device MAC prefixes database
// These will trigger POSSIBLE HIT alerts
struct MedicalDevicePrefixConfig {
    const char* prefix;
    const char* deviceType;
    const char* manufacturer;
};

const MedicalDevicePrefixConfig MEDICAL_DEVICE_PREFIXES[] = {
    // Medtronic Cardiac Devices
    {"70:b3:d5:b3:4", "Pacemaker/ICD/CRT", "Medtronic"},

    // TODO: Research and add more medical device prefixes
    // Dexcom CGM sensors
    // Abbott FreeStyle CGM
    // Boston Scientific cardiac devices
    // Biotronik cardiac devices
    // Insulet OmniPod insulin pumps
    // Cochlear hearing implants
    // Add more as research progresses...
};

const int NUM_MEDICAL_PREFIXES = sizeof(MEDICAL_DEVICE_PREFIXES) / sizeof(MEDICAL_DEVICE_PREFIXES[0]);

// ============================================================================
// BLE SCANNING PARAMETERS
// ============================================================================

// Scan interval (milliseconds) - time between scan starts
#define BLE_SCAN_INTERVAL 500

// Scan window (milliseconds) - active scanning duration
// Must be <= scan interval
#define BLE_SCAN_WINDOW 450

// Scan duration (seconds) - 0 for continuous
#define BLE_SCAN_DURATION 0

// Active scanning (sends SCAN_REQ for better range)
#define BLE_ACTIVE_SCAN true

// ============================================================================
// GPS CONFIGURATION
// ============================================================================

// Movement threshold for position beaconing (meters)
// Node sends position update when it moves > this distance
#define GPS_MOVEMENT_THRESHOLD 25.0

// GPS timeout for initial fix (milliseconds)
#define GPS_INIT_TIMEOUT 2000

// ============================================================================
// ALERT CONFIGURATION
// ============================================================================

// Buzzer/piezo pin
#define BUZZER_PIN 13

// TRUE HIT alert configuration
#define TRUE_HIT_FREQUENCY 2000  // Hz
#define TRUE_HIT_DURATION 200    // ms
#define TRUE_HIT_BEEPS 3         // number of beeps
#define TRUE_HIT_DELAY 250       // ms between beeps

// POSSIBLE HIT alert configuration
#define POSSIBLE_HIT_FREQUENCY 1500
#define POSSIBLE_HIT_DURATION 300
#define POSSIBLE_HIT_BEEPS 2
#define POSSIBLE_HIT_DELAY 500

// ============================================================================
// LORA MESH CONFIGURATION (TODO)
// ============================================================================

// LoRa frequency (MHz) - must match your region
// US: 915, EU: 868, Asia: 923
#define LORA_FREQUENCY 915

// LoRa TX power (dBm)
#define LORA_TX_POWER 20

// Meshtastic channel name
#define MESH_CHANNEL_NAME "SAR-SEARCH"

// ============================================================================
// HARDWARE PIN DEFINITIONS
// ============================================================================

#if defined(HELTEC_V3)
    #define GPS_RX_PIN 37
    #define GPS_TX_PIN 38
    #define LED_PIN 35
#elif defined(HELTEC_V2)
    #define GPS_RX_PIN 17
    #define GPS_TX_PIN 16
    #define LED_PIN 25
#else
    #warning "Unknown Heltec version - using V3 pin definitions"
    #define GPS_RX_PIN 37
    #define GPS_TX_PIN 38
    #define LED_PIN 35
#endif

// ============================================================================
// DEBUG CONFIGURATION
// ============================================================================

// Enable verbose debug output
#define DEBUG_VERBOSE false

// Show ALL BLE devices detected (not just targets/medical)
// Set to true for testing to see all nearby BLE devices
#define DEBUG_SHOW_ALL_DEVICES false

// Statistics reporting interval (milliseconds)
#define STATS_INTERVAL 30000

#endif // CONFIG_H
