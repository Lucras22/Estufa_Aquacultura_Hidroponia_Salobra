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
#include <WiFi.h>
#include <HTTPClient.h>

// === RTC ===
// #include <Wire.h>
// #include "RTClib.h"
// RTC_DS1307 rtc;

// === NTP ===
 #include <NTPClient.h>
 #include <WiFiUdp.h>

const char* ssid = "Lucas Galindo | POCO C65"; 
const char* password = "lucras22";

//const char* ssid = "IFCE_ESTUFAS"; 
//const char* password = "ifce@bvg22";

// ##############  Configuração dos Reles

#define rele1 18
#define rele2 19
#define rele3 23
#define rele4 25

  bool rele4Ligado = false;
  bool rele3Ligado = false;

// === NTP 
 WiFiUDP ntpUDP;
 NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

// Horários para ativação dos relés (horas, minutos)
int horarios_rele1[][2] = {{8, 0}, {12, 0}, {16, 0}}; // FIO AZUL (Peixe : Fecha) (Solução : Abre)
int horarios_rele2[][2] = {{9, 0}, {13, 0}, {17, 0}}; //FIO VERDE (Peixe : Abre) (Solução : Fecha)

// Formato: {{hora_liga, minuto_liga, hora_desliga, minuto_desliga}, ...}
int horarios_rele3[][4] = {
  {8, 1, 8, 16},
  {8, 31, 8, 46},
  {12, 1, 12, 16},
  {12, 31, 12, 46},
  {16, 1, 16, 16},
  {16, 31, 16, 46}
};

int horarios_rele4[][4] = {
  {0, 1, 0, 16},
  {0, 31, 0, 46},
  {1, 1, 1, 16},
  {1, 31, 1, 46},
  {3, 1, 3, 16},
  {3, 31, 3, 46},
  {4, 1, 4, 16},
  {4, 31, 4, 46},
  {5, 1, 5, 16},
  {5, 31, 5, 46},
  {6, 1, 6, 16},
  {6, 31, 6, 46},
  {7, 1, 7, 16},
  {7, 31, 7, 46},
  {9, 1, 9, 16},
  {9, 31, 9, 46},
  {10, 1, 10, 16},
  {10, 31, 10, 46},
  {11, 1, 11, 16},
  {11, 31, 11, 46},
  {13, 1, 13, 16},
  {13, 31, 13, 46},
  {14, 1, 14, 16},
  {14, 31, 14, 46},
  {15, 1, 15, 16},
  {15, 31, 15, 46},
  {17, 1, 17, 16},
  {17, 31, 17, 46},
  {18, 1, 18, 16},
  {18, 31, 18, 46},
  {19, 1, 19, 16},
  {19, 31, 19, 46},
  {20, 1, 20, 16},
  {20, 31, 20, 46},
  {21, 1, 21, 16},
  {21, 31, 21, 46},
  {22, 1, 22, 16},
  {22, 31, 22, 46},
  {23, 1, 23, 16},
  {23, 31, 23, 46}
};

void setup() {
  Serial.begin(115200);

  // Configuração dos relés como saída
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  pinMode(rele4, OUTPUT);

  // Desliga os relés inicialmente
  digitalWrite(rele1, HIGH);
  digitalWrite(rele2, HIGH);
  digitalWrite(rele3, HIGH);
  digitalWrite(rele4, HIGH);

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

// === RTC INIT ===
//  Wire.begin();
//  if (!rtc.begin()) {         // Verifica se o RTC está conectado
//        Serial.println("Erro: RTC não encontrado!");
//        //while (1);
//    }
//    if (!rtc.isrunning()) {     // Se o RTC não estiver rodando, define a hora
//        Serial.println("Configurando horário...");
//        rtc.adjust(DateTime(2025, 4, 24, 8, 40, 0)); // Ano, Mês, Dia, Hora, Minuto, Segundo
//    }

  // === NTP INIT 
   timeClient.begin();

}

