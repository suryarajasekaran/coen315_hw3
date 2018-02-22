#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <string.h>
#include "FS.h"

/* Set desired credentials. */
const char* newssid = "sudhome-5";
const char* newpassword = "2618suddus";

const char* ssid = "Surya";
const char* password = "";

const char* host = "192.168.8.2";

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleNotFound();          // fucntion for handling errors

String webSite = "";

void setup(void)
{
  Serial.println("\n\n[setup:]");
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  WiFi.mode(WIFI_AP_STA); 
  WiFi.softAP(ssid);      
  
  Serial.println(ssid);                // print the Wifi name and the IP address
  Serial.print("server ip ... "+WiFi.softAPIP());
  Serial.println("connected to ... "+WiFi.SSID());
  Serial.println("ip address ... "+WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
  
  server.on("/", HTTP_GET, handleRoot);       // Call the handleRoot() function when URL/ is called 
  server.onNotFound(handleNotFound);        // Call handleNotFound() when anything other than the above mentioned URLs are called 
  server.begin();                           // Starting the server
  Serial.println("http server started");
}

void loop(void)
{
  server.handleClient();  // Listen for HTTP requests from clients
  delay(1);
}

void handleRoot()
{
  Serial.println("\n\n[handleRoot:]");
  server.send(200, "text/html", "<html><body>Hello Root!</body></html>");
}

void handleNotFound() {
  Serial.println("\n\n[handleNotFound:]");
  String requestHost = server.uri();
  requestHost.replace("/", "");
  Serial.println("proxying request to ... " + requestHost);
  WiFiClient client;
  client.setTimeout(200);
  Serial.println("connecting to ... " + requestHost);
  while (!!!client.connect(requestHost, 80)) {
    Serial.println("connection failed, retrying...");
  }
  Serial.println("connection successful, proceeding...");
  Serial.println("sending a request to ... " + requestHost);
  client.print(String("GET /") + " HTTP/1.1\r\n" +
               "Host: " + requestHost + "\r\n" +
               "Connection: close\r\n" +
               "\r\n"
              );
  String outData="";
  while (client.connected())
  {
    Serial.println("waiting for response, client is still connected ...");
    while (client.available())
    {
      Serial.println("waiting for response, client is still available ...");
      outData += client.readStringUntil('\n');
      Serial.println("waiting for response, temporary response ..."+outData);
    }
  }
  Serial.println("obtained response ... " + outData);
  Serial.println("disconnecting ... ");
  client.stop();
  int pos = outData.indexOf('<');
  String html = outData.substring(pos,-1);
  html.replace("html", "WHAT-IS-THIS");
  html.replace("HTML", "WHAT-IS-THIS");
  server.send(200, "text/html", html);
}
