#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>

const char* ID_PRODUTOR = "P001";
const char* ID_LOTE = "L001";
const char* CULTURA = "cana-de-acucar";

const char* STATUS_LOTE[] = {
  "PLANTADO", "CRESCIMENTO", "COLHIDO", "PROCESSADO", "ENTREGUE"
};
const int TOTAL_STATUS = sizeof(STATUS_LOTE) / sizeof(STATUS_LOTE[0]);

const int PIN_DHT = 15;
const int PIN_PIR = 13;
const int PIN_BOTAO = 14;
const int PIN_LED_VERDE = 27;
const int PIN_LED_AMARELO = 26;
const int PIN_LED_VERMELHO = 25;
const int PIN_BUZZER = 33;

const int OLED_SDA = 21;
const int OLED_SCL = 22;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;

const float TEMP_MIN_ATENCAO = 20.0;
const float TEMP_MAX_ATENCAO = 35.0;
const float HUM_MIN_ATENCAO = 40.0;
const float HUM_MAX_ATENCAO = 70.0;
const float TEMP_MIN_ALERTA = 18.0;
const float TEMP_MAX_ALERTA = 38.0;
const float HUM_MIN_ALERTA = 30.0;
const float HUM_MAX_ALERTA = 80.0;
const float NDVI_MIN_ATENCAO = 0.60;
const float NDVI_MIN_ALERTA = 0.45;
const float EVI_MIN_ATENCAO = 0.35;
const float EVI_MIN_ALERTA = 0.25;

const unsigned long INTERVALO_LEITURA = 2000;
const unsigned long TEMPO_DEBOUNCE = 120;
const unsigned long INTERVALO_TELA = 3000;

enum NivelRisco { NIVEL_NORMAL, NIVEL_ATENCAO, NIVEL_ALERTA };

struct CenarioAgroAPI {
  String nome; float temperatura; float umidade; float chuvaMm;
  float ventoMs; int nuvensPct; float ndvi; float evi; bool movimento;
};

struct Telemetria {
  float temperatura; float umidade; float chuvaMm; float ventoMs;
  int nuvensPct; float ndvi; float evi; bool movimento;
  String statusLote; NivelRisco nivel; String acao;
};

DHTesp dhtSensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long ultimaLeitura = 0, ultimoDebounce = 0, ultimaTrocaTela = 0;
bool leituraBotaoAnterior = HIGH, estadoBotaoEstavel = HIGH;
int indiceStatusLote = 0, paginaTela = 0;
bool modoSimulado = false;
String nomeCenarioAtual = "AUTO_SENSORES";

CenarioAgroAPI cenarioAtual = {"AUTO_SENSORES", 0,0,0,0,0,0,0,false};
CenarioAgroAPI CENARIO_NORMAL = {"CLIMAPI_NORMAL", 27.0, 62.0, 1.0, 2.5, 35, 0.78, 0.52, false};
CenarioAgroAPI CENARIO_SECA = {"CLIMAPI_SECA", 39.0, 28.0, 0.0, 5.8, 10, 0.46, 0.24, false};
CenarioAgroAPI CENARIO_CHUVA = {"CLIMAPI_CHUVA", 24.0, 89.0, 22.0, 4.0, 92, 0.72, 0.48, false};
CenarioAgroAPI CENARIO_TRANSPORTE = {"LOGISTICA_MOV", 30.0, 55.0, 0.0, 3.0, 20, 0.70, 0.45, true};
CenarioAgroAPI CENARIO_VIGOR_ALTO = {"SATVEG_VIGOR_ALTO", 26.0, 68.0, 3.0, 2.2, 45, 0.83, 0.58, false};
CenarioAgroAPI CENARIO_VIGOR_BAIXO = {"SATVEG_VIGOR_BAIXO", 34.0, 38.0, 0.0, 4.8, 18, 0.39, 0.19, false};

Telemetria ultimaTelemetria = {0.0,0.0,0.0,0.0,0,0.65,0.40,false,"PLANTADO",NIVEL_NORMAL,"Operacao dentro do padrao"};

String statusLoteAtual() { return String(STATUS_LOTE[indiceStatusLote]); }
bool faseLogisticaSensivel(const String& s) { return s == "COLHIDO" || s == "PROCESSADO" || s == "ENTREGUE"; }
String textoNivel(NivelRisco n) { if (n == NIVEL_NORMAL) return "NORMAL"; if (n == NIVEL_ATENCAO) return "ATENCAO"; return "ALERTA"; }
String textoRiscoPerda(NivelRisco n) { if (n == NIVEL_NORMAL) return "BAIXO"; if (n == NIVEL_ATENCAO) return "MEDIO"; return "ALTO"; }

