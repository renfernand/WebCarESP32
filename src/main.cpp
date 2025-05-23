#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* SSID = "Rede udi";
const char* password = "1j4b7i2g$4";
String s_mac=""; //MAC Address
WebServer server(80);
uint8_t comando; 


enum Ecommandtype {
  CMD_NENHUM = 0,
  CMD_DIR = 1,
  CMD_ESQ = 2,
  CMD_CIMA = 3,
  CMD_BAIXO = 4
};

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleLedOff() {
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}
void setup() 
{
    Serial.begin(9600);
    //pinMode(LED_AZUL, OUTPUT); // LED_BUILTIN pin as output
    //pinMode(LED_AMAR, OUTPUT); // LED_BUILTIN pin as output
    //pinMode(LED_VERM, OUTPUT); // LED_BUILTIN pin as output
    //pinMode(LED_VERDE, OUTPUT); // LED_BUILTIN pin as output


    /*************** WiFi CONNECT *********************/

    WiFi.begin(SSID, password); //MODBUS SSID and Password

    
    while (WiFi.waitForConnectResult() != WL_CONNECTED) 
    {

        Serial.println("WiFi Connection Failed! Rebooting...");
        delay(2000);
        ESP.restart();
    }
    
    Serial.println("\n\nWiFi OK");

    s_mac += String(WiFi.macAddress());
    
    Serial.print("MAC: ");   Serial.println(s_mac);  

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/led_on", handleLedOn);
    server.on("/led_off", handleLedOff);

}

// HTML com dois botões
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>ESP32 Controle LED</title></head>
<body>
  center><font size='15'>OFICINA ROBOTICA NACIONAL (ESP32) </center></h1>
  <center><font size='10'>Controle remoto do Carro </center>
  <form action="/BTDireita" method="get">
    <button type="submit">DIREITA</button>
  </form>
  <form action="/BTEsquerda" method="get">
    <button type="submit">ESQUERDA</button>
  </form>
  <form action="/BTDescer" method="get">
    <button type="submit">DESCER</button>
  </form>
  <form action="/BTSubir" method="get">
    <button type="submit">SUBIR</button>
  </form>
</body>
</html>
)rawliteral";

//+----------------------------------------------------------------------------------+

void loop() 
{
 
    uint8_t newcommand = 0; //VARIAVEL QUE VERIFICA SE HOUVE NOVO COMANDO
  
    WiFiClient client = server.available(); //VERIFICA SE ALGUM CLIENTE ESTÁ CONECTADO NO SERVIDOR
 
    if (!client) { //SE NÃO EXISTIR CLIENTE CONECTADO, FAZ
      return; //REEXECUTA O PROCESSO ATÉ QUE ALGUM CLIENTE SE CONECTE AO SERVIDOR
    }

    Serial.println("passei 1");

    while(client.available() == 0){ //ENQUANTO CLIENTE ESTIVER CONECTADO
        delay(1); //INTERVALO DE 1 MILISEGUNDO
    }

    //Pagina HTML - Inicio
    String req = client.readStringUntil('\r'); //FAZ A LEITURA DA PRIMEIRA LINHA DA REQUISIÇÃO
    client.flush(); //AGUARDA ATÉ QUE TODOS OS DADOS DE SAÍDA SEJAM ENVIADOS AO CLIENTE
    
    client.println("HTTP/1.1 200 OK"); 
    client.println("Content-Type: text/html"); 
    client.println("");
    client.println("<!DOCTYPE HTML>"); 
    client.println("<html>"); 
    client.println("<h1><center><font size='15'>OFICINA ROBOTICA NACIONAL (ESP32) </center></h1>"); 
    client.println("<center><font size='10'>Controle remoto do Carro </center>"); 
    client.println("<p><center><a href=\"BTDireita\"><button><font size='15'>DIREITA</button></a></p>"); 
    client.println("<p><center><a href=\"BTEsquerda\"><button><font size='15'>ESQUERDA</button></a></p>"); 
    client.println("<p><center><a href=\"BTDescer\"><button><font size='15'>DESCER</button></a></p>"); 
    client.println("<p><center><a href=\"BTSubir\"><button><font size='15'>SUBIR</button></a></p>"); 
    client.println("</html>"); 
    client.flush();
    //FECHA A TAG "html"

    //verifica se algum botao foi pressionado 
    newcommand = 0;
    if (req.indexOf("BTDireita") !=-1){
      comando = (uint8_t) CMD_DIR;
      newcommand = 1;
    }
    else if (req.indexOf("BTEsquerda") !=-1){
      comando = (uint8_t)  CMD_ESQ;
      newcommand = 1;
    }
    else if (req.indexOf("BTDescer") !=-1){
      comando = (uint8_t) CMD_CIMA;
      newcommand = 1;
    }
    else if (req.indexOf("BTSubir") !=-1) {
      comando = (uint8_t) CMD_BAIXO;
      newcommand = 1;
    }
    else{
      comando = (uint8_t) CMD_NENHUM;
    }
        
    client.stop();

    if (newcommand == 1){
      Serial.print("Comando recebido: ");
      Serial.println(comando);
    }
    
  delay(100);  // aguarda 2 segundos

} // FIM - void loop()


