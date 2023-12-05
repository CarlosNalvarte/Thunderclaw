//All these motors are connected to a bipolar stepper motor driver A4988. We use only the STEP and DIR pins (control pins), 12V motors
//All inputs work with 5V
//The claw(servo) works with 5V
#include <Servo.h>
Servo CLAW;

//INPUTS PIN DEFINITION
const int Opto_Art1 = 42;
const int Opto_Art2 = 44;
const int Opto_Art3 = 48;
const int Opto_Art4 = 49;
const int SW_Art56 = 47;

//OUTPUTS PIN DEFINITION
const int Art1_STEP = 28;
const int Art1_DIR = 36;

const int Art2_Left_STEP = 26;
const int Art2_Left_DIR = 34;

const int Art2_Right_STEP = 24;
const int Art2_Right_DIR = 32;

const int Art3_Belt_STEP = 22;
const int Art3_Belt_DIR = 30;

const int Art4_STEP = 23;
const int Art4_DIR = 31;

const int Art56_Left_STEP = 25;
const int Art56_Left_DIR = 33;

const int Art56_Right_STEP = 27;
const int Art56_Right_DIR = 35;

const int Claw = 7;
const int CLAW_pos = 0;

//COUNTING STEPS VARIABLES

int art1_steps = 0;  //all these variables count steps from -32,768 to 32,767 (int datatype range)
int art3_steps = 0;
int art4_steps = 0;
int art5_steps = 0;
int art6_steps = 0;

void setup() {
  //Code that runs once
  CLAW.attach(7);
  Serial.begin(9600);
  while (!Serial)
    ;

  //INPUTS PIN DEFINITION
  //SENSORS INPUTS
  pinMode(42, INPUT);  //Art1 Optodisk 5V
  pinMode(44, INPUT);  //Art2 Optodisk 5V
  pinMode(48, INPUT);  //Art3 LOWER Optodisk 5V
  pinMode(49, INPUT);  //Art4 UPPER Optodisk 5V
  pinMode(47, INPUT);  //Art56 Endstop Switch 5V

  //STEP OUTPUTS-------------------------------------
  pinMode(28, OUTPUT);  //ART 1 STEP BASE
  pinMode(26, OUTPUT);  //ART 2 STEP LEFT MOTOR
  pinMode(24, OUTPUT);  //ART 2 STEP RIGHT MOTOR
  pinMode(22, OUTPUT);  //ART 3 STEP BELT MOTOR
  pinMode(23, OUTPUT);  //ART 4 STEP HEAD BASE MOTOR
  pinMode(25, OUTPUT);  //ART 56 STEP LEFT MOTOR
  pinMode(27, OUTPUT);  //ART 56 STEP RIGHT MOTOR

  //DIRECTION OUTPUTS--------------------------------
  pinMode(36, OUTPUT);  //ART 1 DIR BASE
  pinMode(34, OUTPUT);  //ART 2 DIR LEFT MOTOR
  pinMode(32, OUTPUT);  //ART 2 DIR RIGHT MOTOR
  pinMode(30, OUTPUT);  //ART 3 DIR BELT MOTOR
  pinMode(31, OUTPUT);  //ART 4 DIR HEAD BASE MOTOR
  pinMode(33, OUTPUT);  //ART 56 DIR LEFT MOTOR
  pinMode(35, OUTPUT);  //ART 56 DIR RIGHT MOTOR

  //CLAW OUTPUT-------------------------------------
  //pinMode(7, OUTPUT);  //CLAW PWM OUTPUT 5V
}

