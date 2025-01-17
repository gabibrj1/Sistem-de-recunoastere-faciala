#define LCD_ADDR 0x27 // Adresa I2C a LCD-ului, configurabila pe hardware prin jumperi
#define LCD_CLEARDISPLAY 0x01 // Comanda pentru curatarea ecranului LCD
#define LCD_RETURNHOME 0x02 // Comanda pentru repozitionarea cursorului la pozitia initiala
#define LCD_ENTRYMODESET 0x04 // Seteaza modul de scriere a datelor, adica directia
#define LCD_DISPLAYCONTROL 0x08 // Controleaza afisajul (pornire/oprire, cursor)
#define LCD_CURSORSHIFT 0x10 // Controleaza mutarea cursorului sau scroll-ul
#define LCD_FUNCTIONSET 0x20 // Seteaza modul de operare al LCD-ului (biti, linii, matrice)
#define LCD_SETCGRAMADDR 0x40 // Seteaza adresa pentru memorie CGRAM (caractere custom)
#define LCD_SETDDRAMADDR 0x80 // Seteaza adresa pentru memorie DDRAM (afisaj normal)

// Optiuni pentru modul de intrare al LCD-ului
#define LCD_ENTRYRIGHT 0x00 // Textul este scris de la dreapta la stanga
#define LCD_ENTRYLEFT 0x02 // Textul este scris de la stanga la dreapta (mod implicit)
#define LCD_ENTRYSHIFTINCREMENT 0x01 // Seteaza cursorul sa se mute automat la urmatoarea pozitie
#define LCD_ENTRYSHIFTDECREMENT 0x00 // Cursorul ramane pe aceeasi pozitie dupa scriere

// Optiuni pentru controlul afisajului LCD
#define LCD_DISPLAYON 0x04 // Porneste afisajul
#define LCD_DISPLAYOFF 0x00 // Opreste afisajul
#define LCD_CURSORON 0x02 // Porneste cursorul vizibil
#define LCD_CURSOROFF 0x00 // Opreste cursorul vizibil
#define LCD_BLINKON 0x01 // Cursorul clipeste
#define LCD_BLINKOFF 0x00 // Cursorul nu clipeste

// Optiuni pentru modul de operare al LCD-ului
#define LCD_8BITMODE 0x10 // Modul 8-biti
#define LCD_4BITMODE 0x00 // Modul 4-biti, utilizat in proiect
#define LCD_2LINE 0x08 // Doua linii active
#define LCD_1LINE 0x00 // O singura linie activa
#define LCD_5x10DOTS 0x04 // Matrice caractere 5x10
#define LCD_5x8DOTS 0x00 // Matrice caractere 5x8, utilizat in proiect

// Controlul backlight-ului
#define LCD_BACKLIGHT 0x08 // Porneste backlight-ul
#define LCD_NOBACKLIGHT 0x00 // Opreste backlight-ul

// Controlul pinilor LCD
#define En 0x04 // Enable bit - activeaza semnalele pentru scriere
#define Rw 0x02 // Read/Write bit - selecteaza scrierea sau citirea
#define Rs 0x01 // Register Select bit - selecteaza datele sau comanda

// Variabile globale pentru controlul afisajului si sistemului
bool isAccessGranted = false; // Variabila care indica daca accesul este permis
bool isSystemOff = false; // Variabila care indica daca sistemul este oprit
uint8_t _displayfunction; // Configuratia functiilor afisajului
uint8_t _displaycontrol; // Starea actuala a controlului afisajului
uint8_t _displaymode; // Modul curent de scriere pe LCD
uint8_t _backlightval; // Starea actuala a backlight-ului

// Pini hardware utilizati in sistem
const int ledGreen = 12; // LED verde pentru indicarea accesului permis
const int ledRed = 11; // LED rosu pentru acces respins
const int potentiometerPin = A0; // Pinul potentiometrului pentru ajustari
const int buttonPin = 3; // Pinul butonului pentru oprirea sistemului
const int pirPin = 7; // Pinul senzorului PIR pentru detectie de miscare
const int pirLed = 6; // LED asociat senzorului PIR
#define light A1 // Pinul senzorului de lumina
#define ledPin 10 // Pinul pentru LED-ul care sesizeaza lumina slaba
const int hallSensorPin = 8; // Pinul pentru senzorul Hall

