// Pin definīcijas - konstantes, kas nemainās programmas darbības laikā
#define RED_LED     21    // Sarkanā LED uz GPIO21
#define GREEN_LED   19    // Zaļā LED uz GPIO19

// Taimeru objektu norādes (pointers)
// hw_timer_t - ESP32 taimera datu tips
// * - norādes (pointer) operators, kas norāda uz mainīgā atrašanās vietu atmiņā
// NULL - sākotnējā vērtība, kas norāda, ka pointer nekur nerāda
hw_timer_t *Red_timer = NULL;    // Pointer uz sarkano LED taimeri
hw_timer_t *Green_timer = NULL;  // Pointer uz zaļo LED taimeri

// Pārtraukuma apstrādes funkcijas
// void - funkcija neko neatgriež
// IRAM_ATTR - norāda kompilatoram likt šo funkciju ātrājā IRAM atmiņā
// Tas ir svarīgi, jo pārtraukuma funkcijām jāizpildās ātri
void IRAM_ATTR onRedTimer(){
    // !digitalRead() - invertē LED stāvokli (ja bija 1, būs 0 un otrādi)
    digitalWrite(RED_LED, !digitalRead(RED_LED));
}

void IRAM_ATTR onGreenTimer(){
    digitalWrite(GREEN_LED, !digitalRead(GREEN_LED));
}

void setup() {
    // Piņu konfigurācija kā izejas
    pinMode(RED_LED, OUTPUT);    // LED piņš kā izeja
    pinMode(GREEN_LED, OUTPUT);
    
    // === SARKANĀS LED TAIMERA IESTATĪŠANA ===
    // timerBegin() atgriež pointer uz taimera objektu
    Red_timer = timerBegin(0,     // Taimera numurs (0-3)
                          80,     // Preskaleris (dala CPU 80MHz/80 = 1MHz)
                          true);  // Skaitīt uz augšu
    
    // Pievieno pārtraukuma funkciju
    // &onRedTimer - & ir adreses operators, kas norāda uz funkcijas atrašanās vietu atmiņā                    
    timerAttachInterrupt(Red_timer,      // Taimera pointer
                        &onRedTimer,     // Funkcijas adrese
                        true);          // Pārtraukums uz augšējo fronti

    // Iestata taimera periodu
    timerAlarmWrite(Red_timer,      // Taimera pointer
                   1000000,        // 1 sekunde mikrosekundēs
                   true);         // Automātiska pārlāde

    timerAlarmEnable(Red_timer);  // Aktivizē taimeri

    // === ZAĻĀS LED TAIMERA IESTATĪŠANA ===
    // Līdzīgi kā sarkanai LED, bet ar citu periodu
    Green_timer = timerBegin(1,     // Izmantojam otro taimeri (1)
                            80,     // Tas pats preskaleris
                            true);  
    
    timerAttachInterrupt(Green_timer, &onGreenTimer, true);
    
    timerAlarmWrite(Green_timer,     
                   500000,          // 0.5 sekundes (500,000 mikrosekundes)
                   true);          
    
    timerAlarmEnable(Green_timer);
}

void loop() {
    // Tukša cilpa, jo visu darbu veic taimeri caur pārtraukumiem
}
