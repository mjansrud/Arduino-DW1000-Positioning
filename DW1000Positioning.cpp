/*
 * Fader
 * Version 0.1 October, 2015
 * Copyright 2015 Alan Zucconi
 *
 */


#include "DW1000Positioning.h"

DW1000PositioningClass DW1000Positioning;

void DW1000PositioningClass::startAsAnchor(uint8_t _address){
    
    _isTag = false;
    _device = &_devices[_address];
    
}

void DW1000PositioningClass::startAsTag(uint8_t _address){
    
    _isTag = true;
    _device = &_devices[_address];
    
}

void DW1000PositioningClass::initTestDevices(){
    for (uint8_t i = 0; i < _NUM_DEVICES; i++){
        struct _Node _node;
        _node.address = i;
        _node.distance = random(2, 100);
        for (uint8_t j = 0; j < _NUM_DEVICES; j++){
            _node.distances[j].distance = random(2, 100);
        }
        _devices[i] = _node;
    }
}


void DW1000PositioningClass::initDevices(){
    for (uint8_t i = 0; i < _NUM_DEVICES; i++){
        struct _Node _node;
        _node.address = i;
        _devices[i] = _node;
    }
}

void DW1000PositioningClass::activeDevice(uint8_t _address){
    _devices[_address].active = true;
}

void DW1000PositioningClass::inactiveDevice(uint8_t _address){
    _devices[_address].active = false;
    _devices[_address].distance = 0.0;
}

void DW1000PositioningClass::setDistance(uint8_t _address, float _distance){
    _devices[_address].distance = _distance;
}

void DW1000PositioningClass::setDistanceBetweenDevices(uint8_t _from, uint8_t _to, float _distance){
    _devices[_from].distances[_to].distance = _distance;
    Serial.print("Msg,from:");
    Serial.print(_from);
    Serial.print(",to:");
    Serial.print(_to);
    Serial.print(",range:");
    Serial.println(_distance);
}

void DW1000PositioningClass::calculatePositions(){
    calculateAnchorPositions();
    calculateTagPositions();
}

void DW1000PositioningClass::calculateAnchorPositions(){
    
    //We define anchor 1 with position [0,0,0];
    //Define X axis between 1 -> 2.
    //Then we know that anchor 2 has position [X,0,0];
    _devices[2].position.x = _devices[1].distances[2].distance;
    
    //We use pytagoras to calculate position in C, make two triangles and solve two equations for X and Y.
    float A = _devices[1].distances[2].distance;
    float B = _devices[2].distances[3].distance;
    float C = _devices[1].distances[3].distance;
    
    float X = (pow(A,2)+pow(B,2)-pow(C,2))/(2*A);
    float Y = sqrt((-pow(A, 4)+2*pow(A,2)*(pow(B,2)+pow(C,2))-pow(pow(B,2)-pow(C,2),2))/(2*A));
    
    _devices[3].position.x = X;
    _devices[3].position.y = Y;

}
void DW1000PositioningClass::calculateTagPositions(){
    
    //We use pytagoras to calculate position in C, make two triangles and solve two equations for X and Y.
    float A = _devices[1].distances[2].distance;
    float B = _devices[1].distance;
    float C = _devices[2].distance;
    
    float X = (pow(A,2)+pow(B,2)-pow(C,2))/(2*A);
    float Y = sqrt((-pow(A, 4)+2*pow(A,2)*(pow(B,2)+pow(C,2))-pow(pow(B,2)-pow(C,2),2))/(2*A));
    
    _device->position.x = X;
    _device->position.y = Y;
    
}

void DW1000PositioningClass::calculatePositionsAndDraw(){
    calculatePositions();
    serialDrawDistances();
    serialSendPosititions();
}

String DW1000PositioningClass::createJsonDistance(struct _Node _node){
    /*
     TODO: Send device information more effiecient than json
     */
    String json;
    json += "{\"type\":";
    json += DISTANCE;
    json += ",\"from\":";
    json += _device->address;
    json += ",\"to\":";
    json += _node.address;
    json += ",\"range\":";
    json += _node.distance;
    json += "}";
    return json;
}

void DW1000PositioningClass::serialSendPositition(struct _Node _node){
    Serial.print("Dev:");
    Serial.print(_node.address);
    Serial.print("[");
    Serial.print(_node.position.x);
    Serial.print(",");
    Serial.print(_node.position.y);
    Serial.print(",");
    Serial.print(_node.position.z);
    Serial.println("]");
}

