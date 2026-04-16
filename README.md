# Sistema de Rastreabilidade Agrícola com Prevenção de Perdas

## Visão geral
Este projeto integra duas camadas:

- **Wokwi / ESP32**: monitoramento simulado de lote agrícola com sensores, OLED e alertas.
- **Python + Oracle**: persistência, rastreabilidade, consultas e relatórios.

## Objetivo
Demonstrar uma solução aplicada ao agronegócio para:
- monitorar variáveis ambientais;
- detectar movimentações em fases sensíveis;
- classificar risco de perda;
- registrar produtores, lotes e movimentações;
- manter histórico de rastreabilidade no Oracle;
- exportar dados em JSON.

## Estrutura
```text
sistema-rastreabilidade-agricola/
├─ README.md
├─ DOCUMENTACAO_TECNICA.md
├─ .gitignore
├─ sql/
│  ├─ 01_create_tables.sql
│  ├─ 02_create_sequences_indexes.sql
│  ├─ 03_create_trigger.sql
│  ├─ 04_populate.sql
│  ├─ 05_views.sql
│  ├─ 06_create_package_consultas.sql
│  ├─ 07_test_consultas.sql
│  └─ run_all.sql
├─ python/
│  └─ main.py
└─ wokwi/
   ├─ sketch.ino
   ├─ diagram.json
   └─ libraries.txt
```

## Wokwi
Simulação: https://wokwi.com/projects/461490404470609921

### Componentes
- ESP32
- DHT22
- PIR
- Botão
- OLED SSD1306
- LED verde, amarelo e vermelho
- Buzzer

### Comandos do Serial Monitor
- `help`
- `api normal`
- `api seca`
- `api chuva`
- `api transporte`
- `api vigoralto`
- `api vigorbaixo`
- `api auto`
- `status`

## Oracle
### Tabelas
- `PRODUTORES`
- `LOTES`
- `MOVIMENTACOES`

### Views
- `VW_LOTES_COMPLETO`
- `VW_HISTORICO_MOVIMENTACOES`
- `VW_RESUMO_PRODUCAO`

### Package
- `PKG_CONSULTAS_RASTREABILIDADE`

## Execução Oracle
No SQL Developer, execute:
1. `01_create_tables.sql`
2. `02_create_sequences_indexes.sql`
3. `03_create_trigger.sql`
4. `04_populate.sql`
5. `05_views.sql`
6. `06_create_package_consultas.sql`
7. `07_test_consultas.sql`

Ou rode `run_all.sql`.

## Execução Python
Instale:
```bash
pip install oracledb
```

Defina credenciais via variáveis de ambiente:
```bash
ORACLE_USER
ORACLE_PASSWORD
ORACLE_HOST
ORACLE_PORT
ORACLE_SID
```

Depois execute:
```bash
python main.py
```

## Observação importante
Não publique credenciais reais no GitHub.

## Integrantes
- Preencher nomes do grupo

## Repositório
- GitHub: preencher link
- Wokwi: https://wokwi.com/projects/461490404470609921
