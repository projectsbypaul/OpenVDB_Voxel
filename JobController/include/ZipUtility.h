#pragma once
#include <filesystem>
namespace fs = std::filesystem;


namespace ZIPutil {

	namespace Functions {
		int zip_create_archive(fs::path new_zip_name);
		int zip_write_subfolder(const fs::path& zip_path, const fs::path& folder_to_zip, const std::string& zip_prefix = "");
		int zip_extract_subfolder(const fs::path& zip_path, const fs::path& output_dir, const std::string& subfolder);
	}//Functions

	namespace Macros {

	}//Macros

	namespace Tests {

	}//Macros

}//HDF5util