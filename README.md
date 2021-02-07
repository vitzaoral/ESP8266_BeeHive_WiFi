# Bee hive temperature/humidity and security monitoring with ESP8266 (Wemos D1 mini)
Data are sent to the Blynk.

> To build a project, you need to download all the necessary libraries and create the *settings.cpp* file in the *src* folder:
```c++
// Project settings
struct Settings
{
    const char *blynkAuth = "XXX";
    const char *version = "1.0.0";

    const char *wifiSSID = "WIFI";
    const char *wifiPassword = "123";

    const char *firmwareVersionUrl = "http://iot.example.com/beehive/version.txt";
    const char *firmwareBinUrl = "http://iot.example.com/beehive/firmware.bin";
};
```
### Features
* measuring temperature and humidity with SHT31 sensor
* Sends push notification, when alarm is triggered (magnetic sensors)

### Currents list:

* *WEMOS D1 Mini*
* *SHT31/SHT3x* temperature & humidity sensor
* [5V to 3.3V regulator](https://www.aliexpress.com/item/32807311456) for 4.2V 18650 LiIon battery 
* [Magnetic sensors](https://www.aliexpress.com/item/1005001309886250.html)

### Schema:
![Schema](https://github.com/vitzaoral/ESP8266_BeeHive_WiFi/blob/master/schema/schema.png?raw=true)

