---
id: cyd-atmos-report-log
title: "Registro Cronológico de Alterações — Atmos BR"
type: report
status: active
owner: "Emerson Bernardino"
last_reviewed: 2026-08-27
confidence: high
tags:
  - changelog
  - technical-log
  - swiss-minimalist
  - dieter-rams
  - ui-redesign
relations:
  depends_on:
    - "[[docs/index]]"
  related_to: []
  supersedes: []
---

# 📜 Histórico Cronológico de Alterações (Changelog)

Registro de auditoria e desenvolvimento do firmware de código aberto Atmos BR para o ESP32 CYD.

---

### [v1.6.0] — 2026-08-27
#### Prontidão Open-Source: Licença, Segurança, CI e Limpeza
- **Licença MIT:** Arquivo `LICENSE` adicionado na raiz (antes só declarada no README, sem valor legal).
- **Autenticação no Portal Web:** HTTP Basic Auth (`PORTAL_AUTH_USER`/`PORTAL_AUTH_PASS` em `Config.h`) protegendo todos os endpoints do `WebPortal` — antes qualquer dispositivo na mesma rede Wi-Fi podia ler/alterar redes memorizadas e forçar restart sem credencial.
- **Reconexão Automática de Wi-Fi:** `loop()` agora detecta queda de conexão (`WiFi.status() != WL_CONNECTED`) e tenta reconectar via `WiFiMulti` a cada 30s, sem precisar reiniciar o dispositivo.
- **Indicador de Dado Desatualizado:** Tela de Settings sinaliza "DADO DESATUALIZADO" quando a última atualização de clima bem-sucedida passou de 2 ciclos (30 min).
- **Dependências Git Fixadas:** `XPT2046_Touchscreen` e `WiFiManager` deixaram de apontar para o HEAD do branch (risco de quebra silenciosa) e passaram a usar tag fixa no `platformio.ini`.
- **CI de Build:** GitHub Actions (`.github/workflows/build.yml`) valida `pio run` a cada push/PR.
- **Infraestrutura de Comunidade:** `CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`, templates de issue (bug/feature) e de Pull Request.
- **Limpeza de Código Morto:** Removidos `drawHeader()` (stub vazio nunca chamado) e defines não utilizados (`COLOR_CYAN`, `COLOR_BLUE`) de `Config.h`.
- **Higiene de Repositório:** Mockups de referência (`estilos/*.jpg`, ~7.1MB) removidos do versionamento (mantidos localmente, ignorados via `.gitignore`); `screenshot.png` movido para `docs/`.

---

### [v1.5.0] — 2026-08-26
#### Seletor de Tema Visual, Sprites de Clima e LED por Condição
- **Tema Pixel Art (Retro):** Segunda opção visual completa nas 5 telas, com gradiente de céu na tela "Agora" e sprites 16x16 reais (sol/lua/nuvem/chuva/tempestade/nevoeiro) via bitmap `PROGMEM`.
- **Seleção de Tema:** Portal web (dropdown) e toque direto na tela de Settings do CYD, persistido na NVS.
- **LED RGB por Condição Climática:** Cor do LED reflete o tempo atual (âmbar/azul/branco/ciano/vermelho), respeitando o toggle liga/desliga existente.
- **Atalho de Toque para Settings:** Ícone no canto superior direito, presente em todas as telas, pula direto para a tela de configurações.
- **Correção de Calibração de Toque:** Eixo X do painel resistivo XPT2046 estava fisicamente invertido em relação à tela — corrigido em `TouchHandler.cpp`.

---

### [v1.4.0] — 2026-08-25
#### Implementação do Estilo Visual Suíço / Dieter Rams Minimalist (Estilo #5)
- **Redesign Visual Completo:** Reformulação da interface gráfica com base nos princípios de design industrial funcionalista de Dieter Rams e tipografia suíça.
- **Tipografia Helvetica Escalar:** Substituição de fontes de 7 segmentos por numerais limpos em escala 2x com símbolo geométrico de grau (`°`).
- **Acento Dieter Rams:** Ponto âmbar/laranja minimalista de equilíbrio visual central.
- **Grade Hairline de 4 Quadrantes:** Divisão geométrica nítida em preto absoluto (`#000000`) para Vento, Umidade, Pressão e Qualidade do Ar.
- **Galeria de Estilos:** Inclusão de 12 conceitos visuais de interface no diretório `estilos/`.

---

### [v1.3.0] — 2026-08-25
#### Correção de Toques Fantasmas no Painel Touch Resistivo
- **Eliminação de Ruído no GPIO 36:** Polling direto via SPI sem interrupções espúrias.
- **Filtro de Pressão (Z-Threshold):** Rejeição de eventos com pressão `p.z < 600`.
- **Debounce de 600ms:** Estabilização da troca de telas.

---

### [v1.2.0] — 2026-08-25
#### Tratamento e Normalização de Caracteres Acentuados (UTF-8)
- **Sanitização de Diacríticos:** `DisplayManager::sanitizeText()` para normalização de acentos.

---

### [v1.1.0] — 2026-08-25
#### Gerenciador Multi-Wi-Fi e Portal Web Embutido
- **Multi-Wi-Fi NVS & Portal Web:** Suporte a múltiplas redes e servidor na porta 80.
