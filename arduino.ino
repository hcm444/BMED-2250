#include <Arduino.h>
#include <algorithm> // For std::sort

const int GSR = A0;
const int SAMPLES = 10;
int readings[SAMPLES];
int gsr_average = 0;
bool isRunning = false;

void setup() {
   Serial.begin(9600);
   Serial.println("Press 's' to start/stop data collection");
}

void loop() {
    if (Serial.available() > 0) {
        char input = Serial.read();
        if (input == 's' || input == 'S') {
            // Initialize readings
            for (int i = 0; i < SAMPLES; i++) {
                readings[i] = analogRead(GSR);
            }

            // Sort readings for median calculation
            std::sort(readings, readings + SAMPLES);
            int medianGSR = readings[SAMPLES / 2];

            int human_resistance = 0;
            if (516 - medianGSR != 0) {
                human_resistance = abs(((1024 + 2 * medianGSR) * 10000) / (516 - medianGSR));
            }

            isRunning = !isRunning;
            if (!isRunning) {
                Serial.print("Final Median Resistance: ");
                Serial.println(human_resistance);
            } else {
                Serial.println("Starting data collection...");
            }
        }
    }

    if (isRunning) {
        // Collect new readings
        for (int i = 0; i < SAMPLES; i++) {
            readings[i] = analogRead(GSR);
        }

        // Sort readings
        std::sort(readings, readings + SAMPLES);

        // Calculate Q1, Q2 (median), and Q3
        int Q1 = readings[SAMPLES / 4];
        int Q2 = readings[SAMPLES / 2];  // median
        int Q3 = readings[3 * SAMPLES / 4];

        // Calculate IQR and outlier bounds
        int IQR = Q3 - Q1;
        int lowerBound = Q1 - (1.5 * IQR);
        int upperBound = Q3 + (1.5 * IQR);

        // Calculate valid readings (exclude outliers)
        int validCount = 0;
        long validSum = 0;

        for (int i = 0; i < SAMPLES; i++) {
            if (readings[i] >= lowerBound && readings[i] <= upperBound) {
                validSum += readings[i];
                validCount++;
            }
        }

        // Final GSR average calculation
        gsr_average = validCount > 0 ? validSum / validCount : Q2;

        int human_resistance = 0;
        if (516 - gsr_average != 0) {
            human_resistance = abs(((1024 + 2 * gsr_average) * 10000) / (516 - gsr_average));
        }

        Serial.print("Human Resistance: ");
        Serial.println(human_resistance);
    }

    delay(50);  // Delay for readability in the serial plotter
}
