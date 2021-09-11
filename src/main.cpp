#include <Arduino.h>
#include <TimerOne.h>
#include <FuzzyController.h>
#include <ArduinoSTL.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <string>


//variable declaration
//float fSerial[]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
//int fSerialSize= sizeof(fSerial)/sizeof(fSerial[0]);

static const int ARR_DATA_SIZE=14;
float fSerial[ARR_DATA_SIZE]={0};
static const int fSerialSize=ARR_DATA_SIZE;

std::vector<float> s_data;
std::map<std::string,float> status_data {{"ref_temp",0.0f},{"current_temp",0.0f},{"error",0.0f},{"integral",0.0f}};

static const int ARR_RULE_SIZE=9;
bool rulesApplied[ARR_RULE_SIZE]={0};

float err=0;
float err_0=0;
float derr=0;
float out=0;

float ref_temp=0;
//float ref_temp=40;

bool main_run = false;
//bool main_run = true;
bool new_data = false;
const byte NUM_CHARS = 32;
char recv_chars[NUM_CHARS];

//constant declaration
int dutyCycleOne=0;
constexpr float vRef = 1.1;
constexpr int invSensorScale=100;//(1/scale)
//constexpr int ref_temp=30;

//Input/Output variable mapping
int inOne=0;
int outOne=3;
int outTwo=5;

//object instantiation
FuzzyController* fcontroller = new FuzzyController(inOne, outOne, outTwo);


//function definition
void serialData()
{
  if (main_run == true){
    char sData[100];
    memset(sData, 0, sizeof sData);//--> to clear the data allocated in the memory pointed by the array
    char buffer[10];
    for (int i=0; i < fSerialSize;i++){
      dtostrf(fSerial[i],4,2,buffer);
      strcat(sData,buffer);
      if (i < (fSerialSize-1))
        strcat(sData,",");
    };
    Serial.println(sData);
    //strcpy(sData,"");
  }
}


//s_data is a vector with the data to send
//THIS FUNCTION IS NOT WORKING 20210311, memory issues|
void serial_data(){

  std::ostringstream sdata;

  if (!s_data.empty()){
    std::copy(s_data.begin(),s_data.end()-1,std::ostream_iterator<int>(sdata,","));
    sdata << s_data.back();
  }
  
  std::cout <<sdata.str().c_str()<<std::endl;

}

//When we passs a class argument as reference, we access the methods inside with the '.' operator
/*
void updateData(float arrData[], const FuzzyController& fc, float& out, const float& refTemp, const float& currentTemp, const float& error)
{
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

  arrData[9]=out;
  arrData[10]=fc.getOutput();
  arrData[13]=error;

  arrData[11]=refTemp;
  arrData[12]=currentTemp;
}
*/

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

/*
void update_data(std::vector<float>& data, const FuzzyController& fc, const std::map<std::string,float>& status_data){

  //Serial.println("update_data::BEGIN");
  fuzzyInErrorRegion* inErrR = fc.getInErrorRegion();
  fuzzyInDErrorRegion* inDErrR = fc.getInDErrorRegion();
  fuzzyOutRegion* outR = fc.getOutRegion();

  if (data.size() == 14){
    data.at(0) = (inErrR->negErr)->getPertinence();
    data.at(1) = (inErrR->zeroErr)->getPertinence();
    data.at(2) = (inErrR->posErr)->getPertinence();

    data.at(3) = (inDErrR->negDErr)->getPertinence();
    data.at(4) = (inDErrR->zeroDErr)->getPertinence();
    data.at(5) = (inDErrR->posDErr)->getPertinence();

    data.at(6) = (outR->negOut)->getPertinence();
    data.at(7) = (outR->zeroOut)->getPertinence();
    data.at(8) = (outR->posOut)->getPertinence();

    data.at(9) = (status_data.find("integral")->second);
    data.at(10) = (fc.getOutput());
    data.at(13) = (status_data.find("error")->second);

    data.at(11) = (status_data.find("ref_temp")->second);
    data.at(12) = (status_data.find("current_temp")->second);
  }
  //Serial.println("update_data::END");
}
*/