void loop() {

  //this small section is useful to test the functionality of the opto sensors and switc
  /*
  Serial.println("Gladys says: ");
  Serial.print("Art1: ");
  Serial.println(digitalRead(42));
  Serial.print("Art3: ");
  Serial.println(digitalRead(48));
  Serial.print("Art4: ");
  Serial.println(digitalRead(49));
  Serial.print("SWITCH: ");
  Serial.println(digitalRead(47));
  delay(1000);
   
  Serial.println("art1: ");
  Serial.println(art1_steps);
  Serial.println("art3: ");
  Serial.println(art3_steps);
  Serial.println("art4: ");
  Serial.println(art4_steps);
  Serial.println("art5: ");
  Serial.println(art5_steps);
  Serial.println("art6: ");
  Serial.println(art6_steps);
  delay(1000);
*/

  auto aval = Serial.available();

  if (aval > 0) {
    auto input = Serial.readStringUntil('\r');
    auto read = input.length();
    while (aval > ++read)
      Serial.read();
    CLAW.write(45);
    auto incomingByte = input.toInt();

    switch (incomingByte) {

      case 1:  //HOME POSITION
        Serial.println("HOMIE POSITION");
        //Serial.println("Major Tom is going HOME position");
        //It can be tested in group

        //something that can be added to the 'home' function is a parameter to increase/decrease the steps (FOR loops)
        //and also how fast the movement is (set up the delays with a constant variable)
        // STEP, DIR, OPTO
        home1Motor(28, 36, 42);  // delay(2500); //this works for articulation 1 (rotating base)
                                 //I haven't defined code for articulation 2 as it's not working properly - 2 motors at the same time
        home1Motor(22, 30, 48);  // delay(2500);//this works for articulation 3 (open belt) - currently working perfectly. The belt and the motor hold the weight of the head
        home1Motor(23, 31, 49);  // delay(2500);//this works for articulation 4 (base of the head)
        //for all 3 'home' positions, only one motor is used to perform the movement
        //now I have to do a separate function for the head as it uses a endstop switch to set its initial position
        home2motors_head(25, 33, 47);          // makes contact with the switch and then returns back the base of the claw to the middle of the head
        home_claw_column(25, 33, 27, 35, 47);  //rotates the base of the claw a certain number of degrees based on the number of steps
                                               //needs hard testing to be accurate and a base position to start with



        Serial.println("HOME MODE WAS REACHED!");
        claw_game();

        break;

      case 2:                         //AUTO MODE
        Serial.println("AUTO MODE");  //Need to wait until the swivel is finished
                                      // Serial.println("Transformers: AUTO-robots in disguise");
        break;

      case 3:  //MANUAL MODE
        Serial.println("MANUAL MODE");
        Serial.println("2003 Mazda Protege5 Standard/MANUAL transmission. Not on sale anymore");
        while (true) {

          auto aval2 = Serial.available();
          //Serial.println("I'm waiting");
          if (aval2 > 0) {
            auto input2 = Serial.readStringUntil('\r');
            auto read2 = input2.length();
            while (aval2 > ++read2)
              Serial.read();

            auto incomingByte2 = input2.toInt();

            switch (incomingByte2) {

              case 1:
                Serial.println("he he boi");
                break;
              case 2:
                Serial.println("You got it");
                break;
              case 3:
                Serial.println("No");
                break;
              case 4:
                Serial.println("EXIT KEYBIND PRESSED");
                break;

              default:
                //This has to be redefined when the keybind map is defined (1 keybind per sense of movement per articulation)
                //In total: 2 keybinds per articulation (include limits/range of movement ?)

                Serial.println("Wrong Input. This is a friendly reminder for the available options:");
                Serial.println("1 - UP ");
                Serial.println("2 - DOWN ");
                Serial.println("3 - SOMETHING ");
                break;
            }
            //            Serial.println(incomingByte2);
            if (incomingByte2 == 4) {
              Serial.println("Welcome back to the main menu. Here's the available options: ");
              Serial.println("1 - HOME");
              Serial.println("2 - AUTO");
              Serial.println("3 - MANUAL");
              break;
            }
            //break;
          }
        }
        // HOME_MODE();  //set the robotic arm at home position first so I can count the steps going left or right (DIR pin)
        /*
        int a_home_value = 0;

        int a_CW = manual1MotorCW(28, 36);  //articulation 1 base CW
        a_home_value = a_home_value + a_CW;

        int a_CCW = manual1MotorCCW(28, 36);  //articulation 1 base CCW
        a_home_value = a_home_value + a_CCW;

*/
        //int b=manual1Motor(22,30); //articulation 3 open belt
        //int c=manual1Motor(23,31); //articulation 4 head base

        //still need to work on how to manually control articulation 56 for both type of movements Nov 22nd 2:29PM

        break;

      case 4:
        Serial.println("tests");

        // home1Motor(28, 36, 42);                //art 1
        // home1Motor(22, 30, 48);                //art 3
        //  home1Motor(23, 31, 49);                //art 4
        //  home2motors_head(25, 33, 47);          //art56 left to right movement
        // home_claw_column(25, 33, 27, 35, 47);  // rotation of the claw column

        // delay(2000);
        //  returnHOME(28, 36, 22, 30, 23, 31, 25, 33);
        //claw_game();
        moveit(0, -10,0, 0);
        Serial.println("I just moved it to pos 1");
        //delay(1000);
        //moveit(0, 0, 0, 0);
        //Serial.println("I just moved it to pos 2");

        // HOME_MODE();
        // claw_game();
        break;

      case 5:
        //Claw_pos can only go from 0 to 90. Even better from 5 to 85 just to avoid friction or forceful movements
        claw_game();
        break;

      default:
        Serial.println("Wrong user input. For future reference here's the available modes:");
        Serial.println("1 - HOME");
        Serial.println("2 - AUTO");
        Serial.println("3 - MANUAL");
        break;
    }
  }
}


