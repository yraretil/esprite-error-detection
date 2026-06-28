import asyncio, serial, websockets, serial.tools.list_ports

PORT = "/dev/ttyUSB0"   # change to your port (Windows: "COM3" etc)
BAUD = 115200

async def handler(ws):
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"Connected to {PORT}")
    try:
        while True:
            line = ser.readline().decode(errors="ignore").strip()
            if line:
                await ws.send(line)
            await asyncio.sleep(0)
    except websockets.exceptions.ConnectionClosed:
        pass
    finally:
        ser.close()

async def main():
    async with websockets.serve(handler, "localhost", 8765):
        print("WS server at ws://localhost:8765 — open plotter.html")
        await asyncio.Future()

asyncio.run(main())
