// AUTONOMOUS TOOLCART CAPSTONE PROJECT

// October 2017

// Objective:
//							Accurately measure the angular velocity
//							of the two motors. Measure error between
//							calculated rpm and rpm measured with 
//							laser tachometer. 
//
// 							Approach : 
//							Count the number of ticks within a certain 
//							period of time dt. 



#define BIT(a) (1<<(a))
const double max_volt = 0.5;


// Right motor

int r_mot = 10;
int r_d1 = 8; 
int r_d2 = 6;


// Left motor

int l_mot = 9; 
int l_d1 = 4; 
int l_d2 = 5; 


// Encoder info

double ticks_rev_r = 235;
double ticks_rev_l = 235;
int l_enc = 2; // pin INT0
int r_enc = 3; // pin INT1



// Control System Variables

// cs_start is triggered in the timer compare function 
// (controls timing of control system). 

// Timing: 


volatile bool cs_start = false; 
volatile int waste = 0;
int ms_per_cmp = 16; //
int n_cmp = 6; //
double dt = ms_per_cmp*n_cmp*.001;
int cmp_count = 0;


int PWM;
double ratio;
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
	// With prescalar at 1024 and output compare interrupt at 250 ticks, 
	// compare interrupt vector every 16ms. 
	
  TCCR2A = 0;
  TCCR2B = 0;  
	TCCR2B = BIT(CS22) | BIT(CS21) | BIT(CS20); 
	TIMSK2 = BIT(OCIE2A); 
	OCR2A = 250; 
  
	
	// Set external interrupt registers
	// enable interrupts on puns 2 and 3
  
  EIMSK = 0;
  EIMSK = BIT(INT0) | BIT(INT1); 
  EICRA = BIT(ISC11) | BIT(ISC10) | BIT(ISC01)| BIT(ISC00); 

  PWM = 0;
  Serial.begin(9600);
	Serial.setTimeout(1);

 }

 
void loop() {
	
	
  while(!cs_start){waste++;} 

  
  // 1. Take inputs from serial monitor. 

	if(Serial.available()>0){
		PWM =  Serial.parseInt();
	}	

  
  // 2. Write Motors

  if(PWM < 0) PWM = 0;
  if(PWM >255) PWM = 255;  

  analogWrite(r_mot,PWM);
  analogWrite(l_mot,PWM);

  // 3. Calculate RPM
 
  
  r_rpm = 60*(r_ticks/ticks_rev_r) / dt;
  l_rpm = 60*(l_ticks/ticks_rev_l) / dt;
	r_ticks = 0; 
	l_ticks = 0;

  // 4. Print
	
	Serial.print("Left wheel RPM: ");
	Serial.print(l_rpm);
  Serial.print(".    Left wheel RPM: ");
  Serial.println(r_rpm);


  // 5. Reset 
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
    digitalWrite(r_d1,HIGH); // go backwards
    digitalWrite(r_d2,LOW);
    digitalWrite(l_d1,LOW);
    digitalWrite(l_d2,HIGH);
   }
}


