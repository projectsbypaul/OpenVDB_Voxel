#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include <CGAL/Bbox_3.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

#include "MeshDataHandling.h"
#include "Tools.h"
#include "DatasetToVoxel.h"

namespace fs = std::filesystem;

// Function to process and save the modified file with a new extension
void processFile(const fs::path& srcFilePath, const fs::path& destDir, const std::string& newExtension);

// Function to copy and process files while maintaining structure
void processDirectory(const fs::path& srcDir, const fs::path& destDir, const std::string& newExtension);

// Main function to parse dataset
void ParseDataset(fs::path& srcDir, fs::path& destDir, std::string& newExtension);
