#include <MIDI.h>
//Barometer BME280
#include <Wire.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>

MIDI_CREATE_DEFAULT_INSTANCE();

//Globals
const int halleffect_senstivity = 4;
const unsigned long REFRESH_INTERVAL = 15000; // ms
unsigned long lastRefreshTime = 0;
const int MUX_size = 16;
const int num_of_mux = 3;//4;
const String MUXNotes[num_of_mux][MUX_size] = {
  {"F3", "F#3", "G3", "G#3", "D4", "B3", "A3", "A#3", "D4", "C#4", "E4", "D#4", "F4", "F#4", "G4", "A4"},
      {"G#4","A#4","C#5","D5","C5","D5","E5","D#5","F#5","F5","A#5","A5","G#5","G5","B5","C6"},
    {"C#6","E6","F6","F#6","D#6","D6","G6","G#6", "A6","B2","E2","A1","D2","G1","C2","F2"},
 //   {"A#1","D#2","G#1","C#2", "F#2","B2","E2","A1","D2","G1","C2","F2","A#1","D#2","G#1","C#2"}
};
char * noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
int keyboardChannel = 1;   //MIDI channel, from 1-16
int bassChannel = 2;   //MIDI channel, from 1-16
int chordChannel = 3;   //MIDI channel, from 1-16

int Expression_CC_Number = 11;//11 = Expression
//#define SEALEVELPRESSURE_HPA (1013.25)
//Adafruit_BME280 bme;

//pressure variables
//double Calib_Pressure, Pressure;
//int Pressure_Delta;

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

int pin_In_Muxes[num_of_mux] = {A0, A1, A2}; //{A0}; // A3

int Muxes_States_init[num_of_mux][MUX_size] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};



int Muxes_States[num_of_mux][MUX_size] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

bool Muxes_States_OFF[num_of_mux][MUX_size] = {
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW},
  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW},
//  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW}
};

bool Muxes_States_ON[num_of_mux][MUX_size] = {
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW},
  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW},
//  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW}
};

bool Muxes_States_InZone[num_of_mux][MUX_size] = {
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW},
  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW},
//  {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW}
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
  for(int i =0;i<num_of_mux;i++){
    pinMode(pin_In_Muxes[i], INPUT);
  }
  //Serial.begin(9600);
  init_halleffects(10);
  //init_BMP();
}

void loop() {
  updateMuxes();
  sendMidi(); 
//  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
//  {
//    lastRefreshTime += REFRESH_INTERVAL;
//    init_halleffects(3);
//  }
//  //Serial.println("SENSOR VALUES: ");
//  for (int i = 0; i < num_of_mux; i ++) {
//    for (int j = 0; j < MUX_size; j++) {
//      int muxij_state = Muxes_States[i][j];
//      if (j == MUX_size - 1 && i == num_of_mux - 1) { 
//        Serial.println(muxij_state);
//      }
//      else {
//        Serial.print(muxij_state);
//        Serial.print(",");
//      }
//    }
//  }
//  Serial.println(analogRead(pin_In_Muxes[2]));
  
//    Serial.println("ON: ");
//    for(int i = 0; i < num_of_mux; i ++) {
//      for(int j = 0; j < MUX_size; j++){
//        int muxij_state = Muxes_States_ON[i][j];
//        if(j == MUX_size - 1){
//          Serial.println(muxij_state);
//        }
//        else{
//          Serial.print(muxij_state);
//          Serial.print(",");
//        }
//      }
//    }
  
//    Serial.println("OFF: ");
//    for(int i = 0; i < num_of_mux; i ++) {
//      for(int j = 0; j < MUX_size; j++){
//        int muxij_state = Muxes_States_OFF[i][j];
//        if(j == MUX_size - 1){
//          Serial.println(muxij_state);
//        }
//        else{
//          Serial.print(muxij_state);
//          Serial.print(",");
//        }
//      }
//    }
  //tests - to remove later....
  //Serial.print("Pressure = ");
  //Serial.print(bme.readPressure() / 100.0F);
  //Serial.println("hPa");
  //Serial.print("Velocity = ");
  //int midi_velocity = velocity_bv();
  //Serial.println(midi_velocity);
  //delay(1000);
}


