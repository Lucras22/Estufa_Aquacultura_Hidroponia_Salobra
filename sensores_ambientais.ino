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
#include <NTPClient.h>
#include <WiFiUdp.h>

// ##############  CONFIGURAÇÃO DO WIFI

// const char* ssid = "IFCE_DISCENTES";
// const char* password = "ifce@bvg";

const char* ssid = "Lucas Galindo | POCO C65"; 
const char* password = "lucras22";

// ##############  CONFIGURAÇÃO DO TELEGRAM
const String botToken = "7819770701:AAHpfYpS61lp9U9cU6z17uU1MP0-TEJvNRU";  // Substitua pelo token do seu bot
const String chatId = "5774771081";     // Substitua pelo seu chat ID

WiFiClient client;

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

#define tdsPin1 33   //Sensor TDS1
#define tdsPin2 32   //Sensor TDS2

#define ONE_WIRE_BUS_1 4  // Sensor do Tanque 1
#define ONE_WIRE_BUS_2 5  // Sensor do Tanque 2

#define PH_SENSOR_PIN1 34 // Sensor pH1
#define PH_SENSOR_PIN2 35 // Sensor pH2

#define rele1 18
#define rele2 19
#define rele3 21
#define rele4 25

// ##############  Configuração dos Reles
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000); // UTC-3 (Brasil)

// Horários para ativação dos relés (horas, minutos)
int horarios_rele1[][2] = {{6, 0}, {12, 0}, {18, 0}}; // FIO AZUL
int horarios_rele2[][2] = {{7, 30}, {13, 30}, {19, 30}}; //FIO VERDE
int horarios_rele3[][2] = {{7, 30}, {13, 30}, {19, 30}}; // BOMBA DO MEIO
int horarios_rele4[][2] = {{7, 30}, {13, 30}, {19, 30}}; // BOMBA DOS PEIXES

// ##############  DHT GLOBAL
#define DHTTYPE DHT22
DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);

// ##############  Sensor TDS


// ##############  TEMP AGUA GLOBAL
OneWire oneWire1(ONE_WIRE_BUS_1);
DallasTemperature sensor1(&oneWire1);

OneWire oneWire2(ONE_WIRE_BUS_2);
DallasTemperature sensor2(&oneWire2);

// ##############  SENSOR PH
float calibracao_ph7 = 2.12;
float calibracao_ph4 = 3.30;
float calibracao_ph10 = 2.55;
float m_4_7, b_4_7, m_7_10, b_7_10;

