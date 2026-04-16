-- ============================================================
-- Script 07 - Testes
-- ============================================================

PROMPT === OBJETOS CRIADOS ===
SELECT object_name, object_type, status
FROM user_objects
WHERE object_name IN (
    'VW_LOTES_COMPLETO',
    'VW_HISTORICO_MOVIMENTACOES',
    'VW_RESUMO_PRODUCAO',
    'PKG_CONSULTAS_RASTREABILIDADE'
)
ORDER BY object_type, object_name;

PROMPT === VIEW VW_LOTES_COMPLETO ===
SELECT * FROM vw_lotes_completo ORDER BY id_lote;

PROMPT === VIEW VW_HISTORICO_MOVIMENTACOES ===
SELECT * FROM vw_historico_movimentacoes ORDER BY id_movimentacao;

PROMPT === VIEW VW_RESUMO_PRODUCAO ===
SELECT * FROM vw_resumo_producao ORDER BY cultura, status;

PROMPT === PACKAGE: PR_LISTAR_PRODUTORES ===
VAR rc REFCURSOR;
EXEC pkg_consultas_rastreabilidade.pr_listar_produtores(:rc);
PRINT rc;

PROMPT === PACKAGE: PR_LISTAR_LOTES ===
VAR rc REFCURSOR;
EXEC pkg_consultas_rastreabilidade.pr_listar_lotes(NULL, NULL, :rc);
PRINT rc;

PROMPT === PACKAGE: FN_AREA_TOTAL_POR_STATUS ===
SELECT pkg_consultas_rastreabilidade.fn_area_total_por_status('PLANTADO') AS area_total_plantado
FROM dual;
