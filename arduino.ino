const int GSR = A0;
const int SAMPLES = 10;
int readings[SAMPLES];
int sensorValue = 0;
int gsr_average = 0;
bool isRunning = false;

void setup() {
   Serial.begin(9600);
   Serial.println("Press 's' to start/stop data collection");
}

void loop() {
    // Check for serial input
    if (Serial.available() > 0) {
        char input = Serial.read();
        if (input == 's' || input == 'S') {
            isRunning = !isRunning;
            Serial.println(isRunning ? "Starting data collection..." : "Stopping data collection...");
        }
    }

    // Only collect and send data if isRunning is true
    if (isRunning) {
        // Collect readings into array instead of immediate averaging
        for (int i = 0; i < SAMPLES; i++) {
            readings[i] = analogRead(GSR);
            delay(50);
        }

        // Sort the array (bubble sort since it's a small dataset)
        for (int i = 0; i < SAMPLES - 1; i++) {
            for (int j = 0; j < SAMPLES - i - 1; j++) {
                if (readings[j] > readings[j + 1]) {
                    int temp = readings[j];
                    readings[j] = readings[j + 1];
                    readings[j + 1] = temp;
                }
            }
        }

        // Calculate Q1, Q2 (median), and Q3
        int Q1 = readings[SAMPLES / 4];
        int Q2 = readings[SAMPLES / 2];  // median
        int Q3 = readings[3 * SAMPLES / 4];
        
        // Calculate IQR and bounds for outlier detection
        int IQR = Q3 - Q1;
        int lowerBound = Q1 - (1.5 * IQR);
        int upperBound = Q3 + (1.5 * IQR);

        // Count valid readings (non-outliers)
        int validCount = 0;
        long validSum = 0;
        
        for (int i = 0; i < SAMPLES; i++) {
            if (readings[i] >= lowerBound && readings[i] <= upperBound) {
                validSum += readings[i];
                validCount++;
            }
        }

        // Calculate final GSR average excluding outliers
        gsr_average = validCount > 0 ? validSum / validCount : Q2;
        
        int human_resistance = 0;
        if (516 - gsr_average != 0) {
            human_resistance = abs(((1024 + 2 * gsr_average) * 10000) / (516 - gsr_average));
        }

        // Output median and processed resistance value
        Serial.print(Q2);
        Serial.print(",");
        Serial.println(human_resistance);
    }

    delay(50);  // Delay for readability in plotter
}