void loop() {
    // === RTC
  // DateTime now = rtc.now();
  // int horaAtual = now.hour();
  // int minutoAtual = now.minute();

  // === NTP (COMENTADO)
   timeClient.update();
   int horaAtual = timeClient.getHours();
   int minutoAtual = timeClient.getMinutes();

  Serial.print("Hora atual: ");
  Serial.print(horaAtual);
  Serial.print(":");
  Serial.println(minutoAtual);


  // Verifica os horários para ativar o Relé
  for (int i = 0; i < 3; i++) {
    if (horaAtual == horarios_rele1[i][0] && minutoAtual == horarios_rele1[i][1]) {
      Serial.println("Relé 1 LIGADO: Peixes fechando | Solução abrindo");
      digitalWrite(rele1, LOW);
      delay(60000);  //evitar múltiplas ativações
      digitalWrite(rele1, HIGH);
      Serial.println("Relé 1 DESLIGADO");
    }
  }

  for (int i = 0; i < 3; i++) {
    if (horaAtual == horarios_rele2[i][0] && minutoAtual == horarios_rele2[i][1]) {
      Serial.println("Relé 2 LIGADO: Peixes abrindo | Solução fechando");
      digitalWrite(rele2, LOW);
      delay(60000);
      digitalWrite(rele2, HIGH);
      Serial.println("Relé 2 DESLIGADO");
    }
  }


  int totalHorariosRele3 = sizeof(horarios_rele3) / sizeof(horarios_rele3[0]);
  bool deveLigarRele3 = false;

for (int i = 0; i < totalHorariosRele3; i++) {
  int horaLiga = horarios_rele3[i][0];
  int minutoLiga = horarios_rele3[i][1];
  int horaDesliga = horarios_rele3[i][2];
  int minutoDesliga = horarios_rele3[i][3];

  // Verifica se o horário atual está dentro de algum intervalo
  if ((horaAtual > horaLiga || (horaAtual == horaLiga && minutoAtual >= minutoLiga)) &&
      (horaAtual < horaDesliga || (horaAtual == horaDesliga && minutoAtual <= minutoDesliga))) {
    deveLigarRele3 = true;
    break;  // Já achou um intervalo válido, não precisa continuar
  }
}

// Liga ou desliga o relé com base na verificação
if (deveLigarRele3 && !rele3Ligado) {
  Serial.println("Relé 3 LIGADO: Solução");
  digitalWrite(rele3, LOW);
  rele3Ligado = true;
} else if (!deveLigarRele3 && rele3Ligado) {
  Serial.println("Relé 3 DESLIGADO: Solução");
  digitalWrite(rele3, HIGH);
  rele3Ligado = false;
}

  int totalHorariosRele4 = sizeof(horarios_rele4) / sizeof(horarios_rele4[0]);
  bool deveLigarRele4 = false;

for (int i = 0; i < totalHorariosRele4; i++) {
  int horaLiga4 = horarios_rele4[i][0];
  int minutoLiga4 = horarios_rele4[i][1];
  int horaDesliga4 = horarios_rele4[i][2];
  int minutoDesliga4 = horarios_rele4[i][3];

  // Verifica se o horário atual está dentro de algum intervalo
  if ((horaAtual > horaLiga4 || (horaAtual == horaLiga4 && minutoAtual >= minutoLiga4)) &&
      (horaAtual < horaDesliga4 || (horaAtual == horaDesliga4 && minutoAtual <= minutoDesliga4))) {
    deveLigarRele4 = true;
    break;  // Já achou um intervalo válido, não precisa continuar
  }
}

// Liga ou desliga o relé com base na verificação
if (deveLigarRele4 && !rele4Ligado) {
  Serial.println("Relé 4 LIGADO: Peixe");
  digitalWrite(rele4, LOW);
  rele4Ligado = true;
} else if (!deveLigarRele4 && rele4Ligado) {
  Serial.println("Relé 4 DESLIGADO: Peixe");
  digitalWrite(rele4, HIGH);
  rele4Ligado = false;
}

// Esperar 5 segundos antes da próxima leitura
  delay(5000);

}
