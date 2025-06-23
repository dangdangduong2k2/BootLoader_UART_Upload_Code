import serial
import os
import time
import threading
import tkinter as tk
from tkinter import filedialog, scrolledtext, messagebox

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
            # Hiển thị dữ liệu nhận được
            log_func(f"[MCU] {byte.hex(' ')} | {byte.decode(errors='replace')}")
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
        self.hex_file = ""
        self.com_port = tk.StringVar(value="COM10")
        self.baudrate = tk.StringVar(value="115200")
        self.is_sending = False

        # File selection
        tk.Label(master, text="HEX File:").grid(row=0, column=0, sticky="e")
        self.file_entry = tk.Entry(master, width=40)
        self.file_entry.grid(row=0, column=1, padx=2)
        tk.Button(master, text="Browse", command=self.browse_file).grid(row=0, column=2)

        # COM port
        tk.Label(master, text="COM Port:").grid(row=1, column=0, sticky="e")
        tk.Entry(master, textvariable=self.com_port, width=10).grid(row=1, column=1, sticky="w")
        tk.Label(master, text="Baudrate:").grid(row=1, column=1, sticky="e", padx=(70,0))
        tk.Entry(master, textvariable=self.baudrate, width=10).grid(row=1, column=2, sticky="w")

        # Send button
        self.send_btn = tk.Button(master, text="Send", command=self.start_send_thread)
        self.send_btn.grid(row=2, column=0, columnspan=3, pady=5)

        # Log area
        self.log_area = scrolledtext.ScrolledText(master, width=80, height=25, state='disabled', font=("Consolas", 10))
        self.log_area.grid(row=3, column=0, columnspan=3, padx=5, pady=5)

    def browse_file(self):
        file = filedialog.askopenfilename(filetypes=[("HEX files", "*.hex")])
        if file:
            self.hex_file = file
            self.file_entry.delete(0, tk.END)
            self.file_entry.insert(0, file)

    def log(self, msg):
        self.log_area.config(state='normal')
        self.log_area.insert(tk.END, msg + "\n")
        self.log_area.see(tk.END)
        self.log_area.config(state='disabled')
        self.master.update_idletasks()

    def start_send_thread(self):
        if self.is_sending:
            return
        self.is_sending = True
        self.send_btn.config(state='disabled')
        t = threading.Thread(target=self.send_hex)
        t.daemon = True
        t.start()

    def send_hex(self):
        try:
            hex_file = self.file_entry.get()
            if not os.path.isfile(hex_file):
                self.log("❌ HEX file không hợp lệ.")
                return
            com_port = self.com_port.get()
            baudrate = int(self.baudrate.get())
            memory = parse_hex_file(hex_file)
            all_addresses = sorted(memory.keys())
            total_bytes = len(all_addresses)
            self.log(f"== Tổng số byte cần gửi: {total_bytes}")

            with serial.Serial(com_port, baudrate, timeout=20) as ser:
                self.log(f"\n🚀 Mở {com_port} @ {baudrate} baud...")

                index = 0
                block_num = 1
                success = True

                if index < total_bytes:
                    ser.write(b'check')
                    self.log(f"\n🟡 Gửi 'check', chờ 'ok' từ MCU...")
                    if not wait_for_ok(ser, self.log, timeout=20):
                        self.log(f"❌ Không nhận được 'ok'. Timeout.")
                        success = False
                    else:
                        self.log(f"✅ MCU phản hồi 'ok'. Bắt đầu gửi dữ liệu...\n")

                while index < total_bytes and success:
                    chunk_addrs = all_addresses[index:index+16]
                    data_chunk = [memory[addr] for addr in chunk_addrs]
                    if len(data_chunk) < 16:
                        data_chunk += [0xFF] * (16 - len(data_chunk))
                    checksum = calc_checksum(data_chunk)
                    packet = bytes(data_chunk) + bytes([checksum, 0xAA])

                    # self.log(f"DEBUG: Block {block_num} addr: 0x{chunk_addrs[0]:08X} | Data: {[f'{b:02X}' for b in data_chunk]} | Checksum: {checksum:02X} | Packet: {[f'{b:02X}' for b in packet]}")

                    # Gửi cả block một lần, không log từng byte, không flush từng lần
                    ser.write(packet)
                    # Không cần ser.flush() ở đây để tăng tốc

                    # self.log(f"DEBUG: Sent {len(packet)} bytes for block {block_num}")

                    # Gom log lại, chỉ log thông tin chính
                    sent_bytes = list(data_chunk) + [checksum, 0xAA]
                    hex_str = ' '.join(f'{b:02X}' for b in sent_bytes)
                    self.log(f"[{block_num}] Gửi: {hex_str} → chờ MCU xác nhận...")

                    # self.log(f"DEBUG: Đang chờ 'ok' từ MCU cho block {block_num}...")

                    if not wait_for_ok(ser, self.log, timeout=20):
                        self.log(f"❌ Không nhận được 'ok' sau block {block_num}. Timeout.")
                        if ser.in_waiting:
                            data = ser.read(ser.in_waiting)
                            self.log(f"DEBUG: Dữ liệu nhận được: {data}")
                        success = False
                        break

                    self.log(f"✅ [{block_num}] MCU xác nhận OK.")

                    index += 16
                    block_num += 1

                if success:
                    self.log("\n✅ Transmission successfully completed.")
                    final_packet = bytes([0xAA] * 18)
                    ser.write(final_packet)
                    # Không cần ser.flush() ở đây để tăng tốc
                    self.log(f"Đã gửi {len(final_packet)} byte 0xAA kết thúc: {' '.join(f'{b:02X}' for b in final_packet)}")
                else:
                    self.log("\n❌ Transmission failed.")

                # Đọc dữ liệu còn lại từ MCU (nếu có)
                time.sleep(0.1)
                while ser.in_waiting:
                    data = ser.read(ser.in_waiting)
                    if data:
                        self.log(f"[MCU] {data.hex(' ')} | {data.decode(errors='replace')}")

        except serial.SerialException as e:
            self.log(f"❌ Lỗi mở cổng COM: {e}")
        except Exception as e:
            self.log(f"❌ Lỗi không xác định: {e}")
        finally:
            self.is_sending = False
            self.send_btn.config(state='normal')

if __name__ == '__main__':
    root = tk.Tk()
    app = HexUploaderGUI(root)
    root.mainloop()
