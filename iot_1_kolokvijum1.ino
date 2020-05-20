#define VOZILA_CRVENO 7
#define VOZILA_ZUTO 6
#define VOZILA_ZELENO 5
#define PESACI_CRVENO 9
#define PESACI_ZELENO 8
#define PESACI_DUGME 2
bool stisnuoDugme = false;
bool jeAktivanFlag = true;
int blink = HIGH;
unsigned long prevMillis;
unsigned long intervalZeleno = 1000;
unsigned long intervalZuto = 300;
unsigned long intervalCrvenoVozila = 200;
unsigned long intervalCrvenoPesaciDoZutogVozila = 100;
short stanjeSemafor = 0;



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
      	digitalWrite(VOZILA_ZUTO, LOW);
      	digitalWrite(VOZILA_ZELENO, HIGH);
    	digitalWrite(PESACI_CRVENO, HIGH);
      	jeAktivanFlag = true;
      	blink = true;
      	stanjeSemafor = 0;
      	stisnuoDugme = false;
    } else {
      	digitalWrite(VOZILA_ZELENO, LOW);
        digitalWrite(VOZILA_CRVENO, LOW);
        digitalWrite(PESACI_ZELENO, LOW);
        digitalWrite(PESACI_CRVENO, LOW);
      	digitalWrite(VOZILA_ZUTO, blink);
      	jeAktivanFlag = false;
    }
    prevMillis = millis();
  }
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
  digitalWrite(VOZILA_ZELENO, HIGH);
  digitalWrite(PESACI_CRVENO, HIGH);
  
  // Prekidna rutina za pritisak pesaka
  attachInterrupt(digitalPinToInterrupt(PESACI_DUGME), pesakStisnuo, RISING);
}

void loop(){
  proveriStanje();
  if(jeAktivanFlag){
    // Drzi miniumum deset sekundi zeleno za vozila.
    // I onda proveri da li je stisnuto dugme za semafor.
    bool aktiviratiSekvencuZaPesake = stisnuoDugme && stanjeSemafor == 0;
    if(millis() - prevMillis >= intervalZeleno && aktiviratiSekvencuZaPesake){
      	digitalWrite(VOZILA_ZELENO, LOW);
  		digitalWrite(VOZILA_ZUTO, HIGH);
      	stanjeSemafor = 1;
      	prevMillis = millis();
    }
    // Ako je promenjeno stanje sa zelenog za vozila za pesake
    if(stanjeSemafor != 0){
      // Prosao je period za zuto za vozila, prebaci u stanje za crveno.
      if(stanjeSemafor == 1 && millis() - prevMillis >= intervalZuto){
      	stanjeSemafor = 2;
        digitalWrite(VOZILA_ZUTO, LOW);
  		digitalWrite(VOZILA_CRVENO, HIGH);
        prevMillis = millis();
      } else if (stanjeSemafor == 2 && millis() - prevMillis >= intervalCrvenoVozila){
      	stanjeSemafor = 3;
        digitalWrite(PESACI_CRVENO, LOW);
  		digitalWrite(PESACI_ZELENO, HIGH);
        prevMillis = millis();
      } else if (stanjeSemafor == 3 && millis() - prevMillis >= intervalZeleno){
      	stanjeSemafor = 4;
        digitalWrite(PESACI_ZELENO, LOW);
  		digitalWrite(PESACI_CRVENO, HIGH);
        prevMillis = millis();
      } else if (stanjeSemafor == 4 && millis() - prevMillis >= intervalCrvenoPesaciDoZutogVozila){
      	stanjeSemafor = 5;
        digitalWrite(VOZILA_ZUTO, HIGH);
        prevMillis = millis();
      } else if (stanjeSemafor == 5 && millis() - prevMillis >= intervalZuto){
      	stanjeSemafor = 0; // Vrati na zeleno za vozila i crveno za pesake
        digitalWrite(VOZILA_CRVENO, LOW);
  		digitalWrite(VOZILA_ZUTO, LOW);
  		digitalWrite(VOZILA_ZELENO, HIGH);
        stisnuoDugme = false;
        prevMillis = millis();
      }
    }
  // Deaktiviran sistem i onda samo zuto svetlo blinka.
  } else {
      if(millis() - prevMillis >= 500){
          blink = !blink;
          digitalWrite(VOZILA_ZUTO, blink);
          prevMillis = millis();
      }
  } 
}