/*
 * Send a message to a dedicated website
 * 
 * Ethernet shield attached to pins 10, 11, 12, 13
 */

#include <SPI.h>
#include <Ethernet.h>
#define SERVER "www.chargueraud.fr"


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = SERVER;    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
// pas utilisé pour l'instant // IPAddress ip(192,168,0,177);

EthernetClient client;




void sendMessage(long message) {

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    return;

    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    // ==> j'ai désactivé ça pour l'instant: Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(SERVER, 80)) {
    Serial.println("connected, sending message");
    client.print("GET /ino/index.php?log=");
    client.print(message); // le message à envoyer (sans caractères spéciaux)
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(SERVER);
    client.println("Connection: close");
    client.println();
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
    return;
  }

  while (true) {
    // if there are incoming bytes available 
    // from the server, read them and print them:
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
    // if the server's disconnected, stop the client:
    if (! client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      client.stop();
    }
    return;
  }
}


void setup() {
  Serial.begin(9600);
}

long lastDate = 0;

void loop()
{
  long now = millis();
  if (now - lastDate > 15000) { // message toutes les 15 secondes
    lastDate = now;
    sendMessage(now);
  }
}