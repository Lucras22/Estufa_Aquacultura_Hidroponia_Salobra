// ##############  BIBLIOTECAS
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// ##############  PINOS
#define DHT1PIN 12  //Interno
#define DHT2PIN 13 //Externo
#define tdsPin 33 //Sensor TDS
#define ONE_WIRE_BUS 4 //Sensor Temp Agua
#define PH_SENSOR_PIN 34  // Sensor pH

// ##############  DHT GLOBAL
// Definindo o tipo de sensor DHT
#define DHTTYPE DHT22
// Iniciando os sensores DHT
DHT dht1(DHT1PIN, DHTTYPE);
DHT dht2(DHT2PIN, DHTTYPE);

// ##############  TEMP AGUA GLOBAL
// Criando uma instância da classe OneWire
OneWire oneWire(ONE_WIRE_BUS);
// Passando a instância OneWire para a classe DallasTemperature
DallasTemperature sensors(&oneWire);

// ##############  SENSOR PH AGUA

// Definições de calibração para as duas faixas
float calibracao_ph7 = 1.65;  // Tensão obtida na solução de calibração pH 7 (ajuste conforme a leitura real)
float calibracao_ph4 = 1.35;  // Tensão obtida na solução de calibração pH 4 (ajuste conforme a leitura real)
float calibracao_ph10 = 2.05; // Tensão obtida na solução de calibração pH 10 (ajuste conforme a leitura real)

float m_4_7, b_4_7;  // Variáveis para a equação da calibração pH 4 a 7
float m_7_10, b_7_10; // Variáveis para a equação da calibração pH 7 a 10

// ##############  

void setup() {
  Serial.begin(115200);

  pinMode(tdsPin, INPUT);
  
  dht1.begin();
  dht2.begin();
  sensors.begin();

  // Calibração para pH 7 e 4 (faixa ácida)
  m_4_7 = (4.0 - 7.0) / (calibracao_ph4 - calibracao_ph7);
  b_4_7 = 7.0 - m_4_7 * calibracao_ph7;

  // Calibração para pH 7 e 10 (faixa básica)
  m_7_10 = (7.0 - 10.0) / (calibracao_ph7 - calibracao_ph10);
  b_7_10 = 10.0 - m_7_10 * calibracao_ph10;

}

void loop() {

// ##############  SENSOR PH

  int buf[10];

  // Coleta de 10 amostras do sensor
  for (int i = 0; i < 10; i++) {  
    buf[i] = analogRead(PH_SENSOR_PIN);
    delay(10);
  }

  // Ordena os valores coletados em ordem crescente
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
  for (int i = 2; i < 8; i++) {  // Média das 6 amostras centrais
    valorMedio += buf[i];
  }

  // Conversão do ADC para tensão (ESP32 usa ADC de 12 bits, valor máximo = 4095)
  float tensao = (valorMedio * 3.3) / 4095.0 / 6;

  // Calculo do pH com base nas duas faixas
  float ph;

  // Determina a faixa de pH com base na tensão lida
  if (tensao < calibracao_ph7) {
    // Faixa ácida (pH 4 a 7)
    ph = m_4_7 * tensao + b_4_7;
  } else if (tensao > calibracao_ph7) {
    // Faixa básica (pH 7 a 10)
    ph = m_7_10 * tensao + b_7_10;
  } else {
    // Faixa neutra (próxima ao pH 7)
    ph = 7.0;  // pH 7 como valor neutro
  }

// ##############  DHT22
  // Lendo a umidade relativa do ar do DHT1 e DHT2
  float humidity1 = dht1.readHumidity();
  float humidity2 = dht2.readHumidity();

  // Lendo a temperatura em Celsius do DHT1 e DHT2
  float temperature1 = dht1.readTemperature();
  float temperature2 = dht2.readTemperature();
  
  // Verifique se houve falha na leitura dos sensores
  if (isnan(humidity1) || isnan(temperature1)) {
    Serial.println("Falha ao ler do sensor DHT1!");
    return;
  }

  if (isnan(humidity2) || isnan(temperature2)) {
    Serial.println("Falha ao ler do sensor DHT2!");
    return;
  }

// ##############  TDS
  int tdsValue = analogRead(tdsPin);

  // Converte o valor do sensor em milivolts (mV)
  float condutivy = tdsValue * 2;

// ##############  TEMPERATURA DA AGUA
 sensors.requestTemperatures(); // Solicita a leitura de temperatura
  float temperatureWalter = sensors.getTempCByIndex(0); // Obtém a temperatura do primeiro sensor

// ##############

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
  Serial.print(temperatureWalter);
  Serial.println(" C");

  Serial.print("TDS Valor (PPM): ");
  Serial.println(tdsValue);
  Serial.print("Condutividade Eletrica (ECC): ");
  Serial.println(condutivy);

  Serial.print("Tensão medida: ");
  Serial.print(tensao, 3);
  Serial.print(" V | pH: ");
  Serial.println(ph, 2);

  // Repetindo as leituras a cada 5 segundos
  delay(5000);
}