//FROM HERE YOU WILL ONLY SEE FUNCTIONS THAT THE MAIN LOOP USES

void home1Motor(const int STEP, const int DIR, const int OPTO) {
  /*
  Serial.println("--------------");  Serial.println(STEP);  Serial.println(DIR);  Serial.println(OPTO);
*/
  Serial.println("the motor is just about to rotate CW");
  delay(500);
  digitalWrite(DIR, HIGH);  //MOTOR GOES CW

  for (int i = 0; i < 14000; i++) {  //1035 was gud for a full rotation
    //I think I was using full step mode

    if (digitalRead(OPTO) == LOW) {
      Serial.println("the sensor detected a notch after turning CW");
      //i=0;
      digitalWrite(DIR, LOW);
      break;
    }
    if (STEP == 28) { art1_steps++; }
    if (STEP == 22) { art3_steps++; }
    if (STEP == 23) { art4_steps++; }
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);   //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
    digitalWrite(STEP, LOW);  //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
    delayMicroseconds(400);


    //delay(1); //this variable could be controlled
  }
  if (digitalRead(OPTO) == HIGH) {
    Serial.println("the sensor didn't detect a notch");

    digitalWrite(DIR, LOW);
    Serial.println("the motor is just about to rotate CCW");
    delay(500);
    for (int j = 0; j < 20000; j++) {  //1035 was gud for a full rotation
                                       //I think I was using full step mode
      if (STEP == 28) { art1_steps--; }
      if (STEP == 22) { art3_steps--; }
      if (STEP == 23) { art4_steps--; }
      digitalWrite(STEP, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP, LOW);
      delayMicroseconds(400);

      //delay(1);
      if (digitalRead(OPTO) == LOW) {
        Serial.println("the sensor detected a notch after turning CCW");

        break;
      }

    }  // im using two breaks for both senses of rotation
  }
  Serial.print("Sensor value is: ");
  Serial.println(digitalRead(OPTO));
  if (digitalRead(OPTO) == 0) {
    Serial.println("The notch was found!");
  } else {
    Serial.println("I didn't find the notch :(");
  }

  Serial.println("Art1_steps: ");
  Serial.println(art1_steps);
  Serial.println("Art3_steps: ");
  Serial.println(art3_steps);
  Serial.println("Art4_steps: ");
  Serial.println(art4_steps);
  Serial.println("Art5_steps: ");
  Serial.println(art5_steps);
  Serial.println("Art6_steps: ");
  Serial.println(art6_steps);


  Serial.println("Next sequence is about to start");
}
int home2motors_head(int STEP, int DIR, int SW)  //this moves the claw support from left to right until it reaches the switch
                                                 //after that, it returns the claw column back to the middle position based on the number of steps
                                                 //set at the for loop
{
  Serial.println("the motor is just about to rotate CW");
  delay(500);
  digitalWrite(DIR, LOW);            //MOTOR GOES CCW
  for (int i = 0; i < 10000; i++) {  //1035 was gud for a full rotation

    if (digitalRead(SW) == HIGH) {
      Serial.println("The switch was found!!");  //as soon as the switch is pressed by the claw column, the claw goes back to the mid section of the robotic arm head

      digitalWrite(DIR, HIGH);
      break;
    }
    if (STEP == 25) { art5_steps--; }
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);   //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
    digitalWrite(STEP, LOW);  //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
    delayMicroseconds(350);

    //delay(1); //this variable could be controlled
  }
  digitalWrite(DIR, HIGH);          //MOTOR GOES CCW
  for (int i = 0; i < 3200; i++) {  //1035 was gud for a full rotation
    //I think I was using full step mode
    if (STEP == 25) { art5_steps++; }
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);   //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
    digitalWrite(STEP, LOW);  //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
    delayMicroseconds(350);
    //int inc;
    // inc++;
    //Serial.println(inc);
    //delay(1); //this variable could be controlled
  }
  Serial.println("the base of the claw has returned to the middle");

  Serial.println("Art1_steps: ");
  Serial.println(art1_steps);
  Serial.println("Art3_steps: ");
  Serial.println(art3_steps);
  Serial.println("Art4_steps: ");
  Serial.println(art4_steps);
  Serial.println("Art5_steps: ");
  Serial.println(art5_steps);
  Serial.println("Art6_steps: ");
  Serial.println(art6_steps);
}


