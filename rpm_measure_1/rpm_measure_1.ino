// AUTONOMOUS TOOLCART CAPSTONE PROJECT

// 10/22/2017

// Objective:
//							Accurately measure the angular velocity
//							of the two motors. Measure error between
//							calculated rpm and rpm measured with 
//							laser tachometer. 
//
// 							Approach 1: 
//							Count the number of ticks within a certain 
//							period of time. 



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
double r_rpm; 
double l_rpm; 
volatile int r_ticks = 0;
volatile int l_ticks = 0;


void setup() {
  
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


  Serial.begin(9600);
	Serial.setTimeout(1);
	
	
	
 }

 
void loop() {
	
	
  while(!cs_start){waste++;} 
	
//							1. Take inputs from serial monitor. 


	if(Serial.available()>0){
		v_lin =  Serial.parseInt();
	}	
	
	if((v_lin<0) || (v_lin>200)){
		analogWrite(r_mot,200);
		analogWrite(l_mot,200);
	}
	analogWrite(r_mot,v_lin);
	analogWrite(l_mot,v_lin);

	
	
	
  r_rpm = 60*(r_ticks/ticks_rev_r) / dt;
  l_rpm = 60*(l_ticks/ticks_rev_l) / dt;
	r_ticks = 0; //reset tick counts
	l_ticks = 0;
	
	Serial.print(l_rpm);
	Serial.print(" ");
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