void DW1000PositioningClass::serialSendDistance(struct _Node _node){
    Serial.println(createJsonDistance(_node));
}

void DW1000PositioningClass::serialSendPosititions(){
    for (uint8_t i = 0; i < _NUM_DEVICES; i++){
        serialSendPositition(_devices[i]);
    }
}

void DW1000PositioningClass::serialSendDistances(){
    for (uint8_t i = 0; i < _NUM_DEVICES; i++){
        //Prevent sending own distance (0m) over network
        if(_device->address != _devices[i].address){
           serialSendDistance(_devices[i]);
        }
    }
}

void DW1000PositioningClass::serialDrawDistances(){
    
    Serial.println("Y"                );
    Serial.println("▲"                );
    Serial.println("3"                );
    Serial.println("| \\"             );
    Serial.println("|  \\ \\"         );
    Serial.println("|   \\  \\"       );
    Serial.println("|    \\   \\"     );
    Serial.println("|     \\    \\"   );
    Serial.print(  "|    "            );  Serial.print(_devices[3].distance);                 Serial.println(   "   \\"                    );
    Serial.print(  "|       \\     "  );                                                      Serial.println(      " \\"                   );
    Serial.print(  "|        \\     " );  Serial.println(_devices[2].distances[3].distance);
                                          Serial.print(_devices[1].distances[3].distance);
    Serial.print(      "      0  "    );                                                      Serial.println(      "      \\"              );
    Serial.print(  "|       / "       );                                                      Serial.println("     \\      \\"             );
    Serial.print(  "|   "             );  Serial.print(_devices[1].distance);
    Serial.print(     "        "      );  Serial.print(_devices[2].distance);                 Serial.println(            "   \\"           );
    Serial.print(  "|  /        "      );                                                     Serial.println( "           \\  \\"          );
    Serial.print(  "1 ------ "        );  Serial.print(_devices[1].distances[2].distance);    Serial.println(       " ------------ 2►X"  );

    /*
     
     Y
     |
     3
     | \
     |  \ \
     |   \  \
     |    \   \
     |     \    \
     |    51.00   \
     |       \      \
     |        \     17.00
     13.00      0        \
     |       /      \      \
     |   8.00        90.00   \
     |  /                   \  \ 
     1 ------ 53.00 ------------ 2 -> X
     
    // Alternative
     
    Serial.print(  "1 ------ "      );  Serial.print(_devices[1].distances[2].distance);    Serial.println(       " ------ 2");
    Serial.print(  "| \\    "       );                                                      Serial.println( "           //"  );
    Serial.print(  "|   "           );  Serial.print(_devices[1].distance);
    Serial.print(     "      "      );  Serial.print(_devices[2].distance);                 Serial.println(           " /"   );
    Serial.print(  "|       \\"     );                                                      Serial.println(  "    /    /"    );
                                        Serial.print(_devices[1].distances[3].distance);
    Serial.print(      "      0  "  );                                                      Serial.println(     "    /"      );
    Serial.print(  "|        /     ");  Serial.println(_devices[2].distances[3].distance);
    Serial.print(  "|       /     " );                                                      Serial.println(      " /"         );
    Serial.print(  "|     "         );  Serial.print(_devices[3].distance);                 Serial.println(   "   /"          );
    Serial.println("|     /    /"   );
    Serial.println("|    /   /"     );
    Serial.println("|   /  /"       );
    Serial.println("|  / /"         );
    Serial.println("| //"           );
    Serial.println("3");
     
     1 ------ 53.00 ------ 2
     | \                 //
     |   8.00      90.00 /
     |       \    /     /
     13.00      0      /
     |        /     17.00
     |       /      /
     |     51.00  /
     |     /    /
     |    /   /
     |   /  /
     |  / /
     | //
     3

     */
    
}

/*
    Setters and getters
*/

struct _Node DW1000PositioningClass::getNextDevice(){
    _nextDevice == 2 ? _nextDevice = 0 : _nextDevice++;
    return _devices[_nextDevice];
}

struct _Node* DW1000PositioningClass::getDevice(){
    return _device;
}

_STATES DW1000PositioningClass::getState(){
    return _state;
}

void DW1000PositioningClass::setState(_STATES state){
    _state = state;
}
 
