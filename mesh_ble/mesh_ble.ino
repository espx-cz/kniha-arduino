#include "painlessMesh.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define MESH_PREFIX "ESP_MESH"
#define MESH_PASSWORD "meshpassword"
#define MESH_PORT 5555

Scheduler userScheduler;
painlessMesh mesh;

BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID_RX "12345678-1234-1234-1234-123456789013"
#define CHARACTERISTIC_UUID_TX "12345678-1234-1234-1234-123456789014"

void sendMessage() {
  String msg = "Ahoj od uzlu ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast(msg);
  Serial.println("Odesílám zprávu: " + msg);
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Přijato od %u zpráva=%s\n", from, msg.c_str());
  if (deviceConnected) {
    pTxCharacteristic->setValue(msg.c_str());
    pTxCharacteristic->notify();
    Serial.println("Odesílám zprávu do BLE: " + msg);
  }
}

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      Serial.println("Přijata zpráva přes BLE: " + String(rxValue.c_str()));
      mesh.sendBroadcast(String(rxValue.c_str()));
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Nastavení BLE
  BLEDevice::init("ESP32_Mesh_BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pTxCharacteristic->addDescriptor(new BLE2902());
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
  
  // Nastavení mesh sítě
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  Task taskSendMessage(TASK_SECOND * 10, TASK_FOREVER, &sendMessage);
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  userScheduler.execute();
  mesh.update();
  
  // Logika pro odpojení
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}
