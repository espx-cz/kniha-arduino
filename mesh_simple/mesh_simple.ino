#include "painlessMesh.h"

#define MESH_PREFIX "ESP_MESH"
#define MESH_PASSWORD "meshpassword"
#define MESH_PORT 5555

Scheduler userScheduler; // Plánovač úloh pro uživatelské úkoly
painlessMesh mesh;

void sendMessage() {
  String msg = "Ahoj od uzlu ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast(msg);
  Serial.println("Odesílám zprávu: " + msg);
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Přijato od %u zpráva=%s\n", from, msg.c_str());
}

Task taskSendMessage(TASK_SECOND * 10, TASK_FOREVER, &sendMessage); // Definice úkolu

void setup() {
  Serial.begin(115200);
  mesh.setDebugMsgTypes(ERROR | STARTUP); // nastav úroveň ladění
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  userScheduler.addTask(taskSendMessage); // Přidej úkol do plánovače
  taskSendMessage.enable(); // Povolení úkolu
}

void loop() {
  userScheduler.execute();
  mesh.update();
}
