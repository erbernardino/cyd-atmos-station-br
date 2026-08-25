---
id: cyd-atmos-concept-readme
title: "Atmos BR — Estação Meteorológica Aberta para ESP32 CYD"
type: concept
status: active
owner: "Emerson Bernardino"
last_reviewed: 2026-08-25
confidence: high
tags:
  - readme
  - esp32-cyd
  - open-source
relations:
  depends_on: []
  related_to:
    - "[[docs/index]]"
    - "[[docs/log]]"
  supersedes: []
---

# 🌦️ Atmos BR — Estação Meteorológica para ESP32 CYD

> Projeto Open-Source completo em C++ para a placa **ESP32 "Cheap Yellow Display" (CYD / ESP32-2432S028)**, com interface moderna, previsão meteorológica via **Open-Meteo**, relógio NTP e navegação touch em **Português do Brasil**.

---

## 📸 Funcionalidades

- **100% em Português do Brasil:** Dias da semana, condições climáticas, avisos de saúde e menus traduzidos.
- **Previsão em Tempo Real (Open-Meteo):** Sem chaves de API pagas ou cadastro.
- **Telas Informativas:**
  1. **Clima Atual:** Temperatura, Sensação Térmica, Mín/Máx, Vento, Umidade, Pressão, Qualidade do Ar e Dica do Dia.
  2. **Previsão Horária:** Detalhes das próximas 12 horas com probabilidade de precipitação.
  3. **Previsão de 7 Dias:** Semana completa com temperaturas máximas e mínimas.
  4. **Qualidade do Ar:** Níveis de AQI Europeu, PM2.5, PM10, Ozônio e Índice UV.
  5. **Status do Sistema:** Endereço IP local, fuso horário e configurações da estação.
- **Conexão Wi-Fi com Portal Cativo:** Configure sua rede e senha pelo celular sem precisar recompilar o código.
- **Modo Noturno (Eco Mode):** Redução automática do brilho da tela durante a noite.
- **Compatibilidade:** Modelos CYD 2.8" Resistivo ou Capacitivo com case 3D (ex: case Atmos do MakerWorld).

---

## 🛠️ Como Compilar e Gravar

### Pré-requisitos
* [PlatformIO IDE](https://platformio.org/) (extensão para VS Code ou CLI)
* Cabo de dados USB conectado à porta Micro-USB do seu ESP32 CYD.

### Compilação e Upload via Linha de Comando:
```bash
# Compilar o firmware
pio run

# Gravar no ESP32 conectado
pio run --target upload

# Abrir monitor serial
pio device monitor -b 115200
```

---

## 📂 Estrutura do Código

```text
├── include/
│   ├── Config.h            # Pinagem do CYD, cores da interface e estruturas de dados
│   ├── DisplayManager.h    # Renderização gráfica da tela TFT
│   ├── TouchHandler.h      # Driver do painel touch XPT2046
│   └── WeatherService.h    # Integração com as APIs do Open-Meteo
├── src/
│   ├── DisplayManager.cpp
│   ├── TouchHandler.cpp
│   ├── WeatherService.cpp
│   └── main.cpp            # Loop de execução, WiFiManager e NTP
├── docs/                   # Documentação OKF (Open Knowledge Format)
│   ├── index.md
│   └── log.md
└── platformio.ini          # Configuração do compilador e bibliotecas
```

---

## 📄 Licença
Distribuído sob a licença **MIT**. Sinta-se livre para usar, clonar, criar forks e customizar!