// Initializarea magistralei I2C
void i2c_init() {
    // TWSR (TWI Status Register): Seteaza prescaler-ul la 1
    // TWSR contine bitii TWPS1 si TWPS0 pentru a seta prescaler-ul folosit in calculul frecventei I2C
    // Valoarea 0 pentru TWPS1 si TWPS0 (TWSR = 0) corespunde unui prescaler de 1
    TWSR = 0;
    
    // Formula pentru calculul frecventei de ceas a magistralei I2C:
    // SCL_freq = F_CPU / (16 + 2 * TWBR * prescaler)
    // TWBR (TWI Bit Rate Register) seteaza rata de transfer a bitilor
    // Pentru un ceas de 16 MHz si o frecventa dorita de 100 kHz, TWBR este calculat astfel:
    TWBR = ((F_CPU / 100000L) - 16) / 2;

    // TWCR (TWI Control Register): Activeaza interfata TWI (Two-Wire Interface)
    // Bitul TWEN (TWI Enable) este setat pentru a activa hardware-ul I2C
    
    TWCR = (1 << TWEN);
}


// Trimite un semnal START pe magistrala I2C
void i2c_start() {
    // TWCR: Seteaza bitul TWSTA pentru a genera o conditie START
    // TWINT: Este setat pentru a incepe operatia si va fi resetat automat cand operatia este completata
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // Asteapta pana cand TWINT este resetat automat, semnalizand finalizarea operatiei START
    while (!(TWCR & (1 << TWINT)));
}
// Trimite un semnal STOP pe magistrala I2C
void i2c_stop() {
    // TWCR: Seteaza bitul TWSTO pentru a genera o conditie STOP
    // TWEN: Ramane activat pentru a permite operarea interfetei TWI
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

    // Asteapta pana cand TWSTO este resetat, indicand finalizarea operatiei STOP
    while (TWCR & (1 << TWSTO));
}

// Scrie un byte pe magistrala I2C
void i2c_write(uint8_t data) {
    // TWDR (TWI Data Register): Incarca datele ce urmeaza sa fie transmise
    TWDR = data;

    // TWCR: Initiaza transmisia setand bitul TWINT si activand interfata TWI
    TWCR = (1 << TWINT) | (1 << TWEN);

    // Asteapta pana cand TWINT este resetat, indicand finalizarea transmisiei
    while (!(TWCR & (1 << TWINT)));
}

// Trimite un byte catre un dispozitiv I2C specificat prin adresa
void i2c_send_byte(uint8_t addr, uint8_t data) {
    // Trimite semnalul START pentru a incepe transmisia
    i2c_start();

    // Trimite adresa dispozitivului cu bitul de scriere (LSB = 0)
    i2c_write(addr << 1);

    // Trimite byte-ul de date catre dispozitiv
    i2c_write(data);

    // Trimite semnalul STOP pentru a incheia transmisia
    i2c_stop();
}

// Functie pentru scrierea pe expanderul I2C al LCD-ului
void expanderWrite(uint8_t data) {
    // Trimite datele catre adresa I2C a expanderului, combinand datele cu valoarea curenta a backlight-ului
    i2c_send_byte(LCD_ADDR, data | _backlightval);
}

// Functie pentru generarea unui puls Enable pentru LCD
void pulseEnable(uint8_t data) {
    // Activeaza bitul Enable al expanderului pentru a incepe scrierea datelor pe LCD
    expanderWrite(data | En);
    delayMicroseconds(1);
    // dezactiveaza bitul enable al expanderului pentru a finaliza operatia
    expanderWrite(data & ~En);
    delayMicroseconds(50);
}

// Scrie pe 4 biti in LCD (folosind expanderul I2C
void write4bits(uint8_t value) {
    // Trimite nibble-ul catre expander
    expanderWrite(value);

    // Genereaza un puls Enable pentru a valida datele trimise
    pulseEnable(value);
}

void send(uint8_t value, uint8_t mode) {
    // Extrage nibble-ul superior al datelor
    uint8_t highnib = value & 0xF0;
    
    // Extrage nibble-ul inferior al datelor
    uint8_t lownib = (value << 4) & 0xF0;
    
    // Scrie nibble-ul superior si inferior catre LCD, utilizand modul specificat
    write4bits(highnib | mode);
    write4bits(lownib | mode);
}

// Trimite o comanda catre LCD
void command(uint8_t value) {
    // Trimite valoarea catre LCD in modul comanda (mode = 0)
    send(value, 0);
}

void write(uint8_t value) {
    send(value, Rs);
}

