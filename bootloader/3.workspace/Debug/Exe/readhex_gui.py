import tkinter as tk
from tkinter import filedialog, scrolledtext, messagebox
import threading
import serial
import os
import time

def calc_checksum(data_bytes):
    total = sum(data_bytes)
    return (0x100 - (total & 0xFF)) & 0xFF

def wait_for_ok(ser, log_func, timeout=20):
    start_time = time.time()
    buffer = b""
    while time.time() - start_time < timeout:
        while ser.in_waiting:
            byte = ser.read(1)
            buffer += byte
            log_func(f"MCU: {byte.hex(' ')} ({byte})")
            if b"ok" in buffer.lower():
                return True
            if len(buffer) > 10:
                buffer = buffer[-2:]
        time.sleep(0.01)
    return False

def parse_hex_file(filename):
    memory = {}
    base_addr = 0
    with open(filename, 'r') as f:
        for line in f:
            if not line.startswith(':'):
                continue
            byte_count = int(line[1:3], 16)
            address = int(line[3:7], 16)
            record_type = int(line[7:9], 16)
            data = line[9:9+byte_count*2]
            if record_type == 4:
                base_addr = int(data, 16) << 16
            elif record_type == 0:
                for i in range(byte_count):
                    mem_addr = base_addr + address + i
                    byte_val = int(data[i*2:i*2+2], 16)
                    memory[mem_addr] = byte_val
            elif record_type == 1:
                break
    return memory

class HexUploaderGUI:
    def __init__(self, master):
        self.master = master
        master.title("STM32 HEX Uploader")
        master.geometry("700x500")

        self.hex_file = tk.StringVar()
        self.com_port = tk.StringVar(value="COM10")
        self.baudrate = tk.IntVar(value=115200)

        tk.Label(master, text="HEX file:").grid(row=0, column=0, sticky="e")
        tk.Entry(master, textvariable=self.hex_file, width=50).grid(row=0, column=1, padx=5)
        tk.Button(master, text="Browse...", command=self.browse_file).grid(row=0, column=2)

        tk.Label(master, text="COM port:").grid(row=1, column=0, sticky="e")
        tk.Entry(master, textvariable=self.com_port, width=10).grid(row=1, column=1, sticky="w")
        tk.Label(master, text="Baudrate:").grid(row=1, column=1, sticky="e", padx=(0, 60))
        tk.Entry(master, textvariable=self.baudrate, width=10).grid(row=1, column=2, sticky="w")

        tk.Button(master, text="Start Upload", command=self.start_upload_thread).grid(row=2, column=1, pady=10)

        self.log = scrolledtext.ScrolledText(master, width=85, height=25, state='disabled')
        self.log.grid(row=3, column=0, columnspan=3, padx=5, pady=5)

    def browse_file(self):
        filename = filedialog.askopenfilename(filetypes=[("HEX files", "*.hex")])
        if filename:
            self.hex_file.set(filename)

    def log_message(self, msg):
        self.log.configure(state='normal')
        self.log.insert(tk.END, msg + "\n")
        self.log.see(tk.END)
        self.log.configure(state='disabled')

    def start_upload_thread(self):
        t = threading.Thread(target=self.upload_hex)
        t.daemon = True
        t.start()

    def upload_hex(self):
        hex_file = self.hex_file.get()
        com_port = self.com_port.get()
        baudrate = self.baudrate.get()

        if not os.path.isfile(hex_file):
            self.log_message("❌ HEX file not found.")
            return

        try:
            memory = parse_hex_file(hex_file)
        except Exception as e:
            self.log_message(f"❌ Error parsing HEX file: {e}")
            return

        all_addresses = sorted(memory.keys())
        total_bytes = len(all_addresses)
        self.log_message(f"== Tổng số byte cần gửi: {total_bytes}")

        try:
            with serial.Serial(com_port, baudrate, timeout=20) as ser:
                self.log_message(f"\n🚀 Mở {com_port} @ {baudrate} baud...")

                # Chờ người dùng nhấn Enter để bắt đầu gửi
                self.log_message("Nhấn nút 'Start Upload' để bắt đầu gửi dữ liệu tới MCU...")

                index = 0
                block_num = 1
                success = True

                if index < total_bytes:
                    ser.write(b'check')
                    self.log_message(f"\n🟡 Gửi 'check', chờ 'ok' từ MCU...")
                    if not wait_for_ok(ser, self.log_message, timeout=20):
                        self.log_message(f"❌ Không nhận được 'ok'. Timeout.")
                        success = False
                    else:
                        self.log_message(f"✅ MCU phản hồi 'ok'. Bắt đầu gửi dữ liệu...\n")

                while index < total_bytes and success:
                    chunk_addrs = all_addresses[index:index+16]
                    data_chunk = [memory[addr] for addr in chunk_addrs]
                    if len(data_chunk) < 16:
                        data_chunk += [0xFF] * (16 - len(data_chunk))
                    checksum = calc_checksum(data_chunk)
                    packet = bytes(data_chunk) + bytes([checksum, 0xAA])

                    self.log_message(f"DEBUG: Block {block_num} addr: 0x{chunk_addrs[0]:08X} | Data: {[f'{b:02X}' for b in data_chunk]} | Checksum: {checksum:02X}")

                    for i, b in enumerate(packet):
                        ser.write(bytes([b]))
                        ser.flush()
                        self.log_message(f"DEBUG: Sent byte {i}: 0x{b:02X}")
                        time.sleep(0.001)

                    sent_bytes = list(data_chunk) + [checksum, 0xAA]
                    hex_str = ' '.join(f'{b:02X}' for b in sent_bytes)
                    self.log_message(f"📤 [{block_num}] Gửi: {hex_str}  (tổng {len(sent_bytes)} byte) → chờ MCU xác nhận...")

                    self.log_message(f"DEBUG: Đang chờ 'ok' từ MCU cho block {block_num}...")

                    if not wait_for_ok(ser, self.log_message, timeout=20):
                        self.log_message(f"❌ Không nhận được 'ok' sau block {block_num}. Timeout.")
                        if ser.in_waiting:
                            data = ser.read(ser.in_waiting)
                            self.log_message(f"DEBUG: Dữ liệu nhận được: {data}")
                        success = False
                        break

                    self.log_message(f"✅ [{block_num}] MCU xác nhận OK.")

                    index += 16
                    block_num += 1

                if success:
                    self.log_message("\n✅ Transmission successfully completed.")
                    final_packet = bytes([0xAA] * 18)
                    ser.write(final_packet)
                    ser.flush()
                    self.log_message(f"Đã gửi {len(final_packet)} byte 0xAA kết thúc: {' '.join(f'{b:02X}' for b in final_packet)}")
                else:
                    self.log_message("\n❌ Transmission failed.")

                # Đọc tiếp dữ liệu MCU gửi lên (nếu có)
                self.log_message("Đang đọc dữ liệu MCU gửi lên (nếu có)...")
                time.sleep(0.5)
                while ser.in_waiting:
                    data = ser.read(ser.in_waiting)
                    self.log_message(f"MCU: {data.hex(' ')} ({data})")
                    time.sleep(0.1)

        except serial.SerialException as e:
            self.log_message(f"❌ Lỗi mở cổng COM: {e}")
        except Exception as e:
            self.log_message(f"❌ Lỗi không xác định: {e}")

if __name__ == '__main__':
    root = tk.Tk()
    app = HexUploaderGUI(root)
    root.mainloop()
