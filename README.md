# Environment monitoring system with notification system
Budget friendly and lower-power consumption environment monitoring system with ESP32 board, BME680 sensor and PM-G7.

## Overview

This guide is part of a project I built to monitor the air quality, temperature, humidity, pressure and gas in a closed room. 

It's built using an ESP32 board for its low power consumption and small size. 

I've paired this project with a notification system, you can configure different notifications based on my needs. It's placed in a room and notifies me when: 
- The air quality has deteriorated and I need to open a window to refresh the air
- The temperature is running high and I need to cool down with a fan
- The temperature is running low and I need to turn up the heaters
- The air quality is dangerous because of different levels of gas

I've also 3D printed a small case (PLA) to hold the pieces together.

Excluding time for build and development, the breakdown of the costs are as follows: 

- EUR 7.66 for the ESP32
- EUR 18.50 for the BME680 sensor
- EUR 19.99 for the PM-G7 sensor
- EUR 0.67 for the 8 magnets
- EUR 5 for the power supply (I used a micro-USB power supply I had already, I'm guessing you can get one for that price off Amazon)
- EUR for the X grams of PLA
- EUR for the X kilowatts of energy to print the piece (X hours)

The total of this project comes down to round **EUR 60**

## Resources

- [ESP32 board by diymore ](https://www.amazon.fr/dp/B0BS6R68H6?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- [BME680 sensor](https://www.amazon.fr/dp/B07K1CGQTJ?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- [PM-G7 sensor](https://www.amazon.fr/dp/B0B1526JHQ?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- Jumper cables (female to male and )
- PLA + (I have a personal preference for SUNLU PLA +, but you can use any kind of PLA)
- [5x3mm magnets for the case](https://www.amazon.fr/dp/B0C9WQYZ27?ref=ppx_yo2ov_dt_b_product_details&th=1)
- Some strong glue to stick the magnets to the case


## 3D prints

The parts were printed using a Creality CR-20 pro. 

The pieces are as follows: 


The print times are as follows: 


My slicer settings were as follows: 



## Assembly and wiring

![ESP32 pin-out for reference](./images/esp32.jpg)

## Software

I'm assuming you are familiar with the ESP32 micro-controller and now how to access and run it. There are loads of guides to get started, you can use these for reference: 
- https://randomnerdtutorials.com/getting-started-with-esp32/
- https://www.youtube.com/watch?v=tc3Qnf79Ny8

## Final comments


## Future improvements

- Make it more portable by using a portable battery