#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <string.h>
#include "FS.h"

/* Set desired credentials. */
const char* ssid = "Sudhish-Phone-i7";
const char* password = "18188181";

const char* newssid = "Surya";
const char* newpassword = "";

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
  WiFi.softAP(newssid);      
  
  Serial.println(newssid);                // print the Wifi name and the IP address
  Serial.print("server ip ... "+WiFi.softAPIP());
  Serial.println("connected to ... "+WiFi.SSID());
  Serial.println("ip address ... "+WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
  
  server.on("/", HTTP_GET, handleRoot);       // Call the handleRoot() function when URL/ is called 
  server.onNotFound(handleNotFound);        // Call handleNotFound() when anything other than the above mentioned URLs are called 
  server.begin();                           // Starting the server
  Serial.println("http server started");

  if (strcmp (WiFi.SSID().c_str(),ssid) != 0) {
      Serial.print("Connecting to ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);
  }

  while (WiFi.status() != WL_CONNECTED) {
    yield();
  }

  Serial.print("Connected to: ");
  Serial.print(WiFi.SSID());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
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
  String requestLinkOriginal = server.uri();
  String requestLink = requestLinkOriginal.substring(1,-1);
  int posSplit = requestLink.indexOf('/');
  String hostSection = requestLink.substring(0,posSplit);
  String uriSection = requestLink.substring(posSplit,-1);
  Serial.println("splitting host section ... " + hostSection);
  Serial.println("splitting uri section ... " + uriSection);
  WiFiClient client;
  client.setTimeout(1000);
  Serial.println("connecting to ... " + hostSection);
  while (!!!client.connect(hostSection, 80)) {
    Serial.println("connection failed, retrying...");
  }
  Serial.println("connection successful, proceeding...");
  Serial.println("sending a request to ... " + hostSection);
  client.print(String("GET "+uriSection) + " HTTP/1.1\r\n" +
               "Host: " + hostSection + "\r\n" +
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
