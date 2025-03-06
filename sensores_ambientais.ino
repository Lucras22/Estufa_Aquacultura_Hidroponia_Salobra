/*
██╗     ██╗   ██╗ ██████╗ ██████╗  █████╗ ███████╗
██║     ██║   ██║██╔═════╗██╔══██╗██╔══██╗██╔════╝
██║     ██║   ██║██║     ║██████╔╝███████║███████╗
██║     ██║   ██║██║     ║██╔══██╗██╔══██║╚════██║
███████╗╚██████╔╝╚██████╔╝██║  ██║██║  ██║███████║
╚══════╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝
-----------------------------------------------------------
📌 Autor: Lucas Galindo
🔗 GitHub: https://github.com/Lucras22
📧 E-mail: devlucasgalindo@email.com
💼 LinkedIn: https://www.linkedin.com/in/lucasgalindoiot/
-----------------------------------------------------------
📜 Instruções de Uso:
- 
-----------------------------------------------------------
📂 Repositório do Projeto:
🔗 https://github.com/Lucras22/Estufa_Aquacultura_Hidroponia_Salobra
-----------------------------------------------------------
🛠️ Licença: ....
*/

// ##############  BIBLIOTECAS
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ##############  CONFIGURAÇÃO DO WIFI
const char* ssid = "Lucas Galindo | POCO C65";       // Substitua pelo seu Wi-Fi
const char* password = "lucras22";  // Substitua pela senha

// ##############  CONFIGURAÇÃO DO TELEGRAM
const String botToken = "7751526303:AAEjh5i6E2B0uGwTbU3TGWbjhbvcdTEvFdg";  // Substitua pelo token do seu bot
const String chatId = "7003158288";     // Substitua pelo seu chat ID

WiFiClientSecure client;

// Função para enviar mensagem para o Telegram via POST
void sendMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage";
    
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Montando os parâmetros da requisição POST
    String postData = "chat_id=" + chatId + "&text=" + message;
    
    // Enviando a requisição
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Código de resposta: " + String(httpResponseCode));
      Serial.println("Resposta: " + response);
    } else {
      Serial.print("Erro na requisição: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("WiFi desconectado");
  }
}

// ##############  PINOS
#define DHT1PIN 14  //Interno
#define DHT2PIN 13  //Externo
#define tdsPin 33   //Sensor TDS
#define ONE_WIRE_BUS 4 //Sensor Temp Agua
#define PH_SENSOR_PIN 34 // Sensor pH

// ##############  DHT GLOBAL
#define DHTTYPE DHT22
DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);

// ##############  TEMP AGUA GLOBAL
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ##############  SENSOR PH
float calibracao_ph7 = 1.65;
float calibracao_ph4 = 1.35;
float calibracao_ph10 = 2.05;
float m_4_7, b_4_7, m_7_10, b_7_10;

void setup() {
  Serial.begin(115200);
  pinMode(tdsPin, INPUT);
  
  dht1.begin();
  dht2.begin();
  sensors.begin();

  m_4_7 = (4.0 - 7.0) / (calibracao_ph4 - calibracao_ph7);
  b_4_7 = 7.0 - m_4_7 * calibracao_ph7;
  m_7_10 = (7.0 - 10.0) / (calibracao_ph7 - calibracao_ph10);
  b_7_10 = 10.0 - m_7_10 * calibracao_ph10;

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void loop() {
  // Leitura do sensor de pH
  int buf[10];
  for (int i = 0; i < 10; i++) {  
    buf[i] = analogRead(PH_SENSOR_PIN);
    delay(10);
  }

  // Ordenação e média das amostras centrais
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        int temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  int valorMedio = 0;
  for (int i = 2; i < 8; i++) {
    valorMedio += buf[i];
  }

  float tensao = (valorMedio * 3.3) / 4095.0 / 6;
  float ph = (tensao < calibracao_ph7) ? (m_4_7 * tensao + b_4_7) : (m_7_10 * tensao + b_7_10);

  // Leitura dos sensores DHT
  float humidity1 = dht1.readHumidity();
  float temperature1 = dht1.readTemperature();
  float humidity2 = dht2.readHumidity();
  float temperature2 = dht2.readTemperature();

  if (isnan(humidity1) || isnan(temperature1) || isnan(humidity2) || isnan(temperature2)) {
    Serial.println("Falha ao ler DHT!");
    return;
  }

  // Leitura do TDS
  int tdsValue = analogRead(tdsPin);
  float conductivity = tdsValue * 2;

  // Leitura da temperatura da água
  sensors.requestTemperatures();
  float temperatureWater = sensors.getTempCByIndex(0);

 //EXIBIR MONITOR SERIAL

  Serial.print("Umidade Interna: ");
  Serial.print(humidity1);
  Serial.print(" %\t");
  Serial.print("Temperatura Interna: ");
  Serial.print(temperature1);
  Serial.println(" °C");

  Serial.print("Umidade Externa: ");
  Serial.print(humidity2);
  Serial.print(" %\t");
  Serial.print("Temperatura Externa: ");
  Serial.print(temperature2);
  Serial.println(" °C");

  Serial.print("Temperatura Agua: ");
  Serial.print(temperatureWater);
  Serial.println(" C");

  Serial.print("TDS Valor (PPM): ");
  Serial.println(tdsValue);
  Serial.print("Condutividade Eletrica (ECC): ");
  Serial.println(conductivity);

  Serial.print("Tensão medida: ");
  Serial.print(tensao, 3);
  Serial.print(" V | pH: ");
  Serial.println(ph, 2);

  // Enviar dados para o Telegram
  String mensagem = " Dados dos Sensores:\n\n";
  mensagem += " Interno: " + String(temperature1) + "°C | " + String(humidity1) + "%\n";
  mensagem += " Externo: " + String(temperature2) + "°C | " + String(humidity2) + "%\n";
  mensagem += " Temp. Água: " + String(temperatureWater) + "°C\n";
  mensagem += " TDS: " + String(tdsValue) + " ppm\n";
  mensagem += " Condutividade: " + String(conductivity) + "ECC\n";
  mensagem += " pH: " + String(ph, 2) + "\n";

  sendMessage(mensagem);  // Envia para o Telegram

  // Esperar 5 segundos antes da próxima leitura
  delay(5000);
}
