// Hand On - Sensor de ré
//Definição do trigger, echo, pinos de bipe e outras constantes

#define trigger 2
#define  echo 3
#define beep 8

#define beep_start 100  // 100 cm 
#define min_distance 5  // 5 cm 

//Definição da velocidade do som (centímetros/microssegundos)
#define c 0.0343

//Definição das variáveis
long tempo;
float space;

void setup() {
  //Definição de entrada e saída
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(beep, OUTPUT);

  //Inicialização do serial
  Serial.begin(9600);
}

void loop() {
  // Antes da medição, o trigger é definido para nível baixo
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);

  // Envia um pulso (o trigger permanece no nível alto por 10 microssegundos)
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  //  Leitura de echo, via pulseIn, que retorna a duração do impulso (em microssegundos)
  // Os dados adquiridos são então divididos por 2 (para frente e para trás)
  tempo =  pulseIn(echo, HIGH) / 2;
  // Cálculo da distância em centímetros
  space  = tempo * c;

  // space é exibido no monitor serial
  Serial.println("Distancia  = " + String(space, 1) + " cm");

  // Se a distância for inferior a um metro
  if (space < beep_start) { 
    //  Abaixo de min_distance cm emite um som contínuo
    tone(beep, 1500); 
    delay(40);
    // Emite sons em intervalos proporcionais à distância (1 m = 400 ms)
    if (space > min_distance)  {
      noTone(beep); 
      delay(space * 4);
    }
  } 
  // Aguarda 50 milissegundos antes de outra medição
  delay(50);
}