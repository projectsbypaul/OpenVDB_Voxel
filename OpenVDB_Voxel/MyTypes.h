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
        float n;
        float m;

        float mapping(float val){
            return this->m * val + this->n;
        }

        void create(float min, float max, float min_map, float max_map) {
            this->m = (max_map - min_map) / (max - min);
            this->n = max_map - this->m * max;
        }
            
    };

    struct ABC_Surface {
        std::string type;
        std::vector<int> face_indices;
        std::vector<int> vert_indices;
    };

    enum WaveType {
        SINE_WAVE,
        TRIANGLE_WAVE
    };
};
#endif 
