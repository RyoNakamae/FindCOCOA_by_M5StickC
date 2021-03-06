#include <M5StickC.h>
#include <BLEDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

//static int giBattery = 0;
double vbat = 0.0;

// Contact Tracing Bluetooth Specification (Apple/Google)
// https://blog.google/documents/58/Contact_Tracing_-_Bluetooth_Specification_v1.1_RYGZbKW.pdf
const char* uuid = "0000fd6f-0000-1000-8000-00805f9b34fb";
bool found = false;
int deviceNum = 0;
int nearDeviceNum = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if(advertisedDevice.haveServiceUUID()){
            if(strncmp(advertisedDevice.getServiceUUID().toString().c_str(),uuid, 36) == 0){
                int rssi = advertisedDevice.getRSSI();
                Serial.print("  >> ADDR: ");
                Serial.print(advertisedDevice.getAddress().toString().c_str());
                Serial.print(" , ");
                Serial.print("RSSI: ");
                Serial.print(rssi);
                deviceNum++;
                if(rssi > -75){
                    found = true;
                    Serial.print(" >> Near! << ");
                    nearDeviceNum++;
                }
                Serial.println("");
            }
        }
    }
};

void setup() {
    M5.begin(true, false, true);
    M5.Lcd.setRotation( 1 );
    M5.Lcd.fillScreen(BLACK);
//    Serial.begin(115200);
    Serial.println("Scanning...");
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // less or equal setInterval value

    M5.Lcd.setTextSize(2);
}

void loop(){

    M5.update();

    Serial.println("Loop Start.");
    found = false;
    deviceNum = 0;
    nearDeviceNum = 0;
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false); 

    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(10, 5);
    M5.Lcd.setTextSize(1);
    M5.Lcd.print("-- COCOA FINDER --");

    
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30, 25);
    M5.Lcd.print("Found : ");
    M5.Lcd.println(deviceNum);
    Serial.print(" - FoundDevice : ");
    Serial.println(deviceNum);

    if(nearDeviceNum > 0){
      M5.Lcd.setTextColor(YELLOW);
    }
    M5.Lcd.setCursor(30, 45);
    M5.Lcd.print("Near : ");
    M5.Lcd.println(nearDeviceNum);
    Serial.print(" - NearDevice : ");
    Serial.println(nearDeviceNum);

    pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
    Serial.println("Loop End.");

    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(10, 70);
    M5.Lcd.setTextColor(WHITE);
    vbat = M5.Axp.GetBatVoltage();
    M5.Lcd.printf("Volt: %.2fV  ", vbat);
    for( int i = 0 ; i < 5 ; i++ ){
      M5.Lcd.print(".");
      delay(500);
      Serial.println(i);
    }

}
