
//the pin to read pulses off of
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
volatile unsigned long cur_pulse = 0;

void setup() {
  //set pinmodes
  pinMode(pulsePin, INPUT);
  pinMode(pinON,INPUT);
  pinMode(pinCW_Left,OUTPUT);
  pinMode(pinCC_Left,OUTPUT);
  pinMode(pinSpeed_Left,OUTPUT);
  pinMode(pinCW_Right,OUTPUT);
  pinMode(pinCC_Right,OUTPUT);
  pinMode(pinSpeed_Right,OUTPUT);
  pinMode(13,OUTPUT);

  //init everything to 0
  stop();
 
  //start serial
  Serial.begin(9600);
 
  //start encoder interrupts
  attachInterrupt(1, count_Left, RISING);
  attachInterrupt(0, count_Right, RISING);
}

void loop() {
  //start stopped
  stop();

  //don't do anything while switch is off
  do {} while (digitalRead(pinON) == LOW);

  //read the pulse width into the var we set before
  Serial.println("Reading pulse width");
  readPulse();
  Serial.println(cur_pulse)

  //check pulse width and execute
  if (cur_pulse < 25000L){
    //range of 14ms to 25ms
    Serial.println("Fwd");
    forward(24, 100);
  }
  else if (cur_pulse < 35000L){
    //range of 25ms to 35ms
    Serial.println("Right");
    right(90, 100);
  }
  else if (cur_pulse < 45000L){
    //range of 35ms to 45ms
    Serial.println("Left");
    left(90, 100);
  }
  else {
    //same as ">= 45ms"
    Serial.println("Stop");
    stop();
  }
    
}

void readPulse(){
  //simply reads the pulse until it gets
  // one of reasonable length (<14ms, our bottom line)
  cur_pulse = 0;
  while(cur_pulse < 14000){
      cur_pulse = pulseIn(pulsePin, HIGH);
  }
  return;
}


void stop() {
  //simply set everything low
  digitalWrite(13,LOW);
  digitalWrite(pinCW_Left, LOW);
  digitalWrite(pinCC_Left, LOW);
  digitalWrite(pinSpeed_Left, LOW);
  digitalWrite(pinCW_Right, LOW);
  digitalWrite(pinCC_Right, LOW);
  digitalWrite(pinSpeed_Right, LOW);
}

void count_Left(){
  //just increment left once
  enc_left++;
}

void count_Right(){
  //just increment right once
  enc_right++;
}

/* start control block
 * Since the difference between controls is just
 * conversions and direction changes, we can use
 * 1 function (simply named go) to do all the movement
 * and the directions to simply set directions and distance
 */
void forward(unsigned int distance, int spd){
  Serial.print("Forward: ");
  go(true, false, distance*75/2, spd);
}

void left(unsigned int distance, int spd){
  Serial.print("Left: ");
  go(false, false, distance*34/9, spd);
}

void right(unsigned int distance, int spd){
  Serial.print("Right: ");
  go(true, true, distance*34/9, spd);
}

void go(boolean LCC, boolean RCC, unsigned int distance, int spd){
  //start stopped
  stop();

  //reset encoder ticks
  enc_left = 0;
  enc_right = 0;

  //set direction
  set_LCC(LCC);
  set_RCC(RCC);
  
  //set speed
  analogWrite(pinSpeed_Right, spd);
  analogWrite(pinSpeed_Left, spd);

  //compensate while both wheels have not gone the full distance
  do{compensate(spd);} while((enc_left < distance)&&(enc_right < distance));

  //stop
  stop();
 
  //print out useful info
  Serial.print(" Left: ");
  Serial.print(enc_left);
  Serial.print(" Right: ");
  Serial.print(enc_right);
  Serial.print(" Diff: ");
  Serial.print(diff());
  Serial.println(' ');

  //reset encoders
  enc_left = 0;
  enc_right = 0;
  return;
}

void compensate(int spd){
  //simply compensate by the difference between encoders squared
  int comp_speed = spd + (diff())^2;

  //overflows are bad
  if (comp_speed >= 255){
    comp_speed = 255;
  } 

  //set correct speeds
  if (enc_left > enc_right){
    analogWrite(pinSpeed_Left, spd);
    analogWrite(pinSpeed_Right, comp_speed);
  }
  if (enc_right > enc_left){
    analogWrite(pinSpeed_Right, spd);
    analogWrite(pinSpeed_Left, comp_speed);
  }
  if (enc_right == enc_left){
    analogWrite(pinSpeed_Left, spd);
    analogWrite(pinSpeed_Right, spd);
  }
}

void set_LCC(boolean LCC){
  if (LCC){
    digitalWrite(pinCC_Left, HIGH);
  }
  else{
    digitalWrite(pinCW_Left, HIGH);
  }
}

void set_RCC(boolean RCC){
  if (RCC){
    digitalWrite(pinCC_Right, HIGH);
  }
  else{
    digitalWrite(pinCW_Right, HIGH);
  }
}

int diff(){
  if (enc_left >= enc_right){
    return (enc_left - enc_right);
  }
  else{
    return (enc_right - enc_left);
  }
}
