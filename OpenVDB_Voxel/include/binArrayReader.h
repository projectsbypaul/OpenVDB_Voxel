#pragma once
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

#include "MyTypes.h"

namespace cppIO {

    struct SegmentMeta {
        int dimX, dimY, dimZ;
    };

    enum ElementType : uint8_t {
        FLOAT32 = 0,
        FLOAT64 = 1,
        UINT8 = 2, // You can add this for uint8 predictions
    };

	namespace Functions {
		Tools::Float3DArray to3D(const std::vector<float>& flat, int X, int Y, int Z);

        // Read segments as float32 or uint8. T must be float or uint8_t.
        template <typename T>
        std::vector<std::vector<T>> read_segments_from_binary(const std::string& filename) {
            std::vector<std::vector<T>> segments;
            std::ifstream bin(filename, std::ios::binary);
            if (!bin) {
                std::cerr << "Failed to open: " << filename << std::endl;
                return segments;
            }

            // 1. Header
            uint32_t magic;
            uint16_t version;
            uint8_t element_type_id;
            uint32_t num_segments;

            bin.read(reinterpret_cast<char*>(&magic), 4);
            bin.read(reinterpret_cast<char*>(&version), 2);
            bin.read(reinterpret_cast<char*>(&element_type_id), 1);
            bin.read(reinterpret_cast<char*>(&num_segments), 4);

            if (magic != 0x5345474D) {
                std::cerr << "Wrong magic: " << std::hex << magic << std::endl;
                return segments;
            }

            // 2. Segment descriptors
            std::vector<SegmentMeta> metas(num_segments);
            for (uint32_t i = 0; i < num_segments; ++i) {
                bin.read(reinterpret_cast<char*>(&metas[i].dimX), 4);
                bin.read(reinterpret_cast<char*>(&metas[i].dimY), 4);
                bin.read(reinterpret_cast<char*>(&metas[i].dimZ), 4);
            }

            // 3. Segment data
            for (uint32_t i = 0; i < num_segments; ++i) {
                size_t count = static_cast<size_t>(metas[i].dimX) * metas[i].dimY * metas[i].dimZ;
                std::vector<T> data(count);
                bin.read(reinterpret_cast<char*>(data.data()), count * sizeof(T));
                segments.push_back(std::move(data));
            }
            return segments;
        }
	}
}
