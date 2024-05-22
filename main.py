import asyncio
import threading
from bleak import BleakClient
from bleak import BleakScanner
import struct
import socket

from RealtimeSTT import AudioToTextRecorder

# Bluetooth Device config
accelServiceUuid = "2BEEF31A-B10D-271C-C9EA-35D865C1F48A"
accCharUuid = "4664E7A1-5A13-BFFF-4636-7D0A4B16496C"
device_name = "IMU"

pitch_roll_yaw = []

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = socket.gethostname()
port = 25567
s.connect((host, port))


def parseFloatsData(data):
    floats = []
    for i in range(0, len(data), 4):
        float_data = data[i:i + 4]
        float_value = struct.unpack('f', float_data)[0]
        floats.append(float_value)
    return floats


async def connect(address):
    client = BleakClient(address, timeout=60.0)
    await client.connect()


async def read_pitch_roll_yaw(client):
    val = await client.read_gatt_char(accCharUuid)  # gatt per 60ms
    pitch_roll_yaw = parseFloatsData(val)
    return pitch_roll_yaw


async def run():
    print('Arduino Nano RP2040 Connect Peripheral Central Service')
    print('Looking for Arduino Nano RP2040 Connect Peripheral Device...')

    found = False
    devices = await BleakScanner.discover()
    for d in devices:
        if d.name is not None and device_name in d.name:
            print('Found Arduino Nano RP2040 Connect Peripheral')
            found = True
            async with BleakClient(d.address) as client:
                print(f'Connected to {d.address}')
                while True:
                    pitch_roll_yaw = await read_pitch_roll_yaw(client)
                    # print(pitch_roll_yaw)
                    pry_str = ' '.join(map(str, pitch_roll_yaw))
                    s.send(pry_str.encode('utf-8'))

    if not found:
        print('Could not find Arduino Nano RP2040 Connect Peripheral')


def process_text(text):
    print(text)
    if 'Start' in text:
        s.send('voice:1'.encode('utf-8'))
    elif 'Stop' in text:
        s.send('voice:0'.encode('utf-8'))


async def run_recorder():
    with AudioToTextRecorder() as recorder:
        while True:
            recorder.text(process_text)


def run_recorder_in_new_loop():

    loop2 = asyncio.new_event_loop()
    asyncio.set_event_loop(loop2)
    loop2.run_until_complete(run_recorder())

if __name__ == '__main__':
    try:
        recorder_thread = threading.Thread(target=run_recorder_in_new_loop)
        recorder_thread.start()

        loop = asyncio.get_event_loop()
        loop.run_until_complete(run())
    except KeyboardInterrupt:
        print('\nReceived Keyboard Interrupt')
    finally:
        print('Program finished')
        s.close()
