#define BIT(a) (1<<(a))
const double max_volt = 0.8;
// Initialize hardware pins


/*...................*/
// Approach 2: Count the time between ticks. Have to be careful about interrupt priorities. 
// This approach is NOT reliable so far. 

int r_enc = 2; // pin INT0
int l_enc = 3; // pin INT1
//int l_enc2 = 12;
//cint r_enc2 = 11; 
int r_mot = 10;
int r_d1 = 8; 
int r_d2 = 6;
int l_mot = 9; 
int l_d1 = 4; 
int l_d2 = 5; 


// Control system variables

double ticks_rev_r = 235;
double ticks_rev_l = 235;
double dt = 0;

//Timer count variables
double t1 = 0;
double t2 = 0;


void setup() {
  
  pinMode(r_mot,OUTPUT);
  pinMode(r_d1,OUTPUT);
  pinMode(r_d2,OUTPUT);
  pinMode(l_mot,OUTPUT);
  pinMode(l_d1,OUTPUT);
  pinMode(l_d2,OUTPUT);
  pinMode(l_enc,INPUT);
  pinMode(r_enc,INPUT);


  set_direction(0);

  TCCR2A = 0; //set timer2 to default
  TCCR2B = 0; // same
  TCCR2B = BIT(CS22)| BIT(CS20); // .004ms/tick
  

  EIMSK = 0;
  EIMSK = BIT(INT1);// | BIT(INT0); //enable interrupts on puns 2 and 3
  EICRA = BIT(ISC10);// | BIT(ISC01)| BIT(ISC00);  
  Serial.begin(9600);
    analogWrite(r_mot,70);
  analogWrite(l_mot,40);

 }

void loop() {

  
  //every 16ms, cs_start is set to true in the overflow function and the loop begins.    
  
  // Control system goes here. .
  
  
  // 1. Get joystick info (INPUT);
  double ratio = 1;  // Left and right motors go at same speed
  double v_lin = 1;  // Desired linear velocity. 
  //convert to rpm
  
  // 2. Read sensors (OUTPUT);
 
  
  // 3. Compute error si100gnal (INPUT - OUTPUT)

  
  // 4. Write actuators (CONTROL SIGNAL); 
  Serial.println(dt);
  
  
}


ISR(INT1_vect){
  t2 = t1; 
  t1 = TCNT2;
  dt = t1-t2;
}






void set_direction(int dir){

  // direction is what direction you want, d1 and d2 
  
  if(dir==0){                 //go straight
    digitalWrite(r_d1,LOW);
    digitalWrite(r_d2,HIGH);
    digitalWrite(l_d1,HIGH);
    digitalWrite(l_d2,LOW);
  }
  else if(dir==1){            // turn right on the spot
    digitalWrite(r_d1,LOW);
    digitalWrite(r_d2,HIGH);
    digitalWrite(l_d1,LOW);
    digitalWrite(l_d2,HIGH);
  }
   else if(dir==2){
    digitalWrite(r_d1,HIGH); //turn left on the spot
    digitalWrite(r_d2,LOW);
    digitalWrite(l_d1,HIGH);
    digitalWrite(l_d2,LOW);
   }
   else if(dir==3){
    digitalWrite(r_d1,HIGH);
    digitalWrite(r_d2,LOW);
    digitalWrite(l_d1,LOW);
    digitalWrite(l_d2,HIGH);
   }
}


