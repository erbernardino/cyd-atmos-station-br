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
  - utf8-sanitization
relations:
  depends_on:
    - "[[docs/index]]"
  related_to: []
  supersedes: []
---

# 📜 Histórico Cronológico de Alterações (Changelog)

Registro de auditoria e desenvolvimento do firmware de código aberto Atmos BR para o ESP32 CYD.

---

### [v1.2.0] — 2026-08-25
#### Tratamento e Normalização de Caracteres Acentuados (UTF-8)
- **Sanitização de Diacríticos:** Implementação da função `DisplayManager::sanitizeText()` que converte sequências UTF-8 multi-byte acentuadas (como `é`, `á`, `ã`, `ç`, `ô`, `í`) em seus equivalentes ASCII limpos (`e`, `a`, `c`, `o`, `i`).
- **Compatibilidade com Fontes TFT:** Evita falhas de renderização e caracteres ausentes em nomes de cidades com acentos gráficos (ex: *Santo André*, *São Paulo*, *Brasília*, *Ribeirão Preto*).

---

### [v1.1.0] — 2026-08-25
#### Gerenciador Multi-Wi-Fi e Portal Web Embutido
- **Multi-Wi-Fi NVS:** Implementação do `StorageManager` com suporte à persistência de múltiplas redes Wi-Fi (SSID + Senha) na memória flash do ESP32 via biblioteca `Preferences`.
- **Conexão Inteligente:** Utilização do `WiFiMulti` para tentar conexão automática em todas as redes conhecidas memorizadas.
- **Portal Web Local:** Servidor HTTP embutido (`WebPortal`) na porta 80 com busca de cidades, gerenciamento de Wi-Fi e controle de brilho.
- **Particionamento:** Expansão para partição `huge_app.csv` (3MB Flash).

---

### [v1.0.0] — 2026-08-25
#### Inicialização do Projeto de Código Aberto
- **Scaffolding:** Criação da estrutura de projeto C++ no PlatformIO para o módulo ESP32-2432S028.
- **Display e UI:** Implementação do `DisplayManager` com tema escuro e cards modernos utilizando `TFT_eSPI`.
- **Navegação Touch:** Driver `TouchHandler` integrando o controlador SPI `XPT2046`.
- **Serviço Meteorológico:** Implementação do `WeatherService` com consumo da API do Open-Meteo em Português do Brasil.
