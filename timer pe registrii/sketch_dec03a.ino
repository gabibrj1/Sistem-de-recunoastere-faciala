void setup() {
  // Configurare Fast PWM pe OC1B (PB2, Pin Digital 10 pe Arduino Uno)

  // TCCR1A controleaza modul de operare al Timerului 1 si setarile canalelor de iesire.
  // COM1B1: Seteaza modul de lucru pentru pinul OC1B (Pin Digital 10).
  // - COM1B1 = 1 și COM1B0 = 0 configureaza OC1B pentru Clear on Compare Match.
  //   Aceasta inseamna ca semnalul PWM va fi setat la inceputul perioadei si resetat cand
  //   contorul ajunge la valoarea specificata în OCR1B (Output Compare Register B).
  TCCR1A = (1 << COM1B1);

  // WGM11: Parte a setarilor pentru modul de operare al timerului 
  // WGM11 = 1 și WGM13 = 1 impreuna cu WGM12 = 1 selecteaz modul Fast PWM cu TOP în ICR1.
  //   in acest mod, semnalul PWM are o rezolutie de 16 biti, iar valoarea maxima a contorului
  //   este definita in ICR1.
  TCCR1A |= (1 << WGM11);

  // TCCR1B controleaza prescaler-ul si modul de operare al Timerului 1.
  // WGM13 și WGM12: Completeaza setarea modului "Fast PWM cu TOP în ICR1".
  TCCR1B = (1 << WGM13) | (1 << WGM12);

  // CS11: Selecteaza prescaler-ul timerului (Clock Select).
  // CS11 = 1 configureaza prescaler-ul la 8, ceea ce inseamna ca frecvenata de intrare
  //   a timerului este ceasul principal (16 MHz pentru Arduino Uno) impartit la 8.
  //   Acest prescaler este ales pentru a permite generarea unui semnal PWM cu frecventa de 50 Hz.
  TCCR1B |= (1 << CS11);

  // ICR1 defineste valoarea TOP pentru timer în modul Fast PWM.
  // ICR1 controleaza perioada semnalului PWM; in acest caz, vrem un semnal PWM
  //   cu o frecventa de 50 Hz (perioada de 20 ms).
  //   Formula este:
  //     ICR1 = (F_CPU / (Prescaler * PWM_Frequency)) - 1
  //     ICR1 = (16,000,000 / (8 * 50)) - 1 = 40,000 - 1
  //   Pentru Arduino Uno cu ceas de 16 MHz și prescaler 8, valoarea ICR1 este 40,000.
  ICR1 = 40000;

  // OCR1B defineste durata impulsului PWM pentru canalul B (OC1B).
  // - Valoarea initiala este 3000, ceea ce corespunde unui semnal "high" de 1.5 ms.
  //   Acesta este semnalul neutru pentru servo-uri, adica pozitia de mijloc.
  OCR1B = 3000;

  // DDRB: Configuram pinul PB2 (OC1B, Pin Digital 10) ca iesire.
  // - PB2 este conectat la servo si trebuie sa genereze semnalul PWM.
  DDRB |= (1 << PB2);
}

void loop() {
  // Modificam valoarea OCR1B pentru a schimba durata impulsului PWM si pozitia servo-ului.

  // OCR1B = 2000: Semnal PWM "high" timp de 1 ms.
  // - 1 ms corespunde poziției minime pentru majoritatea servo-urilor.
  OCR1B = 2000;
  delay(1000); // Asteapta 1 secunda inainte de urmatoarea schimbare.

  // OCR1B = 4000: Semnal PWM "high" timp de 2 ms.
  // - 2 ms corespunde pozitiei maxime pentru majoritatea servo-urilor.
  OCR1B = 4000;
  delay(1000); // asteapta 1 secunda inainte de urmatoarea schimbare.
}