String acaoRecomendada(NivelRisco nivel, bool movimento, const String& statusLote) {
  if (nivel == NIVEL_ALERTA && movimento && faseLogisticaSensivel(statusLote)) return "Inspecionar lote e validar transporte";
  if (nivel == NIVEL_ALERTA) return "Inspecionar lote imediatamente";
  if (nivel == NIVEL_ATENCAO && movimento) return "Verificar movimentacao e rastreabilidade";
  if (nivel == NIVEL_ATENCAO) return "Monitorar lote e corrigir variacao";
  return "Operacao dentro do padrao";
}

NivelRisco avaliarRisco(float temperatura, float umidade, bool movimento, const String& statusLote, float ndvi, float evi) {
  bool ambienteAlerta = temperatura < TEMP_MIN_ALERTA || temperatura > TEMP_MAX_ALERTA || umidade < HUM_MIN_ALERTA || umidade > HUM_MAX_ALERTA;
  bool ambienteAtencao = temperatura < TEMP_MIN_ATENCAO || temperatura > TEMP_MAX_ATENCAO || umidade < HUM_MIN_ATENCAO || umidade > HUM_MAX_ATENCAO;
  bool vigorAlerta = ndvi < NDVI_MIN_ALERTA || evi < EVI_MIN_ALERTA;
  bool vigorAtencao = ndvi < NDVI_MIN_ATENCAO || evi < EVI_MIN_ATENCAO;
  if (ambienteAlerta || vigorAlerta) return NIVEL_ALERTA;
  if (movimento && faseLogisticaSensivel(statusLote)) return NIVEL_ALERTA;
  if (ambienteAtencao || vigorAtencao || movimento) return NIVEL_ATENCAO;
  return NIVEL_NORMAL;
}

void aplicarCenario(const CenarioAgroAPI& c) {
  modoSimulado = true;
  cenarioAtual = c;
  nomeCenarioAtual = c.nome;
  Serial.println("\n>>> CENARIO AGRO APLICADO <<<");
  Serial.print("Cenario: "); Serial.println(c.nome); Serial.println();
}

void imprimirAjudaSerial() {
  Serial.println("Comandos disponiveis:");
  Serial.println("help");
  Serial.println("api normal");
  Serial.println("api seca");
  Serial.println("api chuva");
  Serial.println("api transporte");
  Serial.println("api vigoralto");
  Serial.println("api vigorbaixo");
  Serial.println("api auto");
  Serial.println("status");
  Serial.println();
}

void processarComandoSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim(); cmd.toLowerCase();
  if (cmd == "api normal") aplicarCenario(CENARIO_NORMAL);
  else if (cmd == "api seca") aplicarCenario(CENARIO_SECA);
  else if (cmd == "api chuva") aplicarCenario(CENARIO_CHUVA);
  else if (cmd == "api transporte") aplicarCenario(CENARIO_TRANSPORTE);
  else if (cmd == "api vigoralto") aplicarCenario(CENARIO_VIGOR_ALTO);
  else if (cmd == "api vigorbaixo") aplicarCenario(CENARIO_VIGOR_BAIXO);
  else if (cmd == "api auto") {
    modoSimulado = false; nomeCenarioAtual = "AUTO_SENSORES";
    Serial.println("\n>>> MODO AUTO REATIVADO <<<");
    Serial.println("Leituras voltaram para DHT22 + PIR.\n");
  } else if (cmd == "status") {
    Serial.println("\n>>> STATUS ATUAL <<<");
    Serial.print("Modo: "); Serial.println(modoSimulado ? "SIMULADO" : "AUTO");
    Serial.print("Cenario/Fonte: "); Serial.println(nomeCenarioAtual);
    Serial.print("Status do lote: "); Serial.println(statusLoteAtual()); Serial.println();
  } else imprimirAjudaSerial();
}

void atualizarStatusLotePorBotao() {
  bool leituraAtual = digitalRead(PIN_BOTAO);
  if (leituraAtual != leituraBotaoAnterior) ultimoDebounce = millis();
  if ((millis() - ultimoDebounce) > TEMPO_DEBOUNCE) {
    if (leituraAtual != estadoBotaoEstavel) {
      estadoBotaoEstavel = leituraAtual;
      if (estadoBotaoEstavel == LOW) {
        indiceStatusLote++;
        if (indiceStatusLote >= TOTAL_STATUS) indiceStatusLote = 0;
        Serial.println("\n>>> STATUS DO LOTE ALTERADO MANUALMENTE <<<");
        Serial.print("Novo status: "); Serial.println(statusLoteAtual()); Serial.println();
      }
    }
  }
  leituraBotaoAnterior = leituraAtual;
}

