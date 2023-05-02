#include <Z906.h>
#include <WiFi.h>
#include <PubSubClient.h>

/* Wifi Settings */
const char *ssid = "WIFI_SSID";
const char *password = "WIFI_PASSWORD";
const char *mqtt_server = "MQTT_HOSTNAME";
const char* mqtt_username = "MQTT_USERNAME";
const char* mqtt_password = "MQTT_PASSWORD";

/* Initialize Z906 */
Z906 LOGI(Serial2);
bool speakerPowerStatus = false;

/* MQTT */
WiFiClient espClient;
PubSubClient client(espClient);
long lastUpdate = 0;

void setup() {
  Serial.begin(57600);

  /* Loop until connect to Wifi */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.setBufferSize(1024);

  /* Initialize Pins */
  pinMode(32, OUTPUT);
}


void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");

      /* Send discovery messages */
      sendDiscovery();

      /* Subscribe to topics */
      client.subscribe("homeassistant/switch/z906-power");
      client.subscribe("homeassistant/button/z906-up");
      client.subscribe("homeassistant/button/z906-down");
      client.subscribe("homeassistant/button/z906-sub-up");
      client.subscribe("homeassistant/button/z906-sub-down");
      client.subscribe("homeassistant/button/z906-rear-up");
      client.subscribe("homeassistant/button/z906-rear-down");
      client.subscribe("homeassistant/button/z906-center-up");
      client.subscribe("homeassistant/button/z906-center-down");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}


void sendDiscovery() {;

  const char* powerSwitch = "{\"name\": \"Z906 Power\", "
    "\"command_topic\": \"homeassistant/switch/z906-power\", "
    "\"state_topic\": \"homeassistant/switch/z906-power/state\", "
    "\"payload_on\": \"1\", "
    "\"payload_off\": \"0\", "
    "\"state_on\": \"on\", "
    "\"state_off\": \"off\", "
    "\"unique_id\": \"test_button_z906\", "
    "\"optimistic\": false, "
    "\"qos\": 1, "
    "\"retain\": true, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* upBtn = "{\"name\": \"Z906 Volume Up\", "
    "\"command_topic\": \"homeassistant/button/z906-up\", "
    "\"unique_id\": \"z906-up\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* downBtn = "{\"name\": \"Z906 Volume Down\", "
    "\"command_topic\": \"homeassistant/button/z906-down\", "
    "\"unique_id\": \"z906-down\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* subUpBtn = "{\"name\": \"Z906 Subwoofer Up\", "
    "\"command_topic\": \"homeassistant/button/z906-sub-up\", "
    "\"unique_id\": \"z906-sub-up\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* subDownBtn = "{\"name\": \"Z906 Subwoofer Down\", "
    "\"command_topic\": \"homeassistant/button/z906-sub-down\", "
    "\"unique_id\": \"z906-sub-down\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* rearUpBtn = "{\"name\": \"Z906 Rear Up\", "
    "\"command_topic\": \"homeassistant/button/z906-rear-up\", "
    "\"unique_id\": \"z906-rear-up\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* rearDownBtn = "{\"name\": \"Z906 Rear Down\", "
    "\"command_topic\": \"homeassistant/button/z906-rear-down\", "
    "\"unique_id\": \"z906-rear-down\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* centerUpBtn = "{\"name\": \"Z906 Center Up\", "
    "\"command_topic\": \"homeassistant/button/z906-center-up\", "
    "\"unique_id\": \"z906-center-up\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  const char* centerDownBtn = "{\"name\": \"Z906 Center Down\", "
    "\"command_topic\": \"homeassistant/button/z906-center-down\", "
    "\"unique_id\": \"z906-center-down\", "
    "\"payload_press\": \"\", "
    "\"qos\": 1, "
    "\"device\": {"
    "\"identifiers\": [\"esp32_z906\"], "
    "\"name\": \"Z906 Controller\", "
    "\"model\": \"Z906\", "
    "\"manufacturer\": \"Logitech\"}}";

  client.publish("homeassistant/switch/z906-power/config", powerSwitch);
  client.publish("homeassistant/button/z906-up/config", upBtn);
  client.publish("homeassistant/button/z906-down/config", downBtn);
  client.publish("homeassistant/button/z906-sub-up/config", subUpBtn);
  client.publish("homeassistant/button/z906-sub-down/config", subDownBtn);
  client.publish("homeassistant/button/z906-rear-up/config", rearUpBtn);
  client.publish("homeassistant/button/z906-rear-down/config", rearDownBtn);
  client.publish("homeassistant/button/z906-center-up/config", centerUpBtn);
  client.publish("homeassistant/button/z906-center-down/config", centerDownBtn);
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.println("]");
  
  if(strcmp(topic, "homeassistant/switch/z906-power") == 0) {
    if (payload[0] == '1') {
      client.publish("homeassistant/switch/z906-power/state", "on");
      digitalWrite(32, HIGH);
      speakerPowerStatus = true;
      
      delay(2500);

      LOGI.cmd(PWM_ON);
      LOGI.cmd(SELECT_INPUT_3);
      LOGI.cmd(SELECT_EFFECT_3D);
      LOGI.cmd(MUTE_OFF);
    } else if(payload[0] == '0') {
      client.publish("homeassistant/switch/z906-power/state", "off");
      digitalWrite(32, LOW);
      speakerPowerStatus = false;
    }
  } else if(strcmp(topic, "homeassistant/button/z906-up") == 0) {
    LOGI.cmd(LEVEL_MAIN_UP);
  } else if(strcmp(topic, "homeassistant/button/z906-down") == 0) {
    LOGI.cmd(LEVEL_MAIN_DOWN);
  } else if(strcmp(topic, "homeassistant/button/z906-sub-up") == 0) {
    LOGI.cmd(LEVEL_SUB_UP);
  } else if(strcmp(topic, "homeassistant/button/z906-sub-down") == 0) {
    LOGI.cmd(LEVEL_SUB_DOWN);
  } else if(strcmp(topic, "homeassistant/button/z906-rear-up") == 0) {
    LOGI.cmd(LEVEL_REAR_UP);
  } else if(strcmp(topic, "homeassistant/button/z906-rear-down") == 0) {
    LOGI.cmd(LEVEL_REAR_DOWN);
  } else if(strcmp(topic, "homeassistant/button/z906-center-up") == 0) {
    LOGI.cmd(LEVEL_CENTER_UP);
  } else if(strcmp(topic, "homeassistant/button/z906-center-down") == 0) {
    LOGI.cmd(LEVEL_CENTER_DOWN);
  }
}


void loop() {
  /* MQTT */
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  /* Update state every second */
  long currentTime = millis();
  if (currentTime - lastUpdate > 1000) {
    lastUpdate = currentTime;

    /* Update state */
    if(speakerPowerStatus) {
      client.publish("homeassistant/switch/z906-power/state", "on");
    } else {
      client.publish("homeassistant/switch/z906-power/state", "off");
    }
  }
}