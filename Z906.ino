#include "config.h"
#include "Z906.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* Initialize Z906 */
Z906 LOGI(Serial2);
bool speakerPowerStatus = false;

/* MQTT */
WiFiClient espClient;
PubSubClient client(espClient);
long lastUpdate = 0;
unsigned long last10SecondsUpdate = 0;

// Define the structure before using it
struct Entity {
  const char* name;
  const char* topic;
  const char* type;
};

void setup() {
  Serial.begin(115200);

  /* Loop until connect to Wifi */
  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.setBufferSize(1024);

  /* Initialize Pins */
  pinMode(TurnOnSpeakerPin, INPUT);
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
      client.subscribe("homeassistant/select/z906-input");
      client.subscribe("homeassistant/select/z906-effect");
      client.subscribe("homeassistant/button/z906-save");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}



void generateEntityConfig(DynamicJsonDocument& doc, const Entity& entity) {
  doc.clear();  // Clear the previous document for reuse

  // Basic fields
  doc["name"] = entity.name;
  doc["command_topic"] = "homeassistant/" + String(entity.type) + "/" + String(entity.topic);
  doc["unique_id"] = "z906-" + String(entity.topic);
  doc["qos"] = 1;
  doc["device"]["identifiers"] = "esp32_z906";
  doc["device"]["name"] = "Z906 Controller";
  doc["device"]["model"] = "Z906";
  doc["device"]["manufacturer"] = "Logitech";

  // Type-specific fields
  if (strcmp(entity.type, "switch") == 0) {
    doc["state_topic"] = "homeassistant/switch/" + String(entity.topic) + "/state";
    doc["payload_on"] = "1";
    doc["payload_off"] = "0";
    doc["state_on"] = "on";
    doc["state_off"] = "off";
    doc["optimistic"] = false;
    doc["retain"] = true;
  } else if (strcmp(entity.type, "button") == 0) {
    doc["payload_press"] = "";
  } else if (strcmp(entity.type, "select") == 0) {
    if (strcmp(entity.name, "Z906 Input") == 0) {
      JsonArray options = doc.createNestedArray("options");
      options.add("Input 1");
      options.add("Input 2");
      options.add("Input 3");
      options.add("Input 4");
      options.add("Input 5");
      options.add("Input AUX");
    } else if (strcmp(entity.name, "Z906 Effect") == 0) {
      JsonArray options = doc.createNestedArray("options");
      options.add("Effect 3D");
      options.add("Effect 4.1");
      options.add("Effect 2.1");
      options.add("No Effect");
    }
    doc["state_topic"] = "homeassistant/select/" + String(entity.topic) + "/state";
  } else if (strcmp(entity.type, "sensor") == 0) {
    doc["state_topic"] = "homeassistant/sensor/" + String(entity.topic) + "/state";
    doc["value_template"] = "{{ value }}";
    doc["object_id"] = entity.topic;

    // Sensor-specific properties
    if (strcmp(entity.name, "Z906 Temperature") == 0) {
      doc["unit_of_measurement"] = "°C";
      doc["icon"] = "mdi:thermometer";
    } else if (strcmp(entity.name, "Z906 Dolby") == 0) {
      doc["icon"] = "mdi:dolby";
    } else if (strcmp(entity.name, "Z906 Firmware") == 0) {
      doc["icon"] = "mdi:package";
    }
  }
}

void sendDiscovery() {
  // Define the entities in an array of structs
  Entity entities[] = {
    {"Z906 Power", "z906-power", "switch"},
    {"Z906 Volume Up", "z906-up", "button"},
    {"Z906 Volume Down", "z906-down", "button"},
    {"Z906 Subwoofer Up", "z906-sub-up", "button"},
    {"Z906 Subwoofer Down", "z906-sub-down", "button"},
    {"Z906 Rear Up", "z906-rear-up", "button"},
    {"Z906 Rear Down", "z906-rear-down", "button"},
    {"Z906 Center Up", "z906-center-up", "button"},
    {"Z906 Center Down", "z906-center-down", "button"},
    {"Z906 Input", "z906-input", "select"},
    {"Z906 Effect", "z906-effect", "select"},
    {"Z906 Save", "z906-save", "button"},
    {"Z906 Temperature", "z906-temp", "sensor"},
    {"Z906 Dolby", "z906-dolby", "sensor"},
    {"Z906 Firmware", "z906-firmware", "sensor"}
  };

  DynamicJsonDocument doc(1024);

  // Loop over all entities and generate the config
  for (int i = 0; i < sizeof(entities) / sizeof(entities[0]); ++i) {
    String topic = "homeassistant/" + String(entities[i].type) + "/" + String(entities[i].topic) + "/config";
    
    // Generate the entity configuration
    generateEntityConfig(doc, entities[i]);

    // Convert JSON object to string and send
    String config;
    serializeJson(doc, config);
    client.publish(topic.c_str(), config.c_str());
  }
}




