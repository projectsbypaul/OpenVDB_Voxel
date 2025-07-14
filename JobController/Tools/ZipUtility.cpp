#include "ZipUtility.h"

#include <zip.h>

#include <iostream>
#include <fstream>
#include <sstream> // For std::ostringstream
#include <filesystem>

namespace fs = std::filesystem;

namespace ZIPutil {
	namespace Functions {

        int zip_create_archive(fs::path new_zip_name) {
            
                std::cout << "Creating empty archive: " << new_zip_name << std::endl;

                if(fs::exists(new_zip_name)){
                    std::cerr << "[INFO] Archive: " << new_zip_name.filename().generic_string() << "already exists!" << std::endl;
                    return -1;
                }

                int error;
                zip_t* zip = zip_open(new_zip_name.generic_string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
                if (!zip) {
                    zip_error_t ziperror;
                    zip_error_init_with_code(&ziperror, error);
                    fprintf(stderr, "Failed to create zip: %s\n", zip_error_strerror(&ziperror));
                    zip_error_fini(&ziperror);
                    return 1;
                }

                // Add dummy directory entry to ensure archive is written
                if (zip_dir_add(zip, "temp/", ZIP_FL_ENC_UTF_8) < 0) {
                    std::cerr << "Failed to add dummy directory to zip: " << zip_strerror(zip) << std::endl;
                    zip_discard(zip);
                    return 2;
                }

                if (zip_close(zip) < 0) {
                    fprintf(stderr, "Failed to finalize zip: %s\n", zip_strerror(zip));
                    return 2;
                }

                std::cout << "Created zip with empty directory:" << new_zip_name << std::endl;
           
            return 0; 
        }

        int zip_write_subfolder(const fs::path& zip_path, const fs::path& folder_to_zip, const std::string& zip_prefix) {
            // Add all files under "data/subdir" into "archive.zip", stored under "images/"
            //zip_write_subfolder("archive.zip", "data/subdir", "images")
            // Just add the folder's contents at root of zip (no prefix)
            //zip_write_subfolder("archive.zip", "data/subdir");
            int error;
            zip_t* zip = zip_open(zip_path.string().c_str(), ZIP_CREATE , &error);
            if (!zip) {
                zip_error_t ziperror;
                zip_error_init_with_code(&ziperror, error);
                std::cerr << "Failed to create zip: " << zip_error_strerror(&ziperror) << std::endl;
                zip_error_fini(&ziperror);
                return 1;
            }

            for (const auto& entry : fs::recursive_directory_iterator(folder_to_zip)) {
                if (!fs::is_regular_file(entry)) continue;

                fs::path rel_path = fs::relative(entry.path(), folder_to_zip);
                std::string zip_entry_name = (zip_prefix.empty() ? "" : zip_prefix + "/") + rel_path.generic_string();

                zip_source_t* source = zip_source_file(zip, entry.path().string().c_str(), 0, 0);
                if (!source) {
                    std::cerr << "Failed to create zip source for " << entry.path() << ": " << zip_strerror(zip) << std::endl;
                    zip_discard(zip);
                    return 2;
                }

                zip_int64_t idx = zip_file_add(zip, zip_entry_name.c_str(), source, ZIP_FL_ENC_UTF_8);
                if (idx < 0) {
                    std::cerr << "Failed to add file to zip: " << zip_entry_name << ": " << zip_strerror(zip) << std::endl;
                    zip_source_free(source);
                    zip_discard(zip);
                    return 3;
                }

                // Store without compression (ZIP_CM_STORE), can change to ZIP_CM_DEFLATE for compression
                if (zip_set_file_compression(zip, idx, ZIP_CM_STORE, 0) != 0) {
                    std::cerr << "Failed to set compression for file: " << zip_entry_name << ": " << zip_strerror(zip) << std::endl;
                    zip_discard(zip);
                    return 4;
                }

                std::cout << "Added " << zip_entry_name << std::endl;
            }

            if (zip_close(zip) < 0) {
                std::cerr << "Failed to close zip: " << zip_strerror(zip) << std::endl;
                zip_discard(zip);
                return 5;
            }

            return 0;
        }

        int zip_extract_subfolder(const fs::path& zip_path, const fs::path& output_dir, const std::string& subfolder) {
            int err = 0;
            zip_t* za = zip_open(zip_path.string().c_str(), ZIP_RDONLY, &err);
            if (!za) {
                zip_error_t ze;
                zip_error_init_with_code(&ze, err);
                std::cerr << "Cannot open archive: " << zip_error_strerror(&ze) << std::endl;
                zip_error_fini(&ze);
                return 1;
            }

            zip_int64_t num_entries = zip_get_num_entries(za, 0);
            std::string prefix = subfolder;
            if (prefix.back() != '/')
                prefix += '/';

            for (zip_uint64_t i = 0; i < num_entries; ++i) {
                const char* name = zip_get_name(za, i, ZIP_FL_ENC_GUESS);
                if (!name) continue;

                std::string entry_name(name);
                if (entry_name.rfind(prefix, 0) != 0) continue;  // does not start with subfolder

                // Determine output path
                fs::path rel_path = fs::path(entry_name).lexically_relative(prefix);
                fs::path out_path = output_dir / rel_path;

                // If entry is a directory
                if (entry_name.back() == '/') {
                    fs::create_directories(out_path);
                    continue;
                }

                // Ensure parent directories exist
                fs::create_directories(out_path.parent_path());

                zip_file_t* zf = zip_fopen_index(za, i, 0);
                if (!zf) {
                    std::cerr << "Failed to open zip entry: " << entry_name << std::endl;
                    continue;
                }

                std::ofstream out(out_path, std::ios::binary);
                if (!out) {
                    std::cerr << "Failed to create file: " << out_path << std::endl;
                    zip_fclose(zf);
                    continue;
                }

                char buffer[4096];
                zip_int64_t bytes_read;
                while ((bytes_read = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
                    out.write(buffer, bytes_read);
                }

                if (bytes_read < 0)
                    std::cerr << "Error reading from archive: " << entry_name << std::endl;

                zip_fclose(zf);
            }

            zip_close(za);
            return 0;
        }

        std::vector<std::string> read_job_file(fs::path job_file_path) {

            std::vector<std::string> jobs;

            std::string line;
            std::ifstream job_file(job_file_path);
            while (std::getline(job_file, line)) {
                jobs.push_back(line);
            }

            return jobs;
        }

        std::vector<fs::path> get_job_zip_target_paths(fs::path source_dir, std::vector<std::string> jobs) {
            
            std::vector<fs::path> zip_targets;
            
            for (const auto& job : jobs) {

               

                fs::path subdir_path = source_dir / job;

                if (fs::exists(subdir_path) && fs::is_directory(subdir_path)) {
                    for (const auto& entry : fs::directory_iterator(subdir_path)) {
                        zip_targets.push_back(entry);
                    }
                }
            }

            return zip_targets;
        }

        int zip_path_list(fs::path zip_location, std::string zip_file_name, fs::path zip_source_dir ,std::vector<fs::path> zip_targets, int mode = 0) {
            
            // Ensure zip_location exists 
            if (!fs::exists(zip_location)) {
                try {
                    fs::create_directories(zip_location);
                }
                catch (const fs::filesystem_error& e) {
                    std::cerr << "Failed to create zip location directory: " << e.what() << std::endl;
                    return 1;
                }
            }


          
            fs::path zip_path = zip_location / zip_file_name;

            int error;
            zip_t* zip = zip_open(zip_path.generic_string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
            if (!zip) {
                zip_error_t ziperror;
                zip_error_init_with_code(&ziperror, error);
                fprintf(stderr, "Failed to create zip: %s\n", zip_error_strerror(&ziperror));
                zip_error_fini(&ziperror);
                return 2;
            }

            for (const auto& target : zip_targets) {
                if (!fs::exists(target)) {
                    std::cerr << "Target does not exist: " << target << std::endl;
                    continue;  // skip this file
                }

                fs::path rel_path;
                try {
                    rel_path = fs::relative(target, zip_source_dir);
                }
                catch (const fs::filesystem_error& e) {
                    std::cerr << "Failed to compute relative path: " << e.what() << std::endl;
                    continue;
                }

                // Create a zip source from the file
                zip_source_t* source = zip_source_file(zip, target.generic_string().c_str(), 0, 0);
                if (!source) {
                    std::cerr << "Failed to create zip source for " << target << ": " << zip_strerror(zip) << std::endl;
                    zip_discard(zip);
                    return 3;
                }

                // Add file
                zip_int64_t idx = zip_file_add(zip, rel_path.generic_string().c_str(), source, ZIP_FL_ENC_UTF_8);
                if (idx < 0) {
                    std::cerr << "Failed to add file to zip: " << rel_path << ": " << zip_strerror(zip) << std::endl;
                    zip_source_free(source);
                    zip_discard(zip);
                    return 4;
                }
                else
                {
                    std::cout << "Added " << rel_path.generic_string() << " to " << zip_file_name << std::endl;
                }

                //No Compression ZIP_CM_STORE mode == 0
                //Compression ZIP_CM_DEFLATE mode == 1
                zip_int32_t compression_method = ZIP_CM_STORE;
                if (mode == 1) {
                    compression_method = ZIP_CM_DEFLATE;
                }

                // Set Compression Method
                if (zip_set_file_compression(zip, idx, compression_method, 0) != 0) {
                    std::cerr << "Failed to set compression for file: " << rel_path << ": " << zip_strerror(zip) << std::endl;
                    zip_discard(zip);
                    return 6;
                }
            }

            // Close the zip archive
            if (zip_close(zip) < 0) {
                std::cerr << "Failed to close zip: " << zip_strerror(zip) << std::endl;
                zip_discard(zip);
                return 5;
            }

            return 0;  // success
        }

        int unzip_file_to_target(const fs::path& zip_path, const fs::path& output_dir) {
            int err = 0;
            zip_t* za = zip_open(zip_path.generic_string().c_str(), ZIP_RDONLY, &err);
            if (!za) {
                zip_error_t ziperror;
                zip_error_init_with_code(&ziperror, err);
                std::cerr << "Failed to open zip archive: " << zip_error_strerror(&ziperror) << std::endl;
                zip_error_fini(&ziperror);
                return 1;
            }

            zip_int64_t num_entries = zip_get_num_entries(za, 0);
            for (zip_uint64_t i = 0; i < num_entries; ++i) {
                const char* name = zip_get_name(za, i, ZIP_FL_ENC_GUESS);
                if (!name) {
                    std::cerr << "Failed to get file name for entry " << i << ": " << zip_strerror(za) << std::endl;
                    continue;
                }

                fs::path out_path = output_dir / fs::path(name);

                // Check if this is a directory
                if (name[strlen(name) - 1] == '/') {
                    fs::create_directories(out_path);
                    continue;
                }

                // Make sure parent directory exists
                fs::create_directories(out_path.parent_path());

                zip_file_t* zf = zip_fopen_index(za, i, 0);
                if (!zf) {
                    std::cerr << "Failed to open file in archive: " << name << ": " << zip_strerror(za) << std::endl;
                    continue;
                }

                std::ofstream out_file(out_path, std::ios::binary);
                if (!out_file) {
                    std::cerr << "Failed to create output file: " << out_path << std::endl;
                    zip_fclose(zf);
                    continue;
                }

                // Read and write data
                char buffer[4096];
                zip_int64_t bytes_read;
                while ((bytes_read = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
                    out_file.write(buffer, bytes_read);
                }

                if (bytes_read < 0) {
                    std::cerr << "Error reading file from archive: " << name << std::endl;
                }

                zip_fclose(zf);
            }

            zip_close(za);
            return 0;
        }



    }//Functions

    namespace Macros {

        int unzip_dir_to_target(fs::path source_dir, fs::path output_dir) {

            for (const auto& zip_file : fs::directory_iterator(source_dir)) {

                Functions::unzip_file_to_target(zip_file, output_dir);
            }
            return 0;
        }

        int zip_source_by_job_dir(fs::path source_dir, fs::path zip_location, fs::path job_dir) {
            
            for (const fs::path& job_file : fs::directory_iterator(job_dir)) {

                fs::path archive_name = job_file.filename().replace_extension(".zip");

                std::vector<std::string> jobs = Functions::read_job_file(job_file);

                std::vector<fs::path> targets = Functions::get_job_zip_target_paths(source_dir, jobs);

                if (!fs::is_regular_file(job_file)) {
                    std::cout << "job_file path is not a file" << std::endl;
                    return 1;
                }

                Functions::zip_path_list(zip_location, archive_name.generic_string(), source_dir, targets);

            }

            return 0; 
        }

        int zip_source_by_job_file(fs::path source_dir, fs::path zip_location, fs::path job_file) {
            
            std::vector<std::string> jobs = Functions::read_job_file(job_file);

            std::vector<fs::path> targets = Functions::get_job_zip_target_paths(source_dir, jobs);

            if (!fs::is_regular_file(job_file)) {
                std::cout << "job_file path is not a file" << std::endl;
                return 1;
            }

            fs::path archive_name = job_file.filename().replace_extension(".zip");

            Functions::zip_path_list(zip_location, archive_name.generic_string(), source_dir, targets);

            return 0; 
        }

    }//Macros

	namespace Tests {
        int test_for_Jobcontroller(fs::path source_zip, fs::path output_zip, std::string subdir_name) {
            
            std::cout << "Creating Archive" << std::endl;
            Functions::zip_create_archive(output_zip);
            fs::path out_dir = output_zip.parent_path();
            fs::path temp_dir = out_dir / "temp";

            std::cout << "unpacking to temp" << std::endl;
            Functions::zip_extract_subfolder(source_zip, temp_dir / subdir_name, subdir_name);

            std::cout << "writing subdir to archive" << std::endl;
            Functions::zip_write_subfolder(output_zip, temp_dir / subdir_name, subdir_name);

            std::cout << "Test for Jobcontroller completed" << std::endl;

            return 0;
        }

		int test_create(fs::path target) {
            std::cout << "Running zip test" << std::endl;
            int error;
            zip_t* zip = zip_open(target.generic_string().c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
            if (!zip) {
                zip_error_t ziperror;
                zip_error_init_with_code(&ziperror, error);
                fprintf(stderr, "Failed to create zip: %s\n", zip_error_strerror(&ziperror));
                zip_error_fini(&ziperror);
                return 1;
            }

            // Add empty directory
            if (zip_dir_add(zip, "test_dir/", ZIP_FL_ENC_UTF_8) < 0) {
                fprintf(stderr, "Failed to add directory: %s\n", zip_strerror(zip));
                zip_discard(zip);
                return 2;
            }

            if (zip_close(zip) < 0) {
                fprintf(stderr, "Failed to finalize zip: %s\n", zip_strerror(zip));
                return 3;
            }

            printf("Created zip with empty directory: test_dir/\n");
            return 0;

		}
	}//Tests
}//HDF5util