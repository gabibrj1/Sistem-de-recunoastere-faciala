#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Obiecte pentru servomotor și LCD
Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adresa LCD-ului I2C (adaptați 0x27 dacă diferă)

// Variabile globale
bool isAccessGranted = false; // Flag pentru controlul accesului

// Pini hardware
const int ledGreen = 12; // LED verde pentru acces permis
const int ledRed = 11;   // LED roșu pentru acces respins
const int potentiometerPin = A0; // Pin pentru potențiometru

void setup() {
  Serial.begin(115200);  // Inițializare conexiune serială
  myservo.attach(9);     // Pinul de semnal pentru servomotor
  myservo.write(0);      // Poziția inițială a servomotorului

  pinMode(ledGreen, OUTPUT); // Setare pin LED verde ca ieșire
  pinMode(ledRed, OUTPUT);   // Setare pin LED roșu ca ieșire

  digitalWrite(ledGreen, LOW); // LED-urile sunt inițial stinse
  digitalWrite(ledRed, LOW);

  // Inițializare LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Sistem pornit"); // Mesaj inițial
  delay(2000);               // Pauză pentru afișaj
  lcd.clear();

  Serial.println("Sistem Arduino pornit. Aștept mesaje...");
}

void loop() {
  // Verificăm dacă există mesaje primite
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n'); // Citire mesaj complet până la '\n'

    if (message == "PERMIS") {
      Serial.println("Acces permis: Pornesc servomotorul.");
      isAccessGranted = true;
      lcd.clear();
      lcd.print("Acces permis"); // Mesaj pe LCD
    } else if (message == "RESPINS") {
      Serial.println("Acces respins: Oprire servomotor.");
      isAccessGranted = false;
      myservo.write(0);             // Revine la poziția inițială
      lcd.clear();
      lcd.print("Acces respins");   // Mesaj pe LCD
    } else {
      Serial.println("Mesaj necunoscut primit.");
      lcd.clear();
      lcd.print("Mesaj necunoscut");
    }
  }

  // Controlul servomotorului pe baza flag-ului
  if (isAccessGranted) {
    static int pos = 0;             // Poziția curentă a servomotorului
    static bool direction = true;  // Direcția de mișcare

    // Controlează mișcarea continuă înainte și înapoi
    if (direction) {
      pos += 1; // Crește poziția
      if (pos >= 180) direction = false; // Inversează direcția la 180°
    } else {
      pos -= 1; // Scade poziția
      if (pos <= 0) direction = true; // Inversează direcția la 0°
    }

    myservo.write(pos); // Setează poziția servomotorului
    delay(15);          // Întârziere pentru o mișcare lină
  }

  // Citire valoare de la potențiometru
  int potValue = analogRead(potentiometerPin); // Citire valoare între 0 și 1023
  int ledIntensity = map(potValue, 0, 1023, 0, 255); // Mapare la valoare PWM (0-255)

  // Reglare intensitate LED-uri
  if (isAccessGranted) {
    analogWrite(ledGreen, ledIntensity); // Control intensitate LED verde
    analogWrite(ledRed, 0);             // LED roșu stins
  } else {
    analogWrite(ledRed, ledIntensity);  // Control intensitate LED roșu
    analogWrite(ledGreen, 0);           // LED verde stins
  }
}
