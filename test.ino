// ========== PIN DEFINĪCIJAS ==========
const int ledPin = 13;        // LED pins - konstante, kas nemainās programmas izpildes laikā
const int buzzerPin = 8;      // Skaņotāja pins
const int buttonPin = 2;      // Pogas pins

// ========== TAIMERA MAINĪGIE ==========
// hw_timer_t - ESP32 aparatūras taimera struktūras tips
// * - norādes operators (pointer), norāda uz atmiņas adresi, kur glabājas taimera objekts
// NULL - sākotnējā vērtība, kas norāda, ka norāde nekur nerāda
hw_timer_t *blinkTimer = NULL;

// volatile - norāda kompilatoram, ka šī mainīgā vērtība var mainīties pārtraukumos
// bool - loģiskais tips (true/false), aizņem 1 baitu atmiņā
volatile bool isBlinking = true;    // Kontrolē LED mirgošanas stāvokli
volatile bool buttonPressed = false; // Kontrolē pogas nospiešanas stāvokli

// ========== PĀRTRAUKUMU APSTRĀDES FUNKCIJAS ==========
// void - funkcija neatgriež vērtību
// IRAM_ATTR - atribūts, kas norāda, ka funkcija jāievieto ātrās piekļuves IRAM atmiņā
// Tas ir svarīgi pārtraukumu funkcijām, jo tām jāizpildās ātri
void IRAM_ATTR onTimer() {
    if (isBlinking) {
        // digitalRead() - nolasa pina stāvokli (0 vai 1)
        // ! - loģiskā inversija (NOT operators)
        // digitalWrite() - iestata pina stāvokli
        digitalWrite(ledPin, !digitalRead(ledPin));
        
        // Pārbaudam LED stāvokli un kontrolējam skaņu
        if (digitalRead(ledPin)) {
            tone(buzzerPin, 1000);  // Ieslēdz skaņu ar 1kHz frekvenci
        } else {
            noTone(buzzerPin);      // Izslēdz skaņu
        }
    }
}

// Pogas pārtraukuma apstrādes funkcija
// Tiek izsaukta, kad notiek pogas nospiešana
void IRAM_ATTR buttonInterrupt() {
    buttonPressed = true;   // Iestatām pogas stāvokļa karogu
    isBlinking = false;     // Apturams mirgošanu
}

// ========== SETUP FUNKCIJA ==========
// Tiek izpildīta vienu reizi pēc ieslēgšanas
void setup() {
    // Inicializējam seriālo komunikāciju ar 115200 baudu ātrumu
    Serial.begin(115200);
    
    // Konfigurējam pinu režīmus
    pinMode(ledPin, OUTPUT);         // LED pins kā izeja
    pinMode(buzzerPin, OUTPUT);      // Skaņotāja pins kā izeja
    pinMode(buttonPin, INPUT_PULLUP); // Poga ar iebūvēto pull-up rezistoru
    
    // Konfigurējam pogas pārtraukumu
    // digitalPinToInterrupt() - konvertē pina numuru uz pārtraukuma numuru
    // FALLING - pārtraukums tiek izsaukts, kad signāls mainās no HIGH uz LOW
    attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, FALLING);
    
    // ===== TAIMERA KONFIGURĀCIJA =====
    // 1. Inicializējam taimeri
    // timerBegin(timer_nr, prescaler, countUp)
    // - timer_nr: 0-3 (ESP32 ir 4 taimeri)
    // - prescaler: 80 (dala 80MHz CPU frekvenci ar 80, iegūstot 1MHz)
    // - countUp: true (skaita uz augšu)
    blinkTimer = timerBegin(0, 80, true);
    
    // 2. Pievienojam pārtraukuma apstrādes funkciju
    // &onTimer - funkcijas adreses operators (&)
    timerAttachInterrupt(blinkTimer, &onTimer, true);
    
    // 3. Iestatām taimera vērtību
    // 500000 mikrosekundes = 0.5 sekundes
    // true - automātiska pārlāde
    timerAlarmWrite(blinkTimer, 500000, true);
    
    // 4. Startējam taimeri
    timerAlarmEnable(blinkTimer);
    
    Serial.println("Sistema inicializeta");
}

// ========== GALVENĀ PROGRAMMAS CILPA ==========
void loop() {
    // Pārbaudam vai poga ir nospiesta
    if (buttonPressed) {
        Serial.println("Poga nospiesta - pauze 10 sekundes");
        
        // Izslēdzam LED un skaņu
        digitalWrite(ledPin, LOW);   // LOW = 0V uz pina
        noTone(buzzerPin);          // Izslēdzam skaņu
        
        delay(10000);  // Gaidām 10 sekundes (10000 milisekundes)
        
        // Atjaunojam normālo darbību
        buttonPressed = false;  // Notīrām pogas karogu
        isBlinking = true;      // Atļaujam mirgošanu
        
        Serial.println("Atsākam normālo darbību");
    }
    
    // Mazs delay, lai samazinātu procesora noslodzi
    // Tas nav kritisks, jo LED kontroli veic taimeris
    delay(100);
}
