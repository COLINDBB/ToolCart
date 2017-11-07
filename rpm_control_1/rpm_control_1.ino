// AUTONOMOUS TOOLCART CAPSTONE PROJECT

// November 2017

// RPM Control System 

// Objective: 	Given a step rpm input, ensure motor output rpm 
// 							reaches specified value with minimum steady state 
//							error. 

// Approach : 	Get RPM.  
//							Dt is determined by the clock setting (CSxx). 
// 						  Determine if proportional control is adequate. 



#define BIT(a) (1<<(a))
const double max_volt = 0.5;


// Right Motor

int r_mot = 10;
int r_d1 = 8; 
int r_d2 = 6;


// Left Motor

int l_mot = 9; 
int l_d1 = 4; 
int l_d2 = 5; 


// Encoder info

double ticks_rev_r = 235;
double ticks_rev_l = 235;
int l_enc = 2; // pin INT0
int r_enc = 3; // pin INT1



// CONTROL SYSTEM VARIABLES
// In present configuration, 16ms per compare match interrupt generated. The frequency and period of one control loop iteration and is determined both by clock prescalar and how many overflows are required to trigger cs_start. 


// Timing: 

volatile bool cs_start = false;  
volatile int waste = 0;
int ms_per_cmp = 16; 

// number of compare matches before cs_start is triggered

int n_cmp = 6;
int cmp_count = 0;  


// Period of one control loop iteration = dt

double dt = ms_per_cmp*n_cmp*.001;


// Input, error and controller gains

double v_lin;
double input_r_rpm;
double input_l_rpm;
double ratio;
int e_r,u_r,e_l,u_l; //error and control signals
double kp; //proportional term gain


// Feedback variables

double r_rpm; 
double l_rpm; 
volatile int r_ticks = 0;
volatile int l_ticks = 0;


// Testing variables

int delay_s;
int target;
int iter;
int targit;



void setup() {
  
	// Configure pins.   
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
	
	// With 1024 prescalar, compare match at 250ticks = 16ms. 
 
  TCCR2A = 0;
  TCCR2B = 0;  
	TCCR2B = BIT(CS22) | BIT(CS21) | BIT(CS20); 
	TIMSK2 = BIT(OCIE2A); //enable interrupt
	OCR2A = 250; 
  
	
	// Set external interrupt registers
	// enable interrupts on puns 2 and 3
  
  EIMSK = 0;
  EIMSK = BIT(INT0) | BIT(INT1); 
  EICRA = BIT(ISC11) | BIT(ISC10) | BIT(ISC01)| BIT(ISC00); 


	// To do: Incorporate Serial Communication	
  
  Serial.begin(9600);
	
	
	// Test variables: 
 
	kp = 3;




	
 }

 
void loop() {
	
	
  while(!cs_start){waste++;} 
  

// CONTROL LOOP BEGINS HERE (EVERY dt = n_cmp*ms_per_cmp ms)						
	
	
// 1. RECEIVE COMMAND (INPUT)


//  todo: add dan's code for joystick. 
// 	todo: receive commands from computer. 
// 	todo: incorporate kinematic equations (rpm_r and l based on R and Vlin)

  //  a. Inputs from the serial monitor. 

  /*
  if(Serial.available()){
	  v_lin = Serial.parseInt();
  }	
  */


  //  b. Constant inputs. 	

  ratio = 1;  // Left and right motors go at same speed
  v_lin = 50;  // Desired linear velocity. 

  input_r_rpm = v_lin;
  input_l_rpm = v_lin;
	

  
// 2. READ SENSOR (OUTPUT)

  r_rpm = 60*(r_ticks/ticks_rev_r) / dt;
  l_rpm = 60*(l_ticks/ticks_rev_l) / dt;
	r_ticks = 0; //reset tick counts
	l_ticks = 0;


  
//  3. COMPUTE ERROR SIGNAL (E = INPUT - OUTPUT)

	e_r = (input_r_rpm - r_rpm);
	e_l = (input_l_rpm - l_rpm);
	
	
	
//  4. COMPUTE CONTROL SIGNAL (U)
//  
//  todo: determine the controller, write data to files. 
//  current controller = proportional control (kp);
//  If error is negative, reverse motor direction
//  using set_x_mot() 1 = forward, 0 = back 
  
  if(e_r < 0 ) set_r_mot(0); 
  else set_r_mot(1);

  if(e_l < 0) set_l_mot(0); 
  else set_l_mot(1);

	u_r = kp*abs(e_r);
	u_l = kp*abs(e_l);

  // Saturate Inputs 
  
	if(u_r > 255) u_r = 255; 
  if(u_l > 255) u_l = 255; 
	
	
	
// 5. WRITE ACTUATORS WITH CONTROL SIGNAL
						 
  analogWrite(r_mot,u_r);
  analogWrite(l_mot,u_l);


  
// 6. MISCELLANEOUS 
	
	//Serial.print(u_r);
  //Serial.print("  ");
  Serial.println(r_rpm);


	cs_start = false; 
}




// INTERRUPT FUNCTIONS

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


// MOTOR DIRECTION FUNCTIONS

void set_r_mot(int dir){
  
  if(dir==1){
    
    // go forward
    digitalWrite(r_d1,LOW);
    digitalWrite(r_d2,HIGH);
    
  }else{
    
    // go backward
    digitalWrite(r_d1,HIGH);
    digitalWrite(r_d2,LOW);
    
  }
}

void set_l_mot(int dir){

  if(dir==1){
    digitalWrite(l_d1,HIGH);
    digitalWrite(l_d2,LOW);
  }else{
    digitalWrite(l_d1,LOW);
    digitalWrite(l_d2,HIGH);
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
  else if(dir==1){            // turn right 
    digitalWrite(r_d1,LOW);
    digitalWrite(r_d2,HIGH);
    digitalWrite(l_d1,LOW);
    digitalWrite(l_d2,HIGH);
  }
   else if(dir==2){
    digitalWrite(r_d1,HIGH); //turn left 
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


