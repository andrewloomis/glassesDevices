#ifndef USERTYPES_H
#define USERTYPES_H

#include <vector>
#include <ctime>

enum class Direction
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Gesture
{
    Direction dir = Direction::NONE;
    unsigned int timestamp = 0;

    Gesture(Direction dir, unsigned int timestamp)
        : dir(dir), timestamp(timestamp) {}
};

struct GestureBuffer
{
    std::vector<Gesture> buffer;

    GestureBuffer()
    {
        buffer.reserve(4);
    }

    void add(Direction dir)
    {
        if (buffer.size() > 3) buffer.pop_back();
        buffer.emplace(buffer.begin(), dir, clock());
    }
    Gesture getLatest() const
    {
        return buffer[0];
    }
};

#endif