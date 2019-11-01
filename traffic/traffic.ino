#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "NC_2.4GHz";
const char* password = "0932398390";
const char* mqtt_server = "103.86.49.233";

WiFiClient espClient;
PubSubClient client(espClient);

const int light_red = D2 , light_yellow = D3 , light_green = D4 , light_blink = D5;
String data , mode_sw , sw , red_time , yellow_time , green_time , mode_con , blink_con;
String red_time_con , yellow_time_con , green_time_con;
long lastMsg = 0 , last_time_auto = 0;
int state_blink = 0 , state_auto_mode = 0 , state_off_light = 0 , state_off_light_mannual = 0;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  data = (char*)payload;
  Serial.println(data);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);

  mode_sw = root["mode"].as<String>();
  sw = root["sw"].as<String>();
  red_time = root["red_time"].as<String>();
  yellow_time = root["yellow_time"].as<String>();
  green_time = root["green_time"].as<String>();

  if (mode_sw == "auto") {
    mode_con = "auto";
  }
  else if (mode_sw == "mannual") {
    mode_con = "mannual";
  }

  if (sw == "b_on") {
    blink_con = "blink_on";
  }
  else if (sw == "b_off") {
    blink_con = "blink_off";
  }

  if (red_time != "" && yellow_time != "" && green_time) {
    red_time_con = red_time;
    yellow_time_con = yellow_time;
    green_time_con = green_time;
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) continue;
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  client.connect("Client" , "mymqtt" , "0932398390");
  pinMode(light_red , OUTPUT);
  pinMode(light_yellow , OUTPUT);
  pinMode(light_green , OUTPUT);
  pinMode(light_blink , OUTPUT);
}

void loop() {
  long now = millis();
  client.loop();
  client.subscribe("server/read_mqtt");

  if (mode_con == "mannual") {
    if (state_off_light_mannual == 0) {
      digitalWrite(light_red , LOW);
      digitalWrite(light_yellow , LOW);
      digitalWrite(light_green , LOW);
      digitalWrite(light_blink , LOW);
      state_off_light = 0;
      state_off_light_mannual = 1;
    }

    if (sw == "r_on") {
      blink_con = "";
      digitalWrite(light_red , HIGH);
      Serial.println("----------> red on");
      sw = "";
    }
    else if (sw == "r_off") {
      blink_con = "";
      digitalWrite(light_red , LOW);
      Serial.println("----------> red off");
      sw = "";
    }

    if (sw == "y_on") {
      blink_con = "";
      digitalWrite(light_yellow , HIGH);
      Serial.println("----------> yellow on");
      sw = "";
    }
    else if (sw == "y_off") {
      blink_con = "";
      digitalWrite(light_yellow , LOW);
      Serial.println("----------> yellow off");
      sw = "";
    }

    if (sw == "g_on") {
      blink_con = "";
      digitalWrite(light_green , HIGH);
      Serial.println("----------> green on");
      sw = "";
    }
    else if (sw == "g_off") {
      blink_con = "";
      digitalWrite(light_green , LOW);
      Serial.println("----------> green off");
      sw = "";
    }

    if (blink_con == "blink_on") {
      if (now - lastMsg > 1000 && state_blink == 0) {
        lastMsg = now;
        digitalWrite(light_blink , HIGH);
        Serial.println("----------> blink on");
        state_blink = 1;
      }
      if (now - lastMsg > 1000 && state_blink == 1) {
        lastMsg = now;
        digitalWrite(light_blink , LOW);
        Serial.println("----------> blink off");
        state_blink = 0;
      }
    }
    else if (blink_con == "blink_off") {
      digitalWrite(light_blink , LOW);
      Serial.println("----------> blink off");
      blink_con = "";
    }
  }
  else if (mode_con == "auto") {
    if (state_off_light == 0) {
      blink_con = "";
      digitalWrite(light_red , LOW);
      digitalWrite(light_yellow , LOW);
      digitalWrite(light_green , LOW);
      digitalWrite(light_blink , LOW);
      state_off_light_mannual = 0;
      state_off_light = 1;
    }
    if (red_time_con != "" && yellow_time_con != "" && green_time_con != "") {
      if (now - last_time_auto > red_time_con.toInt() * 1000 && state_blink == 0) {
        last_time_auto = now;
        digitalWrite(light_green , LOW);
        digitalWrite(light_red , HIGH);
        Serial.println("----------> auto red");
        state_blink = 1;
      }
      else if (now - last_time_auto > yellow_time_con.toInt() * 1000 && state_blink == 1) {
        last_time_auto = now;
        digitalWrite(light_red , LOW);
        digitalWrite(light_yellow , HIGH);
        Serial.println("----------> auto yellow");
        state_blink = 2;
      }
      else if (now - last_time_auto > green_time_con.toInt() * 1000 && state_blink == 2) {
        last_time_auto = now;
        digitalWrite(light_yellow , LOW);
        digitalWrite(light_green , HIGH);
        Serial.println("----------> auto green");
        state_blink = 0;
      }
    }
  }
}
