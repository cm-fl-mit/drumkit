#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// DFPlayer Mini setup
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX pins for DFPlayer
DFRobotDFPlayerMini myDFPlayer;

// Pin definitions for 4 piezo sensors
const int knockSensors[4] = {A0, A1, A2, A3}; // piezo sensors on analog pins 0-3
const char* drumNames[4] = {"Kick", "Snare", "HiHat", "Crash"}; // names for each drum
const int drumSounds[4] = {1, 2, 3, 4}; // which sound file to play for each drum

// Sound settings
const bool soundMode = false;   // true = hit-strength sensitive volume, false = constant volume
const int constantVolume = 20; // volume when soundMode is false (0-30)

// Sensor settings
const int threshold = 100;  // threshold value for hits
const int knockWindow = 200; // time window in ms to capture the full knock

// Variables for each sensor
int sensorReadings[4] = {0, 0, 0, 0};      // current readings
int maxReadings[4] = {0, 0, 0, 0};         // maximum readings during knock
unsigned long knockStartTimes[4] = {0, 0, 0, 0}; // when each knock started
bool inKnock[4] = {false, false, false, false};  // flags for active knocks

void setup() {
  // Initialize serial communications
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  
  Serial.println("Initializing DFPlayer...");
  
  // Initialize DFPlayer
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("Unable to begin DFPlayer:");
    Serial.println("1. Please recheck the connection!");
    Serial.println("2. Please insert the SD card!");
    while(true); // Stop here if DFPlayer fails
  }
  
  Serial.println("DFPlayer Mini online.");
  
  // Give DFPlayer time to initialize properly
  delay(1000);
  
  // Check SD card status
  Serial.println("Checking SD card...");
  int fileCount = myDFPlayer.readFileCounts();
  Serial.print("Total files found: ");
  Serial.println(fileCount);
  
  // Set volume (0-30, adjust as needed)
  myDFPlayer.volume(25);
  delay(100);
  
  Serial.print("Current volume: ");
  Serial.println(myDFPlayer.readVolume());
  
  // Optional: Set equalizer (NORMAL, POP, ROCK, JAZZ, CLASSIC, BASS)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  delay(100);
  
  // Test each drum sound
  Serial.println("Testing drum sounds...");
  for (int i = 0; i < 4; i++) {
    Serial.print("Testing ");
    Serial.print(drumNames[i]);
    Serial.println("...");
    myDFPlayer.play(drumSounds[i]);
    delay(1500); // Wait between test sounds
  }
  
  Serial.println("Electronic Drum Kit Ready!");
  Serial.println("Sensors: A0=Kick, A1=Snare, A2=HiHat, A3=Crash");
  Serial.println();
}

void loop() {
  // Check all 4 sensors
  for (int i = 0; i < 4; i++) {
    // Read the current sensor
    sensorReadings[i] = analogRead(knockSensors[i]);
    
    // Check if this sensor reading is above threshold
    if (sensorReadings[i] >= threshold) {
      if (!inKnock[i]) {
        // Start of a new knock on this sensor
        inKnock[i] = true;
        knockStartTimes[i] = millis();
        maxReadings[i] = sensorReadings[i];
      } else {
        // We're already in a knock, update max reading if this one is higher
        if (sensorReadings[i] > maxReadings[i]) {
          maxReadings[i] = sensorReadings[i];
        }
      }
    }
    
    // Check if we need to end the current knock detection window for this sensor
    if (inKnock[i] && (millis() - knockStartTimes[i] > knockWindow)) {
      // End of knock window - calculate volume and play sound
      Serial.print(drumNames[i]);
      Serial.print(" Hit! Strength: ");
      Serial.println(maxReadings[i]);
      
      // Set volume based on sound mode
      if (soundMode) {
        // Map hit strength to volume (threshold-1023 maps to 5-25)
        int hitVolume = map(maxReadings[i], threshold, 1023, 5, 25);
        hitVolume = constrain(hitVolume, 5, 25); // Ensure volume stays in bounds
        myDFPlayer.volume(hitVolume);
        Serial.print("  Volume: ");
        Serial.println(hitVolume);
      } else {
        // Use constant volume
        myDFPlayer.volume(constantVolume);
      }
      
      // Small delay to ensure volume command is processed
      delay(10);
      
      // Play the corresponding drum sound
      myDFPlayer.play(drumSounds[i]);
      
      // Reset for next knock on this sensor
      inKnock[i] = false;
      maxReadings[i] = 0;
    }
  }
  
  delay(1);  // minimal delay for stability
}
