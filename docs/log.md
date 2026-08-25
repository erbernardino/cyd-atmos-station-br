---
id: cyd-atmos-report-log
title: "Registro Cronológico de Alterações — Atmos BR"
type: report
status: active
owner: "Emerson Bernardino"
last_reviewed: 2026-08-25
confidence: high
tags:
  - changelog
  - technical-log
  - release
relations:
  depends_on:
    - "[[docs/index]]"
  related_to: []
  supersedes: []
---

# 📜 Histórico Cronológico de Alterações (Changelog)

Registro de auditoria e desenvolvimento do firmware de código aberto Atmos BR para o ESP32 CYD.

---

### [v1.0.0] — 2026-08-25
#### Inicialização do Projeto de Código Aberto
- **Scaffolding:** Criação da estrutura de projeto C++ no PlatformIO para o módulo ESP32-2432S028.
- **Display e UI:** Implementação do `DisplayManager` com tema escuro e cards modernos utilizando `TFT_eSPI`.
- **Navegação Touch:** Driver `TouchHandler` integrando o controlador SPI `XPT2046` para transição de páginas.
- **Serviço Meteorológico:** Implementação do `WeatherService` com consumo da API do Open-Meteo (tempo real, previsão horária de 12h, previsão de 7 dias e qualidade do ar) com códigos traduzidos para Português do Brasil.
- **Configuração de Rede:** Integração com `WiFiManager` para portal de configuração sem credenciais hardcoded.
- **Controle de Brilho & Eco Mode:** Suporte ao controle PWM no backlight TFT e redução de brilho noturna.
