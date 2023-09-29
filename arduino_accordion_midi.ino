#include <MIDI.h>
#include <Wire.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// Globals
const int halleffect_senstivity = 4;
const unsigned long REFRESH_INTERVAL = 15000; // ms
unsigned long lastRefreshTime = 0;
const int MUX_size = 16;
const int num_of_mux = 3; // 4;
const String MUXNotes[num_of_mux][MUX_size] = {
    {"F3", "F#3", "G3", "G#3", "D4", "B3", "A3", "A#3", "D4", "C#4", "E4", "D#4", "F4", "F#4", "G4", "A4"},
    {"G#4", "A#4", "C#5", "D5", "C5", "D5", "E5", "D#5", "F#5", "F5", "A#5", "A5", "G#5", "G5", "B5", "C6"},
    {"C#6", "E6", "F6", "F#6", "D#6", "D6", "G6", "G#6", "A6", "B2", "E2", "A1", "D2", "G1", "C2", "F2"}};
char *noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
int keyboardChannel = 1; // MIDI channel, from 1-16
int bassChannel = 2;     // MIDI channel, from 1-16
int chordChannel = 3;    // MIDI channel, from 1-16

int Expression_CC_Number = 11; // 11 = Expression
int prev_velocity = 127;
int curr_velocity = 127;
int velocity = 127;
boolean velocity_active = false;

// Hall effect sensors
int pin_Out_S0 = 8;
int pin_Out_S1 = 9;
int pin_Out_S2 = 10;
int pin_Out_S3 = 11;

int pin_In_Muxes[num_of_mux] = {A0, A1, A2}; // {A0}; // A3

int Muxes_States_init[num_of_mux][MUX_size] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int Muxes_States[num_of_mux][MUX_size] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

bool Muxes_States_OFF[num_of_mux][MUX_size] = {
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}};

bool Muxes_States_ON[num_of_mux][MUX_size] = {
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}};

bool Muxes_States_InZone[num_of_mux][MUX_size] = {
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}};

// Setup & Loop
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(115200);

  pinMode(pin_Out_S0, OUTPUT);
  pinMode(pin_Out_S1, OUTPUT);
  pinMode(pin_Out_S2, OUTPUT);
  pinMode(pin_Out_S3, OUTPUT);
  for (int i = 0; i < num_of_mux; i++) {
    pinMode(pin_In_Muxes[i], INPUT);
  }
  init_halleffects(10);
}

void loop() {
  updateMuxes();
  sendMidi();
}

// Functions
void sendMidi() {
  for (int i = 0; i < MUX_size; i++) {
    for (int j = 0; j < num_of_mux; j++) {
      int muxij_state = Muxes_States[j][i];
      if (Muxes_States_ON[j][i] == HIGH) {
        // note on
        noteOn(getChannel(j, i), getMidiNumber(MUXNotes[j][i]), velocity_bv());
      } else {
        // note off
        if (Muxes_States_OFF[j][i] == HIGH) {
          noteOff(getChannel(j, i), getMidiNumber(MUXNotes[j][i]), velocity_bv());
        }
      }
      if (muxij_state > 100) {
        // expression
        // controlChange(getChannel(j, i), Expression_CC_Number, midi_velocity);
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
  MIDI.sendNoteOn(pitch, velocity, channel);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOff(pitch, velocity, channel);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
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
    for (int i = 0; i < MUX_size; i++) {
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

void updateMuxes() {
  for (int i = 0; i < MUX_size; i++) {
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
        Muxes_States_InZone[j][i] = LOW; // OUT OF ZONE
      } else
        Muxes_States_OFF[j][i] = LOW;

      if (Muxes_States_InZone[j][i] == LOW && Muxes_States[j][i] - initial_value > halleffect_senstivity) {
        Muxes_States_ON[j][i] = HIGH;
        Muxes_States_InZone[j][i] = HIGH; // IN THE ZONE
      } else
        Muxes_States_ON[j][i] = LOW;
    }
  }
}

int velocity_bv() {
  return 100.0 + random(10);
}

double readPressure() {
  return 100.0 + random(10);
}