int getAnalogTemp( int& analogInput )
{
  int adcRead = 0;

  // Average 50 readings for accurate reading
  for(int i = 0; i < 50; i++) {
     adcRead += analogRead(analogInput); 
     delay(20);
  }
  return adcRead*0.02;
}

float convertAnalogTemp(int adcValue, float vRef, int invSensorScale)
{
  //to get the temperature 
  //--> temp = analog_ref * (vref/1024) * (1/lm_scale_factor) 
  //--> temp = [0-1023] * (5 /1024) * (1/.01)
  float analogTemp = adcValue*(vRef/1024)*(invSensorScale);
  return analogTemp;
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

  //Timer1 initialization
  //Timer1.initialize(2000000);
  Timer1.initialize(500000);
  Timer1.attachInterrupt(serialData);
  //Timer1.attachInterrupt(serial_data);

  //s_data.reserve(11);

}

//float r = 186; //20°
//float r = 47; //5°
//float rand_temp = 5.0;

void loop() {

  /*
  read_serial(main_run, new_data, recv_chars);

  if (main_run){

    //Serial.println("loop::main_run::BEGIN");
    if (new_data){
      ref_temp = atof(recv_chars);
      status_data["ref_temp"]=ref_temp;
      new_data = false;
    }

    if (rand_temp >= ref_temp){
      r = r + random(-2,2);
    }
    else
      r = r + random(10);

    rand_temp = convertAnalogTemp(r,vRef, invSensorScale);

    //status_data.insert(std::pair<std::string,float>("current_temp",rand_temp));-->NOK for update
    status_data["current_temp"]=rand_temp;


    err=ref_temp-rand_temp;
    derr=(err-err_0);
    err_0=err;


    status_data["error"]=rand_temp;

    fcontroller->loadInputs(err,derr);
    fcontroller->generateOutput();

    //Serial.print("rules applied = ");
    //fcontroller->getRulesApplied(rulesApplied);

    //for(int i = 0; i < ARR_RULE_SIZE; i++)
    //  Serial.print(rulesApplied[i]);

    //Serial.println();

    //integral
    out = out + fcontroller->getOutput();

    
    std::map<std::string,float>::iterator it = status_data.find("integral");
    if (it != status_data.end()){
      Serial.println("found key");
      it->second=out;
      Serial.println(out);
      Serial.println(it->second);
    }
    
      
    //status_data.find("integral")->second=out;

    status_data["integral"]=out;


    if (out >= 255)
      out = 255;
    
    if (out < 0)
      out = 0;

    analogWrite(outOne,out);
    analogWrite(outTwo,out);

    //updateData(fSerial,*fcontroller,out,ref_temp,rand_temp,err);
    //update_data(s_data,*fcontroller,status_data);

    update_data(fSerial,*fcontroller,status_data);
  */
 
  //Planta
  read_serial(main_run, new_data, recv_chars);

  if (main_run){

    if (new_data){
      ref_temp = atof(recv_chars);
      status_data["ref_temp"]=ref_temp;
      new_data = false;
    }

    int adcTemp = getAnalogTemp(inOne);
    float current_temp = convertAnalogTemp(adcTemp,vRef, invSensorScale);
    status_data["current_temp"] = current_temp;

    err=ref_temp-current_temp;
    derr=(err-err_0);
    err_0=err;

    status_data["error"]=err;

    fcontroller->loadInputs(err,derr);
    fcontroller->generateOutput();

    //Serial.print("rules applied = ");
    //fcontroller->getRulesApplied(rulesApplied);

    //for(int i = 0; i < ARR_RULE_SIZE; i++)
    //  Serial.print(rulesApplied[i]);

    //Serial.println();

    //integral
    out = out + fcontroller->getOutput();

    status_data["integral"]=out;

    if (out >= 255)
      out = 255;
    
    if (out < 0)
      out = 0;


    analogWrite(outOne,out);
    analogWrite(outTwo,out);

    //updateData(fSerial,*fcontroller,out,ref_temp,analogTemp,err);

    update_data(fSerial,*fcontroller,status_data);

    
    //delay(500);
    }
}