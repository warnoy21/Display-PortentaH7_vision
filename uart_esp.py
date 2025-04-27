import sensor, image, time,pyb
from pyb import UART
uart = UART(1, 115200, timeout_char=1000)    # or machine.UART('P4',115200) on Portenta
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)       # or RGB565 for color
sensor.set_framesize(sensor.QVGA)            # 320×240
sensor.skip_frames(time=2000)
while True:
    img = sensor.snapshot()
    jpg = img.compress(quality=10)          # JPEG quality (0–100)
    uart.write(jpg)                         # send raw JPEG bytes
    pyb.delay(500)
