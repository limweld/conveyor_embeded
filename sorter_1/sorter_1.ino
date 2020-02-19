#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

const char* ssid = "ubuntu_buster";
const char* password = "raspbianlinux";
const char* mqtt_server = "192.168.254.50";

Servo servo;
WiFiClient espClient;
PubSubClient client(espClient);

IPAddress ip(192, 168, 254, 202);   
IPAddress gateway(192, 168, 254, 254); 
IPAddress subnet(255, 255, 255, 0); 
IPAddress dns(8,8,8,8); 


long lastMsg = 0;
char msg[50];
int value = 0;

int max_angle = 90;
int origin_angle = 0;

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    Serial.println("open");  
 //   servo.write(origin_angle);              
     sorter_direction( origin_angle, max_angle, '1');
  } else {
    Serial.println("close");
  //  servo.write(max_angle); 
  sorter_direction( origin_angle, max_angle, '0');             
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("sorter_1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
 
  Serial.begin(115200);
  setup_wifi();
  servo.attach(2);
  servo.write(0);      
  servo.detach();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


void  sorter_direction( int origin_angle, int max_angle,char trigger_type) {
  int pos;

  servo.attach(2);
  delay(10);
  if(trigger_type == '1'){
    for (pos = origin_angle; pos <= max_angle; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);                       // waits 15ms for the servo to reach the position
    }
  }
  if(trigger_type == '0'){
    for (pos = max_angle; pos >= origin_angle; pos -= 1) { // goes from 180 degrees to 0 degrees
      servo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);                       // waits 15ms for the servo to reach the position
    }
  }
  servo.detach();
}

