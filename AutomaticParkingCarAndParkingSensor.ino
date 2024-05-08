#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define echoPinFront 3
#define trigPinFront 2
#define buzzerPinFront 8
#define echoPinRear 7
#define trigPinRear 6
#define buzzerPinRear 9
#define ledPin 11 // LED pin numarası

#define I2C_ADDR    0x27 // I2C adresi, LCD ekranın adresine göre değiştirilebilir

int maximumRange = 20;
int minimumRange = 0;
int bluetoothState = 0; // Bluetooth komutunu saklamak için değişken

LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2); // 16x2 LCD ekran için

void setup() {
  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(buzzerPinFront, OUTPUT);
  pinMode(trigPinRear, OUTPUT);
  pinMode(echoPinRear, INPUT);
  pinMode(buzzerPinRear, OUTPUT);
  pinMode(ledPin, OUTPUT); // LED pinini çıkış olarak ayarla

  Serial.begin(9600); // Seri iletişimi 9600 Baud hızında başlat

  lcd.init();  // LCD ekranı başlat
  lcd.backlight();  // Arkadan aydınlatmayı aç
}

void loop() {
  if (Serial.available() > 0) {
    bluetoothState = Serial.read(); // Bluetooth komutunu oku
  }

  if (bluetoothState == '0') {
    int measurementRear = distance(trigPinRear, echoPinRear);
    melody(buzzerPinRear, measurementRear * 10);
    displayDistance(distance(trigPinFront, echoPinFront), measurementRear);
    blinkLED(buzzerPinRear, measurementRear * 10); // LED'i yanıp söndürme fonksiyonunu çağır
  } else if (bluetoothState == '1') {
    int measurementFront = distance(trigPinFront, echoPinFront);
    melody(buzzerPinFront, measurementFront * 10);
    displayDistance(measurementFront, distance(trigPinRear, echoPinRear));
    blinkLED(buzzerPinFront, measurementFront * 10); // LED'i yanıp söndürme fonksiyonunu çağır
  }
}

int distance(int trigPin, int echoPin) {
  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  delay(50);

  if (distance >= maximumRange || distance <= minimumRange)
    return 0;
  return distance;
}

void melody(int buzzerPin, int dly) {
  tone(buzzerPin, 400);
  delay(dly);
  noTone(buzzerPin);
  delay(dly);
}

void blinkLED(int buzzerPin, int dly) {
  digitalWrite(ledPin, HIGH); // LED'i yak
  delay(dly);
  digitalWrite(ledPin, LOW); // LED'i söndür
  delay(dly);
}

void displayDistance(int frontDistance, int rearDistance) {
  lcd.clear(); // Ekranı temizle
  lcd.setCursor(0, 0);
  if (bluetoothState == '0') {
    lcd.print("Front : -");
  } else if (bluetoothState == '1') {
    lcd.print("Front : ");
    lcd.print(frontDistance);
    lcd.print(" cm");
  }

  lcd.setCursor(0, 1);
  if (bluetoothState == '0') {
    lcd.print("Rear  : ");
    lcd.print(rearDistance);
    lcd.print(" cm");
  } else if (bluetoothState == '1') {
    lcd.print("Rear  : -");
  }
}
