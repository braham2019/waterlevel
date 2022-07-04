![IMG_1668](https://user-images.githubusercontent.com/56874881/177161099-1738cd71-b9c6-4f0d-9509-f5b5ccc28ed7.jpg)
# Waterlevel

My first ever project based on Arduino code. I recently installed two 10000 liter rainwater tanks, and I wanted to visualize how much water is left in the tank in real time.  The information needed to be visual on a small LCD screen inside my garage (next to the watertanks) and also on a web page, where it could be scraped or viewed. I also wanted to be able to update code using a web browser.

Components used:
- Adafruit ESP32 Huzzah (https://www.adafruit.com/product/3591)
- Adafruit Assembled Terminal Block Breakout FeatherWing (https://www.adafruit.com/product/2926)
- A02YYUW waterproof distance meter (https://wiki.dfrobot.com/_A02YYUW_Waterproof_Ultrasonic_Sensor_SKU_SEN0311p)
- Grove 16x2 LCD white on blue (https://wiki.seeedstudio.com/Grove-16x2_LCD_Series/)
- Plastic box to mount LCD and ESP

Software components (Arduino libraries) used:
- Wifi: for network connectivity
- rgb_lcd: for the Grove Seeed LCD screen
- Softwareserial: to represent the software serial port for the ultrasonic distance sensor
- ArduinoJSON: to buid the web page
- Webserver & ElegantOTA: for Over The Air updates of the code

The JSON webserver is running on port 80, the OTA server on port 81.