void atualizarSinalizacao(NivelRisco nivel) {
  bool pisca = ((millis() / 250) % 2) == 0;
  digitalWrite(PIN_LED_VERDE, LOW);
  digitalWrite(PIN_LED_AMARELO, LOW);
  digitalWrite(PIN_LED_VERMELHO, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  if (nivel == NIVEL_NORMAL) digitalWrite(PIN_LED_VERDE, HIGH);
  else if (nivel == NIVEL_ATENCAO) digitalWrite(PIN_LED_AMARELO, HIGH);
  else { digitalWrite(PIN_LED_VERMELHO, pisca ? HIGH : LOW); digitalWrite(PIN_BUZZER, pisca ? HIGH : LOW); }
}

void imprimirJson(float temperatura, float umidade, bool movimento, const String& statusLote, NivelRisco nivel, const String& acao, float chuvaMm, float ventoMs, int nuvensPct, float ndvi, float evi) {
  Serial.println("{");
  Serial.print("  \"produtor\": \""); Serial.print(ID_PRODUTOR); Serial.println("\",");
  Serial.print("  \"lote\": \""); Serial.print(ID_LOTE); Serial.println("\",");
  Serial.print("  \"cultura\": \""); Serial.print(CULTURA); Serial.println("\",");
  Serial.print("  \"modo\": \""); Serial.print(modoSimulado ? "SIMULADO" : "AUTO"); Serial.println("\",");
  Serial.print("  \"fonte_cenario\": \""); Serial.print(nomeCenarioAtual); Serial.println("\",");
  Serial.print("  \"status_lote\": \""); Serial.print(statusLote); Serial.println("\",");
  Serial.print("  \"temperatura\": "); Serial.print(temperatura, 1); Serial.println(",");
  Serial.print("  \"umidade\": "); Serial.print(umidade, 1); Serial.println(",");
  Serial.print("  \"chuva_mm\": "); Serial.print(chuvaMm, 1); Serial.println(",");
  Serial.print("  \"vento_ms\": "); Serial.print(ventoMs, 1); Serial.println(",");
  Serial.print("  \"nuvens_pct\": "); Serial.print(nuvensPct); Serial.println(",");
  Serial.print("  \"ndvi\": "); Serial.print(ndvi, 2); Serial.println(",");
  Serial.print("  \"evi\": "); Serial.print(evi, 2); Serial.println(",");
  Serial.print("  \"movimento\": "); Serial.println(movimento ? "true," : "false,");
  Serial.print("  \"nivel\": \""); Serial.print(textoNivel(nivel)); Serial.println("\",");
  Serial.print("  \"risco_perda\": \""); Serial.print(textoRiscoPerda(nivel)); Serial.println("\",");
  Serial.print("  \"acao\": \""); Serial.print(acao); Serial.println("\",");
  Serial.print("  \"tempo_simulacao_s\": "); Serial.println(millis() / 1000);
  Serial.println("}\n");
}

void desenharCabecalho(const String& tituloTela) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0); display.println("Rastreabilidade Agro");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 12); display.println(tituloTela);
}

void desenharTelaResumo() {
  desenharCabecalho("Resumo do Lote");
  display.setCursor(0, 24); display.print("Lote: "); display.println(ID_LOTE);
  display.setCursor(0, 34); display.print("Status: "); display.println(ultimaTelemetria.statusLote);
  display.setCursor(0, 44); display.print("Nivel: "); display.println(textoNivel(ultimaTelemetria.nivel));
  display.setCursor(0, 54); display.print("Risco: "); display.println(textoRiscoPerda(ultimaTelemetria.nivel));
  display.display();
}

void desenharTelaSensores() {
  desenharCabecalho("Sensores");
  display.setCursor(0, 24); display.print("Temp: "); display.print(ultimaTelemetria.temperatura, 1); display.println(" C");
  display.setCursor(0, 34); display.print("Umid: "); display.print(ultimaTelemetria.umidade, 1); display.println(" %");
  display.setCursor(0, 44); display.print("Mov: "); display.println(ultimaTelemetria.movimento ? "SIM" : "NAO");
  display.setCursor(0, 54); display.print("Fonte: "); display.println(modoSimulado ? "SIMULADA" : "DHT/PIR");
  display.display();
}

