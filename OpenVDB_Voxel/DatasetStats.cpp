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
#include "DatasetStats.h"


namespace Tools {

	namespace DatasetStats {

		namespace Functions {

            size_t read_segment_count(const std::string& filepath) {
                std::ifstream in(filepath);
                if (!in.is_open()) {
                    std::cerr << "Error: Could not open file for reading: " << filepath << std::endl;
                    return 0;
                }

                std::string line;
                size_t segment_count = 0;

                while (std::getline(in, line)) {
                    // Remove leading/trailing whitespace
                    line.erase(0, line.find_first_not_of(" \t"));
                    line.erase(line.find_last_not_of(" \t") + 1);

                    if (line.empty() || line[0] == '#') continue;

                    if (line.find("segment_container_count:") == 0) {
                        std::istringstream iss(line.substr(line.find(":") + 1));
                        iss >> segment_count;
                        break;  // We found what we need, stop reading
                    }
                }

                in.close();
                return segment_count;
            }

            std::map<size_t, std::vector<std::string>> read_face_type_map(const std::string& filepath) {
                std::ifstream in(filepath);
                if (!in.is_open()) {
                    std::cerr << "Error: Could not open file for reading: " << filepath << std::endl;
                    return {};
                }

                std::string line;
                std::map<size_t, std::vector<std::string>> face_type_map;
                bool in_face_type_map_section = false;

                while (std::getline(in, line)) {
                    // Remove leading/trailing whitespace
                    line.erase(0, line.find_first_not_of(" \t"));
                    line.erase(line.find_last_not_of(" \t") + 1);

                    if (line.empty() || line[0] == '#') continue;

                    if (line == "[FACE_TYPE_MAP]") {
                        in_face_type_map_section = true;
                        continue;
                    }
                    if (line == "[END_FACE_TYPE_MAP]") {
                        in_face_type_map_section = false;
                        break;  // We finished reading the section, stop reading
                    }

                    if (in_face_type_map_section) {
                        auto colon_pos = line.find(":");
                        if (colon_pos == std::string::npos) continue;

                        size_t index;
                        std::istringstream iss_index(line.substr(0, colon_pos));
                        iss_index >> index;

                        std::string types_str = line.substr(colon_pos + 1);
                        types_str.erase(0, types_str.find_first_not_of(" \t"));

                        std::vector<std::string> types;
                        std::istringstream iss_types(types_str);
                        std::string type;
                        while (std::getline(iss_types, type, ',')) {
                            type.erase(0, type.find_first_not_of(" \t"));
                            type.erase(type.find_last_not_of(" \t") + 1);
                            if (!type.empty()) {
                                types.push_back(type);
                            }
                        }

                        face_type_map[index] = types;
                    }
                }

                in.close();
                return face_type_map;
            }

            std::map<std::string, std::vector<size_t>> build_type_to_faces_map(
                const std::map<size_t, std::vector<std::string>>& face_type_map
            ) {
                std::map<std::string, std::vector<size_t>> type_to_faces;

                for (const auto& face_entry : face_type_map) {
                    size_t face_index = face_entry.first;
                    const auto& types = face_entry.second;

                    for (const auto& type : types) {
                        type_to_faces[type].push_back(face_index);
                    }
                }

                return type_to_faces;
            }
		}
		namespace Macros {

		}
	}

}//namspace Tools