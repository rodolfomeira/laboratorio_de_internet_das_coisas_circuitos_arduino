// Hand On - Semáforo

// "Nomeando" as portas
int verde = 10;
int amarelo = 9;
int vermelho = 8;
 
void setup() {
  // Indicando para o arduíno os tipos das portas
  pinMode(verde, OUTPUT);
  pinMode(amarelo, OUTPUT);
  pinMode(vermelho, OUTPUT);
}
 
void loop() {
  // Vamos começar ligando o verde
  digitalWrite(verde, HIGH);
  digitalWrite(amarelo, LOW);
  digitalWrite(vermelho, LOW);
  // esperamos 5s com o sinal aberto
  delay(1000);
  
  // Apagando o verde e ligando o amarelo
  digitalWrite(verde, LOW);
  digitalWrite(amarelo, HIGH);
  // esperamos 2s com o sinal no amarelo
  delay(500);
 
  // Apagamos o amarelo e ligamos o vermelho
  digitalWrite(amarelo, LOW);
  digitalWrite(vermelho, HIGH);
  // esperamos 5s com o sinal fechado
  delay(1000);   
}