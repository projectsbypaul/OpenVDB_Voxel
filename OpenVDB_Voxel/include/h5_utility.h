#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <type_traits>

#include <H5Cpp.h>        // HDF5 C++ API
#include <H5Cpublic.h>    // for hsize_t
namespace fs = std::filesystem;

namespace H5util {

    template<typename T>
    void write_ndarray_to_h5(const std::vector<T>& data,
        const std::vector<hsize_t>& dims,
        const std::string& dataset_name,
        const fs::path& out_file)
    {
        if (data.empty()) {
            throw std::runtime_error("Empty data, nothing to export.");
        }

        // Open or create file
        H5::H5File file(out_file.string(), H5F_ACC_TRUNC);

        // Dataspace with given dimensions
        H5::DataSpace dataspace(dims.size(), dims.data());

        // Map C++ type to HDF5 type
        const H5::PredType & dtype =
            std::is_same_v<T, int> ? H5::PredType::NATIVE_INT :
            std::is_same_v<T, float> ? H5::PredType::NATIVE_FLOAT :
            std::is_same_v<T, double> ? H5::PredType::NATIVE_DOUBLE :
            std::is_same_v<T, unsigned char> ? H5::PredType::NATIVE_UCHAR :
            std::is_same_v<T, long long> ? H5::PredType::NATIVE_LLONG :
            throw std::runtime_error("Unsupported dtype");

        // Create dataset
        H5::DataSet dataset = file.createDataSet(dataset_name, dtype, dataspace);

        // Write flattened data
        dataset.write(data.data(), dtype);
    }

    // 2D
    template <typename T>
    std::vector<T> flatten2D(const std::vector<std::vector<T>>& grid) {
        std::vector<T> flat;
        size_t x = grid.size();
        size_t y = x ? grid[0].size() : 0;

        flat.reserve(x * y);
        for (size_t i = 0; i < x; i++)
            for (size_t j = 0; j < y; j++)
                flat.push_back(grid[i][j]);
        return flat;
    }

    // 3D
    template <typename T>
    std::vector<T> flatten3D(const std::vector<std::vector<std::vector<T>>>& grid) {
        std::vector<T> flat;
        size_t x = grid.size();
        size_t y = x ? grid[0].size() : 0;
        size_t z = (y && x) ? grid[0][0].size() : 0;

        flat.reserve(x * y * z);
        for (size_t i = 0; i < x; i++)
            for (size_t j = 0; j < y; j++)
                for (size_t k = 0; k < z; k++)
                    flat.push_back(grid[i][j][k]);
        return flat;
    }

    // 4D
    template <typename T>
    std::vector<T> flatten4D(const std::vector<std::vector<std::vector<std::vector<T>>>>& grid) {
        std::vector<T> flat;
        size_t n = grid.size();
        size_t x = n ? grid[0].size() : 0;
        size_t y = (x && n) ? grid[0][0].size() : 0;
        size_t z = (y && x && n) ? grid[0][0][0].size() : 0;

        flat.reserve(n * x * y * z);
        for (size_t i = 0; i < n; i++)
            for (size_t j = 0; j < x; j++)
                for (size_t k = 0; k < y; k++)
                    for (size_t l = 0; l < z; l++)
                        flat.push_back(grid[i][j][k][l]);
        return flat;
    }

    // 2D
    template <typename T>
    std::vector<std::vector<T>> unflatten2D(const std::vector<T>& flat,
        size_t x, size_t y) {
        std::vector<std::vector<T>> grid(x, std::vector<T>(y));
        size_t idx = 0;
        for (size_t i = 0; i < x; i++)
            for (size_t j = 0; j < y; j++)
                grid[i][j] = flat[idx++];
        return grid;
    }

    // 3D
    template <typename T>
    std::vector<std::vector<std::vector<T>>> unflatten3D(const std::vector<T>& flat,
        size_t x, size_t y, size_t z) {
        std::vector<std::vector<std::vector<T>>> grid(
            x, std::vector<std::vector<T>>(y, std::vector<T>(z)));
        size_t idx = 0;
        for (size_t i = 0; i < x; i++)
            for (size_t j = 0; j < y; j++)
                for (size_t k = 0; k < z; k++)
                    grid[i][j][k] = flat[idx++];
        return grid;
    }

    // 4D
    template <typename T>
    std::vector<std::vector<std::vector<std::vector<T>>>> unflatten4D(const std::vector<T>& flat,
        size_t n, size_t x, size_t y, size_t z) {
        std::vector<std::vector<std::vector<std::vector<T>>>> grid(
            n, std::vector<std::vector<std::vector<T>>>(
                x, std::vector<std::vector<T>>(y, std::vector<T>(z))));
        size_t idx = 0;
        for (size_t i = 0; i < n; i++)
            for (size_t j = 0; j < x; j++)
                for (size_t k = 0; k < y; k++)
                    for (size_t l = 0; l < z; l++)
                        grid[i][j][k][l] = flat[idx++];
        return grid;
    }
}
