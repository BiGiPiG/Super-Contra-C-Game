#pragma once

class MapObject {
protected:
    int x;
    int y;

public:
    MapObject(int startX, int startY) : x(startX), y(startY) {}
    virtual ~MapObject() {}

};