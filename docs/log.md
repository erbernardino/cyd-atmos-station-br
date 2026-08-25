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
