# 🔄 BootLoader_UART_Upload_Code

A lightweight UART bootloader for STM32F4 that allows in-field firmware updates **without using ST-Link or ISP tools**. The bootloader receives the application binary over UART and writes it to flash memory. It also supports an update area for downloading new firmware before applying it.

---

## 🚀 Features

- 📦 Receive `.hex` firmware via UART
- 🔁 Automatically jump to application after upload
- 🧠 Optional update area to temporarily store firmware before overwrite
- 🔐 Simple checksum verification
- ⚙️ Flash erase & sector-based writing
- 💡 LED status (optional)

---

## 🧠 How It Works

1. On power-up, bootloader runs from Sector 0–1.
2. Waits for firmware over UART within timeout period.
3. If data is received:
   - Write to application region (Sector 2–8) or
   - Store temporarily in update area (Sector 9+) before flashing.
4. After successful upload or timeout:
   - Jump to application at `0x08008000` (Sector 2).

---

## 🧱 Flash Memory Layout

| Sector | Address Range           | Size   | Purpose               |
|--------|-------------------------|--------|------------------------|
| 0      | 0x0800_0000 – 0x0800_3FFF | 16 KB  | Bootloader             |
| 1      | 0x0800_4000 – 0x0800_7FFF | 16 KB  | Bootloader             |
| 2      | 0x0800_8000 – 0x0800_BFFF | 16 KB  | Application            |
| 3      | 0x0800_C000 – 0x0800_FFFF | 16 KB  | Application            |
| 4      | 0x0801_0000 – 0x0801_FFFF | 64 KB  | Application            |
| 5–8    | 0x0802_0000 – 0x0807_FFFF | 64 KB × 4 | Application        |
| 9+     | 0x0808_0000 – ...         | 128 KB × N | Firmware Update Temp |

📝 Application firmware must be linked to start at `0x08008000`.

---

## 📊 Flowchart – Bootloader Operation

```text
                     +-----------------------+
                     |  Power On / Reset     |
                     +----------+------------+
                                |
                                v
                  +-----------------------------+
                  | Initialize UART / Peripherals|
                  +-----------------------------+
                                |
                                v
          +-------------------------------------------+
          | Wait for firmware data over UART (timeout)|
          +----------------+--------------------------+
                           |
            +--------------+---------------+
            |                              |
            v                              v
+------------------------+       +------------------------+
|  Data received         |       | Timeout occurred       |
|  -> Erase App sectors  |       | -> Jump to Application |
|  -> Write new firmware |       |    if valid            |
+------------------------+       +------------------------+
            |
            v
+----------------------------+
|  Verify and Jump to App   |
+----------------------------+
