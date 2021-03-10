// Crawler Controller

#include <Servo.h> 
#define DEBUG 1

#define FLASH_PERIOD 100
#define BRAKE_FLASH_PERIOD 25

// Assign Pins

#define CHANNEL_1_PIN 2
#define CHANNEL_2_PIN 3
#define CHANNEL_3_PIN 4
//#define CHANNEL_4_PIN 5

#define STEERING_SERVO_PIN 6
#define THROTTLE_ESC_PIN 7

#define MAIN_LIGHTS_PIN 8
#define INDICATOR_LEFT_PIN 9
#define INDICATOR_RIGHT_PIN 10
#define BRAKE_LIGHTS_PIN 11
#define FRONT_SPOTS_PIN 12

#define BUZZER_PIN 14
//#define AUX_LIGHTS 13

// Switch around if your servo is reversed and reverse <> signs ( will add configuration facility soon )
#define LEFT_INDICATOR_MS_ON 1400
#define RIGHT_INDICATOR_MS_ON 1600

// change to 1400 if throttle is reversed and reverse > signs ( will add configuration facility soon )
#define BRAKE_ON 1600
bool brakes_on=false;
unsigned long brake_flash;
bool brake_light_on=false;

#define LIGHTS_OFF_MS 900 // lights off < 1250
#define LIGHTS_ON_MS 1250 // lights on 1250 - 1600
#define LIGHTS_FULL 1600 // full beam 1600-2000



enum light_status { main_lights_off, main_lights_on, fullbeam };

light_status crawler_lights  = main_lights_off;

Servo steering_servo;
Servo throttle_esc;  

int channel_1;
int channel_2;
int channel_3;

unsigned long next_flash_left_indicator=0;
unsigned long next_flash_right_indicator=0;
bool left_indicator_off=true;
bool right_indicator_off=true;
bool indicating_lights_on=false;

void debugit( String textit)
{
  #if DEBUG
      Serial.println(textit);
  #endif
}
void debugit( int intit)
{
  #if DEBUG
      Serial.println(intit);
  #endif
}

void  lights_status()
{

  // only change the light settings when there has been a change.


  switch (crawler_lights)
  {
    case main_lights_off:
    {
      debugit(" Status : main_lights_off");
      break;
    }
    case main_lights_on:
    {
      debugit(" Status : main_lights_on");
      break;
    }
    case fullbeam:
    {
      debugit(" Status : fullbeam");
      break;
    }
  }


  if (channel_3<=LIGHTS_ON_MS && crawler_lights!=main_lights_off)
  {
    debugit("Lights off");
    crawler_lights=main_lights_off;
    digitalWrite(MAIN_LIGHTS_PIN, HIGH);
    digitalWrite(FRONT_SPOTS_PIN, HIGH);
    digitalWrite(BRAKE_LIGHTS_PIN, HIGH);
    
  }

  if (channel_3>=LIGHTS_ON_MS && channel_3<=LIGHTS_FULL && crawler_lights!=main_lights_on)
  {
    debugit("Lights on");
    crawler_lights=main_lights_on;
    digitalWrite(MAIN_LIGHTS_PIN, LOW);
    digitalWrite(FRONT_SPOTS_PIN, HIGH);
    digitalWrite(BRAKE_LIGHTS_PIN, LOW);
    brake_light_on=true;
    
  }

  if (channel_3>LIGHTS_FULL && crawler_lights!=fullbeam)
  {
    debugit("Lights on Full Beam");
    crawler_lights=fullbeam;
    digitalWrite(MAIN_LIGHTS_PIN, LOW);
    digitalWrite(FRONT_SPOTS_PIN, LOW);
    digitalWrite(BRAKE_LIGHTS_PIN, LOW);
    brake_light_on=true;
  }

}

