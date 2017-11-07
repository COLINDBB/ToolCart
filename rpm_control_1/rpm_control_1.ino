

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
double dt = ms_per_cmp*n_cmp*.001;
int cmp_count = 0;

double v_lin = 0;
double ratio = 1;

int e_r,u_r,e_l,u_l; //error and command control signal, respectively
double kp = 2; //proportional control gain

double r_rpm; 
double l_rpm; 
volatile int r_ticks = 0;
volatile int l_ticks = 0;


// TESTING VARIABLES 
 	int delay_s;
	int target;
	int iter;
	int targit;


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
	
	
	
	
	
	
	// Test variables. 
	kp = 3;
	n_cmp = 1;
	dt = ms_per_cmp*n_cmp*.001;
	delay_s = 4;
	target = 150;
	targit = delay_s/dt;
	iter = 0;
	
 }

 
void loop() {
	
	
  while(!cs_start){waste++;} 
  

// CONTROL LOOP BEGINS HERE (EVERY dt = n_cmp*ms_per_cmp MILLISECONDS)						
	


	
// 1. RECEIVE COMMAND (INPUT)
// 							todo: add dan's code for joystick. 
// 							in future: commands from computer. 
// 							incorporate kinematic equations (rpm_r and l based on R and Vlin)
//							  
//
//							a. Hard-coded step inputs. kicks in after "targit" seconds
	iter++; 
	if(iter == targit) v_lin = target; 
	//if(iter == 4*targit) v_lin = target*2;
	//if(iter == 8*targit) v_lin = target*3;

	
//							b. Inputs from the serial monitor. 

	//if(Serial.available()){
	//	v_lin = Serial.parseInt();
	//}	


//							c. Constant inputs. 	

	//ratio = 1;  // Left and right motors go at same speed
	//v_lin = 250;  // Desired linear velocity. 
	//r_rpm_target = ; 
	//l_rpm_target = ;
  
	
	
	
// 2. READ SENSOR (OUTPUT)

  r_rpm = 60*(r_ticks/ticks_rev_r) / dt;
  l_rpm = 60*(l_ticks/ticks_rev_l) / dt;
	r_ticks = 0; //reset tick counts
	l_ticks = 0;
	
// 3. COMPUTE ERROR SIGNAL (E = INPUT - OUTPUT)

	e_r = (v_lin - r_rpm);
	e_l = (v_lin - l_rpm);
	
	
	
// 4. COMPUTE CONTROL SIGNAL (U)
//							todo: saturate the inputs
//							todo: make sure negatives dont go through
//							but incorporate in future
//							todo: determine the controller (lol) 
//							current controller = proportional control (kp);

  if(e_r < 0 ){		
		set_direction(3); //reverse motor
		e_r = 0; //don't reverse the motors. 
	}

 if(e_l < 0 ){   
    set_direction(3); //reverse motor
    e_l = 0; //don't reverse the motors. 
  }
	else set_direction(0);
	
	u_r = kp*abs(e_r);
  u_l = kp*abs(e_l);
	
	if(u_r > 255) u_r = 255; //saturation inputs
	if(u_l > 255) u_l = 255;
	
	
	
// 5. WRITE ACTUATORS WITH CONTROL SIGNAL
//							 
	
  analogWrite(r_mot,u_r);
  analogWrite(l_mot,u_l);

  
// 6. MISCELLANEOUS 
	
	Serial.print(u_r);
  Serial.print("  ");
  Serial.println(r_rpm);

  
  
	cs_start = false; 
}













ISR(INT0_vect){
  l_ticks++;
}

ISR(INT1_vect){
  r_ticks++;
}



ISR(TIMER2_COMPA_vect){
	cmp_count++;
	if(cmp_count==n_cmp){
		cs_start = true;
		cmp_count = 0;
	}
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


