#include <MIDI.h>
//Barometer BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//Globals
const int MUX_size = 16;
const int num_of_mux = 1;//4;
const String MUXNotes[num_of_mux][MUX_size] = {
  {"F3","F#3","G3","G#3","A3","A#3","B3","C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4"},
//  {"A4","A#4","B4","C5","C#5","D5","D5#","E5","F5","F5#","G5","G5#","A5","A#5","B5","C6"},
//  {"C#6","D6","D6#","E6","F6","F6#","G6","G6#", "F#2","B2","E2","A1","D2","G1","C2","F2"},
//  {"A#1","D#2","G#1","C#2", "F#2","B2","E2","A1","D2","G1","C2","F2","A#1","D#2","G#1","C#2"}
};

int keyboardChannel = 1;   //MIDI channel, from 1-16
int bassChannel = 2;   //MIDI channel, from 1-16
int chordChannel = 3;   //MIDI channel, from 1-16

int Expression_CC_Number = 11;//11 = Expression
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

//pressure variables
double Calib_Pressure, Pressure;
int Pressure_Delta;

//timing variables
unsigned long initial_us = 0; 
unsigned long final_us = 0;

int prev_velocity = 127;
int curr_velocity = 127;
int velocity = 127;
boolean velocity_active = false;

// Hall effect sensors
int pin_Out_S0 = 8;
int pin_Out_S1 = 9; 
int pin_Out_S2 = 10; 
int pin_Out_S3 = 11; 

//int pin_In_Mux1 = A0; //Keyboard 1-16 (16 pins)
//int pin_In_Mux2 = A1; //Keyboard 17-32 (16-pins)
//int pin_In_Mux3 = A2; //Keyboard 33-40 (8 pins) + last 8 pins for Bass 1-8 (we will be remained with 17 sensors in the bass side). 
////THE LAST keyboard sensor (the highest A) has to be digital hall effect because no anloug left. I will complete it later.
//int pin_In_Mux4 = A3; //Bass 9-24 (16 pins)

int pin_In_Muxes[4] = {A0}; //{A0, A1, A2, A3};

int Muxes_States[num_of_mux][MUX_size] = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int Muxes_States_OFF[num_of_mux][MUX_size] = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int Muxes_States_ON[num_of_mux][MUX_size] = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


//####################################################################
//
//
//                            Setup & Loop
//
//####################################################################

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(115200);
  
  pinMode(pin_Out_S0, OUTPUT);
  pinMode(pin_Out_S1, OUTPUT);
  pinMode(pin_Out_S2, OUTPUT);
  pinMode(pin_Out_S3, OUTPUT);
  pinMode(pin_In_Muxes[0], INPUT);
  //Serial.begin(9600);

  init_BMP();
}

void loop() {
  updateMuxes();
  sendMidi();
//  for(int i = 0; i < num_of_mux; i ++) {
//    for(int j = 0; j < MUX_size; j++){
//      int muxij_state = Muxes_States[i][j];
//      if(j == MUX_size - 1){
//        Serial.println(muxij_state);
//      }
//      else{
//        Serial.print(muxij_state);
//        Serial.print(",");
//      }
//      
//    }
//  }
      
  //tests - to remove later....
  //Serial.print("Pressure = ");
  //Serial.print(bme.readPressure() / 100.0F);
  //Serial.println("hPa");
  //Serial.print("Velocity = ");
  //int midi_velocity = velocity_bv();
  //Serial.println(midi_velocity);
  
}


