-- ============================================================
-- Script 06 - Package de consultas
-- ============================================================

CREATE OR REPLACE PACKAGE pkg_consultas_rastreabilidade AS
    PROCEDURE pr_listar_produtores (p_cursor OUT SYS_REFCURSOR);
    PROCEDURE pr_listar_lotes (
        p_status  IN VARCHAR2 DEFAULT NULL,
        p_cultura IN VARCHAR2 DEFAULT NULL,
        p_cursor  OUT SYS_REFCURSOR
    );
    PROCEDURE pr_listar_movimentacoes_lote (
        p_id_lote IN NUMBER,
        p_cursor  OUT SYS_REFCURSOR
    );
    FUNCTION fn_area_total_por_status (p_status IN VARCHAR2) RETURN NUMBER;
    FUNCTION fn_qtd_lotes_produtor (p_id_produtor IN NUMBER) RETURN NUMBER;
END pkg_consultas_rastreabilidade;
/
SHOW ERRORS PACKAGE pkg_consultas_rastreabilidade;

CREATE OR REPLACE PACKAGE BODY pkg_consultas_rastreabilidade AS

    PROCEDURE pr_listar_produtores (p_cursor OUT SYS_REFCURSOR) IS
    BEGIN
        OPEN p_cursor FOR
            SELECT id_produtor, nome, cpf_cnpj, telefone, cidade, estado
            FROM produtores
            ORDER BY nome;
    END pr_listar_produtores;

    PROCEDURE pr_listar_lotes (
        p_status  IN VARCHAR2 DEFAULT NULL,
        p_cultura IN VARCHAR2 DEFAULT NULL,
        p_cursor  OUT SYS_REFCURSOR
    ) IS
    BEGIN
        OPEN p_cursor FOR
            SELECT
                l.id_lote,
                l.cultura,
                l.area_ha,
                TO_CHAR(l.data_plantio, 'DD/MM/YYYY') AS data_plantio,
                l.status,
                l.observacao,
                p.id_produtor,
                p.nome AS produtor,
                p.cidade,
                p.estado
            FROM lotes l
            JOIN produtores p ON p.id_produtor = l.id_produtor
            WHERE (p_status IS NULL OR l.status = p_status)
              AND (p_cultura IS NULL OR l.cultura = p_cultura)
            ORDER BY l.id_lote;
    END pr_listar_lotes;

    PROCEDURE pr_listar_movimentacoes_lote (
        p_id_lote IN NUMBER,
        p_cursor  OUT SYS_REFCURSOR
    ) IS
    BEGIN
        OPEN p_cursor FOR
            SELECT
                m.id_movimentacao,
                m.id_lote,
                m.status_anterior,
                m.status_novo,
                TO_CHAR(m.data_movimentacao, 'DD/MM/YYYY HH24:MI') AS data_hora,
                m.responsavel,
                m.observacao
            FROM movimentacoes m
            WHERE m.id_lote = p_id_lote
            ORDER BY m.data_movimentacao DESC;
    END pr_listar_movimentacoes_lote;

    FUNCTION fn_area_total_por_status (p_status IN VARCHAR2) RETURN NUMBER IS
        v_total NUMBER := 0;
    BEGIN
        SELECT NVL(SUM(area_ha), 0)
          INTO v_total
          FROM lotes
         WHERE status = p_status;
        RETURN v_total;
    END fn_area_total_por_status;

    FUNCTION fn_qtd_lotes_produtor (p_id_produtor IN NUMBER) RETURN NUMBER IS
        v_qtd NUMBER := 0;
    BEGIN
        SELECT COUNT(*)
          INTO v_qtd
          FROM lotes
         WHERE id_produtor = p_id_produtor;
        RETURN v_qtd;
    END fn_qtd_lotes_produtor;

END pkg_consultas_rastreabilidade;
/
SHOW ERRORS PACKAGE BODY pkg_consultas_rastreabilidade;
