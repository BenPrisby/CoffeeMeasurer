/**
 * Arduino sketch for the CoffeeMeasurer project.
 * Ben Prisby <ben@benprisby.com>
 */

#include "HX711.h"
#include <avr/sleep.h>
/*---------------------------------------------------------------------------*/

// Coffee-to-water ratio (strength), where 1:16 is considered average, lower
// is stronger, and higher is weaker. The factor below is just the water.
const int STRENGTH_FACTOR = 13;

// Interval to blink prompt LEDs.
const int BLINK_INTERVAL_MS = 500;

// State LED pins.
const int INIT_LED_PIN = 2;    // Red
const int COFFEE_LED_PIN = 3;  // Yellow
const int WATER_LED_PIN = 4;   // Blue
const int DONE_LED_PIN = 5;    // Green

// Illuminated push button pins.
const int BUTTON_PIN = 8;
const int BUTTON_LED_PIN = 9;

// HX711 pins.
const int LOADCELL_DOUT_PIN = 10;
const int LOADCELL_SCK_PIN = 11;

// HX711 scale object.
HX711 g_Scale;

// Scale calibration factor obtained using the sketch included in the library.
const float CALIBRATION_FACTOR = 384.618f;

// Combined coffee and water weight used as a reference when water is added.
float g_fTotalExpectedWeight = 0.0;

// Flag that indicates when enough water has been added to satisfy the ratio.
bool g_bDone = false;

// Tracking variables used to blink the water LED.
unsigned long g_ulPreviousWaterLEDTimestamp = 0;
int g_iWaterLEDState = HIGH;
/*---------------------------------------------------------------------------*/

void setup()
{
    float fCoffeeWeight = 0.0;
    float fWaterToAdd = 0.0;
    int iCoffeeLEDState = HIGH;
    unsigned long ulPreviousTimestamp = 0;

    // Set up and immediately turn on the first LED.
    pinMode( INIT_LED_PIN, OUTPUT );
    digitalWrite( INIT_LED_PIN, HIGH );
  
    // Set up the other LEDs.
    pinMode( COFFEE_LED_PIN, OUTPUT );
    pinMode( WATER_LED_PIN, OUTPUT );
    pinMode( DONE_LED_PIN, OUTPUT );
    pinMode( BUTTON_LED_PIN, OUTPUT );

    // Set up the button pin using the internal pullup resistor.
    pinMode( BUTTON_PIN, INPUT );
    digitalWrite( BUTTON_PIN, HIGH );

    // Set up serial output for debug information.
    Serial.begin( 9600 );

    // Initialize and zero out the scale.
    Serial.println( "Initializing scale..." );
    g_Scale.begin( LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN );
    g_Scale.set_scale( CALIBRATION_FACTOR );
    g_Scale.tare();

    // Done initializing, turn on the next pair of LEDs.
    digitalWrite( INIT_LED_PIN, LOW );
    digitalWrite( COFFEE_LED_PIN, HIGH );
    digitalWrite( BUTTON_LED_PIN, HIGH );
    
    Serial.println( "Add coffee now..." );
    ulPreviousTimestamp = millis();

    // Blink the coffee LED until the button is pressed.
    while ( HIGH == digitalRead( BUTTON_PIN ) )
    {
        unsigned long ulCurrentTimestamp = millis();
        
        // Check if enough time has passed to toggle the LED.
        if ( ( ulCurrentTimestamp - ulPreviousTimestamp ) 
             >= BLINK_INTERVAL_MS )
        {
            // Toggle the LED.
            if ( HIGH == iCoffeeLEDState )
            {
                digitalWrite( COFFEE_LED_PIN, LOW );
                iCoffeeLEDState = LOW;
            }
            else
            {
                digitalWrite( COFFEE_LED_PIN, HIGH );
                iCoffeeLEDState = HIGH;
            }

            // Save the timestamp as part of the non-blocking approach.
            ulPreviousTimestamp = ulCurrentTimestamp;
        }
    }

    // Button pressed, move on to the next stage.
    digitalWrite( BUTTON_LED_PIN, LOW );
    digitalWrite( COFFEE_LED_PIN, LOW );
    digitalWrite( WATER_LED_PIN, HIGH );

    // Use the same non-blocking blinking approach, so save the timestamp.
    g_ulPreviousWaterLEDTimestamp = millis();

    // Figure out the total expected weight based on how much coffee was added.
    fCoffeeWeight = g_Scale.get_units();
    fWaterToAdd = fCoffeeWeight * STRENGTH_FACTOR;
    g_fTotalExpectedWeight = fCoffeeWeight + fWaterToAdd;
    Serial.print( "Coffee weight:\t" );
    Serial.println( fCoffeeWeight, 2 );
    Serial.print( "Water to add:\t" );
    Serial.println( fWaterToAdd, 2 );
}
/*---------------------------------------------------------------------------*/

void loop()
{
    if ( !g_bDone )
    {
        float fReading = g_Scale.get_units();
        unsigned long ulCurrentWaterLEDTimestamp = millis();

        // Toggle the water LED if necessary using the same method as before.
        if ( ( ulCurrentWaterLEDTimestamp - g_ulPreviousWaterLEDTimestamp )
             >= BLINK_INTERVAL_MS )
        {
            if ( HIGH == g_iWaterLEDState )
            {
                digitalWrite( WATER_LED_PIN, LOW );
                g_iWaterLEDState = LOW;
            }
            else
            {
                digitalWrite( WATER_LED_PIN, HIGH );
                g_iWaterLEDState = HIGH;
            }

            g_ulPreviousWaterLEDTimestamp = ulCurrentWaterLEDTimestamp;
        }

        // Check if enough water has been added.
        if ( g_fTotalExpectedWeight > fReading )
        {
            // Not enough, keep waiting.
            Serial.print( "Still expecting:\t" );
            Serial.println( g_fTotalExpectedWeight - fReading, 2 );
            delay( 50 );
        }
        else
        {
            // Enough added, signal that we are done.
            digitalWrite( WATER_LED_PIN, LOW );
            digitalWrite( DONE_LED_PIN, HIGH );
            g_bDone = true;
        }
    }
    else
    {
        // Power everything down indefinitely.
        Serial.println( "Enough water added, sleeping." );
        g_Scale.power_down();
        set_sleep_mode( SLEEP_MODE_PWR_DOWN );
        sleep_enable();
        sleep_mode();
    }
}
/*---------------------------------------------------------------------------*/
