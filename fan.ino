
#include "Energia.h"


// Include application, user and local libraries

#ifndef __CC3200R1M1RGC__
#include <SPI.h>                // Do not include SPI for CC3200 LaunchPad
#endif
#include <WiFi.h>


// Define structures and classes


// Define variables and constants
char wifi_name[] = "fan";
char wifi_password[] = "password";

WiFiServer myServer(80);
uint8_t oldCountClients = 0;
uint8_t countClients = 0;
long int count=0;
int timer=0;
// Add setup code
void setup()
{
    Serial.begin(115200);
    delay(500);
    
    Serial.println("*** LaunchPad CC3200 WiFi Web-Server in AP Mode");
    
    // Start WiFi and create a network with wifi_name as the network name
    // with wifi_password as the password.
    Serial.print("Starting AP... ");
    WiFi.beginNetwork(wifi_name, wifi_password);
    while (WiFi.localIP() == INADDR_NONE)
    {
        // print dots while we wait for the AP config to complete
        Serial.print('.');
        delay(300);
    }
    Serial.println("DONE");
    
    Serial.print("LAN name = ");
    Serial.println(wifi_name);
    Serial.print("WPA password = ");
    Serial.println(wifi_password);
    
    
    pinMode(RED_LED, OUTPUT);      // set the LED pin mode
    digitalWrite(RED_LED, LOW);
    pinMode(GREEN_LED, OUTPUT);      // set the LED pin mode
    digitalWrite(GREEN_LED, LOW);
    pinMode(YELLOW_LED, OUTPUT);      // set the LED pin mode
    digitalWrite(YELLOW_LED, LOW);
    
    IPAddress ip = WiFi.localIP();
    Serial.print("Webserver IP address = ");
    Serial.println(ip);
    
    Serial.print("Web-server port = ");
    myServer.begin();                           // start the web server on port 80
    Serial.println("80");
    Serial.println();
}

// Add loop code
void loop()
{
    
    countClients = WiFi.getTotalDevices();
    
    // Did a client connect/disconnect since the last time we checked?
    if (countClients != oldCountClients)
    {
        if (countClients > oldCountClients)
        {  // Client connect
            //            digitalWrite(RED_LED, !digitalRead(RED_LED));
            Serial.println("Client connected to AP");
            for (uint8_t k = 0; k < countClients; k++)
            {
                Serial.print("Client #");
                Serial.print(k);
                Serial.print(" at IP address = ");
                Serial.print(WiFi.deviceIpAddress(k));
                Serial.print(", MAC = ");
                Serial.println(WiFi.deviceMacAddress(k));
                Serial.println("CC3200 in AP mode only accepts one client.");
            }
        }
        else
        {  // Client disconnect
            //            digitalWrite(RED_LED, !digitalRead(RED_LED));
            Serial.println("Client disconnected from AP.");
            Serial.println();
        }
        oldCountClients = countClients;
    }
    
    WiFiClient myClient = myServer.available();
    
    if (myClient||timer==1)
    {                             // if you get a client,
        Serial.println(". Client connected to server");           // print a message out the serial port
        char buffer[150] = {0};                 // make a buffer to hold incoming data
        int8_t i = 0;
        while (myClient.connected()||timer==1)
        {            // loop while the client's connected
            if (myClient.available()||timer==1)
            {             // if there's bytes to read from the client,
                char c = myClient.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character
                    
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (strlen(buffer) == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        myClient.println("HTTP/1.1 200 OK");
                        myClient.println("Content-type:text/html");
                        myClient.println();
                        
                        // the content of the HTTP response follows the header:
                        myClient.println("<html><head><title>WIFI-Fan Control - ANAND</title></head><body align=center>");
                        myClient.println("<h1 align=center><font color=\"red\">WiFi Fan Working fine  ANAND</font></h1>");
                        myClient.print("TIMER  <button onclick=\"location.href='/TH'\">ON</button>");
                        myClient.print("RESET  <button onclick=\"location.href='/RS'\">Reset</button>");
                        
                        myClient.print("FAN  <button onclick=\"location.href='/GH'\">ON</button>");
                        myClient.println(" <button onclick=\"location.href='/GL'\">OFF</button><br>");
                        
                        // The HTTP response ends with another blank line:
                        myClient.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    {      // if you got a newline, then clear the buffer:
                        memset(buffer, 0, 150);
                        i = 0;
                    }
                }
                else if (c != '\r')
                {    // if you got anything else but a carriage return character,
                    buffer[i++] = c;      // add it to the end of the currentLine
                }
                
                Serial.println();
                String text = buffer;
                // Check to see if the client request was "GET /H" or "GET /L":
               
                
                
                if (text.endsWith("GET /RS"))
                {
                    digitalWrite(RED_LED, LOW);               // GET /H turns the LED on
                    Serial.println("Timer reset");
                 count=0;   
                timer=0;
                }
                else if (text.endsWith("GET /TH")||timer==1)
                {
                  timer=1;
                  digitalWrite(RED_LED, LOW);
                 count++;
                 delay(200);
                 if(count>=30000)
                  {
                    digitalWrite(RED_LED, HIGH);
                    timer=0;
                    count=0;
                   }

                }
                
                  
                  
                
                else if (text.endsWith("GET /GH"))
                {
                    digitalWrite(RED_LED, HIGH);               // GET /H turns the LED on
                   Serial.println("Fan ON");       
                }
                else if (text.endsWith("GET /GL"))
                {
                    digitalWrite(RED_LED, LOW);                // GET /L turns the LED off
                    Serial.println("FAN off");
                }
                
            }
        }
        
        // close the connection:
        
        myClient.stop();
        Serial.println("Client disconnected from server");
        Serial.println();
       
    }
}