int home_claw_column(int STEP1, int DIR1, int STEP2, int DIR2, int SW) {
  Serial.println("the claw is about to rotate on its own axis");
  //delay(1000);
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  for (int i = 0; i < 6400; i++) {

    if (digitalRead(SW) == HIGH) {
      digitalWrite(DIR1, LOW);
      digitalWrite(DIR2, LOW);
      Serial.println("somehow I pressed the switch");
      break;
    }

    if (digitalRead(SW) == LOW) {
      art6_steps++;
      digitalWrite(STEP1, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP1, LOW);
      delayMicroseconds(550);

      digitalWrite(STEP2, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP2, LOW);
      delayMicroseconds(550);
    }
  }
  Serial.println("the claw has rotated");

  Serial.println("Art1_steps: ");
  Serial.println(art1_steps);
  Serial.println("Art3_steps: ");
  Serial.println(art3_steps);
  Serial.println("Art4_steps: ");
  Serial.println(art4_steps);
  Serial.println("Art5_steps: ");
  Serial.println(art5_steps);
  Serial.println("Art6_steps: ");
  Serial.println(art6_steps);
  delay(1);
}

void HOME_MODE() {  //gathers all the home position commands into one function

  home1Motor(28, 36, 42);                //art 1
  home1Motor(22, 30, 48);                //art 3
  home1Motor(23, 31, 49);                //art 4
  home2motors_head(25, 33, 47);          //art56 left to right movement
  home_claw_column(25, 33, 27, 35, 47);  // rotation of the claw column
}

