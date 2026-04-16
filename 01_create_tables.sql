-- ============================================================
-- Script 01 - Criação das tabelas
-- ============================================================

BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE movimentacoes CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE lotes CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL;
END;
/

BEGIN
    EXECUTE IMMEDIATE 'DROP TABLE produtores CASCADE CONSTRAINTS';
EXCEPTION WHEN OTHERS THEN NULL;
END;
/

CREATE TABLE produtores (
    id_produtor NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    nome        VARCHAR2(100) NOT NULL,
    cpf_cnpj    VARCHAR2(20)  NOT NULL,
    telefone    VARCHAR2(20),
    cidade      VARCHAR2(80),
    estado      CHAR(2),
    CONSTRAINT uq_produtores_cpf_cnpj UNIQUE (cpf_cnpj)
);

CREATE TABLE lotes (
    id_lote      NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    cultura      VARCHAR2(50)   NOT NULL,
    area_ha      NUMBER(10,2)   NOT NULL,
    data_plantio DATE           NOT NULL,
    status       VARCHAR2(20)   DEFAULT 'PLANTADO' NOT NULL,
    id_produtor  NUMBER         NOT NULL,
    observacao   VARCHAR2(255),
    CONSTRAINT fk_lotes_produtores
      FOREIGN KEY (id_produtor) REFERENCES produtores(id_produtor),
    CONSTRAINT chk_lotes_status
      CHECK (status IN ('PLANTADO', 'CRESCIMENTO', 'COLHIDO', 'PROCESSADO', 'ENTREGUE')),
    CONSTRAINT chk_lotes_area_ha
      CHECK (area_ha > 0)
);

CREATE TABLE movimentacoes (
    id_movimentacao   NUMBER GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    id_lote           NUMBER        NOT NULL,
    status_anterior   VARCHAR2(20),
    status_novo       VARCHAR2(20)  NOT NULL,
    data_movimentacao DATE          DEFAULT SYSDATE NOT NULL,
    responsavel       VARCHAR2(100),
    observacao        VARCHAR2(255),
    CONSTRAINT fk_movimentacoes_lotes
      FOREIGN KEY (id_lote) REFERENCES lotes(id_lote) ON DELETE CASCADE
);
