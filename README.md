# Sistem de Acces Controlat prin Recunoaștere Facială

Acest proiect implementează un sistem inteligent de acces bazat pe recunoașterea facială, utilizând o combinație de hardware (Arduino Uno, ESP32-CAM) și software (Python și Arduino IDE). Sistemul controlează accesul prin deschiderea unei uși sau a unui sertar, oferind feedback vizual și auditiv utilizatorului.

---
## 🗂️ Documentația Proiectului

Documentația proiectului este împărțită în două părți pentru a acoperi toate aspectele dezvoltării:

1. **Prima parte**: 
   - Concepută ca o lucrare de laborator, această parte explică detaliat fiecare implementare.
   - Include exemple și exerciții pentru înțelegerea conceptelor.
   - Se concentrează pe o versiune simplificată a proiectului, utilizând:
     - Programarea pe registrii doar la nivel de timer pentru motor.
     - LED-uri pentru feedback vizual (verde și roșu).
     - LCD cu I2C, motor servo, ESP32-CAM, Arduino și potențiometru.
   - Scopul acestei părți este educațional, oferind o bază solidă pentru înțelegerea elementelor de bază.

2. **A doua parte**:
   - Este o documentație clasică de proiect, care detaliază o implementare mai complexă.
   - Adaugă funcționalități avansate prin programarea pe registrii pentru întregul script Arduino.
   - Integrează componente suplimentare pentru îmbunătățirea sistemului:
     - Buton pentru oprirea motorului.
     - Senzor de lumină TEMT6000 pentru optimizarea capturii ESP32-CAM.
     - Senzor Hall pentru detectarea obstacolelor.
     - Senzor PIR pentru detectarea mișcării.
     - Mai multe LED-uri pentru indicarea stării senzorilor.

Documentația completă se află în fișierele dedicate din directorul proiectului:
- **Sistem de Recunoaștere Facială - Partea 1**: Lucrare de laborator detaliată.
- **Sistem de Recunoaștere Facială - Partea 2**: Documentație clasică de proiect.

Aceste fișiere oferă informații detaliate despre implementare și utilizare.
---

## 🛠️ Componente Hardware

1. **Arduino Uno**: Microcontroler principal care gestionează perifericele.
2. **ESP32-CAM**: Modul pentru capturarea imaginilor faciale.
3. **Motor Servo MG996R**: Controlează mecanismul fizic de deschidere.
4. **LCD 16x2 cu I2C**: Afișează mesaje pentru utilizator.
5. **LED-uri**:
   - Verde: Acces permis.
   - Roșu: Acces respins.
6. **Potențiometru**: Ajustează intensitatea luminozității LED-urilor.
7. **Senzor PIR**: Detectează prezența unei persoane în apropiere.
8. **Senzor magnetic Hall**: Detectează obstacolele în apropierea motorului servo.
9. **Senzor de lumină**: Îmbunătățește condițiile de captură pentru ESP32-CAM.
10. **Buton**: Permite oprirea motorului în timpul funcționării, dacă accesul este permis.

---

## ⚙️ Funcționalitate

1. **Capturarea imaginilor**: 
   - ESP32-CAM capturează imaginea feței utilizatorului și o transmite către un script Python pentru procesare.
   
2. **Procesarea imaginilor**:
   - Python utilizează librăria `face_recognition` pentru a compara imaginea cu o bază de date existentă.
   
3. **Controlul accesului**:
   - Dacă fața este recunoscută:
     - LED-ul verde se aprinde.
     - Mesajul "Acces permis" este afișat pe LCD.
     - Motorul servo deschide mecanismul, generând feedback auditiv prin sunetul emis în timpul mișcării.
   - Dacă accesul este refuzat:
     - LED-ul roșu se aprinde.
     - Mesajul "Acces respins" este afișat pe LCD.

4. **Feedback suplimentar**:
   - **Senzorul Hall**: Detectează obstacole și oprește motorul, afișând un mesaj corespunzător pe LCD.
   - **Senzorul PIR**: Detectează mișcare și aprinde un LED asociat.
   - **Senzorul de lumină**: Optimizează condițiile de captură prin ajustarea luminii ambientale.
   - **Butonul**: Dacă accesul este permis, utilizatorul poate opri motorul apăsând butonul dedicat.

---

## 🧰 Instalare și Configurare

1. **Configurare Hardware**:
   - Conectați componentele conform schemei de conexiuni.
   - Verificați ca toate senzorii și actuatoarele să fie conectați corect.

2. **Setări ESP32-CAM**:
   - Configurați rețeaua WiFi în codul ESP32-CAM:
     ```cpp
     const char* ssid = "nume_retea";
     const char* password = "parola_retea";
     ```

3. **Python**:
   - Instalați dependințele:
     ```bash
     pip install face_recognition opencv-python requests pyserial
     ```
   - Configurați calea imaginilor în `config.json`:
     ```json
     {
       "image_path_gabi": "C:/path/to/face1.jpg",
       "image_path_rares": "C:/path/to/face2.jpg"
     }
     ```

4. **Arduino**:
   - Încărcați codurile necesare pe placa Arduino utilizând Arduino IDE.

---

## 🚀 Utilizare

1. Porniți sistemul.
2. ESP32-CAM va începe să captureze imagini.
3. Sistemul va analiza imaginea și va decide:
   - **Acces permis**:
     - LED-ul verde se aprinde.
     - Motorul deschide mecanismul.
     - Sunetul emis de motor oferă feedback auditiv.
   - **Acces respins**:
     - LED-ul roșu se aprinde.
     - Mesajul corespunzător este afișat pe LCD.

4. Apăsați butonul pentru a opri motorul în timpul funcționării, dacă accesul este permis.

---


Pentru orice întrebări sau sugestii, contactați-ne la: **g.brujbeanu18@gmail.com**; **ionescurares414@gmail.com**;
