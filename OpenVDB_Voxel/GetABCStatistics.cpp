#include "GetABCStatistics.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>


namespace Scripts {


    namespace fs = std::filesystem;
    std::mutex csv_mutex;
    std::mutex slot_mutex;
    std::condition_variable slot_cv;
    unsigned active_threads = 0;

    void process_directory(const fs::path& entry, std::ofstream& csv, int k_size, int n_min_k, int padding, int bandwidth) {
        for (const auto& file_entry : fs::directory_iterator(entry)) {

            if (file_entry.is_regular_file()) {

                std::string dir_name = entry.filename().string();
                std::string file_name = file_entry.path().filename().string();
                std::string full_path = file_entry.path().string();

                std::vector<std::string> filter = { "vc", "vn" };

                Tools::util::filterObjFile(full_path, filter);

                std::cout << "Processing: " << full_path << std::endl;
                Surface_mesh mesh;
                MDH::readMesh(&full_path, &mesh);
                auto dimension = Tools::CGALbased::GetBBoxDimensions(mesh);
                auto rec_voxel_size = DLPP::CGALbased::calculateRecommendeVoxelsize(k_size, n_min_k, bandwidth, padding, mesh);

                {
                    std::lock_guard<std::mutex> lock(csv_mutex);
                    csv << dir_name << "," << rec_voxel_size << "," << dimension[0] << "," << dimension[1] << "," << dimension[2] << "\n";
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(slot_mutex);
            --active_threads;
        }
        slot_cv.notify_one();
    }

    int ProcessABCDir(std::string target_dir, std::string log_file, int& k_size, int& n_min_k, int& padding, int& bandwidth, unsigned max_threads ) {
        if (max_threads == 0) {
            max_threads = std::thread::hardware_concurrency();
            if (max_threads == 0) max_threads = 1;
        }

        bool file_exists = fs::exists(log_file);
        std::ofstream csv(log_file, std::ios::app);
        if (!csv.is_open()) {
            std::cerr << "Failed to open log file.\n";
            return 1;
        }

        if (!file_exists) {
            csv << "HEADER\n";
            csv << "filename,k_size,n_min_k,padding,bandwidth\n";
            csv << log_file << "," << k_size << "," << n_min_k << "," << padding << "," << bandwidth << "\n";
            csv << "BODY\n";
            csv << "dir_name,rec_voxelsize,dim_x,dim_y,dim_z\n";
        }

        std::vector<std::thread> threads;

        for (const auto& entry : fs::directory_iterator(target_dir)) {
            if (!entry.is_directory()) continue;

            {
                std::unique_lock<std::mutex> lock(slot_mutex);
                slot_cv.wait(lock, [&] { return active_threads < max_threads; });
                ++active_threads;
            }

            threads.emplace_back(process_directory, entry.path(), std::ref(csv), k_size, n_min_k, padding, bandwidth);
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        return 0;
    }

}