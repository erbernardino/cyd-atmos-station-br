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
  - touch-debounce
  - bugfix
relations:
  depends_on:
    - "[[docs/index]]"
  related_to: []
  supersedes: []
---

# 📜 Histórico Cronológico de Alterações (Changelog)

Registro de auditoria e desenvolvimento do firmware de código aberto Atmos BR para o ESP32 CYD.

---

### [v1.3.0] — 2026-08-25
#### Correção de Toques Fantasmas no Painel Touch Resistivo
- **Eliminação de Ruído no GPIO 36:** Desativação da leitura por interrupção flutuante (IRQ) no pino 36, passando a realizar polling direto com validação de coordenadas.
- **Filtro de Pressão (Z-Threshold):** Rejeição de qualquer evento de toque com pressão `p.z < 600` ou coordenadas fora dos limites de calibração (`200 <= p <= 3900`).
- **Debounce de 600ms:** Implementação de intervalo de proteção de 600ms entre toques para estabilização da navegação entre as telas.

---

### [v1.2.0] — 2026-08-25
#### Tratamento e Normalização de Caracteres Acentuados (UTF-8)
- **Sanitização de Diacríticos:** Implementação da função `DisplayManager::sanitizeText()` para conversão de UTF-8 acentuado em ASCII puro.

---

### [v1.1.0] — 2026-08-25
#### Gerenciador Multi-Wi-Fi e Portal Web Embutido
- **Multi-Wi-Fi NVS & Portal Web:** Suporte a múltiplas redes e servidor na porta 80.