// Functie pentru initializarea LCD-ului
void lcd_init() {
    // Initializeaza magistrala I2C pentru comunicatia cu LCD-ul
    i2c_init();

    // Configuratia initiala a afisajului: modul 4 biti, 2 linii, caractere 5x8 puncte
    _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;

    // Activeaza backlight-ul LCD-ului
    _backlightval = LCD_BACKLIGHT;

    delay(50); // Pauza pentru stabilizarea initiala a LCD-ului
    expanderWrite(0); // Reseteaza expanderul
    delay(1000); // Pauza mai lunga pentru resetare

    // Secventa specifica pentru initializarea in modul 4 biti
    write4bits(0x03 << 4);
    delayMicroseconds(4500);
    write4bits(0x03 << 4);
    delayMicroseconds(4500);
    write4bits(0x03 << 4);
    delayMicroseconds(150);
    write4bits(0x02 << 4);

    // Configureaza functiile de baza ale LCD-ului
    command(LCD_FUNCTIONSET | _displayfunction);

    // Activeaza afisajul, fara cursor sau clipire
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    command(LCD_DISPLAYCONTROL | _displaycontrol);

    // Curata ecranul si seteaza modul de scriere
    lcd_clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _displaymode);

    // Trimite cursorul la pozitia initiala
    command(LCD_RETURNHOME);
    delay(2);
}

// Functie pentru curatarea LCD-ului
void lcd_clear() {
    // Trimite comanda de curatare a afisajului
    command(LCD_CLEARDISPLAY);
    delayMicroseconds(2000);// Pauza pentru finalizarea operatiei
}

// Functie pentru pozitionarea cursorului pe LCD
void lcd_setCursor(uint8_t col, uint8_t row) {
    // Offseturi pentru fiecare linie a LCD-ului, utilizate pentru setarea pozitiei DDRAM
    static const int row_offsets[] = {0x00, 0x40, 0x14, 0x54};

    // Seteaza adresa DDRAM corespunzatoare coloanei si liniei dorite
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Functie pentru afisarea unui text pe LCD
void lcd_print(const char* str) {
    // Scrie fiecare caracter din sirul primit catre LCD
    while (*str) write(*str++);
}

bool readHallSensor() {
    static unsigned long lastDebounceTime = 0;
    static bool lastHallState = HIGH;
    static bool hallState = HIGH;
    const unsigned long debounceDelay = 50;
    
    bool reading = digitalRead(hallSensorPin);
    
    if (reading != lastHallState) {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != hallState) {
            hallState = reading;
        }
    }
    
    lastHallState = reading;
    return hallState;
}

// Configurari hardware in functia setup
void setup() {
    Serial.begin(115200) ;// Initializeaza comunicatia seriala la 115200 bps
    DDRB |= (1 << PB4) | (1 << PB5); // Configureaza pinii PB4 si PB5 ca iesiri pentru LED-uri
    DDRD |= (1 << PD6); // Configureaza pinul PD6 ca iesire pentru LED-ul PIR
    DDRD &= ~(1 << PD7); // Configureaza pinul PD7 ca intrare pentru senzor extern
    DDRB |= (1 << PB2); // Configureaza pinul PB2 ca iesire pentru un LED de stare
    DDRD &= ~(1 << PD3); // Configureaza pinul PD3 ca intrare pentru buton
    PORTD |= (1 << PD3); // Activeaza rezistenta de pull-up pentru pinul PD3 (buton)
    DDRB |= (1 << PB1); // Configureaza pinul PB1 ca iesire PWM pentru servo

     pinMode(hallSensorPin, INPUT_PULLUP); // Configureaza pinul senzorului Hall ca intrare

    lcd_init(); // Initializeaza LCD-ul
    lcd_print("Sistem pornit!"); // Afiseaza mesaj de pornire pe LCD
    delay(2000); // Pauza pentru afisare
    lcd_clear(); // Curata LCD-ul
    
    // TCCR1A (Timer/Counter Control Register A)
    // - WGM11: Seteaza modul de lucru al timerului in Waveform Generation Mode (PWM Phase Correct cu ICR1 ca top)
    // - COM1A1: Activeaza modul non-inverting pentru iesirea pe pinul OC1A
    //   In modul non-inverting, semnalul PWM este HIGH la inceputul ciclului si scade la LOW cand atinge valoarea OCR1A
    TCCR1A = (1 << WGM11) | (1 << COM1A1);

    // TCCR1B (Timer/Counter Control Register B)
    // - WGM13 si WGM12: Completeaza configurarea pentru modul PWM Phase Correct cu ICR1 ca top
    // - CS11: Seteaza prescaler-ul timerului la 8, reducand frecventa de ceas aplicata timerului
    //   Prescaler-ul este utilizat pentru a adapta frecventa de lucru a timerului la cerintele PWM (50 Hz in acest caz)
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    // ICR1 (Input Capture Register 1)
    // - Determina valoarea top pentru modul PWM Phase Correct
    //   Pentru o perioada PWM de 20 ms (50 Hz):
    //   Perioada = (ICR1 + 1) * Prescaler / F_CPU
    //   20ms = (ICR1 + 1) * 8 / 16MHz => ICR1 = 39999
    ICR1 = 39999;

    // OCR1A (Output Compare Register 1A)
    // - Seteaza pulsul PWM initial pentru servo
    //   Pulsul de 1000 microsecunde (1 ms) reprezinta pozitia de start a servo-ului
    //   Valoarea este calculata astfel: OCR1A = (Puls * F_CPU) / (Prescaler * Perioada_Maxima)
    //   Puls = 1ms, Prescaler = 8, Perioada_Maxima = 20ms => OCR1A = 1000
    OCR1A = 1000;
    
    Serial.println("Sistem Arduino pornit. Astept mesaje...");
}

