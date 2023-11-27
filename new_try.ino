//Define Pins for Motor 1
#define Motor1_stp 28 //base 
#define Motor1_dir 36 

//Define Pins for Motor 2
#define Motor2_stp 22 //belt
#define Motor2_dir 30


//Set up timing
unsigned long previousMotor1Time = millis();
unsigned long previousMotor2Time = millis();
long Motor1Interval = 10;
long Motor2Interval = 1;


void setup() {

  Serial.begin(9600);
  pinMode(Motor1_stp, OUTPUT);
  pinMode(Motor1_dir, OUTPUT);

  pinMode(Motor2_stp, OUTPUT);
  pinMode(Motor2_dir, OUTPUT);

  digitalWrite(Motor1_dir, LOW);
  digitalWrite(Motor2_dir, LOW); 

}
//In the loop, use the millis() command again for both motors, and save it to a new variable. This will be used to determine how much time has passed. Write the STEP pins to LOW. For each motor, if the current time subtracted by the time previous time is greater than the defined time interval for that motor, then write the STEP pin to HIGH to move the motor one step. Then reset the start time to the current time. This is explained in the code snippet below.

void loop() {
  unsigned long currentMotor1Time = millis();
  unsigned long currentMotor2Time = millis();

 // Serial.println(currentMotor1Time);
 // Serial.println(currentMotor2Time);
  digitalWrite(Motor1_stp, LOW);
  digitalWrite(Motor2_stp, LOW);

  

  if(currentMotor1Time - previousMotor1Time > Motor1Interval){
    digitalWrite(Motor1_stp, HIGH);
    previousMotor1Time = currentMotor1Time;
  }


  if(currentMotor2Time - previousMotor2Time > Motor2Interval){
    digitalWrite(Motor2_stp, HIGH);
    previousMotor2Time = currentMotor2Time;
  }
}