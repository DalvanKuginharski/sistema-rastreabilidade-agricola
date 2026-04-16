-- ============================================================
-- Script 03 - Trigger
-- ============================================================

CREATE OR REPLACE TRIGGER trg_rastrear_status
AFTER UPDATE OF status ON lotes
FOR EACH ROW
WHEN (OLD.status != NEW.status)
BEGIN
    INSERT INTO movimentacoes (
        id_lote,
        status_anterior,
        status_novo,
        data_movimentacao,
        observacao
    ) VALUES (
        :NEW.id_lote,
        :OLD.status,
        :NEW.status,
        SYSDATE,
        'Atualização automática via sistema'
    );
END;
/
