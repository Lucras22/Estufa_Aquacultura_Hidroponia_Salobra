# 🌱 Estufa Automatizada com Monitoramento via Telegram

Este projeto apresenta o desenvolvimento de uma **estufa automatizada**, com foco na **coleta de dados ambientais** e no **monitoramento da qualidade da água** de dois tanques integrados ao sistema. Além do monitoramento, o sistema realiza o **controle automático de válvulas e da irrigação** por meio de um módulo de relés, seguindo horários predefinidos para garantir um funcionamento eficiente e confiável. O acompanhamento remoto é feito via um **Bot do Telegram**, permitindo o acesso contínuo às informações das variáveis ambientais e operacionais da estufa.

---

## 📦 Materiais Utilizados

### 🎛 Microcontrolador
- ESP32 (com Wi-Fi integrado)

### 🌡 Sensores
- 2 × DHT22 — Temperatura e Umidade (ambiente interno e externo)
- 2 × DS18B20 — Temperatura da água (um para cada tanque)
- 2 × Sensor de pH com Módulo PH-4502C (tanque de peixes e tanque de filtragem)
- 2 × TDS Meter V1.0 — Medição de condutividade (um para cada tanque)

### ⚙ Atuadores
- 1 × Módulo Relé de 4 canais (controle de bombas e válvulas)

### 📡 Comunicação
- Bot do Telegram via Wi-Fi (ESP32)

### 🧰 Outros
- Protoboard  
- Jumpers (cabos de conexão)  
- Fontes de alimentação apropriadas  
- Estrutura física: estufa, tanque de peixes, tanque de filtragem

---

## ⚙️ Metodologia

### 🌬 Monitoramento Ambiental
Dois sensores **DHT22** monitoram a **temperatura** e a **umidade relativa do ar**:
- 1 interno (estufa)
- 1 externo (ambiente ao redor)

Essa configuração permite analisar as variações climáticas e identificar possíveis impactos no cultivo.

### 💧 Monitoramento da Qualidade da Água
Cada tanque possui:
- **DS18B20**: mede a **temperatura da água**
- **Sensor de pH**: mede o nível de **acidez**
- **TDS Meter**: mede a **condutividade elétrica**, indicando a quantidade de sólidos dissolvidos

Essas medições são essenciais para manter a saúde dos organismos aquáticos e garantir a qualidade da água.

### 🕹 Controle de Atuadores
O sistema conta com uma **placa de relés (4 canais)** para o acionamento automático de **bombas e válvulas**.

O controle ocorre **em horários específicos e definidos no código**, de forma **independente das leituras dos sensores**.

> Isso garante uma operação estável e previsível, mesmo sem a tomada de decisões em tempo real.

### 📲 Comunicação e Monitoramento Remoto
A cada **5 horas**, o ESP32 envia os dados para um **Bot do Telegram**.

O usuário pode acompanhar, em tempo quase real, o funcionamento da estufa, os parâmetros ambientais e a qualidade da água.

---

## 🔁 Fluxo de Operação

1. Os sensores coletam dados ambientais e de qualidade da água.  
2. O ESP32 organiza e processa as leituras.  
3. A cada 5 horas, os dados são enviados ao **Telegram**.  
4. Os relés são acionados automaticamente conforme horários programados.

> ✅ A coleta de dados serve exclusivamente para **monitoramento**.  
> ⚙️ As ações de controle são realizadas **apenas com base nos horários definidos**, sem depender das medições.

Essa arquitetura separa claramente **monitoramento** e **controle**, resultando em um sistema confiável, seguro e de fácil manutenção.

---

## 🔌 Diagrama de Ligação Elétrica do Controle de Válvulas e Irrigação

### ⚙️ Ligações Elétricas

#### 🔌 Alimentação AC (220V)
- **Fase (L)**: conectada aos relés (COM dos SSR)
- **Neutro (N)**: conectado diretamente ao fio neutro de cada válvula e bomba

#### 🔧 Relé 1 (pino 18 do ESP32)
- **COM** → fase 220V  
- **NO** → todos os fios de **abertura das válvulas da solução** e **fechamento das válvulas dos peixes**

#### 🔧 Relé 2 (pino 19 do ESP32)
- **COM** → fase 220V  
- **NO** → todos os fios de **fechamento das válvulas da solução** e **abertura das válvulas dos peixes**

#### 🔧 Relé 3 (pino 23 do ESP32)
- **COM** → fase 220V  
- **NO** → ligado ao fio fase das **6 bombas da solução**

#### 🔧 Relé 4 (pino 25 do ESP32)
- **COM** → fase 220V  
- **NO** → ligado ao fio fase da **bomba dos peixes**

---

## 📬 Contribuições
Contribuições são bem-vindas! Sinta-se à vontade para abrir uma issue ou pull request com melhorias, correções ou sugestões.
