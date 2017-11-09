/*
 * Fader
 * Version 0.1 October, 2015
 * Copyright 2015 Alan Zucconi
 *
 */


#include "DW1000Positioning.h"

DW1000PositioningClass DW1000Positioning;

void DW1000PositioningClass::startAsAnchor(uint16_t _address){
    
    _isAnchor = true;
    _device.type = "anchor";
    _device.address = _address;
    initEmptyBeacons();
    
}

void DW1000PositioningClass::startAsBeacon(uint16_t _address){
    
    _isAnchor = false;
    _device.type = "beacon";
    _device.address = _address;
    initEmptyBeacons();
    
}

void DW1000PositioningClass::loop(){
    
    
}

void DW1000PositioningClass::initTestBeacons(){
    for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
        struct _Node _beacon;
        _beacon.type = "beacon";
        _beacon.address = random(20, 60);
        _beacon.distance = random(2, 100);
        _networkDevices[i] = _beacon;
    }
}


void DW1000PositioningClass::initEmptyBeacons(){
    for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
        struct _Node _beacon;
        _beacon.type = "beacon";
        _networkDevices[i] = _beacon;
    }
}

void DW1000PositioningClass::addNetworkDevice(uint16_t _address){
    bool _exists = false;
    for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
        if(_networkDevices[i].address == _address){
            _exists = true;
            break;
        }
    }
    if(!_exists){
        for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
            if(!_networkDevices[i].address){
                _networkDevices[i].address = _address;
                break;
            }
        }
    }
    serialSendDistances();
}

void DW1000PositioningClass::removeNetworkDevice(uint16_t _address){
    for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
        if(_networkDevices[i].address == _address){
            _networkDevices[i].address = 0;
            _networkDevices[i].distance = 0.0;
            _networkDevices[i].position.x = 0.0;
            _networkDevices[i].position.y = 0.0;
            _networkDevices[i].position.z = 0.0;
            break;
        }
    }
}

void DW1000PositioningClass::setDistance(uint16_t _address, float _distance){
    for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
        if(_networkDevices[i].address == _address){
            _networkDevices[i].distance = _distance;
            break;
        }
    }
}

String DW1000PositioningClass::createJsonPosition(struct _Node _node){
    /*
        TODO: Send device information more effiecient than json
    */
    String json;
    json += "{type:";
    json += POSITION;
    json += ",device:'";
    json += _node.type;
    json += "',address:'";
    json += String(_node.address, HEX);
    json += "',data:{x:";
    json += _node.position.x;
    json += ",y:";
    json += _node.position.y;
    json += ",z:";
    json += _node.position.z;
    json += "}}";
    return json;
}

String DW1000PositioningClass::createJsonDistance(struct _Node _node){
    /*
     TODO: Send device information more effiecient than json
     */
    String json;
    json += "{type:";
    json += DISTANCE;
    json += ",to:";
    json += _node.address;
    json += ",distance:";
    json += _node.distance;
    json += "}"; 
    return json;
}


String DW1000PositioningClass::createJsonDistances(){
    /*
     TODO: Send device information more effiecient than json
     */
    String json;
    json += "[";
    if(_device.address > 0){
        json += createJsonDistance(_device);
        json += ",";
    }
    for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
        json += createJsonDistance(_networkDevices[i]);
        if(i !=_NUM_NETWORK_DEVICES - 1){
            json += ",";
        }
    }
    json += "]";
    return json;

}

String DW1000PositioningClass::createJsonPositions(){
    String json;
    json += "[";
    if(_device.type.length() > 0){
        json += createJsonPosition(_device);
        json += ",";
    }
    for (uint8_t i = 0; i < _NUM_NETWORK_DEVICES; i++){
        json += createJsonPosition(_networkDevices[i]);
        if(i !=_NUM_NETWORK_DEVICES - 1){
            json += ",";
        }
    }
    json += "]";
    return json;
}

void DW1000PositioningClass::serialSendPositition(struct _Node _device){
    Serial.println(createJsonPosition(_device));
}

void DW1000PositioningClass::serialSendPosititions(){
    Serial.println(createJsonPositions());
}

void DW1000PositioningClass::serialSendDistance(struct _Node _device){
    Serial.println(createJsonPosition(_device));
}

void DW1000PositioningClass::serialSendDistances(){
    Serial.println(createJsonPositions());
}

/*
    Setters and getters
*/

struct _Node DW1000PositioningClass::getNextDevice(){
    _nextDevice == 2 ? _nextDevice = 0 : _nextDevice++;
    return _networkDevices[_nextDevice];
}

struct _Node DW1000PositioningClass::getDevice(){
    return _device;
}

_STATES DW1000PositioningClass::getState(){
    return _state;
}

void DW1000PositioningClass::setState(_STATES state){
    _state = state;
}
 
