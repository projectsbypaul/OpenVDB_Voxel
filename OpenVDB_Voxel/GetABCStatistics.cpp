#include "GetABCStatistics.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>


namespace Scripts {

    std::tuple<double, std::vector<double>> analyzeMesh(Surface_mesh mesh, int&k_size, int& n_min_kernel, int& band_width, int& padding) {
        
        std::vector<double> dims = Tools::CGALbased::GetBBoxDimensions(mesh);
        double rec_voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, n_min_kernel, band_width, padding, mesh);
        
        return std::make_tuple(rec_voxel_size, dims);
    }


    std::vector<fs::path> GetFilesOfType(const fs::path& rootDir, const std::string& extension) {
        std::vector<fs::path> matchingFiles;

        if (!fs::exists(rootDir) || !fs::is_directory(rootDir)) {
            std::cerr << "Directory does not exist: " << rootDir << std::endl;
            return matchingFiles;
        }

        for (const auto& entry : fs::recursive_directory_iterator(rootDir)) {
            if (entry.is_regular_file() && entry.path().extension() == extension) {
                matchingFiles.push_back(entry.path());
            }
        }

        return matchingFiles;
    }

    int AnalyzeABCDir(fs::path rootDir, std::string extension, int& k_size, int& n_min_kernel, int& band_width, int& padding, unsigned int max_threads)
    {
        auto files = GetFilesOfType(rootDir, extension);

        std::atomic<int> success_counter{ 0 };
        std::mutex cout_mutex;

        const unsigned int thread_count = std::min(max_threads, std::thread::hardware_concurrency());
        std::vector<std::thread> threads;

        std::ofstream csv_log(rootDir.generic_string() + "/log.csv");
        std::mutex csv_mutex;

        // Write header
        csv_log << "file_name,voxel_size,dimx,dimy,dimz,time_ms,file_size\n";
        csv_log.close();

        auto worker = [&](size_t start, size_t end) {
            for (size_t i = start; i < end; ++i) {
                const auto& file = files[i];
                std::string path_string = file.generic_string();

                {
                    std::lock_guard<std::mutex> lock(cout_mutex);
                    std::cout << file << std::endl;
                }

                Surface_mesh mesh;

                auto start_time = std::chrono::steady_clock::now();

                bool success = MDH::readMesh(&path_string, &mesh);

                auto [rec_voxel_size, dimensions] = analyzeMesh(mesh, k_size, n_min_kernel, band_width, padding);

                auto end_time = std::chrono::steady_clock::now();
                auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

                if (success) {
                    int count = ++success_counter;

                    std::uintmax_t file_size = 0;

                    try {
                        file_size = std::filesystem::file_size(file);  // file is already an fs::path
                    }
                    catch (const std::filesystem::filesystem_error& e) {
                        std::lock_guard<std::mutex> lock(cout_mutex);
                        std::cerr << "Error getting file size: " << e.what() << "\n";
                    }

                    // Log to CSV
                    {
                        std::lock_guard<std::mutex> lock(csv_mutex);
                        std::ofstream csv_append(rootDir.generic_string() + "/log.csv", std::ios::app);
                        csv_append << file.filename().string() << ",";
                        csv_append << std::fixed << std::setprecision(6) << rec_voxel_size << ",";
                        csv_append << dimensions[0] << ",";
                        csv_append << dimensions[1] << ",";
                        csv_append << dimensions[2] << ",";
                      
                        csv_append << duration_ms << ",";
                        csv_append << file_size;
                        csv_append << "\n";
                        csv_append.close();
                    }

                    std::lock_guard<std::mutex> lock(cout_mutex);
                    std::cout << "Success count: " << count
                        << " | Time: " << duration_ms << " ms" << std::endl;
                }
                else {
                    std::lock_guard<std::mutex> lock(cout_mutex);
                    std::cout << "Failed to read | Time: " << duration_ms << " ms" << std::endl;
                }
            }
            };

        size_t chunk_size = (files.size() + thread_count - 1) / thread_count;

        for (size_t i = 0; i < thread_count; ++i) {
            size_t start = i * chunk_size;
            size_t end = std::min(start + chunk_size, files.size());
            if (start < end) {
                threads.emplace_back(worker, start, end);
            }
        }

        for (auto& t : threads) {
            t.join();
        }

        std::cout << "Final successful reads: " << success_counter.load() << std::endl;
        return success_counter;

    }
}