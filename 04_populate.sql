-- ============================================================
-- Script 04 - Dados iniciais
-- ============================================================

DELETE FROM movimentacoes;
DELETE FROM lotes;
DELETE FROM produtores;
COMMIT;

INSERT INTO produtores (nome, cpf_cnpj, telefone, cidade, estado)
VALUES ('João da Silva', '123.456.789-00', '(16) 99999-1111', 'Ribeirão Preto', 'SP');

INSERT INTO produtores (nome, cpf_cnpj, telefone, cidade, estado)
VALUES ('Fazenda Boa Vista LTDA', '12.345.678/0001-99', '(64) 98888-2222', 'Rio Verde', 'GO');

INSERT INTO produtores (nome, cpf_cnpj, telefone, cidade, estado)
VALUES ('Maria Aparecida Costa', '987.654.321-00', '(41) 97777-3333', 'Castro', 'PR');

INSERT INTO lotes (cultura, area_ha, data_plantio, status, id_produtor, observacao)
VALUES ('cana-de-acucar', 120.50, TO_DATE('2024-04-10', 'YYYY-MM-DD'), 'COLHIDO',
        (SELECT id_produtor FROM produtores WHERE cpf_cnpj = '123.456.789-00'),
        'Lote com colheita concluída');

INSERT INTO lotes (cultura, area_ha, data_plantio, status, id_produtor, observacao)
VALUES ('soja', 85.00, TO_DATE('2024-09-15', 'YYYY-MM-DD'), 'CRESCIMENTO',
        (SELECT id_produtor FROM produtores WHERE cpf_cnpj = '12.345.678/0001-99'),
        'Lote em desenvolvimento');

INSERT INTO lotes (cultura, area_ha, data_plantio, status, id_produtor, observacao)
VALUES ('milho', 200.75, TO_DATE('2024-08-01', 'YYYY-MM-DD'), 'PROCESSADO',
        (SELECT id_produtor FROM produtores WHERE cpf_cnpj = '12.345.678/0001-99'),
        'Lote já enviado para processamento');

INSERT INTO lotes (cultura, area_ha, data_plantio, status, id_produtor, observacao)
VALUES ('cafe', 45.00, TO_DATE('2023-11-20', 'YYYY-MM-DD'), 'ENTREGUE',
        (SELECT id_produtor FROM produtores WHERE cpf_cnpj = '987.654.321-00'),
        'Entrega concluída');

INSERT INTO lotes (cultura, area_ha, data_plantio, status, id_produtor, observacao)
VALUES ('algodao', 310.00, TO_DATE('2024-07-05', 'YYYY-MM-DD'), 'PLANTADO',
        (SELECT id_produtor FROM produtores WHERE cpf_cnpj = '123.456.789-00'),
        'Lote recém-plantado');

COMMIT;
