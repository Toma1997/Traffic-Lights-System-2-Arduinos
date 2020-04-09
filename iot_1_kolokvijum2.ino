#define ZELENO_AKTIVAN 5
#define CRVENO_NEAKTIVAN 6
#define AKTIVACIJA_DEAKTIVACIJA_DUGME 2
int neaktivan = LOW; // U pocetku je aktivan sistem

// Interrupt funkcija za promenu stanja i tansmisiju stanja,
// Transmiter salje kao output, dok ce prvi arduino prihvatiti kao Prijemnik.
void promeniStanje(){
  neaktivan = !neaktivan;
  String output = (neaktivan == HIGH) ? "neaktivan" : "aktivan";
  Serial.println(output);
}

void setup(){
  Serial.begin(9600); // Inicijalizuj serijsku komunikaciju.
  pinMode(ZELENO_AKTIVAN, OUTPUT);
  pinMode(CRVENO_NEAKTIVAN, OUTPUT);
  pinMode(AKTIVACIJA_DEAKTIVACIJA_DUGME, INPUT);
  attachInterrupt(digitalPinToInterrupt(AKTIVACIJA_DEAKTIVACIJA_DUGME), promeniStanje, RISING);
}

void loop(){
  // Invertuj paljenje i gasenje dioda.
  digitalWrite(CRVENO_NEAKTIVAN, neaktivan);
  digitalWrite(ZELENO_AKTIVAN, !neaktivan);
  delay(1000);
  // Treperenje upaljenje diode (svetli 1s, pa ne svetli 0.5 s).
  if (neaktivan == LOW) digitalWrite(ZELENO_AKTIVAN, neaktivan);
  else digitalWrite(CRVENO_NEAKTIVAN, !neaktivan);
  delay(500);

}