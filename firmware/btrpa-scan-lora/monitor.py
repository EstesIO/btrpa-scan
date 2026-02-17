#!/usr/bin/env python3
"""
Continuous serial monitor for Heltec device
Press Ctrl+C to stop
"""
import serial
import sys

try:
    ser = serial.Serial('/dev/cu.usbserial-0001', 115200, timeout=0.1)
    print("=" * 70)
    print("Connected to Heltec V3 - Monitoring Serial Output")
    print("Press RST button on device to see boot messages")
    print("Press Ctrl+C to stop")
    print("=" * 70)
    print()

    while True:
        if ser.in_waiting > 0:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(line)
                    sys.stdout.flush()
            except Exception:
                pass

except serial.SerialException as e:
    print(f"Error: Could not open serial port: {e}")
    print("Make sure:")
    print("  1. Device is plugged in via USB")
    print("  2. No other program (Arduino IDE, screen, etc.) is using the port")
    sys.exit(1)
except KeyboardInterrupt:
    print("\n\nStopped by user")
    ser.close()
