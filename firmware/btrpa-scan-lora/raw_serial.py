#!/usr/bin/env python3
import serial
import time

port = '/dev/cu.usbserial-0001'
print(f"Opening {port}...")

try:
    ser = serial.Serial(port, 115200, timeout=0.1)
    print("Port opened successfully!")
    print("Waiting for data...")
    print("-" * 60)

    # Try to trigger a reset via DTR
    ser.dtr = False
    time.sleep(0.1)
    ser.dtr = True
    time.sleep(0.5)

    start = time.time()
    got_data = False

    while time.time() - start < 10:
        data = ser.read(100)
        if data:
            got_data = True
            try:
                print(data.decode('utf-8', errors='replace'), end='', flush=True)
            except:
                print(data, flush=True)

    print()
    print("-" * 60)

    if not got_data:
        print("NO DATA RECEIVED!")
        print("\nPossible issues:")
        print("1. Device is in bootloader mode (press RST button)")
        print("2. Wrong baud rate")
        print("3. USB cable is power-only (no data)")
        print("4. Serial port permissions issue")

    ser.close()

except Exception as e:
    print(f"Error: {e}")
