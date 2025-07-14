#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include "MyTypes.h"
#include <string>
#include <filesystem>
#include <unordered_map>  // For std::unordered_map
namespace fs = std::filesystem;

namespace cppIOUtility {
    /**
     * @brief An abstract base class for DataContainers to collect outputs before saving them
     *
     * This class defines the basic structure for directory processing tasks,
     * providing source and target directory paths and a virtual run method.
     */
    class GenericDataContainer {

    public:

        GenericDataContainer();
        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~GenericDataContainer() = default;
        /**
         * @brief Executes the Container dumping processing logic.
         *
         * @param DumpDir Location for dumping container contents.
         * @note This is a pure virtual function, so derived classes must implement it.
         */
        virtual void dump(const fs::path& DumpDir = "") = 0;
    };

    /**
     * @brief An abstract base class for DataContainers to collect outputs before saving them
     *
     * This class defines the basic structure for directory processing tasks,
     * providing source and target directory paths and a virtual run method.
     */
    class SegmentationDataContainer : public GenericDataContainer {
    private:
        //Grid Parameters
        double background_;
        double voxel_size_;
        double minVal_;
        //containers
        std::vector<Tools::Float3DArray> segment_container_;
        Tools::FloatMatrix origin_container_;
        Tools::FloatMatrix FaceToGridIndex_container_;
        //Tables
        Tools::MappingTable FaceTypeMap_;
        std::unordered_map<std::string, int> TypeCount_;
        Tools::MappingTable VertTypeMap_;

    public:
        #pragma region GettersSetter
        // --- Getters and Setters ---
        double getBackground() const {
            return background_;
        }
        void setBackground(double background) {
            background_ = background;
        }

        // For voxel_size_
        double getVoxelSize() const {
            return voxel_size_;
        }
        void setVoxelSize(double voxelSize) {
            voxel_size_ = voxelSize;
        }
        // For minVal_
        double getMinVal() const {
            return minVal_;
        }
        void setMinVal(double minVal) {
            minVal_ = minVal;
        }
        // For segment_container_
        // Getter returning a const reference to avoid copying
        const std::vector<Tools::Float3DArray>& getSegmentContainer() const {
            return segment_container_;
        }
        void setSegmentContainer(const std::vector<Tools::Float3DArray>& segmentContainer) {
            segment_container_ = segmentContainer;
        }
        // Or, provide methods to modify the container, e.g., addSegment
        void addSegment(const Tools::Float3DArray& segment) {
            segment_container_.push_back(segment);
        }
        // For origin_container_
        const Tools::FloatMatrix& getOriginContainer() const {
            return origin_container_;
        }
        void setOriginContainer(const Tools::FloatMatrix& originContainer) {
            origin_container_ = originContainer;
        }
        // For FaceTypeMap_
        const Tools::MappingTable& getFaceTypeMap() const {
            return FaceTypeMap_;
        }
        void setFaceTypeMap(const Tools::MappingTable& faceTypeMap) {
            FaceTypeMap_ = faceTypeMap;
        }
        // For TypeCount_
        const std::unordered_map<std::string, int>& getTypeCount() const {
            return TypeCount_;
        }
        void setTypeCount(const std::unordered_map<std::string, int>& typeCount) {
            TypeCount_ = typeCount;
        }
        // For VertTypeMap_
        const Tools::MappingTable& getVertTypeMap() const {
            return VertTypeMap_;
        }
        void setVertTypeMap(const Tools::MappingTable& vertTypeMap) {
            VertTypeMap_ = vertTypeMap;
        }
        //For FaceToGridIndex_
            const Tools::FloatMatrix & getFaceToGridIndex_container() const {
                return FaceToGridIndex_container_;
        }
        void setFaceToGridIndex_container(const Tools::FloatMatrix& FaceToGridIndex_container) {
            FaceToGridIndex_container_ = FaceToGridIndex_container;
        }

        #pragma endregion.

        SegmentationDataContainer();

        void dump(const fs::path& DumpDir = "") override;
    };
}

#endif // GENERICDIRECTORYPROCESS_H