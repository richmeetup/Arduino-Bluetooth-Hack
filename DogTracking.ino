#include <SoftwareSerial.h>

#define btRxPin 5
#define btTxPin 6

#define maxColumns 10
#define loopDelay 10000 // in millis

const String columnMacAddresses[maxColumns] = {
  "60C5:47:23889A", // column0
  "",
  "",
  "",
  "60C5:47:23828A", // column4
  "",
  "",
  "",
  "",
  "60C5:47:236B34"  // column9
};

SoftwareSerial bluetoothSerial(btRxPin, btTxPin);

void setup() {
  Serial.begin(38400); 
 
  // connect to bluetooth, and do initial setup
  bluetoothSerial.begin(38400);
  sendBluetoothCommand("AT");
  sendBluetoothCommand("AT+INIT");
  sendBluetoothCommand("AT+NAME=arduinoRich");
  sendBluetoothCommand("AT+ROLE=1");
  sendBluetoothCommand("AT+INQM=1,25,10");
}

// read until OK or ERROR:, or if multilined, return | delimited
String sendBluetoothCommand(String command) {
  bluetoothSerial.println(command);
  
  String response = "";  
  
  if (bluetoothSerial.overflow()) { 
    Serial.println("SoftwareSerial overflow!"); 
  }
  
  // doing it this way to avoid softwareserial overflow    
  while (1) {
    String line = "";
      
    do {
      if (bluetoothSerial.available()) {
        char c = bluetoothSerial.read();
        line += c;
      }
    } while (!line.endsWith("\r\n"));
    line.trim();
      
    if ((line == "OK") || line.startsWith("ERROR:") || (line == "FAIL"))  {
      Serial.println(command + " -> " + line);
      break;
    }
      
    response += line + " ";
  }
    
  return response;
}

unsigned int hexToDec(char hex) {
  unsigned int val;  
  
  if (hex >= '0' && hex <= '9') {
      val = hex - '0';
  }
  else {
    val = 10 + hex - 'A';
  }
    
  return val;
}

// assume 0xFF__
int twosComplement(String hex) {
  unsigned int val = 0;
  val = ~((~val << 8) | (hexToDec(hex[2]) << 4) | (hexToDec(hex[3])));

  int total = -val - 1;
  return total;  
}

void loop() {
  Serial.println("Sending AT+INQ...");
  String response = sendBluetoothCommand("AT+INQ");

  String devices[15];
  int rssis[15];
  int pings[15];
  int i = 0;

  int begin = 0;
  int end = response.indexOf(' ');

  while ((i < 15) && (end > begin)) {
    String device = response.substring(begin + 5, end - 12);
    String rssiStr = response.substring(end - 4, end);    

    boolean containsDevice = false;
    for (int j = 0; j < i; j++) {
      if (device.equals(devices[j])) {
        containsDevice = true;
        pings[j] = pings[j] + 1;
        
        break;
      }
    }

    if (!containsDevice) {
      pings[i] = 1;
      rssis[i] = twosComplement(rssiStr);
      devices[i] = device;
      i++;
    }
    
    begin = end + 1;
    end = response.indexOf(' ', begin);
  }

  int columnRssi[10];
  for (int j = 0; j < 10; j++) {
    columnRssi[j] = -255;
  }

  for (int j = 0; j < i; j++) {
    String device = devices[j];
    int rssi = rssis[j];

    Serial.print("device = " + device + ", rssi = ");
    Serial.print(rssi);
    Serial.print(", ping = ");
    Serial.println(pings[j]);
    
    for (int col = 0; col < maxColumns; col++) {
      if (device.equals(columnMacAddresses[col])) {
        columnRssi[col] = rssi;
      }
    }
  }

  int closestColumn = -1;
  int highestRssi = -255;
  for (int col = 0; col < maxColumns; col++) {
    if (columnRssi[col] > highestRssi) {
      closestColumn = col;
      highestRssi = columnRssi[col];
    }
  }

  if (closestColumn != -1) {
    Serial.print("CLOSEST COLUMN = ");
    Serial.println(closestColumn);
  }
  
  Serial.println("Sleeping for 10 seconds...");
  delay(loopDelay);
}

/*
// simple loop to write from serial to bluetooth, and get response
void loop() {
  if (bluetoothSerial.available()) {
    char c = bluetoothSerial.read();
    Serial.print(c);
  }
  if (Serial.available()) {
    char c = Serial.read();
    bluetoothSerial.print(c);
  }
}
*/