void  indicator_status()
{ 
   
   if (indicating_lights_on)
   {
     debugit("Status : Indicators On");
   }
   else
   {
     debugit("Status : Indicators Off");
   }

  if ( channel_1 < LEFT_INDICATOR_MS_ON )
  {
    indicating_lights_on=true;
    debugit("Indicating Left");
    if (next_flash_left_indicator<millis() )
    {
     
     if ( left_indicator_off)
     {
        debugit("Indicating Left Bulb On");
        digitalWrite(INDICATOR_LEFT_PIN, LOW);   // turn the LED on (HIGH is the voltage level)
        left_indicator_off=false;
     }
      else
      {
        debugit("Indicating Left Bulb Off");
        digitalWrite(INDICATOR_LEFT_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        left_indicator_off=true;
      }
      next_flash_left_indicator=millis()+FLASH_PERIOD;
    }
  }
  
  else
  {
    if ( channel_1 > RIGHT_INDICATOR_MS_ON)
    {
      indicating_lights_on=true;
      debugit("Indicating Right");
      if (next_flash_right_indicator<millis() )
      {
      
        if ( right_indicator_off)
        {
            debugit("Indicating Right Bulb On");
            digitalWrite(INDICATOR_RIGHT_PIN, LOW);   // turn the LED on (HIGH is the voltage level)
            right_indicator_off=false;
        }
        else
        {
            debugit("Indicating Right Bulb Off");
            digitalWrite(INDICATOR_RIGHT_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            right_indicator_off=true;
        }
        next_flash_right_indicator=millis()+FLASH_PERIOD;
      }
    }
    else
    {
      if (indicating_lights_on)
      {
        digitalWrite(INDICATOR_LEFT_PIN, HIGH);
        digitalWrite(INDICATOR_RIGHT_PIN, HIGH);
        debugit("Indicators off");
        indicating_lights_on=false;
      }
    }
  }
}

void  brake_light_status()
{ 

   if (brakes_on)
   {
     debugit("Status : Brakes On");
   }
   else
   {
     debugit("Status : Brakes Off");
   }

  if (channel_2>BRAKE_ON )
  {
    
    if (crawler_lights==main_lights_on || crawler_lights==fullbeam )
    {
      brakes_on=true;
      if (brake_flash<millis() )
      {
      
        if ( brake_light_on)
        {
            debugit("Brakes flash off");
            digitalWrite(BRAKE_LIGHTS_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            brake_light_on=false;
            
        }
        else
        {
            debugit("Brakes flash on");
            digitalWrite(BRAKE_LIGHTS_PIN, LOW);   // turn the LED on (HIGH is the voltage level)
            brake_light_on=true;
        }
        brake_flash=millis()+BRAKE_FLASH_PERIOD;
      }
    }
    else
    {
      if (!brakes_on)
      {
        debugit("Switch Brakes On");
        digitalWrite(BRAKE_LIGHTS_PIN, LOW);
        brakes_on=true;
        brake_light_on=true;
      }
   /*   else
      {
        if (brakes_on)
        {
          debugit("Switch Brakes Off");
          digitalWrite(BRAKE_LIGHTS_PIN,HIGH);
          brakes_on=false;
        }
      } */
    }
  }
  else
  {
    brakes_on=false;
    if (crawler_lights==main_lights_on || crawler_lights==fullbeam)
    {
      if (!brake_light_on )
      {
            debugit("Switch Brakes On - for main lights");
            digitalWrite(BRAKE_LIGHTS_PIN,LOW);
            brake_light_on=true;
       }
     }
     else    
     {
       if (brake_light_on )
       {
          debugit("Switch Brakes Off - no braking");
          digitalWrite(BRAKE_LIGHTS_PIN,HIGH);
          brake_light_on=false;
       }
     }   
  }
}

void setup()
{
  Serial.begin(115200); 
  Serial.print("Starting Crawler Controller:"); 

  steering_servo.attach(STEERING_SERVO_PIN);
  throttle_esc.attach(THROTTLE_ESC_PIN);
  
  pinMode(MAIN_LIGHTS_PIN, OUTPUT);
  digitalWrite(MAIN_LIGHTS_PIN, HIGH); 
  pinMode(INDICATOR_LEFT_PIN, OUTPUT);
  digitalWrite(INDICATOR_LEFT_PIN, HIGH);
  pinMode(INDICATOR_RIGHT_PIN, OUTPUT);
  digitalWrite(INDICATOR_RIGHT_PIN, HIGH);
  pinMode(BRAKE_LIGHTS_PIN, OUTPUT);
  digitalWrite(BRAKE_LIGHTS_PIN, HIGH);
  pinMode(FRONT_SPOTS_PIN, OUTPUT);
  digitalWrite(FRONT_SPOTS_PIN, HIGH);      

  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, 20, 300);
  delay(400);
  tone(BUZZER_PIN, 50, 300);
  delay(400);
  tone(BUZZER_PIN, 110, 300);
  //noTone(BUZZER_PIN);

 // tone(BUZZER_PIN, 200, 1000);
}

void loop()
{
  
  // for some reason if adjacent pins are read  pulseIn fails.
  noInterrupts();
  channel_1 = pulseIn(CHANNEL_1_PIN, HIGH, 250000); // Read the pulse width of
  channel_3 = pulseIn(CHANNEL_3_PIN, HIGH, 250000);
  channel_2 = pulseIn(CHANNEL_2_PIN, HIGH, 250000); // each channel
  interrupts();

  steering_servo.writeMicroseconds(channel_1); 
  throttle_esc.writeMicroseconds(channel_2);  
 
  #if DEBUG
  Serial.println(millis());
  #endif

  debugit("Channel 1:");
  debugit(channel_1);
  debugit("Channel 2:");
  debugit(channel_2);
  debugit("Channel 3:");
  debugit(channel_3);
  
  lights_status();
  indicator_status();
  brake_light_status();

 // delay(2);
  yield();

}
