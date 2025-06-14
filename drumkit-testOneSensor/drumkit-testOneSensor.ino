//Pins and threshold value
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 100;  // threshold value for hits

int sensorReading = 0;      // variable to store the value read from the sensor pin
int maxReading = 0;         // variable to store the maximum reading during a knock

//Debouncing for single hit detection
unsigned long knockStartTime = 0;  // when the current hit started
bool inKnock = false;       // flag to track if we're currently processing a hit
const int knockWindow = 200; // time window in ms to capture the full hit

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read the sensor and store it in the variable sensorReading:
  sensorReading = analogRead(knockSensor);
  
  // if the sensor reading is greater than the threshold:
  if (sensorReading >= threshold) {
    if (!inKnock) {
      // Start of a new knock
      inKnock = true;
      knockStartTime = millis();
      maxReading = sensorReading;
    } else {
      // We're already in a knock, update max reading if this one is higher
      if (sensorReading > maxReading) {
        maxReading = sensorReading;
      }
    }
  }
  
  // Check if we need to end the current knock detection window
  if (inKnock && (millis() - knockStartTime > knockWindow)) {
    // End of knock window - report the maximum value we found
    Serial.print("Hit! Strength: ");
    Serial.println(maxReading);
    
    // Reset for next knock
    inKnock = false;
    maxReading = 0;
  }
  
  delay(1);  // minimal delay for stability
}
