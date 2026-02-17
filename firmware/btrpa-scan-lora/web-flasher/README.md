# btrpa-scan-lora Web Flasher

## Quick Start

1. **Serve the web flasher locally:**
   ```bash
   cd web-flasher
   python3 -m http.server 8000
   ```

2. **Open in browser:**
   - Navigate to `http://localhost:8000`
   - Use Chrome, Edge, or Opera (requires Web Serial API)

3. **Configure & Flash:**
   - Enter target MAC address(es) from phone app disconnect events
   - Download the generated `config.h` file
   - Replace `include/config.h` with the downloaded file
   - Rebuild firmware: `pio run` (from project root)
   - Connect Heltec device via USB
   - Click "Connect & Flash Firmware"
   - Select the serial port when prompted
   - Wait for flashing to complete (1-2 minutes)

## Deployment to GitHub Pages

To make this available online for easy access by SAR teams:

```bash
# From repository root
git add firmware/btrpa-scan-lora/web-flasher/
git commit -m "Add web flasher for easy deployment"
git push

# Enable GitHub Pages in repository settings:
# Settings > Pages > Source: main branch > /firmware/btrpa-scan-lora/web-flasher
```

Access URL will be: `https://yourusername.github.io/repo-name/firmware/btrpa-scan-lora/web-flasher/`

## Browser Requirements

- **Chrome** 89+ (recommended)
- **Edge** 89+
- **Opera** 75+
- Safari and Firefox do not support Web Serial API

## Files

- `index.html` - Main web flasher interface
- `manifest.json` - ESP Web Tools firmware manifest
- `firmware.bin` - Main application firmware
- `bootloader.bin` - ESP32-S3 bootloader
- `partitions.bin` - Partition table

## Updating Firmware

When you make code changes:

1. Build new firmware: `pio run`
2. Copy binaries to web-flasher:
   ```bash
   cp .pio/build/heltec_wifi_lora_32_V3/firmware.bin web-flasher/
   cp .pio/build/heltec_wifi_lora_32_V3/bootloader.bin web-flasher/
   cp .pio/build/heltec_wifi_lora_32_V3/partitions.bin web-flasher/
   ```
3. Update version in `manifest.json`
4. Commit and push changes

## Security Note

The web flasher downloads config.h but requires manual rebuild before flashing. This ensures proper compilation and validation of MAC addresses.

For truly one-click deployment, consider implementing OTA configuration updates in future versions.
