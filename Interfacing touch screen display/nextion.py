import Adafruit_BBIO.UART as UART
        UART.setup("UART1")
        Import serial 
serialobj= serial.Serial(port = "/dev/ttyO1",  baudrate=9600, timeout=0.3)
While(1):
print(serialobj.read(10))
