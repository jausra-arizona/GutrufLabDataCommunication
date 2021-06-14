const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

const int pintrigger = 8;   //set TTL output pin to D8 
const int bit0_ON = 200;    //0 bit pulse length in ms     
const int bit1_ON = 700;    //1 bit pulse length in ms     
const int OFF_time = 1000;  //off time between bit pulses 

// variables to hold the parsed data
char messageFromPC[numChars] = {0};
byte FREQ;
byte DUTY;

boolean newData = false;

void setup() {
  pinMode(pintrigger, OUTPUT);
  digitalWrite(pintrigger, LOW);
  Serial.begin(57600);
  Serial.println("Enter data in this style: <Frequency (Hz), Duty Cycle (%*10)>"); //maximum frequency and duty cycle of 64 Hz and 6.4% 
  delay(800);
  Serial.println("Example 1: <10,10>");
  Serial.println("10 Hz and 1% duty cycle");
  delay(800);
  Serial.println("Example 2: <30,5>");
  Serial.println("30 Hz and 0.5% duty cycle");
}

void loop() {
  RecvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars); // this temporary copy is necessary to protect the original data
    ParseData();                      // because strtok() used in ParseData() replaces the commas with \0
    ShowParsedData();
    newData = false;
  }
}

//read user input 
void RecvWithStartEndMarkers() { 
  static boolean recvInProgress = false;
  static byte indx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[indx] = rc;
        indx++;
        if (indx >= numChars) {
          indx = numChars - 1;
        }
      } else {
        receivedChars[indx] = '\0';  // terminate the string
        recvInProgress = false;
        indx = 0;
        newData = true;
      }
    }
    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//parse data
void ParseData() { 
  char * strtokindx;
  strtokindx = strtok(tempChars, ",");
  FREQ = atoi(strtokindx);
  strtokindx = strtok(NULL, ",");
  DUTY = atoi(strtokindx);
}

//show input data that has been parsed
void ShowParsedData() { 
  Serial.println("");
  Serial.println("-----------INPUT------------");
  Serial.print("Frequency (Hz):        ");
  Serial.println(FREQ);
  Serial.print("Duty Cycle (%*10):        ");
  Serial.println(DUTY);
  delay(500);
  SendData(FREQ, DUTY);
}

//send pulse sequence of parsed data
void SendData(byte SDFrequency, byte SDDutyCycle) { 
    Serial.print("Writing frequency: ");
    digitalWrite(pintrigger, LOW);
    delay(10000);
    for (int i = 0; i < 6 ; i++) {
      BitReader(SDFrequency, 5-i);
    }
    Serial.println("");
    Serial.print("Writing duty cycle: ");
    for (int i = 0; i < 6 ; i++) {
      BitReader(SDDutyCycle, 5-i);
    }
    delay(OFF_time);
    digitalWrite(pintrigger, HIGH);
    Serial.println("");
    Serial.println("");
}

//send individual pulses
void BitReader(byte BRbyte, int i) { 
  boolean BRbit = bitRead(BRbyte, i);
  if (BRbit == 1) {
    digitalWrite(pintrigger, HIGH);
    delay(bit1_ON);
    digitalWrite(pintrigger, LOW);
  }
  if (BRbit == 0) {
    digitalWrite(pintrigger, HIGH);
    delay(bit0_ON);
    digitalWrite(pintrigger, LOW);
  }
  delay(OFF_time);
  Serial.print(BRbit);
  Serial.print("");
}
