#include "InternetConnection.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <BlynkSimpleEsp8266.h>
#include "../../src/settings.cpp"

Settings settings;

// HTTP Clients for OTA over WiFi
WiFiClient client;

// Attach Blynk virtual serial terminal
WidgetTerminal terminal(V0);

// alarm notifications are enabled
bool alarmEnabledNotifications = false;

// alarm is enabled
bool alarmIsEnabled = false;

// OTA - Number of milliseconds to wait without receiving any data before we give up
const int otaNetworkTimeout = 30 * 1000;
// OTA - Number of milliseconds to wait if no data is available before trying again
const int otakNetworkDelay = 1000;

// start OTA update process
bool startOTA = false;

// Synchronize settings from Blynk server with device when internet is connected
BLYNK_CONNECTED()
{
    Blynk.syncAll();
}

// Turn on/off alarm notifications
BLYNK_WRITE(V12)
{
    alarmEnabledNotifications = param.asInt();
    Serial.println("Alarm notifications was " + String(alarmEnabledNotifications ? "enabled" : "disabled"));
}

// Turn on/off alarm
BLYNK_WRITE(V11)
{
    alarmIsEnabled = param.asInt();
    Serial.println("Alarm was " + String(alarmIsEnabled ? "enabled" : "disabled"));
}

// Terminal input
BLYNK_WRITE(V0)
{
    String valueFromTerminal = param.asStr();
    Serial.println(valueFromTerminal);

    if (String("update") == valueFromTerminal)
    {
        terminal.println("Start OTA enabled");
        terminal.flush();
        startOTA = true;
    }
}

InternetConnection::InternetConnection()
{
    // true if is actual alarm - run Blynk.run
    isAlarm = false;
}

bool InternetConnection::initializeConnection()
{
    int connAttempts = 0;
    Serial.println("\r\nTry connecting to: " + String(settings.wifiSSID));

    // try config - faster for WiFi connection
    WiFi.begin(settings.wifiSSID, settings.wifiPassword);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if (connAttempts > 30)
        {
            Serial.println("Error - couldn't connect to WIFI");
            return false;
        }

        connAttempts++;
    }

    delay(1000);

    Serial.println("\r\nConnecting to WIFI OK, connnecting to Blynk");

    if (!Blynk.connected())
    {
        Blynk.config(settings.blynkAuth);
        // timeout 6sec
        Blynk.connect(2000);
    }

    return Blynk.connected();
}

void InternetConnection::disconnect()
{
    if (WiFi.isConnected())
    {
        Blynk.disconnect();
        WiFi.disconnect(true);

        Serial.println("Disconnected OK");
    }
    else
    {
        Serial.println("Already disconnected");
    }
}

void InternetConnection::sendDataToBlynk(
    MeteoData meteoData,
    MagneticLockController magneticLockController)
{
    // create data to send to Blynk
    if (Blynk.connected())
    {
        // meteo data
        Blynk.virtualWrite(V1, meteoData.sensor.humidity);
        Blynk.virtualWrite(V2, meteoData.sensor.temperature);

        // magnetic locks data
        setMagneticLockControllerDataToBlynk(magneticLockController);

        // set alarm info
        setAlarmInfoToBlynk();

        // WIFI info
        Blynk.virtualWrite(V5, "IP: " + WiFi.localIP().toString() + "|G: " + WiFi.gatewayIP().toString() + "|S: " + WiFi.subnetMask().toString() + "|DNS: " + WiFi.dnsIP().toString());
        Blynk.virtualWrite(V6, WiFi.RSSI());

        // version
        Blynk.virtualWrite(V10, settings.version);

        Blynk.run();
        delay(1000);

        Serial.println("Sending data to Blynk - DONE");
    }
    else
    {
        Serial.println("Blynk is not connected");
    }
}

void InternetConnection::setMagneticLockControllerDataToBlynk(MagneticLockController magneticLockController)
{
    Blynk.virtualWrite(V4, magneticLockController.sensorRoof.status);

    setAlarmInfoToBlynk();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// ALARM SECTION
////////////////////////////////////////////////////////////////////////////////////////////////////

void InternetConnection::setAlarmInfoToBlynk()
{
    Blynk.virtualWrite(V7, isAlarm ? "AKTUÁLNÍ ALARM!" : "OK");
    Blynk.virtualWrite(V8, alarmEnabledNotifications ? "Alarm notifikace zapnuty" : "Alarm notifikace vypnuty");
    Blynk.virtualWrite(V9, alarmIsEnabled ? "Alarm zapnut" : "Alarm vypnut");

    if (isAlarm)
    {
        Serial.println("\n !! ALARM !! \n");
    }
}

void InternetConnection::blynkRunIfAlarm()
{
    if (alarmIsEnabled && isAlarm)
    {
        Blynk.run();
    }
}

void InternetConnection::setMagneticLockControllerDataToBlynkIfAlarm(MagneticLockController magneticLockController)
{
    if (isAlarm)
    {
        setMagneticLockControllerDataToBlynk(magneticLockController);
    }
}

void InternetConnection::alarmMagneticController(MagneticLockController magneticLockController)
{
    if (!alarmIsEnabled)
    {
        return;
    }

    Serial.println("\n!!! Magnetic alarm !!!\n");

    if (!Blynk.connected())
    {
        initializeConnection();
    }

    if (Blynk.connected())
    {
        setMagneticLockControllerDataToBlynk(magneticLockController);

        if (alarmEnabledNotifications)
        {
            Blynk.notify("! ALARM ! Magnetický zámek je otevřen: " + magneticLockController.getAlarmMessage());
        }
    }
    else
    {
        Serial.println("ALARM but can't connected to Blynk");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// OTA SECTION
////////////////////////////////////////////////////////////////////////////////////////////////////

/// Println message to serial and Blynk terminal
void InternetConnection::printlnToTerminal(String message)
{
    Serial.println(message);
    terminal.println(message);
    terminal.flush();
}

void InternetConnection::checkForUpdates()
{
    Serial.println("Checking new firmware version..");
    if (!startOTA)
    {
        Serial.println("OTA - not setted");
        return;
    }
    else
    {
        startOTA = false;
        printlnToTerminal("OTA - START");
    }

    String message = "";
    String fwVersionURL = settings.firmwareVersionUrl;

    Serial.print("Firmware version URL: ");
    Serial.println(fwVersionURL);

    HTTPClient httpClient;
    WiFiClient client;

    httpClient.begin(client, fwVersionURL);

    int httpCode = httpClient.GET();

    if (httpCode == 200)
    {
        String newFwVersion = httpClient.getString();

        Serial.print("Current firmware version: ");
        Serial.println(settings.version);
        Serial.print("Available firmware version: ");
        Serial.println(newFwVersion);

        if (String(settings.version) != newFwVersion)
        {
            Serial.println("Preparing to update");

            String fwImageURL = settings.firmwareBinUrl;
            t_httpUpdate_return ret = ESPhttpUpdate.update(client, fwImageURL);

            switch (ret)
            {
            case HTTP_UPDATE_OK:
                message = "Successfuly updated!";
                Serial.println(message);
                break;
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                message = "Update failed: " + String(ESPhttpUpdate.getLastErrorString());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                message = "No updates";
                Serial.println(message);
                break;
            }
        }
        else
        {
            message = "Already on latest version";
            Serial.println(message);
        }
    }
    else
    {
        message = "Version check failed, http code: " + String(httpCode) + " ,message: " + httpClient.errorToString(httpCode);
        Serial.println(message);
    }
    httpClient.end();

    terminal.println(message);
    terminal.flush();
} 