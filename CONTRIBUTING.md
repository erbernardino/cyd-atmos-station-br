---
id: cyd-atmos-procedure-contributing
title: "Procedimento — Como Contribuir com o Atmos BR"
type: procedure
status: active
owner: "Emerson Rocco Bernardino"
last_reviewed: 2026-08-27
confidence: high
tags: [contribuicao, git, workflow]
relations:
  depends_on: ["[[docs/index]]"]
  related_to: ["[[docs/log]]"]
  supersedes: []
---

# Como Contribuir

Obrigado por considerar contribuir com o Atmos BR! Este é um projeto hobby/DIY para ESP32 CYD, e contribuições são bem-vindas.

## Antes de começar

1. Verifique as [issues abertas](../../issues) para não duplicar esforço.
2. Para mudanças grandes (nova tela, nova arquitetura), abra uma issue primeiro para alinhar a ideia antes de codar.
3. Para bugs simples e melhorias pequenas, pode ir direto para o Pull Request.

## Ambiente de desenvolvimento

- [PlatformIO](https://platformio.org/) (CLI ou extensão VS Code)
- ESP32 "Cheap Yellow Display" (CYD / ESP32-2432S028) para testar fisicamente, ou pelo menos `pio run` para validar compilação

```bash
git clone https://github.com/erbernardino/cyd-atmos-station-br.git
cd cyd-atmos-station-br
pio run                     # valida que compila
pio run --target upload     # grava no seu CYD (com ele conectado via USB)
```

## Padrão de commits

Mensagens de commit em português, no formato `tipo: descrição curta`:

- `feat:` nova funcionalidade
- `fix:` correção de bug
- `docs:` mudança só de documentação
- `refactor:` mudança de código sem alterar comportamento
- `chore:` manutenção (dependências, CI, etc.)

Exemplo: `fix: corrige inversao do eixo X no driver de toque XPT2046`

Commits não devem conter assinatura, rodapé ou menção a ferramentas de IA usadas na geração do código.

## Enviando um Pull Request

1. Fork o repositório e crie uma branch a partir de `main`.
2. Garanta que `pio run` compila sem erros (o CI do GitHub Actions valida isso automaticamente no PR).
3. Descreva no PR o que mudou e por quê. Se alterar a interface visual, inclua uma foto/vídeo da tela do CYD.
4. Um mantenedor revisará e poderá pedir ajustes antes do merge.

## Reportando bugs

Abra uma issue usando o template de bug report, incluindo:
- Versão do firmware (mostrada na tela de Settings do CYD)
- Modelo exato da placa CYD (resistivo/capacitivo, com/sem áudio)
- Passos para reproduzir
- Log da porta serial, se possível (`pio device monitor -b 115200`)

## Código de conduta

Este projeto segue o [Código de Conduta](CODE_OF_CONDUCT.md). Ao contribuir, você concorda em respeitá-lo.
