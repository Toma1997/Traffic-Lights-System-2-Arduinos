#define VOZILA_CRVENO 7
#define VOZILA_ZUTO 6
#define VOZILA_ZELENO 5
#define PESACI_CRVENO 9
#define PESACI_ZELENO 8
#define PESACI_DUGME 2
bool stisnuoDugme = false;
bool jeAktivanFlag = true;
bool semaforZaVozila = true; // Kada ide prvih 10 sekundi za vozila.

// Interrupt dugme pomocna funkcija,
// da registruje zahtev pesaka nakon 10 sekundi zelenog za vozila.
void pesakStisnuo(){
  stisnuoDugme = true;
}

void proveriStanje(){
  
  // Provera da li ima poruke na serijskom portu.
  if(Serial.available() > 0){
    String stanje = Serial.readString();
    
    // Ako je sistem pocinje sa aktivan (zbog \r\n, 
    // setuj stanje na atkivan i inicijalizuj na pocetak semafor vozila.
    // Ugasi zuto svetlo nakon aktivacije sistema.
    if(stanje.startsWith("aktivan")){
      	semaforZaVozila = true;
      	digitalWrite(VOZILA_ZUTO, LOW);
      	jeAktivanFlag = true;
    } else {
    	jeAktivanFlag = false;
    }
  }
}

// Funkcija koja izvrsava sekvencu svetla za pesake.
void opsluziPesake(){
  digitalWrite(VOZILA_ZELENO, LOW);
  digitalWrite(VOZILA_ZUTO, HIGH);
  delay(3000);
  proveriStanje();
  if(!jeAktivanFlag) return;
  digitalWrite(VOZILA_ZUTO, LOW);
  digitalWrite(VOZILA_CRVENO, HIGH);
  delay(2000);
  proveriStanje();
  if(!jeAktivanFlag) return;
  digitalWrite(PESACI_CRVENO, LOW);
  digitalWrite(PESACI_ZELENO, HIGH);
  
  // Proveravaj 10 puta nakon svake sekunde da li se deaktivirao sistem.
  for(int i = 0; i < 10; i++){
    delay(1000);
    proveriStanje();
  	if(!jeAktivanFlag) return;
  }
  digitalWrite(PESACI_ZELENO, LOW);
  digitalWrite(PESACI_CRVENO, HIGH);
  delay(1000);
  proveriStanje();
  if(!jeAktivanFlag) return;
  digitalWrite(VOZILA_ZUTO, HIGH);
  delay(3000);
  proveriStanje();
  if(!jeAktivanFlag) return;
  digitalWrite(VOZILA_CRVENO, LOW);
  digitalWrite(VOZILA_ZUTO, LOW);
  digitalWrite(VOZILA_ZELENO, HIGH);
  semaforZaVozila = true;
  
}

// Ako je neaktivan sistem, samo zuto svetlo upali.
void neaktivanSistem(){
  digitalWrite(VOZILA_ZELENO, LOW);
  digitalWrite(VOZILA_CRVENO, LOW);
  digitalWrite(PESACI_ZELENO, LOW);
  digitalWrite(PESACI_CRVENO, LOW);
  digitalWrite(VOZILA_ZUTO, HIGH);
  delay(1000);
  digitalWrite(VOZILA_ZUTO, LOW);
  delay(500);
}

void setup(){
  // Otpocinje serijska komunikacija, ova vrednost je najniza frekvencija.
  Serial.begin(9600);
  pinMode(VOZILA_CRVENO, OUTPUT);
  pinMode(VOZILA_ZUTO, OUTPUT);
  pinMode(VOZILA_ZELENO, OUTPUT);
  pinMode(PESACI_CRVENO, OUTPUT);
  pinMode(PESACI_ZELENO, OUTPUT);
  pinMode(PESACI_DUGME, INPUT);
  
  // Prekidna rutina za pritisak pesaka
  attachInterrupt(digitalPinToInterrupt(PESACI_DUGME), pesakStisnuo, RISING);
}

void loop(){
  proveriStanje();
  if(jeAktivanFlag){
    
    // Drzi deset sekundi samo ako se tek aktivirao semafor.
    if(semaforZaVozila){
    	digitalWrite(VOZILA_ZELENO, HIGH);
    	digitalWrite(PESACI_CRVENO, HIGH);
      
      	// 10 sekundi za vozila je minimum.
      	// Nakon svake sekunde proveri stanje sistema.
        for(int i = 0; i < 10; i++){
        	delay(1000);
          	proveriStanje();
  			if(!jeAktivanFlag) break;
        }
      	semaforZaVozila = false;
    }
    
    // Ako je pesak stisnuo dugme pre ili nakon 10 sekundi,
    // Opsluzi pesaka sa zelenim svetlom.
    if(stisnuoDugme && jeAktivanFlag){
      stisnuoDugme = false;
      opsluziPesake();
    }
    
  // Deaktiviraj sistem.
  } else {
    neaktivanSistem();
  } 
}