#ifndef __MY_MQTT_CLIENT_H
#define __MY_MQTT_CLIENT_H

#include "stdio.h"	
#include "sys.h" 
#include "delay.h"




typedef struct {
    char *apn;
    char *apn_user;
    char *apn_pass;
} sim_t;

typedef struct {
    char *host;
    uint16_t port;
    uint8_t connect;
} mqttServer_t;

typedef struct {
    char *username;
    char *pass;
    char *clientID;
    unsigned short keepAliveInterval;
} mqttClient_t;

typedef struct {
    uint8_t newEvent;
    unsigned char dup;
    int qos;
    unsigned char retained;
    unsigned short msgId;
    unsigned char payload[64];
    int payloadLen;
    unsigned char topic[64];
    int topicLen;
} mqttReceive_t;

typedef struct {
    sim_t sim;
    mqttServer_t mqttServer;
    mqttClient_t mqttClient;
    mqttReceive_t mqttReceive;
} SIM800_t;


extern SIM800_t SIM800;

void InitSim800(void);

void Sim800_RxCallBack(void);

void clearRxBuffer(void);

void clearMqttBuffer(void);

int SIM800_SendCommand(char *command, char *reply, uint16_t delay);

int MQTT_Init(void);

void MQTT_Connect(void);

void MQTT_Pub(char *topic, char *payload);

void MQTT_PingReq(void);

void MQTT_Sub(char *topic);

void MQTT_Receive(unsigned char *buf);




#endif