void loop() {
    if (isSystemOff) {
        PORTB &= ~(1 << PB4);// Opreste LED-ul verde
        PORTB &= ~(1 << PB5);// Opreste LED-ul rosu
        PORTD &= ~(1 << PD6);// Opreste LED-ul PIR
        PORTB &= ~(1 << PB2);// Opreste LED-ul de stare
        setServoPosition(0);// Pune servo-ul in pozitia initiala
        lcd_clear();// Curata LCD-ul
        lcd_print("Sistem oprit!");
        while (1);// Blocheaza sistemul intr-o stare inactiva
    }

    if (!(PIND & (1 << PD3))) {
        Serial.println("Buton apasat: Oprire sistem");// Afiseaza mesaj de oprire in consola
        isSystemOff = true;// Seteaza sistemul ca oprit
        delay(500);// Pauza pentru debounce
        return;
    }

    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');// Citeste mesajul serial
        if (message == "PERMIS") {
            isAccessGranted = true;// Acces permis
            lcd_clear();// Curata LCD-ul
            lcd_print("Acces permis");
        } else if (message == "RESPINS") {
            isAccessGranted = false;// Acces respins
            setServoPosition(0);// Pune servo-ul in pozitia initiala
            lcd_clear();
            lcd_print("Acces respins");
        } else {
            lcd_clear();
            lcd_print("Mesaj necunoscut"); // Afiseaza mesaj de eroare
        }
    }

    if (digitalRead(pirPin) == HIGH) {
        digitalWrite(pirLed, HIGH);// Porneste LED-ul PIR daca detecteaza miscare
    } else {
        digitalWrite(pirLed, LOW);
    }

    int Lvalue = analogRead(light);// Citeste valoarea de la senzorul de lumina
    int mVolt = map(Lvalue, 0, 1023, 0, 5000);// Converteste valoarea in milivolti
    float volt = (double)mVolt / 1000;// Converteste in volti
    if (volt < 2.0) {
        PORTB |= (1 << PB2);// Porneste LED-ul de stare daca lumina este slaba
    } else {
        PORTB &= ~(1 << PB2);// Opreste LED-ul de stare daca lumina este suficienta
    }

    if (isAccessGranted) {
        static int pos = 0; // Pozitia servo-ului
        static bool direction = true;// Directia servo-ului
        
        bool hallValue = readHallSensor();
        
        if (hallValue == LOW) {
            lcd_clear();
            lcd_print("Obstacol găsit!");
            Serial.println("Senzor Hall: Obstacol găsit!");
            setServoPosition(0);
            delay(1000);
        } else {
            if (direction) {
                pos += 1; // Creste pozitia
                if (pos >= 180) direction = false; // Schimba directia la limita superioara
            } else {
                pos -= 1;
                if (pos <= 0) direction = true; // Schimba directia la limita inferioara
            }
            setServoPosition(pos); // Actualizeaza pozitia servo-ului
            delay(15); // Pauza intre miscari
        }

        int potValue = analogRead(potentiometerPin); // Citeste valoarea potentiometrului
        int ledIntensity = map(potValue, 0, 1023, 0, 255); // Mapare la intensitatea LED-ului
        analogWrite(ledGreen, ledIntensity); // Ajusteaza LED-ul verde
        digitalWrite(ledRed, LOW);// Opreste LED-ul rosu
    } else {
        int potValue = analogRead(potentiometerPin);// Citeste valoarea potentiometrului
        int ledIntensity = map(potValue, 0, 1023, 0, 255); // Mapare la intensitatea LED-ului
        digitalWrite(ledGreen, LOW);// Opreste LED-ul verde
        analogWrite(ledRed, ledIntensity);// Ajusteaza LED-ul rosu
    }
}

// Functie pentru setarea pozitiei servo-ului
void setServoPosition(int angle) {
    // Mapare a unghiului servo-ului (0-180 grade) la semnal PWM (2000-4000 microsecunde)
    int pwmValue = map(angle, 0, 180, 2000, 4000);
    OCR1A = pwmValue;// Incarca valoarea PWM calculata in registrul OCR1A pentru iesirea PWM
    Serial.print("PWM Value: "); // Afiseaza valoarea PWM pentru debug
    Serial.println(pwmValue);// Afiseaza valoarea PWM in consola seriala
}
