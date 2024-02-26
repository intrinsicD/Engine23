//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_FILEPATH_H
#define ENGINE23_FILEPATH_H

#include <string>
#include <vector>

namespace Bcg {

    struct FilePath {
        // Utility to normalize a path
        static std::string Normalized(const std::string &filename);

        // get directory name (not including '/').
        static std::string Dirname(const std::string &filename);

        // get extension (including '.').
        static std::string Extension(const std::string &filename);

        // get m_filename without directory.
        static std::string Filename(const std::string &filename);

        // get m_filename without directory and extension.
        static std::string Basename(const std::string &filename);

        // get relative path without m_filename.
        static std::string Relative(const std::string &filename);

        // get parent path without m_filename.
        static std::string Parent(const std::string &filename);

        // Joins paths
        static std::string Join(const std::string &patha, const std::string &pathb);

        static std::string Join(const std::string &patha, const std::string &pathb, const std::string &pathc);

        // split path
        static std::vector<std::string> Split(const std::string &path);

        // Replaces extensions
        static std::string ReplaceExtension(const std::string &filename, const std::string &ext);

        // Check if a file can be opened for reading.
        static bool Exists(const std::string &filename);

        // Check if a file is a directory
        static bool IsDir(const std::string &filename);

        // Check if a file is a file
        static bool IsFile(const std::string &filename);

        // Get file size in bytes
        static size_t SizeInBytes(const std::string &filename);

        // Get last write time
        static time_t LastWriteTime(const std::string &filename);

        // List the contents of a directory
        static std::vector<std::string> ListDirectory(const std::string &filename);

        // Create a directory and all missing parent directories if needed
        static bool MakeDirectory(const std::string &dirname, std::string &error);

        // Destroy a directory or file
        static bool Remove(const std::string &dirname, std::string &error);

        // Copy a file
        static bool Copy(const std::string &src, const std::string &dst, bool overwrite = false);

        // Move a file
        static bool Move(const std::string &src, const std::string &dst, bool overwrite = false);

        // Load a text file
        static bool LoadText(const std::string &filename, std::string &text);

        // Save a text file
        static bool SaveText(const std::string &filename, const std::string &text);

        // Load a binary file
        static bool LoadBinary(const std::string &filename, void *data, size_t size_in_bytes);

        // Save a binary file
        static bool SaveBinary(const std::string &filename, const void *data, size_t size_in_bytes);

        // Get the current directory
        static std::string Current();

        static std::string Separator();

    };
}
#endif //ENGINE23_FILEPATH_H