//####################################################################
//
//
//                            FUNCTIONS
//
//####################################################################
void sendMidi() {
  for (int i = 0; i < MUX_size; i ++) {
    for (int j = 0; j < num_of_mux; j++) {
      int muxij_state = Muxes_States[j][i];
      //Serial.println(muxij_state);
      if (Muxes_States_ON[j][i] == HIGH) {
        //note on
        noteOn(getChannel(j, i), getMidiNumber(MUXNotes[j][i]), velocity_bv());
//        Serial.print("ON: ");
//        Serial.println(MUXNotes[j][i]);
      }
      else {
        //note off
        if (Muxes_States_OFF[j][i] == HIGH) {
          noteOff(getChannel(j, i), getMidiNumber(MUXNotes[j][i]), velocity_bv());
//          Serial.print("OFF: ");
//          Serial.println(MUXNotes[j][i]);
        }
      }
      if (muxij_state > 100) {
        //expression
        //controlChange(getChannel(j, i), Expression_CC_Number, midi_velocity);
      }
    }
  }
}




int getChannel(int mux_num, int pin_num) {
  if (mux_num <= 2)
    return keyboardChannel;
  else {
    if (mux_num == 3 && pin_num <= 7)
      return keyboardChannel;
    else {
      if (mux_num == 3)
        return bassChannel;
      else {
        if (mux_num == 4 && pin_num <= 3)
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

int getMidiNumber(String noteName) {
  int octave = (noteName.substring(noteName.length() - 1)).toInt();
  int noteInOctave = -1;
  for (int i = 0; i < sizeof(noteNames); i++) {
    if (noteName.substring(0, noteName.length() - 1) == noteNames[i]) {
      noteInOctave = i;
      break;
    }
  }
  return noteInOctave + (octave + 1) * 12;
}


void init_halleffects(int times) {
  for (int k = 0; k < times; k++) {
    for (int i = 0; i < MUX_size; i ++) {
      digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
      digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
      digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
      digitalWrite(pin_Out_S3, HIGH && (i & B00001000));
      for (int j = 0; j < num_of_mux; j++) {
        Muxes_States[j][i] += analogRead(pin_In_Muxes[j]);
        Muxes_States_init[j][i] += analogRead(pin_In_Muxes[j]);
        if (k == times - 1) {
          Muxes_States[j][i] = Muxes_States[j][i] / (k + 1);
          Muxes_States_init[j][i] = Muxes_States_init[j][i] / (k + 1);
        }
      }
    }
  }
}

void init_BMP() {
//  // Initialize and calibrate the sensor
//  if (bme.begin(0x76))//()
//  {
//    //get an average reading
//    for (int i = 0; i < 32; i++) {
//      Calib_Pressure += bme.readPressure();
//      delayMicroseconds(500);
//    }
//    Calib_Pressure = Calib_Pressure / 32;
//  }
//  else //something went wrong - this is usually a connection problem
//  {
//    Serial.println("Could not find a valid BME280 sensor, check wiring!");
//    while (1);
//  }
//  //reset_flags();
}

void updateMuxes () {
  for (int i = 0; i < MUX_size; i ++) {
    digitalWrite(pin_Out_S0, HIGH && (i & B00000001));
    digitalWrite(pin_Out_S1, HIGH && (i & B00000010));
    digitalWrite(pin_Out_S2, HIGH && (i & B00000100));
    digitalWrite(pin_Out_S3, HIGH && (i & B00001000));
    for (int j = 0; j < num_of_mux; j++) {
      int previous_value = Muxes_States[j][i];
      int initial_value = Muxes_States_init[j][i];
      Muxes_States[j][i] = analogRead(pin_In_Muxes[j]);

      if (Muxes_States_InZone[j][i] == 1 && Muxes_States[j][i] - initial_value <= halleffect_senstivity) {
        Muxes_States_OFF[j][i] = HIGH;
        Muxes_States_InZone[j][i] = LOW; //OUT OF ZONE
      }
      else
        Muxes_States_OFF[j][i] = LOW;

      if (Muxes_States_InZone[j][i] == LOW && Muxes_States[j][i] - initial_value > halleffect_senstivity) {
        Muxes_States_ON[j][i] = HIGH;
        Muxes_States_InZone[j][i] = HIGH; //IN THE ZONE
      }
      else
        Muxes_States_ON[j][i] = LOW;
    }
  }
}

int velocity_bv() {
//  int pressure_low_limit = 10;//default=10
//  int pressure_high_limit = 120;//default=120
//
//  Pressure = readPressure(); //bme.readPressure();
//  Pressure_Delta = Pressure - Calib_Pressure;
//  Pressure_Delta = abs(Pressure_Delta);
//
//  if (Pressure_Delta <= pressure_low_limit) {
//    velocity = 0;
//  }
//  else {
//    velocity = map(Pressure_Delta, pressure_low_limit, pressure_high_limit, 0, 127);
//    if (velocity > 127) {
//      velocity = 127;
//    }
//  }
//  return velocity + 90;
  return 100.0 + random(10);
}

double readPressure() {
  //return bme.readPressure();
  return 100.0 + random(10);
}