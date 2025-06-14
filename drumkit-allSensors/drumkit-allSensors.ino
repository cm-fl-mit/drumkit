// Pin definitions for 4 piezo sensors
const int knockSensors[4] = {A0, A1, A2, A3}; // piezo sensors on analog pins 0-3
const char* drumNames[4] = {"Kick", "Snare", "HiHat", "Crash"}; // names for each drum

// Sensor settings
const int threshold = 100;  // threshold value for hits
const int knockWindow = 200; // time window in ms to capture the full knock

// Variables for each sensor (using arrays to keep it clean)
int sensorReadings[4] = {0, 0, 0, 0};      // current readings
int maxReadings[4] = {0, 0, 0, 0};         // maximum readings during knock
unsigned long knockStartTimes[4] = {0, 0, 0, 0}; // when each knock started
bool inKnock[4] = {false, false, false, false};  // flags for active knocks

void setup() {
  Serial.begin(9600);
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
      // End of knock window - report the maximum value we found
      Serial.print(drumNames[i]);
      Serial.print(" Hit! Strength: ");
      Serial.println(maxReadings[i]);
      
      // Reset for next knock on this sensor
      inKnock[i] = false;
      maxReadings[i] = 0;
    }
  }
  
  delay(1);  // minimal delay for stability
}