void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received [");
  Serial.print(topic);
  Serial.println("]");
  
  // Convert payload to a null-terminated string
  char payloadStr[length + 1];
  memcpy(payloadStr, payload, length);
  payloadStr[length] = '\0';

  if(strcmp(topic, "homeassistant/switch/z906-power") == 0) {
    if (payload[0] == '1') {
      client.publish("homeassistant/switch/z906-power/state", "on");
      pinMode(TurnOnSpeakerPin, OUTPUT);
      digitalWrite(TurnOnSpeakerPin, LOW);
      speakerPowerStatus = true;
      
      delay(2500);

      LOGI.cmd(PWM_ON);
      
      // Select input
      LOGI.cmd(SELECT_INPUT_3);
      client.publish("homeassistant/select/z906-input/state", "Input 3");

      LOGI.cmd(MUTE_OFF);
    } else if(payload[0] == '0') {
      client.publish("homeassistant/switch/z906-power/state", "off");
      pinMode(TurnOnSpeakerPin, INPUT);
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
  } else if(strcmp(topic, "homeassistant/select/z906-input") == 0) {
    LOGI.cmd(PWM_ON);
    if(strcmp(payloadStr, "Input 1") == 0) {
      LOGI.cmd(SELECT_INPUT_1);
    } else if(strcmp(payloadStr, "Input 2") == 0) {
      LOGI.cmd(SELECT_INPUT_2);
    } else if(strcmp(payloadStr, "Input 3") == 0) {
      LOGI.cmd(SELECT_INPUT_3);
    } else if(strcmp(payloadStr, "Input 4") == 0) {
      LOGI.cmd(SELECT_INPUT_4);
    } else if(strcmp(payloadStr, "Input 5") == 0) {
      LOGI.cmd(SELECT_INPUT_5);
    } else if(strcmp(payloadStr, "Input AUX") == 0) {
      LOGI.cmd(SELECT_INPUT_AUX);
    }
    LOGI.cmd(MUTE_OFF);
  } else if(strcmp(topic, "homeassistant/select/z906-effect") == 0) {
    LOGI.cmd(PWM_ON);
    if(strcmp(payloadStr, "Effect 3D") == 0) {
      LOGI.cmd(SELECT_EFFECT_3D);
    } else if(strcmp(payloadStr, "Effect 4.1") == 0) {
      LOGI.cmd(SELECT_EFFECT_41);
    } else if(strcmp(payloadStr, "Effect 2.1") == 0) {
      LOGI.cmd(SELECT_EFFECT_21);
    } else if(strcmp(payloadStr, "No Effect") == 0) {
      LOGI.cmd(SELECT_EFFECT_NO);
    }
    LOGI.cmd(MUTE_OFF);
  } else if(strcmp(topic, "homeassistant/button/z906-save") == 0) {
    LOGI.cmd(EEPROM_SAVE);
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

  /* Run task every 10 seconds */
  if (currentTime - last10SecondsUpdate > 10000) {  // 10 seconds
    last10SecondsUpdate = currentTime;

    // Update Logitech information
    LOGI.update();

    // Debug
    if(DEBUG) {
      LOGI.showStatus();
  
      Serial.println("----------------------------------------------");
      Serial.println("Running task every 10 seconds...");
    }

    // Version state update
    String version = String("v" + String(LOGI.status[LOGI.STATUS_VER_A]) + "." +
                     String(LOGI.status[LOGI.STATUS_VER_B]) + "." +
                     String(LOGI.status[LOGI.STATUS_VER_C]));
    if (strcmp(version.c_str(), "v0.0.0") != 0) {
      client.publish("homeassistant/sensor/z906-firmware/state", version.c_str(), true);
    }

    // Dolby state
    int DolbyStatus = LOGI.status[LOGI.STATUS_SPDIF_STATUS];
    String DolbyStatusMessage = (DolbyStatus == 1) ? "Dolby Digital 5.1" : "PCM 2.1"; 
    if(DolbyStatus != 0) {
      client.publish("homeassistant/sensor/z906-dolby/state", DolbyStatusMessage.c_str(), true);
    }

    // Temperature state
    uint8_t TempSensorData = LOGI.main_sensor();
    if(TempSensorData != 0) {
      client.publish("homeassistant/sensor/z906-temp/state", String(TempSensorData).c_str(), true);
    }
  }
}