#include "../include/GenericDirectoryProcess.h"

namespace ProcessingUtility {
    /**
     * @brief Constructs a GenericDirectoryProcess object.
     *
     * @param sourceDir The path to the source directory.
     * @param targetDir The path to the target directory.
     */
    GenericDirectoryProcess::GenericDirectoryProcess(const fs::path& sourceDir, const fs::path& targetDir)
        : sourceDir_(sourceDir), targetDir_(targetDir) {
    }

    /**
     * @brief Gets the source directory path.
     *
     * @return The source directory path as a constant string reference.
     */
    const fs::path& GenericDirectoryProcess::getSourceDir() const {
        return sourceDir_;
    }

    /**
     * @brief Gets the target directory path.
     *
     * @return The target directory path as a constant string reference.
     */
    const fs::path& GenericDirectoryProcess::getTargetDir() const {
        return targetDir_;
    }
}