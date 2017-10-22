#define BIT(a) (1<<(a))
const double max_volt = 0.5;




// Approach 1: 
// Count the number of ticks in a set amount of time. 
// Dt is determined by the clock setting (CSxx). 
//


int l_enc = 2; // pin INT0
int r_enc = 3; // pin INT1

int r_mot = 10;
int r_d1 = 8; 
int r_d2 = 6;
int l_mot = 9; 
int l_d1 = 4; 
int l_d2 = 5; 


// Control system variables

volatile bool cs_start = false; 
int cs_iter = 5; // count the number of ticks over a number of loops (# = cs_iter) to increase resolution of rpm. 
double cs_dt = 0.016384; 
volatile double dt = cs_dt*cs_iter; // cs_dt*cs_iter gives the period over which the ticks were counteds
volatile int waste = 0;
volatile int iter_count = 0;
int u_r,e_r;
double kp = 1;


volatile int r_ticks = 0;
volatile int l_ticks = 0;
double ticks_rev_r = 235;
double ticks_rev_l = 235;
double r_rpm; 
double l_rpm; 


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
  TIMSK2 = BIT(TOIE2); // enable overflow interrupt
  TCCR2B = BIT(CS22) | BIT(CS21)|BIT(CS20); // 61.04hz overflow.  

  EIMSK = 0;
  EIMSK = BIT(INT0) | BIT(INT1); //enable interrupts on puns 2 and 3
  EICRA = BIT(ISC11) | BIT(ISC10) | BIT(ISC01)| BIT(ISC00);  
  Serial.begin(9600);
 }

void loop() {
	
  while(!cs_start){waste++;} 
  
	//every 16ms, cs_start is set to true in the overflow function and the loop begins.    
  
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
  analogWrite(l_mot,0);

  
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


