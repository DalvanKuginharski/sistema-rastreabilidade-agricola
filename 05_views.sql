-- ============================================================
-- Script 05 - Views
-- ============================================================

CREATE OR REPLACE VIEW vw_lotes_completo AS
SELECT
    l.id_lote,
    l.cultura,
    l.area_ha,
    TO_CHAR(l.data_plantio, 'DD/MM/YYYY') AS data_plantio,
    l.status,
    l.observacao,
    p.id_produtor,
    p.nome AS produtor,
    p.cidade AS cidade,
    p.estado AS estado
FROM lotes l
LEFT JOIN produtores p ON l.id_produtor = p.id_produtor;

CREATE OR REPLACE VIEW vw_historico_movimentacoes AS
SELECT
    m.id_movimentacao,
    m.id_lote,
    l.cultura,
    l.area_ha,
    m.status_anterior,
    m.status_novo,
    TO_CHAR(m.data_movimentacao, 'DD/MM/YYYY HH24:MI') AS data_hora,
    m.responsavel,
    m.observacao
FROM movimentacoes m
JOIN lotes l ON m.id_lote = l.id_lote;

CREATE OR REPLACE VIEW vw_resumo_producao AS
SELECT
    cultura,
    status,
    COUNT(*) AS qtd_lotes,
    SUM(area_ha) AS area_total_ha,
    AVG(area_ha) AS area_media_ha
FROM lotes
GROUP BY cultura, status;