void desenharTelaAcao() {
  desenharCabecalho("Acao Recomendada");
  display.setCursor(0, 24); display.println(textoNivel(ultimaTelemetria.nivel));
  display.setCursor(0, 36);
  if (ultimaTelemetria.nivel == NIVEL_NORMAL) display.println("Operacao normal");
  else if (ultimaTelemetria.nivel == NIVEL_ATENCAO) { display.println("Monitorar lote"); display.println("e corrigir variacao"); }
  else { display.println("Inspecionar lote"); display.println("imediatamente"); }
  display.display();
}

void desenharTelaCenario() {
  desenharCabecalho("Cenario Agro");
  display.setCursor(0, 24); display.print("Modo: "); display.println(modoSimulado ? "SIMULADO" : "AUTO");
  display.setCursor(0, 34); display.print("NDVI: "); display.print(ultimaTelemetria.ndvi, 2);
  display.setCursor(68, 34); display.print("EVI: "); display.print(ultimaTelemetria.evi, 2);
  display.setCursor(0, 44); display.print("Chuva: "); display.print(ultimaTelemetria.chuvaMm, 1); display.println(" mm");
  display.setCursor(0, 54); display.print("Vento: "); display.print(ultimaTelemetria.ventoMs, 1); display.println(" m/s");
  display.display();
}

void atualizarTelaOLED() {
  if (millis() - ultimaTrocaTela >= INTERVALO_TELA) {
    ultimaTrocaTela = millis();
    paginaTela++;
    if (paginaTela > 3) paginaTela = 0;
  }
  if (paginaTela == 0) desenharTelaResumo();
  else if (paginaTela == 1) desenharTelaSensores();
  else if (paginaTela == 2) desenharTelaAcao();
  else desenharTelaCenario();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_BOTAO, INPUT_PULLUP);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_AMARELO, OUTPUT);
  pinMode(PIN_LED_VERMELHO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (true) {}
  dhtSensor.setup(PIN_DHT, DHTesp::DHT22);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(8, 10); display.println("Sistema iniciado");
  display.setCursor(8, 24); display.println("Monitor de perdas");
  display.setCursor(8, 38); display.println("e rastreabilidade");
  display.display();
  Serial.println("Sistema iniciado.");
  imprimirAjudaSerial();
}

void loop() {
  processarComandoSerial();
  atualizarStatusLotePorBotao();

  if (millis() - ultimaLeitura >= INTERVALO_LEITURA) {
    ultimaLeitura = millis();
    float temperatura, umidade, chuvaMm = 0.0, ventoMs = 0.0, ndvi = 0.65, evi = 0.40;
    int nuvensPct = 0;
    bool movimento;

    if (modoSimulado) {
      temperatura = cenarioAtual.temperatura;
      umidade = cenarioAtual.umidade;
      movimento = cenarioAtual.movimento;
      chuvaMm = cenarioAtual.chuvaMm;
      ventoMs = cenarioAtual.ventoMs;
      nuvensPct = cenarioAtual.nuvensPct;
      ndvi = cenarioAtual.ndvi;
      evi = cenarioAtual.evi;
    } else {
      TempAndHumidity dados = dhtSensor.getTempAndHumidity();
      if (isnan(dados.temperature) || isnan(dados.humidity)) return;
      temperatura = dados.temperature;
      umidade = dados.humidity;
      movimento = digitalRead(PIN_PIR) == HIGH;
      nuvensPct = 20;
      nomeCenarioAtual = "AUTO_SENSORES";
    }

    String statusLote = statusLoteAtual();
    NivelRisco nivel = avaliarRisco(temperatura, umidade, movimento, statusLote, ndvi, evi);
    String acao = acaoRecomendada(nivel, movimento, statusLote);

    ultimaTelemetria.temperatura = temperatura;
    ultimaTelemetria.umidade = umidade;
    ultimaTelemetria.chuvaMm = chuvaMm;
    ultimaTelemetria.ventoMs = ventoMs;
    ultimaTelemetria.nuvensPct = nuvensPct;
    ultimaTelemetria.ndvi = ndvi;
    ultimaTelemetria.evi = evi;
    ultimaTelemetria.movimento = movimento;
    ultimaTelemetria.statusLote = statusLote;
    ultimaTelemetria.nivel = nivel;
    ultimaTelemetria.acao = acao;

    atualizarSinalizacao(nivel);
    imprimirJson(temperatura, umidade, movimento, statusLote, nivel, acao, chuvaMm, ventoMs, nuvensPct, ndvi, evi);
  }

  atualizarTelaOLED();
}
