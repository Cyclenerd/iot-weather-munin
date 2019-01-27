# Arduino Uno & DHT22

## Set-Up

Setup and configuration exactly the same as [ESP8266](../README.md).

## Requirements

You need:

* One Arduino Uno
* One Arduino Uno Ethernet Shield 
* One DHT22 sensor
* A few cables to connect everything

## Wiring

Connect the DHT22 data pin to `DIGITAL 7`.

```
  _______
 /  ( )  \
 +-------+
 | [] [] |
 | [] [] |
 | [] [] |
 +-------+
  | | | |
  
   \ \ \ \__ Ground: Connect it to GND
    \ \ \___ Nothing
     \ \____ Data pin: Connect it to D7
      \_____ Positive pin: Connect it to 3v3
```