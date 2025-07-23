#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

#include "../include/MyTypes.h"

namespace cppIO {

	namespace Functions {

        Tools::Float3DArray to3D(const std::vector<float>& flat, int X, int Y, int Z) {
            Tools::Float3DArray arr(X, std::vector<std::vector<float>>(Y, std::vector<float>(Z)));
            for (int x = 0; x < X; ++x)
                for (int y = 0; y < Y; ++y)
                    for (int z = 0; z < Z; ++z)
                        arr[x][y][z] = flat[x * Y * Z + y * Z + z];
            return arr;
        }
	}

}//cppIO