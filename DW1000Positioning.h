/*
 * Fader
 * Version 0.1 October, 2015
 * Copyright 2015 Alan Zucconi
 * wwww.alanzucconi.com
 *
 */

#ifndef DW1000Positioning_h
#define DW1000Positioning_h

#include <string.h>
#include <arduino.h>

// structs
struct _Node{
    String type;
    uint16_t address = 0;
    float distance = 0.0;
    struct {
        float x = 0.0;
        float y = 0.0;
        float z = 0.0;
    } position;
};

// states
enum _STATES{
    CONFIG,  //Anchor -> Receive messages/distanes from beacons, Beacons -> Fetch distances from other beacons
    SENDER,  //Beacons -> Send all distances to anchor
    RECEIVER,//Anchor -> Receive all distances from beacon, parallell with SENDER
    RANGER,  //Nodes receives distances from other nodes
};

// states
enum _MESSAGE_TYPE{
    POSITION,
    DISTANCE,
};

// conts
#define _NUM_NETWORK_DEVICES 3

// Results returned from the decoder
class DW1000PositioningClass {
  public:
    
    void DW1000PositioningClass::startAsAnchor(uint16_t address);
    void DW1000PositioningClass::startAsBeacon(uint16_t address);
    void DW1000PositioningClass::initTestBeacons();
    void DW1000PositioningClass::initEmptyBeacons();
    void DW1000PositioningClass::addNetworkDevice(uint16_t address);
    void DW1000PositioningClass::removeNetworkDevice(uint16_t address);
    void DW1000PositioningClass::setState(_STATES state);
    void DW1000PositioningClass::serialSendPosititions();
    void DW1000PositioningClass::serialSendPositition(struct _Node _node);
    void DW1000PositioningClass::serialSendDistances();
    void DW1000PositioningClass::serialSendDistance(struct _Node _node);
    void DW1000PositioningClass::loop();
    void DW1000PositioningClass::setDistance(uint16_t _address, float _distance);
    struct _Node DW1000PositioningClass::getNextDevice();
    struct _Node DW1000PositioningClass::getDevice();
    String DW1000PositioningClass::createJsonPosition(struct _Node node);
    String DW1000PositioningClass::createJsonDistance(struct _Node node);
    String DW1000PositioningClass::createJsonPositions();
    String DW1000PositioningClass::createJsonDistances();
    _STATES DW1000PositioningClass::getState();

  private:
    
    _STATES _state = CONFIG;
    int _nextDevice = 0;
    bool _isAnchor = true;
    struct _Node _device;
    struct _Node _networkDevices[_NUM_NETWORK_DEVICES];
    
};

extern DW1000PositioningClass DW1000Positioning;

#endif
