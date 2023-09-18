#pragma once

#include <string>

enum EventType {
    UPDATE,
    START_RECORDING,
    STOP_RECORDING
};

struct Event {
    EventType type;
    void* data;
};

struct UpdateEvent {
    int nodeId;
    std::string parameter;
    float value;
    UpdateEvent(int i, std::string p, float v)
        : nodeId(i)
        , parameter(p)
        , value(v) {}
};

struct UpdateStringEvent {
    int nodeId;
    std::string parameter;
    std::string value;
    UpdateStringEvent(int i, std::string p, std::string v)
        : nodeId(i)
        , parameter(p)
        , value(v) {}
};



struct LinkEvent {
    int from;
    int to;
};

