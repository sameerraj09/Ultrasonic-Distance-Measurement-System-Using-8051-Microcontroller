/*
 Find distance of an Object by interfacing Ultrasonic HC-SR04 module with
8051(AT89S52)
*/
#include<reg52.h> // Include 8051 register definitions
#include <stdio.h> // Include standard I/O library
#include <LCD_8_bit.h> // Include LCD library
#define sound_velocity 34300 // Speed of sound in cm/s
#define period_in_us 0.000001 // Period of 1 microsecond in seconds
#define Clock_period 1.085*period_in_us // Period for clock cycle of 8051 (approximately)
sbit Trigger_pin=P2^6; // Define Trigger pin
sbit Echo_pin=P2^7; // Define Echo pin
void Delay_us()
{
 TL0=0xF5; // Load Timer0 Low byte with 0xF5
 TH0=0xFF; // Load Timer0 High byte with 0xFF
 TR0=1; // Start Timer0
 while (TF0==0); // Wait until Timer0 overflows (TF0 becomes 1)
 TR0=0; // Stop Timer0
 TF0=0; // Clear Timer0 overflow flag
}
void init_timer(){
 TMOD=0x01; // Set Timer0 in Mode 1 (16-bit mode)
 TF0=0; // Clear Timer0 overflow flag
 TR0 = 0; // Stop Timer0 initially
}
void send_trigger_pulse(){
 Trigger_pin= 1; // Pull trigger pin HIGH
 Delay_us(); // Provide 10uS Delay
 Trigger_pin = 0; // Pull trigger pin LOW
}
// convert a floating-point number to a string representation with specified precision.
void my_sprintf(char buffer[], float value, int precision) {
 int whole_part = (int)value; // Extract the whole part of the float
 int temp = whole_part; // Temporary variable to handle the whole part
 float fractional_part = value - whole_part; // Extract the fractional part
 int i;
 int digit;

 // Convert the whole part to string
 int whole_part_index = 0;
 if (whole_part < 0) {
 buffer[whole_part_index++] = '-';
 whole_part = -whole_part;
 }
 do {
 buffer[whole_part_index++] = (temp % 10) + '0';
 temp /= 10;
 } while (temp > 0);

 // Reverse the whole part string
 for (i = 0; i < whole_part_index / 2; ++i) {
 char tmp = buffer[i];
 buffer[i] = buffer[whole_part_index - i - 1];
 buffer[whole_part_index - i - 1] = tmp;
 }

 // Add decimal point
 buffer[whole_part_index++] = '.';

 // Convert fractional part to string
 for (i = 0; i < precision; ++i) {
 fractional_part *= 10;
 digit = (int)fractional_part;
 buffer[whole_part_index++] = digit + '0';
 fractional_part -= digit;
 }

 // Null-terminate the string
 buffer[whole_part_index] = '\0';
}
void main()
{
 float distance_measurement, value;
 unsigned char distance_in_cm[10];

 LCD_Init(); // Initialize 16x2 LCD
 LCD_String_xy(1,1,"Distance"); // Display "Distance" at LCD row 1, column 1
 init_timer(); // Initialize Timer

 while(1) {
 send_trigger_pulse(); // Send trigger pulse of 10us

 while(!Echo_pin); // Wait for Echo pin to go high

 TR0 = 1; // Start Timer
 while(Echo_pin && !TF0); // Wait for Echo pin to go low or Timer overflow

 TR0 = 0; // Stop the Timer

 // Calculate distance using timer
 value = Clock_period * sound_velocity;
 distance_measurement = (TL0|(TH0<<8)); // Read timer register for time count
 distance_measurement = (distance_measurement * value) / 2.0; // Calculate distance (in cm)

 my_sprintf(distance_in_cm, distance_measurement, 2); // Convert distance to string
 LCD_String_xy(2,1,distance_in_cm); // Show distance on 16x2 LCD
 LCD_String(" cm");
 delay(100); // Delay for stability
 }
} 
