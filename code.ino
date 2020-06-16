#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3); //RX no pino 3, TX no pino 2.
int pinoPIR = 12; //PINO DIGITAL UTILIZADO PELO SENSOR DE PRESENÇA
int pinoLED = 13; //PINO DIGITAL UTILIZADO PELO LED
int qtdpessoas = 0; //CONTADOR DO SENSOR DE PRESENÇA
int verifica = false; //verifica se o sensor já executou
String SSID = "****"; //nome da rede
String PASS = "****"; //senha da rede
#define DEBUG true
String HOST = "http://iot.leticiacansoudosemestre.com";
int PORT = 8080; 

void setup(){
  Serial.begin(9600);
  pinMode(pinoLED, OUTPUT); //DEFINE O PINO COMO SAÍDA
  pinMode(pinoPIR, INPUT); //DEFINE O PINO COMO ENTRADA
  esp8266.begin(115200); // inicia a segunda serial do módulo wifi
  sendData("AT+RST\r\n",2000, DEBUG); // começa resetando o módulo wifi
  sendData("AT",5,DEBUG); //habilita informação sobre erros
  sendData("AT+CWMODE=1",5,DEBUG); //habilita o módulo como cliente, ele conecta no wifi
  sendData("AT+CWJAP=\""+ SSID +"\",\""+ PASS +"\"",1000,DEBUG); //conecta no wifi
}

void loop(){
 int estado = digitalRead(pinoPIR);
 if(estado == HIGH && !verifica){ //SE A LEITURA DO PINO FOR IGUAL A HIGH
    digitalWrite(pinoLED, HIGH); //ACENDE O LED
    delay(1000);
    digitalWrite(pinoLED, LOW); //APAGA O LED 
    qtdpessoas += 1;
 }else if(estado = LOW){ //SENÃO, FAZ
  digitalWrite(pinoLED, LOW); //APAGA O LED
  verifica = false;
 }
  Serial.print("Quantidade de pessoas no local ");
  Serial.println(qtdpessoas);
}

void envia(char *dados){
 String getData = "GET /adicionarregistro?id=2&movimento= " + String(qtdpessoas);
 sendData("AT+CIPMUX=1",1000,DEBUG); // HABILITA MULTIPLAS CONEXÕES
 sendData("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,1000,DEBUG); //CHAMA O HOST
 sendData("AT+CIPSEND=0," +String(getData.length()+4),1000,DEBUG); //inicializa a memória
 esp8266.println(getData); //obtem os dados
}

void sendData(String command, const int timeout, boolean debug)
{
    esp8266.print(command); //envia dados pro módulo wifi
    long int time = millis(); //espera a resposta do servidor até o timeout
    while( (time+timeout) > millis()) 
    {
      while(esp8266.available())
      {
        Serial.write(esp8266.read()); //se tiver resposta manda pro DEBUG
      }  
    }
}
