/* Wifi Settings */
const char *ssid = "<WIFI_SSID>>";
const char *password = "<WIFI_PASSWORD>";

/* MQTT Settings */
const char *mqtt_server = "<MQTT_IP>";
const char *mqtt_username = "<MQTT_USERNAME>";
const char *mqtt_password = "<MQTT_PASSWORD>";

/* Pin 32 on the ESP32 that connects to Pin 15 on the DB-15 connector on the subwoofer/amplifier */
int TurnOnSpeakerPin = 32;

/* Debug mode. This can be turned off but it outputs a little more data over the serial bus for testing the code */
bool DEBUG = false;