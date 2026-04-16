import json
import os
from datetime import datetime
from pathlib import Path

try:
    import oracledb
except ImportError:
    oracledb = None

BASE_DIR = Path(__file__).resolve().parent
DATA_DIR = BASE_DIR / "dados"
JSON_FILE = DATA_DIR / "resumo_rastreabilidade.json"
LOG_FILE = DATA_DIR / "logs_rastreabilidade.txt"

DB_CONFIG = {
    "user": os.getenv("ORACLE_USER", "SEU_USUARIO"),
    "password": os.getenv("ORACLE_PASSWORD", "SUA_SENHA"),
    "host": os.getenv("ORACLE_HOST", "oracle.fiap.com.br"),
    "port": int(os.getenv("ORACLE_PORT", "1521")),
    "sid": os.getenv("ORACLE_SID", "ORCL"),
}

STATUS_LOTE = ("PLANTADO", "CRESCIMENTO", "COLHIDO", "PROCESSADO", "ENTREGUE")


def titulo(texto: str) -> None:
    print("\n" + "=" * 80)
    print(texto.center(80))
    print("=" * 80)


def garantir_estrutura() -> None:
    DATA_DIR.mkdir(exist_ok=True)
    if not JSON_FILE.exists():
        JSON_FILE.write_text("[]", encoding="utf-8")
    if not LOG_FILE.exists():
        LOG_FILE.write_text("=== LOG DO SISTEMA ===\n", encoding="utf-8")


def registrar_log(mensagem: str) -> None:
    agora = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
    with open(LOG_FILE, "a", encoding="utf-8") as arq:
        arq.write(f"[{agora}] {mensagem}\n")


def conectar_oracle():
    if oracledb is None:
        print("Biblioteca oracledb não instalada. Use: pip install oracledb")
        return None
    try:
        dsn = oracledb.makedsn(
            host=DB_CONFIG["host"],
            port=DB_CONFIG["port"],
            sid=DB_CONFIG["sid"]
        )
        return oracledb.connect(
            user=DB_CONFIG["user"],
            password=DB_CONFIG["password"],
            dsn=dsn
        )
    except Exception as erro:
        print(f"Erro ao conectar no Oracle: {erro}")
        registrar_log(f"Erro de conexão Oracle: {erro}")
        return None


def testar_conexao():
    titulo("TESTE DE CONEXÃO")
    conn = conectar_oracle()
    if conn is None:
        print("Falha na conexão.")
        return
    conn.close()
    print("Conexão com Oracle realizada com sucesso.")


def listar_produtores():
    conn = conectar_oracle()
    if conn is None:
        return
    sql = "SELECT id_produtor, nome, cpf_cnpj, cidade, estado FROM produtores ORDER BY id_produtor"
    cur = conn.cursor()
    try:
        cur.execute(sql)
        for linha in cur.fetchall():
            print(linha)
    finally:
        cur.close()
        conn.close()


def listar_lotes():
    conn = conectar_oracle()
    if conn is None:
        return
    sql = """
    SELECT l.id_lote, l.cultura, l.area_ha, l.status, p.nome
    FROM lotes l
    JOIN produtores p ON p.id_produtor = l.id_produtor
    ORDER BY l.id_lote
    """
    cur = conn.cursor()
    try:
        cur.execute(sql)
        for linha in cur.fetchall():
            print(linha)
    finally:
        cur.close()
        conn.close()


def exportar_resumo_json():
    conn = conectar_oracle()
    if conn is None:
        return
    cur = conn.cursor()
    try:
        cur.execute("SELECT COUNT(*) FROM produtores")
        qtd_produtores = cur.fetchone()[0]
        cur.execute("SELECT COUNT(*) FROM lotes")
        qtd_lotes = cur.fetchone()[0]
        cur.execute("SELECT COUNT(*) FROM movimentacoes")
        qtd_mov = cur.fetchone()[0]

        resumo = [{
            "data_exportacao": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "totais": {
                "produtores": qtd_produtores,
                "lotes": qtd_lotes,
                "movimentacoes": qtd_mov
            }
        }]
        with open(JSON_FILE, "w", encoding="utf-8") as arq:
            json.dump(resumo, arq, ensure_ascii=False, indent=4)
        print(f"Resumo exportado em: {JSON_FILE}")
    finally:
        cur.close()
        conn.close()


def menu():
    titulo("SISTEMA DE RASTREABILIDADE AGRÍCOLA")
    print("1 - Testar conexão")
    print("2 - Listar produtores")
    print("3 - Listar lotes")
    print("4 - Exportar resumo JSON")
    print("0 - Sair")


def main():
    garantir_estrutura()
    while True:
        menu()
        opcao = input("\nEscolha uma opção: ").strip()
        if opcao == "1":
            testar_conexao()
        elif opcao == "2":
            listar_produtores()
        elif opcao == "3":
            listar_lotes()
        elif opcao == "4":
            exportar_resumo_json()
        elif opcao == "0":
            break
        else:
            print("Opção inválida.")
        input("\nPressione ENTER para continuar...")


if __name__ == "__main__":
    main()
