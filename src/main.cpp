#include <Arduino.h>
#include <TimerOne.h>
#include <FuzzyController.h>
#include <ArduinoSTL.h>
#include <map>
#include <string>


static const int ARR_DATA_SIZE=14;
float f_serial[ARR_DATA_SIZE]={0};
static const int f_serial_size=ARR_DATA_SIZE;

std::map<std::string,float> status_data {{"ref_temp",0.0f},{"current_temp",0.0f},{"error",0.0f},{"integral",0.0f}};

static const int ARR_RULE_SIZE=9;

float err=0;
float err_0=0;
float derr=0;
float out=0;

float ref_temp=0;

bool main_run = false;
bool new_data = false;
const byte NUM_CHARS = 32;
char recv_chars[NUM_CHARS];

//constant declaration
constexpr float v_ref = 1.1;
constexpr int inv_sensor_scale=100;//(1/scale)

//Input/Output variable mapping
int in_one=0;
int out_one=3;
int out_two=5;

//object instantiation
FuzzyController* fcontroller = new FuzzyController(in_one, out_one, out_two);


//function definition
void serial_data()
{
  if (main_run == true){
    char data[100];
    memset(data, 0, sizeof data);//--> to clear the data allocated in the memory pointed by the array
    char buffer[10];
    for (int i=0; i < f_serial_size;i++){
      dtostrf(f_serial[i],4,2,buffer);
      strcat(data,buffer);
      if (i < (f_serial_size-1))
        strcat(data,",");
    };
    Serial.println(data);
  }
}

//When we passs a class argument as reference, we access the methods inside with the '.' operator
void update_data(float arrData[], const FuzzyController& fc, const std::map<std::string,float>& status_data){

  fuzzyInErrorRegion* inErrR = fc.getInErrorRegion();
  fuzzyInDErrorRegion* inDErrR = fc.getInDErrorRegion();
  fuzzyOutRegion* outR = fc.getOutRegion();

  arrData[0]=(inErrR->negErr)->getPertinence();
  arrData[1]=(inErrR->zeroErr)->getPertinence();
  arrData[2]=(inErrR->posErr)->getPertinence();

  arrData[3]=(inDErrR->negDErr)->getPertinence();
  arrData[4]=(inDErrR->zeroDErr)->getPertinence();
  arrData[5]=(inDErrR->posDErr)->getPertinence();

  arrData[6]=(outR->negOut)->getPertinence();
  arrData[7]=(outR->zeroOut)->getPertinence();
  arrData[8]=(outR->posOut)->getPertinence();

  arrData[9]=status_data.find("integral")->second;
  arrData[10]=fc.getOutput();
  arrData[13]=status_data.find("error")->second;

  arrData[11]=status_data.find("ref_temp")->second;
  arrData[12]=status_data.find("current_temp")->second;

}

int get_analog_temp( int& analogInput )
{
  int adc_read = 0;

  // Average 50 readings for accurate reading
  for(int i = 0; i < 50; i++) {
     adc_read += analogRead(analogInput); 
     delay(20);
  }
  return adc_read*0.02;
}

float convert_analog_temp(int adc_value, float v_ref, int inv_sensor_scale)
{
  //to get the temperature 
  //--> temp = analog_ref * (vref/1024) * (1/lm_scale_factor) 
  //--> temp = [0-1023] * (5 /1024) * (1/.01)
  float analog_temp = adc_value*(v_ref/1024)*(inv_sensor_scale);
  return analog_temp;
}

void read_serial( bool& main_run, bool& new_data, char recv_chars[]) {

  static boolean recv_in_progress = false;
  static byte ndx = 0;
  char start_marker = '<';
  char end_marker = '>';
  char rc;
 
  while (Serial.available() > 0 && new_data == false) {
    rc = Serial.read();

    if (recv_in_progress == true) {
      if (rc != end_marker) {
        recv_chars[ndx] = rc;
        ndx++;
        if (ndx >= NUM_CHARS) {
          ndx = NUM_CHARS - 1;
        }
      }
      else {
        recv_chars[ndx] = '\0'; // terminate the string
        recv_in_progress = false;
        ndx = 0;
        new_data = true;
        main_run = true;
      }
    }

    else if (rc == start_marker) {
      recv_in_progress = true;
    }
  }
}


void setup() {

  fcontroller->begin();
  //(*fcontroller).begin();

  //change the reference of the analog input (1.1v) for more precise reading
  analogReference(INTERNAL);
  
  //initialize serial communication at 9600 bps:
  Serial.begin(9600);

  Timer1.initialize(500000);
  Timer1.attachInterrupt(serial_data);

}

void loop() {
 
  //Planta
  read_serial(main_run, new_data, recv_chars);

  if (main_run){

    if (new_data){
      ref_temp = atof(recv_chars);
      status_data["ref_temp"]=ref_temp;
      new_data = false;
    }

    int adc_temp = get_analog_temp(in_one);
    float current_temp = convert_analog_temp(adc_temp,v_ref, inv_sensor_scale);

    status_data["current_temp"] = current_temp;

    err=ref_temp-current_temp;
    derr=(err-err_0);
    err_0=err;

    status_data["error"]=err;

    fcontroller->loadInputs(err,derr);
    fcontroller->generateOutput();

    //integral
    out = out + fcontroller->getOutput();

    status_data["integral"]=out;

    if (out >= 255)
      out = 255;
    
    if (out < 0)
      out = 0;


    analogWrite(out_one,out);
    analogWrite(out_two,out);

    update_data(f_serial,*fcontroller,status_data);
    
    }
}