-- ============================================================
-- Script 02 - Índices
-- ============================================================

BEGIN EXECUTE IMMEDIATE 'DROP INDEX idx_lotes_status'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP INDEX idx_lotes_cultura'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP INDEX idx_mov_id_lote'; EXCEPTION WHEN OTHERS THEN NULL; END;
/
BEGIN EXECUTE IMMEDIATE 'DROP INDEX idx_lotes_produtor'; EXCEPTION WHEN OTHERS THEN NULL; END;
/

CREATE INDEX idx_lotes_status   ON lotes(status);
CREATE INDEX idx_lotes_cultura  ON lotes(cultura);
CREATE INDEX idx_mov_id_lote    ON movimentacoes(id_lote);
CREATE INDEX idx_lotes_produtor ON lotes(id_produtor);