//####################################################################
//
//
//                            FUNCTIONS
//
//####################################################################
void sendMidi(){
  int midi_velocity = velocity_bv();
  for(int i = 0; i < MUX_size; i ++) {
    for(int j = 0; j < num_of_mux; j++){
      int muxij_state = Muxes_States[j][i];
      //Serial.println(muxij_state);
      if(Muxes_States_ON[j][i] == 1){
        int cur_channel = getChannel(j,i);
        int cur_pitch = getMidiNumber(MUXNotes[j][i]);

        //note on
        noteOn(cur_channel, cur_pitch, midi_velocity);
        //MIDI.sendNoteOn(60, 80, 2);
      }
      else{
       //note off
        if(Muxes_States_OFF[j][i] == 1){
          int cur_channel = getChannel(j,i);
          int cur_pitch = getMidiNumber(MUXNotes[j][i]);
          noteOff(getChannel(j,i), cur_pitch, midi_velocity);
        }
      }
      if(muxij_state > 100){
        //expression 
        int cur_channel = getChannel(j,i);
        controlChange(cur_channel, Expression_CC_Number, midi_velocity);   
      }
    }
  }
   
}




int getChannel(int mux_num, int pin_num){
  if(mux_num <= 2)
    return keyboardChannel;
  else{
    if(mux_num == 3 && pin_num <= 7)
      return keyboardChannel;
    else{
      if(mux_num == 3)
        return bassChannel;
      else{
        if(mux_num == 4 && pin_num <= 3)
          return bassChannel;
        else
          return chordChannel;
      }
    }
  }
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {  
  //midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  //MidiUSB.sendMIDI(noteOn);
  MIDI.sendNoteOn(pitch, velocity, channel);
  
}

void noteOff(byte channel, byte pitch, byte velocity) {
  //midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  //MidiUSB.sendMIDI(noteOff);

  MIDI.sendNoteOff(pitch, velocity, channel);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
//  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
//  MidiUSB.sendMIDI(event);
  MIDI.sendControlChange(control, value, channel);

}

int getMidiNumber(String noteName){
//  Serial.println("#########################");
//  Serial.println(noteName);  
  char * noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  int octave = (noteName.substring(noteName.length()-1)).toInt();
//  Serial.print("Octave: ");
//  Serial.println(octave);
  int noteInOctave = -1;
  for (int i=0;i<sizeof(noteNames);i++){
    if(noteName.substring(0,noteName.length()-1) == noteNames[i]){
      noteInOctave = i;
      break; 
    }
  }
//  Serial.print("Not in Octave: ");
//  Serial.println(noteInOctave);
//  Serial.println(noteInOctave + (octave + 1)*12);
//  Serial.println("#########################");
  return noteInOctave + (octave + 1)*12;
}




void init_BMP() {
  // Initialize and calibrate the sensor
  if (bme.begin(0x76))//()
  {
    //get an average reading
    for (int i=0; i<32; i++){
      Calib_Pressure += bme.readPressure();
      delayMicroseconds(500);
    }
    Calib_Pressure = Calib_Pressure/32;
  }
  else //something went wrong - this is usually a connection problem
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  //reset_flags();
}



void updateMuxes () {
  for(int i = 0; i < MUX_size; i ++) {
    digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
    digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
    digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
    digitalWrite(pin_Out_S3, HIGH && (i & B00001000));
    for(int j = 0; j < num_of_mux; j++){
      int previous_value = Muxes_States[j][i];
      Muxes_States[j][i] = analogRead(pin_In_Muxes[j]);
      if(previous_value - Muxes_States[j][i] > 100){ 
        Muxes_States_OFF[j][i] = 1;
      }
      else
        Muxes_States_OFF[j][i] = 0;

      if(Muxes_States[j][i] - previous_value > 100){
        Muxes_States_ON[j][i] = 1;
      }
      else
        Muxes_States_ON[j][i] = 0;
    }
  }
}

int velocity_bv(){
  int pressure_low_limit = 10;//default=10
  int pressure_high_limit = 120;//default=120
  
  Pressure = readPressure(); //bme.readPressure();
  Pressure_Delta = Pressure - Calib_Pressure;
  Pressure_Delta = abs(Pressure_Delta);

  if (Pressure_Delta <= pressure_low_limit){
    velocity = 0;
  }
  else { 
    velocity = map(Pressure_Delta, pressure_low_limit, pressure_high_limit, 0, 127);
    if (velocity > 127){
      velocity = 127;
    } 
  }
  return velocity + 90;
}

double readPressure(){
  return bme.readPressure();
  //return 100.0 + random(100);
}
