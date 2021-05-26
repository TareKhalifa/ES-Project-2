
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#define RXD2 16
#define TXD2 17
String x;
String heart;
String steps;
int heart2 = 0;
String mypage()
{
  String r = "<!DOCTYPE html>\n";
  r += "<head>";
  r += "<script>\n";
  r += "setInterval(loadDoc,1000);\n";
  r += "function loadDoc() {\n";
  r += "var xhttp = new XMLHttpRequest();\n";
  r += "xhttp.onreadystatechange = function() {\n";
  r += "if (this.readyState == 4 && this.status == 200) {\n";
  r += "document.body.innerHTML =this.responseText;}\n";
  r += "};\n";
  r += "xhttp.open(\"GET\", \"/\", true);\n";
  r += "xhttp.send();\n";
  r += "}\n";
  r += "</script>\n";
  r += "</head>";
  r += "<html>\n<style>\n";
  r += ".card{\nmax-width: 400px;\nmin-height: 250px;\nbackground: #02b875;\npadding: 30px;\nbox-sizing: border-box;\ncolor: #FFF;\nmargin:20px;\nbox-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);\n";
  r += "}\n</style>\n<body>\n<div class=\"card\">\n";
  r += "<h1>Heart Rate : " + heart + "<br>\n";
  r += "<h1>Steps :" + steps + "<br>\n";
  r += "</div>\n</body>\n</html>";

  return r;
}
WebServer server(80);

const char *ssid = "T";
const char *password = "12345678";

void handleRoot()
{
  server.send(200, "text/html", mypage()); //Send web page
}

void handleADC()
{

  server.send(200, "text/html", mypage());
  Serial.println(x);
}

void setup(void)
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  delay(1000);
  while (!Serial)
    ;
  while (!Serial2)
    ;
  Serial.println("Connected!");

  WiFi.mode(WIFI_STA); //Connect to your wifi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);

  server.begin(); //Start server
  Serial.println("HTTP server started");
}

void loop(void)
{
  server.handleClient();
  if (Serial2.available())
  {
    x = "";
    while (Serial2.available())
      x += char(Serial2.read());
    if (x[x.length() - 1] == '2')
      steps = x.substring(0, x.length() - 1);
    else
      heart = x;
    heart2 = (heart.toInt()) / 10;
    Serial.println(x);
  }
  delay(1);
}
