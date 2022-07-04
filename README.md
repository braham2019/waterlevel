# Waterlevel

My first ever project based on Arduino code. I recently installed two 10000 liter rainwater tanks, and I wanted to visualize how much water is left in the tank in real time.  The information needed to be visual on a small LCD screen inside my garage and also on a web page, where it could be scraped or viewed. I also wanted to be able to update the Arduino code using a web browser.

## Hardware components used:
- Adafruit ESP32 Huzzah (https://www.adafruit.com/product/3591)
- Adafruit Assembled Terminal Block Breakout FeatherWing (https://www.adafruit.com/product/2926)
- A02YYUW waterproof distance meter (https://wiki.dfrobot.com/_A02YYUW_Waterproof_Ultrasonic_Sensor_SKU_SEN0311p)
- Grove 16x2 LCD white on blue (https://wiki.seeedstudio.com/Grove-16x2_LCD_Series/)
- Plastic box to mount LCD and ESP

## Software components (Arduino libraries) used:
- Wifi: for network connectivity
- rgb_lcd: for the Grove Seeed LCD screen
- Softwareserial: to represent the software serial port for the ultrasonic distance sensor
- ArduinoJSON: to buid the web page
- Webserver & ElegantOTA: for Over The Air updates of the code

The code is pretty self-explanatory (I think): first Wifi is enabled then the JSON webserver and the OTA webserver are started. The loop will take a reading and update the LCD screen every 5 seconds. When a web client connects, an up-to-date web page will be generated.

## Additional info:
- I gave the ESP32 a static IP address through DHCP reservation.
- The JSON webserver is running on port 80, the OTA server on port 81.
- The LCD screen shows the reading in percentage and displays a progress bar.
- The JSON webserver shows percentage and the distance to the water. The percentage information is gathered by Home Assistant through a REST API call sensor so I have historical metrics available. 

### Home Assistant configuration:
```
sensor:
- platform: rest
    name: regenput
    resource: http://192.168.0.x
    value_template: "{{value_json.data}}"
    unit_of_measurement: "%"
    scan_interval: 60
```
- I also display the information on my Dakboard. As Dakboard can only gather external https resources, I am using a Traefik reverse proxy to allow access to the JSON server.
