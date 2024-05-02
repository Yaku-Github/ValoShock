import serial
import threading
import time
from PIL import ImageGrab

# Координаты и цветовые значения для проверки
deadx, deady = 1696, 419
alivex, alivey = 588, 1022
deadcolor = (255, 255, 255)
alivecolor = (255, 255, 255)

# Флаг для отслеживания состояния
activated = False

# Функция для считывания данных с последовательного порта
def read_serial(serialPort):
    while True:
        try:
            serialString = serialPort.readline()
            if serialString:
                print("Serial:", serialString.decode("Ascii"))
        except Exception as e:
            print("Error reading from serial:", e)

# Функция для проверки состояния пикселей
def read_input(serialPort):
    global activated  # Используем global для изменения переменной
    # Определим область захвата экрана
    bbox = (min(deadx, alivex), min(deady, alivey), max(deadx, alivex) + 1, max(deady, alivey) + 1)

    while True:
        screenshot = ImageGrab.grab(bbox=bbox)  # Захватываем только нужную область

        # Определяем смещения относительно области захвата
        actualdeadcolor = screenshot.getpixel((deadx - bbox[0], deady - bbox[1]))
        actualalivecolor = screenshot.getpixel((alivex - bbox[0], alivey - bbox[1]))

        if actualdeadcolor == deadcolor and not activated:
            serialPort.write(b"TRIGGER")
            print("Command sent: TRIGGER")
            activated = True  # Устанавливаем флаг в True

        elif actualalivecolor == alivecolor and activated:
            activated = False  # Сбрасываем флаг

        # Добавляем задержку для уменьшения нагрузки на CPU
        time.sleep(0.1)  # 100 мс

# Инициализация последовательного порта
serialPort = serial.Serial(
    port="COM6", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE
)
serialPort.write(b"TT 100\r\n")
serialPort.write(b"TP 80\r\n")

# Создание потоков
thread_serial = threading.Thread(target=read_serial, args=(serialPort,))
thread_input = threading.Thread(target=read_input, args=(serialPort,))

# Запуск потоков
thread_serial.start()
thread_input.start()

# Ожидание завершения потоков
thread_input.join()
