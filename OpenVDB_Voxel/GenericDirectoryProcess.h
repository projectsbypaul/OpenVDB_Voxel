#ifndef GENERICDIRECTORYPROCESS_H
#define GENERICDIRECTORYPROCESS_H

#include <string>
#include <filesystem>
namespace fs = std::filesystem;

namespace ProcessingUtility {
    /**
     * @brief An abstract base class for processing directories.
     *
     * This class defines the basic structure for directory processing tasks,
     * providing source and target directory paths and a virtual run method.
     */
    class GenericDirectoryProcess {
    protected:
        fs::path sourceDir_;   ///< The source directory path.
        fs::path targetDir_;   ///< The target directory path.

    public:
        /**
         * @brief Constructs a GenericDirectoryProcess object.
         *
         * @param sourceDir The path to the source directory.
         * @param targetDir The path to the target directory.
         */
        GenericDirectoryProcess(const fs::path& sourceDir, const  fs::path& targetDir);

        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         */
        virtual ~GenericDirectoryProcess() = default;

        /**
         * @brief Executes the directory processing logic.
         *
         * @param subDirPath Optional subdirectory path to process.
         * @note This is a pure virtual function, so derived classes must implement it.
         */
        virtual void run(const std::string& subDirName = "") = 0;

        /**
         * @brief Gets the source directory path.
         *
         * @return The source directory path as a constant string reference.
         */
        const fs::path& getSourceDir() const;

        /**
         * @brief Gets the target directory path.
         *
         * @return The target directory path as a constant string reference.
         */
        const fs::path& getTargetDir() const;
    };
}

#endif // GENERICDIRECTORYPROCESS_H