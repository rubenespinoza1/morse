

// This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.
// Make sure you install the ESP8266 Board Package via the Arduino IDE Board Manager and select the correct ESP8266 board before compiling.

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>

const String letters[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
const String codes[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};
String to_send = "";
String to_send_decrypted = "";
bool last_ch_was_space = false;
int button1;
int button2;
int button3;

// WiFi network info.
char ssid[] = "HUAWEI P30";
char wifiPassword[] = "6b75fdc4c260";

//char ssid[] = "LOL WIFI";
//char wifiPassword[] = "201061911";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "ce81d970-c6fb-11ec-8c44-371df593ba58";
char password[] = "e76b832891e449204d863575e730393ddd8923da";
char clientID[] = "dac41fe0-c6fb-11ec-8da3-474359af83d7";

#define VIRTUAL_CHANNEL 1
#define ACTUATOR_PIN 4 // Do not use digital pins 0 or 1 since those conflict with the use of Serial.

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("atempting to connect");
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  Serial.println("Connected *");
  pinMode(D7, INPUT);
  pinMode(D6, INPUT);
  pinMode(D5, INPUT);
}

void loop() {
  button1 = digitalRead(D7);
  button2 = digitalRead(D6);
  button3 = digitalRead(D5);
	

  Cayenne.loop();
  if (button1 == HIGH) {
    to_send = to_send + ".";
    last_ch_was_space = false;
    Serial.println("message=" + to_send);
    //Serial.println("Boton 1 (.)");
  }
  if (button2 == HIGH) {
    to_send = to_send + "-";
    last_ch_was_space = false;

    Serial.println("message=" + to_send);
    //Serial.println("Boton 2 (-)");
  }
  if (button3 == HIGH) {
    if (last_ch_was_space) {
      String data = decrypt_message();
      const char* to_send_data = data.c_str();
      
      Serial.println("Message sent, check the dashboard");
      Cayenne.virtualWrite(0, to_send_data);
      to_send = "";
      to_send_decrypted = "";
    } else {
      last_ch_was_space = true;
      to_send = to_send + "/";
      Serial.println("message=" + to_send);
    }
    //Serial.println("Boton 3 (/)");
  }

}

char* convert_data_to_char(String data){
  int str_len = data.length() + 1;
  char char_array[str_len];
  data.toCharArray(char_array, str_len);
  //Serial.println(char_array);
  return char_array;
}

CAYENNE_IN(VIRTUAL_CHANNEL)
{
  int value = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", VIRTUAL_CHANNEL, ACTUATOR_PIN, value);
  // Write the value received to the digital pin.
  digitalWrite(ACTUATOR_PIN, value);
}

CAYENNE_IN(2)
{


  //to_send = to_send + ".";
  //last_ch_was_space = false;
  //Serial.println("message=" + to_send);
}

CAYENNE_IN(3)
{

  //to_send = to_send + "-";
  //last_ch_was_space = false;

  //Serial.println("message=" + to_send);
}

CAYENNE_IN(4)
{
  //if (last_ch_was_space) {
    //String data = decrypt_message();
    //Serial.println("Decrypted= " + data);
    //to_send = "";
    //to_send_decrypted = "";
  //} else {
    //last_ch_was_space = true;
    //to_send = to_send + "/";
    //Serial.println("message=" + to_send);
  //}


}

CAYENNE_IN(5)
{
  Serial.println("clear");
  Serial.println("message=" + to_send);
  to_send = "";
  Serial.println("message=" + to_send);

}
// This function is called at intervals to send data to Cayenne and keep the device online.
// Will create a temporary green widget on Channel 0, make it permanent by clicking on '+'.
CAYENNE_OUT(0)
{
  //CAYENNE_LOG("Send data for Virtual Channel 0");
  // This command writes the device's uptime in seconds to the Virtual Channel.
  //Cayenne.virtualWrite(0, millis() / 1000);

}



// Default function for sending sensor data at intervals to Cayenne.
// You can also use functions for specific channels, e.g CAYENNE_OUT(1) for sending channel 1 data.
CAYENNE_OUT_DEFAULT()
{
  // Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
  //Cayenne.virtualWrite(0, millis());




  // Some examples of other functions you can use to send data.
  //Cayenne.celsiusWrite(1, 22.0);
  //Cayenne.luxWrite(2, 700);
  //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}

// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}

String decrypt_message() {
  String to_find_index = "";
  for (int i = 0; i < to_send.length(); i++) {

    if (to_send.charAt(i) != '/') {

      to_find_index = to_find_index + to_send.charAt(i);
    } else {
      int index = find_index(to_find_index);
      to_send_decrypted = to_send_decrypted + letters[index];
      to_find_index = "";
    }
  }
  return to_send_decrypted;
}

int find_index(String code) {
  int index = -1;
  for (int i = 0; i < sizeof(letters); i++) {
    if (codes[i] == code) {
      index = i;
    }
  }
  return index;
}
