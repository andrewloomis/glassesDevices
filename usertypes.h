#ifndef USERTYPES_H
#define USERTYPES_H

#include <vector>
#include <ctime>

namespace Gesture {

    enum class Direction
    {
        NONE,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    struct Buffer
    {
        std::vector<Direction> buffer;

        Buffer()
        {
            buffer.reserve(4);
        }

        void add(Direction dir)
        {
            if (buffer.size() > 3) buffer.pop_back();
            buffer.emplace(buffer.begin(), dir);
        }
        Direction getLatest() const
        {
            return buffer[0];
        }
    };
}

#endif