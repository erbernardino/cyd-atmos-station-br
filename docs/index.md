---
id: cyd-atmos-index-home
title: "Documentação da Estação Meteorológica Atmos BR para ESP32 CYD"
type: index
status: active
owner: "Emerson Bernardino"
last_reviewed: 2026-08-25
confidence: high
tags:
  - esp32
  - cyd
  - open-source
  - weather-station
  - open-meteo
  - platformio
relations:
  depends_on: []
  related_to:
    - "[[docs/log]]"
  supersedes: []
---

# 🌦️ Atmos BR — Estação Meteorológica para ESP32 CYD

Projeto de código aberto em C++ (PlatformIO) desenvolvido para a placa **ESP32 CYD (*Cheap Yellow Display* / ESP32-2432S028)**, integrando previsão do tempo em tempo real, relógio sincronizado e qualidade do ar em **Português do Brasil**.

---

## 🧭 Mapa de Navegação

* **[[docs/log|Histórico Cronológico & Registro de Alterações (Log)]]**
* **Guia de Instalação e Compilação:** Consulte o arquivo `README.md` na raiz do projeto.

---

## 🛠️ Arquitetura do Sistema

```mermaid
graph TD
    A["ESP32 CYD"] --> B["Display TFT 2.8' (TFT_eSPI)"]
    A --> C["Touch Screen (XPT2046)"]
    A --> D["Sensor de Luz (LDR)"]
    A --> E["LED RGB Mood Light"]
    
    A --> F["Wi-Fi / Captive Portal (WiFiManager)"]
    F --> G["Open-Meteo REST API"]
    F --> H["NTP Server (pool.ntp.org)"]
    
    G --> I["Previsão Atual & Horária"]
    G --> J["Previsão 7 Dias"]
    G --> K["Qualidade do Ar (AQI / PM2.5)"]
```

---

## 📱 Telas da Aplicação

1. **Tela 1 — Clima Atual & Relógio:** Hora sincronizada NTP, temperatura atual, sensação térmica, mín/máx, vento, umidade, pressão e qualidade do ar.
2. **Tela 2 — Previsão Horária:** Detalhamento hora a hora das próximas 12 horas com probabilidade de chuva.
3. **Tela 3 — Previsão Semanal (7 Dias):** Temperaturas máximas/mínimas e condições para a semana toda com dias em PT-BR (DOM a SÁB).
4. **Tela 4 — Qualidade do Ar & Dicas:** Índices AQI Europeu, PM2.5, PM10, Ozônio e alertas climáticos (UV, rajadas de vento, geada).
5. **Tela 5 — Informações & Ajustes:** Endereço IP na rede, status de Wi-Fi e modo econômico noturno.
