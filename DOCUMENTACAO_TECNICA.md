# Documentação Técnica do Projeto
## Sistema de Rastreabilidade Agrícola com Prevenção de Perdas

## 1. Introdução
Projeto acadêmico que integra Python, Oracle e simulação embarcada aplicada ao agronegócio.

## 2. Problema
A falta de monitoramento contínuo pode gerar perdas associadas a:
- condições ambientais inadequadas;
- movimentações indevidas em fases críticas;
- falhas no acompanhamento do status do lote;
- ausência de histórico confiável.

## 3. Arquitetura
### 3.1 Camada de campo
Implementada em Wokwi com ESP32.

**Entradas**
- DHT22
- PIR
- botão

**Saídas**
- OLED
- LEDs
- buzzer
- Serial Monitor

### 3.2 Camada de gestão
Implementada em Python e Oracle.

**Funções**
- cadastro de produtores;
- cadastro de lotes;
- alteração de status;
- trigger de auditoria;
- consultas salvas;
- relatórios;
- exportação JSON.

## 4. Simulação no Wokwi
### Componentes
- ESP32
- DHT22
- PIR
- botão
- OLED SSD1306
- LEDs
- buzzer

### Pinos
- DHT22 → GPIO 15
- PIR → GPIO 13
- botão → GPIO 14
- LED verde → GPIO 27
- LED amarelo → GPIO 26
- LED vermelho → GPIO 25
- buzzer → GPIO 33
- OLED SDA → GPIO 21
- OLED SCL → GPIO 22

### Cenários
- `api normal`
- `api seca`
- `api chuva`
- `api transporte`
- `api vigoralto`
- `api vigorbaixo`
- `api auto`

## 5. Banco Oracle
### PRODUTORES
- id_produtor
- nome
- cpf_cnpj
- telefone
- cidade
- estado

### LOTES
- id_lote
- cultura
- area_ha
- data_plantio
- status
- id_produtor
- observacao

### MOVIMENTACOES
- id_movimentacao
- id_lote
- status_anterior
- status_novo
- data_movimentacao
- responsavel
- observacao

## 6. Regras de integridade
- PK em todas as tabelas
- FK entre LOTES e PRODUTORES
- FK entre MOVIMENTACOES e LOTES
- UNIQUE em CPF/CNPJ
- CHECK de status
- CHECK de área positiva

## 7. Trigger
A trigger `TRG_RASTREAR_STATUS` registra automaticamente movimentações quando o status do lote muda.

## 8. Índices
- índice por status
- índice por cultura
- índice por id_lote
- índice por id_produtor

## 9. Views
- `VW_LOTES_COMPLETO`
- `VW_HISTORICO_MOVIMENTACOES`
- `VW_RESUMO_PRODUCAO`

## 10. Package
- `pr_listar_produtores`
- `pr_listar_lotes`
- `pr_listar_movimentacoes_lote`
- `fn_area_total_por_status`
- `fn_qtd_lotes_produtor`

## 11. Integração com Python
A aplicação Python conecta no Oracle, executa operações CRUD, lista dados e exporta resumo em JSON.

## 12. Conclusão
O projeto demonstra como monitoramento de campo e persistência em banco podem compor uma solução didática de rastreabilidade e prevenção de perdas.
