// Definindo as informações da rede Wi-Fi
String ssid = "Simulator Wifi";                          // Nome da rede Wi-Fi
String password = "";                                    // Senha da rede Wi-Fi (em branco para o simulador)
String host = "api.thingspeak.com";                      // Host para a conexão com a API do Thingspeak
const int httpPort = 80;                                 // Porta para a conexão HTTP
String uri = "/update?api_key=UCGAHW0BVXXRX5OG&field1="; // URI para enviar dados para o Thingspeak

// Função para configurar o módulo ESP8266
int setupESP8266(void)
{
    Serial.begin(115200); // Inicializa a comunicação serial com o ESP8266
    Serial.println("AT"); // Envia o comando "AT" para o ESP8266
    delay(10);            // Aguarda a resposta do ESP8266
    if (!Serial.find("OK"))
        return 1; // Verifica se a resposta contém "OK"

    // Conecta à rede Wi-Fi
    Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
    delay(10);
    if (!Serial.find("OK"))
        return 2;

    // Abre uma conexão TCP com o host
    Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
    delay(50);
    if (!Serial.find("OK"))
        return 3;

    return 0; // Retorna 0 se a configuração for bem-sucedida
}

// Função para enviar a temperatura para o Thingspeak
void enviaTemperaturaESP8266(void)
{
    // Lê a temperatura do sensor analógico (A0)
    int temp = map(analogRead(A0), 20, 358, -40, 125);

    // Constrói a chamada HTTP
    String httpPacket = "GET " + uri + String(temp) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
    int length = httpPacket.length();

    // Envia o comprimento da mensagem
    Serial.print("AT+CIPSEND=");
    Serial.println(length);
    delay(10);

    // Envia a solicitação HTTP
    Serial.print(httpPacket);
    delay(10);
    if (!Serial.find("SEND OK\r\n"))
        return; // Verifica se a resposta indica envio bem-sucedido
}

// Configuração inicial
void setup()
{
    setupESP8266();
}

// Loop principal
void loop()
{
    enviaTemperaturaESP8266(); // Envia a temperatura para o Thingspeak
    delay(10000);              // Aguarda 10 segundos antes de enviar novamente
}