float calcularPH(int pin) {
  int buf[10];
  for (int i = 0; i < 10; i++) {  
    buf[i] = analogRead(pin);
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

  float tensao = (valorMedio * 3.3) / (4095.0 * 6);
  float ph = (tensao < calibracao_ph7) ? (m_4_7 * tensao + b_4_7) : (m_7_10 * tensao + b_7_10);

  return ph;
}

void setup() {
  Serial.begin(115200);

  // Configuração dos relés como saída
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  pinMode(rele4, OUTPUT);

  // Desliga os relés inicialmente
  digitalWrite(rele1, LOW);
  digitalWrite(rele2, LOW);
  digitalWrite(rele3, LOW);
  digitalWrite(rele4, LOW);

  //TDS
  pinMode(tdsPin1, INPUT);
  pinMode(tdsPin2, INPUT);
  
  //DHT's
  dht1.begin();
  dht2.begin();

  // Inicializando os sensores D'agua
  sensor1.begin();
  sensor2.begin();

  //Calibrando Ph
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

    // Inicializa o cliente NTP
  timeClient.begin();
}

void loop() {

   timeClient.update();

  // Obtém o horário atual
  int horaAtual = timeClient.getHours();
  int minutoAtual = timeClient.getMinutes();

  // Verifica os horários para ativar o Relé
  for (int i = 0; i < 3; i++) {
    if (horaAtual == horarios_rele1[i][0] && minutoAtual == horarios_rele1[i][1]) {
      Serial.println("Relé 1 LIGADO");
      digitalWrite(rele1, HIGH);
      delay(60000);  //evitar múltiplas ativações
      digitalWrite(rele1, LOW);
      Serial.println("Relé 1 DESLIGADO");
    }
  }

  for (int i = 0; i < 3; i++) {
    if (horaAtual == horarios_rele2[i][0] && minutoAtual == horarios_rele2[i][1]) {
      Serial.println("Relé 2 LIGADO");
      digitalWrite(rele2, HIGH);
      delay(60000);
      digitalWrite(rele2, LOW);
      Serial.println("Relé 2 DESLIGADO");
    }
  }

  for (int i = 0; i < 3; i++) {
    if (horaAtual == horarios_rele3[i][0] && minutoAtual == horarios_rele3[i][1]) {
      Serial.println("Relé 3 LIGADO");
      digitalWrite(rele3, HIGH);
      delay(60000);
      digitalWrite(rele3, LOW);
      Serial.println("Relé 3 DESLIGADO");
    }
  }

  for (int i = 0; i < 3; i++) {
    if (horaAtual == horarios_rele4[i][0] && minutoAtual == horarios_rele4[i][1]) {
      Serial.println("Relé 4 LIGADO");
      digitalWrite(rele4, HIGH);
      delay(60000);
      digitalWrite(rele4, LOW);
      Serial.println("Relé 4 DESLIGADO");
    }
  }

  // Caculando o pH
  float ph1 = calcularPH(PH_SENSOR_PIN1);
  float ph2 = calcularPH(PH_SENSOR_PIN2);

  // Leitura dos sensores DHT
  float humidity1 = dht1.readHumidity();
  float temperature1 = dht1.readTemperature();
  float humidity2 = dht2.readHumidity();
  float temperature2 = dht2.readTemperature();

  if (isnan(humidity1) || isnan(temperature1) || isnan(humidity2) || isnan(temperature2)) {
    Serial.println("Falha ao ler DHT!");
    return;
  }

  // Leitura do TDS 1 e 2
  int tdsValue1 = analogRead(tdsPin1);
  int tdsValue2 = analogRead(tdsPin2);
  float conductivity1 = (tdsValue1 * 2)/1000;
  float conductivity2 = (tdsValue2 * 2)/1000;

  // Leitura da temperatura da água
    // Solicita leituras dos sensores
  sensor1.requestTemperatures();
  sensor2.requestTemperatures();
  // Obtendo os valores de temperatura
  float temperatureWater1 = sensor1.getTempCByIndex(0);
  float temperatureWater2 = sensor2.getTempCByIndex(0);

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

    Serial.println("-------Tanque 1-------");

  Serial.print("Temperatura Agua: ");
  Serial.print(temperatureWater1);
  Serial.println(" °C");

  Serial.print("TDS Valor (PPM): ");
  Serial.println(tdsValue1);
  Serial.print("Condutividade Eletrica (ECC | mS/cm): ");
  Serial.println(conductivity1);

  Serial.print(" V | pH: ");
  Serial.println(ph1, 2);

    Serial.println("-------Tanque 2-------");

  Serial.print("Temperatura Agua: ");
  Serial.print(temperatureWater2);
  Serial.println(" C");

  Serial.print("TDS Valor (PPM): ");
  Serial.println(tdsValue2);
  Serial.print("Condutividade Eletrica (ECC | mS/cm): ");
  Serial.println(conductivity2);

  Serial.print(" V | pH: ");
  Serial.println(ph2, 2);

  // Enviar dados para o Telegram
  String mensagem = " Dados dos Sensores:\n\n";
  mensagem += " Interno: " + String(temperature1) + "°C | " + String(humidity1) + "%\n";
  mensagem += " Externo: " + String(temperature2) + "°C | " + String(humidity2) + "%\n\n";
  mensagem += " Tanque 1:";
  mensagem += " Temp. Água: " + String(temperatureWater1) + "°C\n";
  mensagem += " TDS: " + String(tdsValue1) + " ppm\n";
  mensagem += " Condutividade: " + String(conductivity1) + "ECC\n";
  mensagem += " pH: " + String(ph1, 2) + "\n\n";

  mensagem += " Tanque 2:";
  mensagem += " Temp. Água: " + String(temperatureWater2) + "°C\n";
  mensagem += " TDS: " + String(tdsValue2) + " ppm\n";
  mensagem += " Condutividade: " + String(conductivity2) + "ECC\n";
  mensagem += " pH: " + String(ph2, 2) + "\n";

  sendMessage(mensagem);  // Envia para o Telegram

  // Esperar 5 segundos antes da próxima leitura
  delay(5000);
}