void moveit(int amount_of_steps_1, int amount_of_steps_2, int amount_of_steps_3, int amount_of_steps_4) {

  int STEP_NUMBERS[4] = { amount_of_steps_1, amount_of_steps_2, amount_of_steps_3, amount_of_steps_4 };
  int STEPS_PER_MOTOR[4] = { amount_of_steps_1, amount_of_steps_2, amount_of_steps_3, amount_of_steps_4 };

  int n = sizeof(STEP_NUMBERS) / sizeof(STEP_NUMBERS[0]);


  bubbleSort(STEP_NUMBERS, n);

  for (int i = 0; i < n; i++) {
    Serial.print(STEP_NUMBERS[i]);
    Serial.print(" ");
  }
  Serial.println(" ");


  parallel_motors(STEP_NUMBERS, STEPS_PER_MOTOR, 28, 36, 22, 30, 23, 31, 25, 33);
}
void bubbleSort(int arr[], int n) {  //bubble sorting algorithm

  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        // swap arr[j] and arr[j+1]
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}

void parallel_motors(int array[], int steps[],
                     const int step_a, const int dir_a,
                     const int step_b, const int dir_b,
                     const int step_c, const int dir_c,
                     const int step_d, const int dir_d)

{
  if ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) && (array[3] == 0)) {
    Serial.println("no valid values detected");
    return;
  }

  int max_step = 0;
  int mod_array[4];

  if (steps[0] >= 0) {
    digitalWrite(dir_a, HIGH);
  } else {
    digitalWrite(dir_a, LOW);
  }

  if (steps[1] >= 0) {
    digitalWrite(dir_b, HIGH);
  } else {
    digitalWrite(dir_b, LOW);
  }

  if (steps[2] >= 0) {
    digitalWrite(dir_c, HIGH);
  } else {
    digitalWrite(dir_c, LOW);
  }

  if (steps[3] >= 0) {
    digitalWrite(dir_d, HIGH);
  } else {
    digitalWrite(dir_d, LOW);
  }

  Serial.println("ORIGINAL SORTING ARRAY - LOWER TO GREATER");
  Serial.print("first #: ");
  Serial.println(array[0]);
  Serial.print("second #: ");
  Serial.println(array[1]);
  Serial.print("third #: ");
  Serial.println(array[2]);
  Serial.print("fourth #: ");
  Serial.println(array[3]);

  Serial.println("MODIFIED SORTING ARRAY - LOWER TO GREATER FOR NEGATIVE NUMBERS");
  mod_array[0] = abs(array[0]);
  mod_array[1] = abs(array[1]);
  mod_array[2] = abs(array[2]);
  mod_array[3] = abs(array[3]);

  int ABS_STEP_NUMBERS[4] = { mod_array[0], mod_array[1], mod_array[2], mod_array[3] };

  int n = sizeof(ABS_STEP_NUMBERS) / sizeof(ABS_STEP_NUMBERS[0]);
  bubbleSort(ABS_STEP_NUMBERS, n);
  Serial.println("///////////////////////////////////////// ");
  for (int i = 0; i < n; i++) {
    Serial.print(ABS_STEP_NUMBERS[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  Serial.println("///////////////////////////////////////// ");






  Serial.println("STEPS PER MOTOR - ENTERED WITH MOVEIT FN");
  Serial.print("first #: ");
  Serial.println(steps[0]);
  Serial.print("second #: ");
  Serial.println(steps[1]);
  Serial.print("third #: ");
  Serial.println(steps[2]);
  Serial.print("fourth #: ");
  Serial.println(steps[3]);





  max_step = ABS_STEP_NUMBERS[3];
  Serial.println("Max value in the STEP number array is: ");
  Serial.println(ABS_STEP_NUMBERS[3]);
  int a = 0, b = 0, c = 0, d = 0;
  int art1 = 0, art3 = 0, art4 = 0, art5 = 0;

  digitalWrite(step_a, LOW);
  digitalWrite(step_b, LOW);
  digitalWrite(step_c, LOW);
  digitalWrite(step_d, LOW);

  for (int i = 0; i <= max_step; i++) {
    if ((abs(a) <= abs(steps[0]) && (art1 == 0) && (steps[0]!=0))) {
      if (steps[0] >= 0) {
        a++;
      } else {
        a--;
      }
      digitalWrite(step_a, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_a, LOW);
      delayMicroseconds(350);
    }

    if (abs(a) == abs(steps[0])) {
      art1 = 1;
    }


    if ((abs(b) <= abs(steps[1]) && (art3 == 0) && (steps[1]!=0))) {
      if (steps[1] >= 0) {
        b++;
      } else {
        b--;
      }
      digitalWrite(step_b, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_b, LOW);
      delayMicroseconds(350);
    }

    if (abs(b) == abs(steps[1])) {
      art3 = 1;
    }

    if ((abs(c) <= abs(steps[2])) && (art4 == 0) && (steps[2]!=0)) {
      if (steps[2] >= 0) {
        c++;
      } else {
        c--;
      }
      digitalWrite(step_c, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_c, LOW);
      delayMicroseconds(350);
    }

    if (abs(c) == abs(steps[2])) {
      art4 = 1;
    }

    if ((abs(d) <= abs(steps[3]) && (art5 == 0) && (steps[3]!=0))) {
      if (digitalRead(SW_Art56) == 0) {
        if (steps[3] >= 0) {
          d++;
        } else {
          d--;
        }
        digitalWrite(step_d, HIGH);
        delayMicroseconds(100);
        digitalWrite(step_d, LOW);
        delayMicroseconds(350);
      }
    }

    if (abs(d) == abs(steps[3])) {
      art5 = 1;
    }
  }
  Serial.println("before the sum: ");
  Serial.println(art1_steps);
  Serial.println(art3_steps);
  Serial.println(art4_steps);
  Serial.println(art5_steps);

  art1_steps = art1_steps + a;
  art3_steps = art3_steps + b;
  art4_steps = art4_steps + c;
  art5_steps = art5_steps + d;

  Serial.println("after the sum: ");
  Serial.println(art1_steps);
  Serial.println(art3_steps);
  Serial.println(art4_steps);
  Serial.println(art5_steps);
  Serial.println("a/b/c/d values added: ");
  Serial.println(a);
  Serial.println(b);
  Serial.println(c);
  Serial.println(d);

  if (art1 == 1 && art3 == 1 && art4 == 1 && art5 == 1) {
    Serial.println("MISSION COMPLETE");
  }
}


void claw_game() {
  //the claw dancing like it should

  CLAW.write(5);
  delay(250);
  CLAW.write(15);
  delay(250);
  CLAW.write(25);
  delay(250);
  CLAW.write(35);
  delay(250);
  CLAW.write(45);
  delay(250);
  CLAW.write(55);
  delay(250);
  CLAW.write(65);
  delay(250);
  CLAW.write(75);
  delay(250);
  CLAW.write(85);
  delay(250);

  for (int CLAW_pos = 5; CLAW_pos <= 85; CLAW_pos += 1) {
    CLAW.write(CLAW_pos);
    delay(25);
  }

  for (int CLAW_pos = 85; CLAW_pos >= 5; CLAW_pos -= 1) {
    CLAW.write(CLAW_pos);
    delay(25);
  }

  CLAW.write(45);
}

void returnHOME(const int step_a, const int dir_a,
                const int step_b, const int dir_b,
                const int step_c, const int dir_c,
                const int step_d, const int dir_d) {

  int a = 0, b = 0, c = 0, d = 0;

  digitalWrite(step_a, LOW);
  digitalWrite(step_b, LOW);
  digitalWrite(step_c, LOW);
  digitalWrite(step_d, LOW);

  Serial.println("1st art steps is: ");
  Serial.println(art1_steps);
  Serial.println("3rd art steps is: ");
  Serial.println(art3_steps);
  Serial.println("4th art steps is: ");
  Serial.println(art4_steps);
  Serial.println("5th art steps is: ");
  Serial.println(art5_steps);
  /////////////////////////////////////////////////////////ART1
  if (art1_steps >= 0) {
    digitalWrite(dir_a, LOW);
    for (int i = 0; i < art1_steps; i++) {

      if (art1_steps == 0) {
        break;
      }
      a--;
      digitalWrite(step_a, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_a, LOW);
      delayMicroseconds(350);
    }
  }

  if (art1_steps < 0) {
    digitalWrite(dir_a, HIGH);
    for (int i = 0; i > art1_steps; i--) {
      a++;
      digitalWrite(step_a, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_a, LOW);
      delayMicroseconds(350);
    }
  }
  /////////////////////////////////////////////////////////ART3
  if (art3_steps >= 0) {
    digitalWrite(dir_b, LOW);
    for (int i = 0; i < art3_steps; i++) {

      if (art3_steps == 0) {
        break;
      }
      b--;
      digitalWrite(step_b, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_b, LOW);
      delayMicroseconds(350);
    }
  }
  if (art3_steps < 0) {
    digitalWrite(dir_b, HIGH);
    for (int i = 0; i > art3_steps; i--) {
      b++;
      digitalWrite(step_b, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_b, LOW);
      delayMicroseconds(350);
    }
  }

  /////////////////////////////////////////////////////////ART4
  if (art4_steps >= 0) {
    digitalWrite(dir_c, LOW);
    for (int i = 0; i < art4_steps; i++) {
      if (art4_steps == 0) {
        break;
      }
      c--;
      digitalWrite(step_c, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_c, LOW);
      delayMicroseconds(350);
    }
  }
  if (art4_steps < 0) {
    digitalWrite(dir_c, HIGH);
    for (int i = 0; i > art4_steps; i--) {
      c++;
      digitalWrite(step_c, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_c, LOW);
      delayMicroseconds(350);
    }
  }
  /////////////////////////////////////////////////////////ART5

  if (art5_steps >= 0) {
    digitalWrite(dir_d, LOW);
    for (int i = 0; i < art5_steps; i++) {
      if (art5_steps == 0) {
        break;
      }
      d--;
      digitalWrite(step_d, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_d, LOW);
      delayMicroseconds(350);
    }
  }
  if (art5_steps < 0) {
    digitalWrite(dir_d, HIGH);
    for (int i = 0; i > art5_steps; i--) {
      d++;
      digitalWrite(step_d, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_d, LOW);
      delayMicroseconds(350);
    }
  }

  art1_steps = art1_steps + a;
  art3_steps = art3_steps + b;
  art4_steps = art4_steps + c;
  art5_steps = art5_steps + d;


  Serial.println("RETURN HOME POSITION: ");
  Serial.println("1st art steps is: ");
  Serial.println(art1_steps);
  Serial.println("3rd art steps is: ");
  Serial.println(art3_steps);
  Serial.println("4th art steps is: ");
  Serial.println(art4_steps);
  Serial.println("5th art steps is: ");
  Serial.println(art5_steps);
}

//--------------------------------------------------------------------------------------------------------------------------
//these two functions have NOT been implemented yet  manual1MotorCW and manual1MotorCCW
int manual1MotorCW(int STEP, int DIR) {
  digitalWrite(DIR, HIGH);
  int count_1M = 0;
  for (int i = 0; i > 1000; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP, LOW);
    delayMicroseconds(350);
    count_1M + 100;
  }
  return count_1M;
}

int manual1MotorCCW(int STEP, int DIR) {
  digitalWrite(DIR, LOW);
  int count_1M = 0;
  for (int i = 0; i > 1000; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP, LOW);
    delayMicroseconds(350);
    count_1M - 100;
  }
  return count_1M;
}
