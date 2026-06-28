import struct
import sys
import time
import wave

import serial

# ── config ──────────────────────────────────────────────

PORT = "/dev/ttyUSB0"  # Windows: "COM3", etc.

BAUD = 921600

SAMPLE_RATE = 8000

DURATION = 10  # seconds to record

OUTPUT_FILE = "recording.wav"

# ────────────────────────────────────────────────────────


RECORD_MARKER_END = 0xCAFEBABE


def save_wav(samples, filename, sample_rate):

    with wave.open(filename, "w") as wf:
        wf.setnchannels(1)  # mono

        wf.setsampwidth(2)  # 16-bit

        wf.setframerate(sample_rate)

        # ESP32 I2S gives 32-bit samples, top 16 bits are the actual data

        pcm16 = [s >> 16 for s in samples]

        wf.writeframes(struct.pack(f"<{len(pcm16)}h", *pcm16))

    print(f"Saved {len(pcm16)} samples to {filename}")


def record(port, baud, duration, output_file):

    print(f"Connecting to {port} at {baud} baud...")

    ser = serial.Serial(port, baud, timeout=2)

    time.sleep(2)  # wait for ESP32 to boot

    ser.reset_input_buffer()

    print(f"Recording for {duration} seconds... (Ctrl+C to stop early)")

    ser.write(b"R")  # start recording

    raw_bytes = bytearray()

    expected_bytes = SAMPLE_RATE * 4 * duration  # 4 bytes per sample

    start = time.time()

    try:
        while len(raw_bytes) < expected_bytes:
            chunk = ser.read(min(4096, expected_bytes - len(raw_bytes)))

            if chunk:
                raw_bytes.extend(chunk)

            elapsed = time.time() - start

            done = len(raw_bytes) / expected_bytes * 100

            print(
                f"\r  {elapsed:.1f}s  |  {done:.1f}%  |  {len(raw_bytes)} bytes",
                end="",
                flush=True,
            )

    except KeyboardInterrupt:
        print("\nStopped early.")

    ser.write(b"S")  # stop recording

    print(f"\nDone. Received {len(raw_bytes)} bytes.")

    ser.close()

    # parse as int32 samples

    n_samples = len(raw_bytes) // 4

    samples = struct.unpack(f"<{n_samples}i", raw_bytes[: n_samples * 4])

    save_wav(samples, output_file, SAMPLE_RATE)


if __name__ == "__main__":
    port = sys.argv[1] if len(sys.argv) > 1 else PORT

    record(port, BAUD, DURATION, OUTPUT_FILE)
