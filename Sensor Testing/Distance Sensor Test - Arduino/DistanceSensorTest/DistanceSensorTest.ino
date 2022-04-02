/*Simple distance sensor test
 * Just prints data to the serial plotter.
 * 
 * Manufacture specs: 
 * 10cm = Voltage = 3V
 * 80cm = Voltage = 0.4V
 * 
 * Measured voltages:
 * X < 10 cm = 3.1V
 * X > 80 cm = about 200mV or less.
 * 
 * Voltage change is NOT linear, it looks
 * more logarithmic.
 * Steep increase closer to 3V.
 * Levels out the further away it goes.
 * This matches data sheet.
 * 
 * Tested with hand, envelope, and amazon
 * cardboard mailer. Results about the same.
 * 
 */

double aVolt;
double conversion;

void setup() {
  pinMode(A0, INPUT);

  Serial.begin(9600); //9600 baud
  //We don't need speed, we will delay slightly to prevent overloading
  //serial monitor/plotter

  aVolt = 0;
  conversion = 0;
  delay(45); //Initial reading is unstable per datasheet. Wait for stability.

}

void loop() {
  aVolt = analogRead(A0);
  conversion = aVolt/200; //Converts analog value to actual voltage. Confirmed with a multimeter.

  Serial.println(conversion);

  delay(5);
}
