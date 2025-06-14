#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// DFPlayer Mini setup
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX pins for DFPlayer
DFRobotDFPlayerMini myDFPlayer;

// Piezo sensor setup
const int knockSensor = A0;     // piezo sensor on analog pin 0
const int threshold = 100;      // threshold value for hits
const int knockWindow = 200;     // time window in ms to capture the full knock

int whichSound = 2; //change to vary drum sound played (1-4 on initial setup)
int drumVolume = 20; //change between 0 and 30

// Knock detection variables
int sensorReading = 0;
int maxReading = 0;
unsigned long knockStartTime = 0;
bool inKnock = false;

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
  
  // Check folder 1 file count
  int folder1Count = myDFPlayer.readFileCountsInFolder(1);
  Serial.print("Files in folder 1: ");
  Serial.println(folder1Count);
  
  // Set volume (0-30, adjust as needed)
  myDFPlayer.volume(drumVolume);
  delay(100);
  
  Serial.print("Current volume: ");
  Serial.println(myDFPlayer.readVolume());
  
  // Optional: Set equalizer (NORMAL, POP, ROCK, JAZZ, CLASSIC, BASS)
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  delay(100);
  
  // Test different play methods
  Serial.println("Testing audio - trying different play methods...");
  
  if (folder1Count > 0) {
    Serial.println("Trying playFolder(1, 1)...");
    myDFPlayer.playFolder(1, 1);
    delay(3000);
  } else {
    Serial.println("Folder 1 not found, trying direct play...");
    myDFPlayer.play(1);  // Play first file directly
    delay(3000);
  }
  
  // Check if still playing
  if (myDFPlayer.available()) {
    Serial.println("DFPlayer status available");
    int value = myDFPlayer.readType();
    int param = myDFPlayer.read();
    Serial.print("Type: ");
    Serial.print(value);
    Serial.print(", Parameter: ");
    Serial.println(param);
  }
  
  Serial.println("Electronic Drum Test Ready!");
  Serial.println("Hit the sensor to play sound 01.mp3");
  Serial.println();
}

void loop() {
  // Read the sensor
  sensorReading = analogRead(knockSensor);
  
  // Check if sensor reading is above threshold
  if (sensorReading >= threshold) {
    if (!inKnock) {
      // Start of a new knock
      inKnock = true;
      knockStartTime = millis();
      maxReading = sensorReading;
    } else {
      // Update max reading if this one is higher
      if (sensorReading > maxReading) {
        maxReading = sensorReading;
      }
    }
  }
  
  // Check if we need to end the knock detection window
  if (inKnock && (millis() - knockStartTime > knockWindow)) {
    // End of knock window - play sound and report
    Serial.print("Hit! Strength: ");
    Serial.println(maxReading);
    
    // Play sound - use direct play since folder structure had issues
    myDFPlayer.play(1); // Play first file directly
    
    // Reset for next knock
    inKnock = false;
    maxReading = 0;
  }
  
  // Small delay for stability
  delay(1);
}
