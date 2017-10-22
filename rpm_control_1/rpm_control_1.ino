

// AUTONOMOUS TOOLCART CAPSTONE PROJECT

// 10/22/2017

// RPM Control System 

// Objective: 	Given a step rpm input, ensure motor output rpm 
// 							reaches specified value with minimum steady state 
//							error. 

// Approach 1: 	Count the number of ticks in a set amount of time. 
//							Dt is determined by the clock setting (CSxx). 
// 						



#define BIT(a) (1<<(a))

const double max_volt = 0.5;


// Pins for right motor

int r_mot = 10;
int r_d1 = 8; 
int r_d2 = 6;


// Pins for left motor

int l_mot = 9; 
int l_d1 = 4; 
int l_d2 = 5; 


// Encoder info

double ticks_rev_r = 235;
double ticks_rev_l = 235;
int l_enc = 2; // pin INT0
int r_enc = 3; // pin INT1



// CONTROL SYSTEM VARIABLES //

// cs_start is set to true in the timer compare function 
// (controls timing of control system). 
volatile bool cs_start = false; 
volatile int waste = 0;

int ms_per_cmp = 16; //16ms per compare match interrupt generated (depends on clock)
int n_cmp = 6; //
int dt = ms_per_cmp*n_cmp;
int cmp_count = 0;

int e_r,u_r; //error and command control signal, respectively
double kp = 1; //proportional control gain

double r_rpm; 
double l_rpm; 
volatile int r_ticks = 0;
volatile int l_ticks = 0;




void setup() {
  
	// Configure hardware pins. 
	// to do: conceal this in function  
	
  pinMode(r_mot,OUTPUT);
  pinMode(r_d1,OUTPUT);
  pinMode(r_d2,OUTPUT);
  pinMode(r_enc,INPUT);   
	pinMode(l_mot,OUTPUT);
  pinMode(l_d1,OUTPUT);
  pinMode(l_d2,OUTPUT);
  pinMode(l_enc,INPUT);

  set_direction(0);

	
	// SET TIMER REGISTERS 
	
	// Combination of timer clock prescalar and output compare interrupt 
	// at 250 ticks means that output compare interrupt vector happens 
	// every 16ms. 
	
  TCCR2A = 0;
  TCCR2B = 0;  
	TCCR2B = BIT(CS22) | BIT(CS21) | BIT(CS20); // 1024 prescalar. Compare match at 250ticks = 16ms
	TIMSK2 = BIT(OCIE2A); // enable compare match interrupt. (Output Compare Interrupt Enable)
	OCR2A = 250; // match compare value
  
	
	// Set external interrupt registers
	
  EIMSK = 0;
  EIMSK = BIT(INT0) | BIT(INT1); //enable interrupts on puns 2 and 3
  EICRA = BIT(ISC11) | BIT(ISC10) | BIT(ISC01)| BIT(ISC00); 

	// To do: Incorporate Serial Communication	
  Serial.begin(9600);
 }

 
 
 
void loop() {
	
  while(!cs_start){waste++;} 
  
	   
  
  // HERE'S THE MEAT AND BONES...control system:

	iter_count++;
	
  // 1. Get joystick info (INPUT);
  // todo: add dan's code for joystick. 
  // in future: commands from computer. 
  
  double ratio = 1;  // Left and right motors go at same speed
  double v_lin = 10000;  // Desired linear velocity. 

  
  // 2. Read sensors (OUTPUT);
  r_rpm = 60*(r_ticks/ticks_rev_r) / (cs_dt*iter_count);
  l_rpm = 60*(l_ticks/ticks_rev_l) / (cs_dt*iter_count);
	
  // 3. Compute error signal (INPUT - OUTPUT)
	e_r = (v_lin - r_rpm);
	
	
	
	
	// 4. Compute control signal
  if(e_r < 0 ) e_r = 0;
	
	u_r = kp*e_r;
	
	if(u_r > 255) u_r = 255;
	
	
  // 5. Write actuators (CONTROL SIGNAL); 
  analogWrite(r_mot,u_r);
  analogWrite(l_mot,100);

  
  // 5. Housekeeping.
	
  if(iter_count == cs_iter){
		
		iter_count = 0;
		r_ticks = 0;
		l_ticks = 0;
		
    Serial.print(u_r);
    Serial.print("  ");
    Serial.println(r_rpm);
  }
  
  cs_start = false; 
}

ISR(INT0_vect){
  l_ticks++;
}

ISR(INT1_vect){
  r_ticks++;
}



ISR(TIMER2_OVF_vect){
  cs_start = true;  
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


