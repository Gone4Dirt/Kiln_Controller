#include "./src/logger/logger.h"





Logger _logger;


void setup() {

  Serial.begin(9600);

  _logger.init();

}


void loop()
{
  _logger.write(42);
}
