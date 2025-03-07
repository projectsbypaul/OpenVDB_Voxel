#ifndef MYTYPES_H
#define MYTYPES_H

#include <vector> // Ensure <vector> is included
#include <iostream> // Optional, but not needed for std::vector

constexpr double TWO_PI = 6.28318530717958647692; // 2 PI

constexpr double M_PI= 3.14159265358979323846;

namespace Tools {

    using Float3DArray = std::vector<std::vector<std::vector<float>>>;

    struct MyVertex {
        float x, y, z;
    };

    struct MyFace {
        int v0, v1, v2;
    };

    struct LinearSDFMap {
        float minVal;
        float maxVal; 
        float background;

        float mapping(float val){
            return (minVal - maxVal) / (-2 * background) * val + maxVal + (minVal - maxVal) / 2;
        }

        void create(float min, float max, float bg) {
            this->minVal = min;
            this->maxVal = max;
            this->background = bg;
        }
    };

    enum WaveType {
        SINE_WAVE,
        TRIANGLE_WAVE
    };
};
#endif 
