#pragma once
//cpp lib includes
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <memory>
#include <thread> 
#include <sstream>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
//project includes 

namespace Tools {

    namespace DatasetStats {

        namespace Functions {

            size_t read_segment_count(const std::string& filepath);

            std::map<size_t, std::vector<std::string>> read_face_type_map(const std::string& filepath);

            std::map<std::string, std::vector<size_t>> build_type_to_faces_map(const std::map<size_t, std::vector<std::string>>& face_type_map);

        }
        namespace Macros {

        }
    }

}//namspace Tools