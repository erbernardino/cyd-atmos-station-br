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
  - multi-wifi
  - web-portal
relations:
  depends_on:
    - "[[docs/index]]"
  related_to: []
  supersedes: []
---

# 📜 Histórico Cronológico de Alterações (Changelog)

Registro de auditoria e desenvolvimento do firmware de código aberto Atmos BR para o ESP32 CYD.

---

### [v1.1.0] — 2026-08-25
#### Gerenciador Multi-Wi-Fi e Portal Web Embutido
- **Multi-Wi-Fi NVS:** Implementação do `StorageManager` com suporte à persistência de múltiplas redes Wi-Fi (SSID + Senha) na memória flash do ESP32 via biblioteca `Preferences`.
- **Conexão Inteligente:** Utilização do `WiFiMulti` para tentar conexão automática em todas as redes conhecidas memorizadas.
- **Portal Web Local:** Servidor HTTP embutido (`WebPortal`) na porta 80 com:
  - Lista e exclusão de redes Wi-Fi salvas.
  - Escaneamento de redes sem fio próximas em tempo real.
  - Adição de novos pontos de acesso sem necessidade de recompilação.
  - Busca de cidades via API Geocoding do Open-Meteo.
  - Controle de brilho e programação de Modo Noturno (Eco).
- **Particionamento:** Expansão para partição `huge_app.csv` (3MB Flash) garantindo espaço para o servidor web e bibliotecas gráficas.

---

### [v1.0.0] — 2026-08-25
#### Inicialização do Projeto de Código Aberto
- **Scaffolding:** Criação da estrutura de projeto C++ no PlatformIO para o módulo ESP32-2432S028.
- **Display e UI:** Implementação do `DisplayManager` com tema escuro e cards modernos utilizando `TFT_eSPI`.
- **Navegação Touch:** Driver `TouchHandler` integrando o controlador SPI `XPT2046` para transição de páginas.
- **Serviço Meteorológico:** Implementação do `WeatherService` com consumo da API do Open-Meteo em Português do Brasil.
