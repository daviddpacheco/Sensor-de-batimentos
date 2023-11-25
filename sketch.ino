#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char nomeWifi[] = "Wokwi-GUEST";
char senhaWifi[] = "";
char serverAddress[] = "https://api.tago.io/data";
char contentHeader[] = "application/json";
char tokenHeader[] = "41e8d2c1-0ca7-4949-b6fd-9e35fe7e2f18";

void setup() {
  Serial.begin(115200);
  delay(10);

  display.begin(0x3C, 4, 15);
  display.display();
  delay(2000);
  display.clearDisplay();

  connectToWiFi();
}

void loop() {
  // Lógica de leitura do sensor de batimentos cardíacos simulado
  int batimentosCardiacos = lerBatimentosCardiacos();

  // Exibir no LCD
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Batimentos: ");
  display.print(batimentosCardiacos);
  display.display();

  // Enviar dados para a API da TagoIO
  enviarDadosParaTago(batimentosCardiacos);

  delay(1000); // Aguardar um segundo
}

void connectToWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin(nomeWifi, senhaWifi);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 30) {
    delay(1000);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao WiFi");
  } else {
    Serial.println("\nFalha na conexão ao WiFi. Verifique as credenciais.");
    while (1) {
      delay(1000);
    }
  }
}

int lerBatimentosCardiacos() {
  // Simula a leitura do sensor de batimentos cardíacos
  return random(60, 100); // Exemplo: Retorna um valor aleatório entre 60 e 100
}

void enviarDadosParaTago(int batimentosCardiacos) {
  // Configuração do cliente HTTP
  HTTPClient http;

  // Construção da URL da API da TagoIO
  String url = serverAddress;

  // Construção do payload JSON
  String payload = "{\"variable\":\"batimentosCardiacos\",\"value\":" + String(batimentosCardiacos) + "}";

  // Configuração do cabeçalho HTTP
  http.begin(url);
  http.addHeader("Content-Type", contentHeader);
  http.addHeader("Device-Token", tokenHeader);

  // Realiza a solicitação POST com o payload
  int httpResponseCode = http.POST(payload);

  // Verifica se a solicitação foi bem-sucedida
  if (httpResponseCode > 0) {
    Serial.print("Resposta da TagoIO: ");
    Serial.println(httpResponseCode);

    // Você pode imprimir a resposta do servidor se precisar
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Erro na solicitação HTTP: ");
    Serial.println(httpResponseCode);
  }

  // Libera os recursos do cliente HTTP
  http.end();
}
