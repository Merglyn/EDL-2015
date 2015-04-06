const int pulsePin = 5;

// define pins
const int pinON = 6;
const int pinCC_Right = 8;
const int pinCW_Right = 9;
const int pinSpeed_Right = 7;
const int pinCW_Left = 10;
const int pinCC_Left = 11;
const int pinSpeed_Left = 12;

// define vars
volatile unsigned int enc_left = 0;
volatile unsigned int enc_right = 0;


//define some useful vars
unsigned long tmp_pulse = 0;
unsigned long cur_pulse = 0;
void setup() {
  //set pinmodes
  pinMode(13, OUTPUT);
  pinMode(PIN, INPUT);

  //begin serial
  Serial.begin(9600);
}

void loop() {
    Serial.println("Reading pulse width");
    readPulse();
    if (cur_pulse < 25000L){
        Serial.println("Fwd");
    }
    else if (cur_pulse < 35000L){
        Serial.println("Right");
    }
    else if (cur_pulse < 45000L){
        Serial.println("Left");
    }
    else {
        Serial.println("Stop");
    }
    
}

void readPulse(){
    cur_pulse = 0;
    while(cur_pulse < 50000){
        cur_pulse = pulseIn(PIN, HIGH);
    }
    return;
}
