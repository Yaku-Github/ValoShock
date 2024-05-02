// Назначение пина
const int triggerPin = 6;

// Время по умолчанию в миллисекундах
int triggerDuration = 1;
int triggerPower = 0;
void setup() {
  // Инициализация серийного порта
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  establishContact();
  Serial.println("Connected");
  // Установка режима работы пина
  pinMode(triggerPin, OUTPUT);
  analogWrite(triggerPin, 0);  // Изначально низкий уровень
}

void loop() {
  // Проверка, есть ли данные в серийном порту
  if (Serial.available() > 0) {
    // Считываем строку
    String command = Serial.readStringUntil('\n');  // Считываем до конца строки

    // Если команда - "TRIGGER", даем положительный сигнал на заданное время
    if (command.equals("TRIGGER")) {
      Serial.println("Triggering Shock");
      analogWrite(triggerPin, triggerPower);  // Поднимаем сигнал
      delay(triggerDuration);  // Ждем заданное время
      analogWrite(triggerPin, 0);  // Опускаем сигнал
    }

    // Если команда начинается с "TP", значит, меняем мощность сигнала
    if (command.startsWith("TP ")) {
      // Извлекаем мощность из строки
      String powStr = command.substring(3);  // Берем все после "TT "
      int newPower = powStr.toInt();  // Преобразуем в целое число
      
      if (newPower > 0 and newPower <= 255) {  // Если значение валидное
        triggerPower = newPower;  // Обновляем мощность активации пина
        Serial.print("Trigger power updated to: ");
        Serial.print(triggerPower);
      } else {
        Serial.println("Invalid power value.");
      }
    }
    
    // Если команда начинается с "TT", значит, меняем время сигнала
    if (command.startsWith("TT ")) {
      // Извлекаем время из строки
      String timeStr = command.substring(3);  // Берем все после "TT "
      int newDuration = timeStr.toInt();  // Преобразуем в целое число
      
      if (newDuration > 0) {  // Если значение валидное
        triggerDuration = newDuration;  // Обновляем время активации пина
        Serial.print("Trigger duration updated to: ");
        Serial.print(triggerDuration);
        Serial.println(" ms");
      } else {
        Serial.println("Invalid time value.");
      }
    }
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("Please approve connection by sending a byte");
    delay(1000);
  }
}
