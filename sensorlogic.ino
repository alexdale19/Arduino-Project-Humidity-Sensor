#include <Arduino.h>
#include "DHT_Async.h"
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 2;
DHT_Async dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
// store last 3 humidity readings
float lastHumidities[3] = {0};

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}


static bool measure_environment(float *temperature, float *humidity) {
    static unsigned long measurement_timestamp = millis();

    /* Measure once every four seconds. */
    if (millis() - measurement_timestamp > 1000ul) {
        if (dht_sensor.measure(temperature, humidity)) {
            measurement_timestamp = millis();
            return (true);
        }
    }

    return (false);
}

/*
 * Calculate the average of the last 3 humidity readings.
 */
float calculateAverageHumidity() {
    float sum = 0;
    for (int i = 0; i < 3; ++i) {
        sum += lastHumidities[i];
    }
    return sum / 3.0;
}

/*
 * Main program loop.
 */
void loop() {
    float temperature;
    float humidity;
    
    /* Measure temperature and humidity. */
    if (measure_environment(&temperature, &humidity)) {
        Serial.print("T = ");
        Serial.print(temperature, 1);
        Serial.print(" deg. C, H = ");
        Serial.print(humidity, 1);
        Serial.println("%");
        
        // Store the current humidity reading in the lastHumidities array
        // and calculate the average
        for (int i = 0; i < 2; ++i) {
            lastHumidities[i] = lastHumidities[i + 1];
        }
        lastHumidities[2] = humidity;
        float baselineHumidity = calculateAverageHumidity();
        
        // Activate LED if current humidity is higher than baseline average
        if (humidity > baselineHumidity) {
            digitalWrite(LED_BUILTIN, HIGH);
        } else {
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}
