//
// Created by alex on 20.02.24.
//

#include "File/FilePath.h"
#include "File/FileSystemStl.h"
#include "StringUtils.h"
#include <fstream>

namespace Bcg {
    // Make a path from an utf8 std::string
    static Fs::path MakePath(const std::string &filename) {
        return Fs::u8path(filename);
    }

// Normalize path
    std::string FilePath::Normalized(const std::string &filename) {
        return MakePath(filename).string();
    }

// get directory name (not including /)
    std::string FilePath::Dirname(const std::string &filename) {
        return MakePath(filename).parent_path().string();
    }

// get extension (including .)
    std::string FilePath::Extension(const std::string &filename) {
        return MakePath(filename).extension().string();
    }

// get m_filename without directory.
    std::string FilePath::Filename(const std::string &filename) {
        return MakePath(filename).filename().string();
    }

// get m_filename without directory and extension.
    std::string FilePath::Basename(const std::string &filename) {
        return MakePath(filename).stem().string();
    }

// get relative path without m_filename.
    std::string FilePath::Relative(const std::string &filename) {
        return MakePath(filename).relative_path().string();
    }

    // get parent path without m_filename.
    std::string FilePath::Parent(const std::string &filename) {
        return MakePath(filename).parent_path().string();
    }

// Joins paths
    std::string FilePath::Join(const std::string &patha, const std::string &pathb) {
        return (MakePath(patha) / MakePath(pathb)).string();
    }

    std::string FilePath::Join(
            const std::string &patha, const std::string &pathb, const std::string &pathc) {
        return (MakePath(patha) / MakePath(pathb) / MakePath(pathc))
                .string();
    }

// split path
    std::vector<std::string> FilePath::Split(const std::string &path) {
        return String::Split(path, FilePath::Separator().c_str());
    }

// Replaces extensions
    std::string FilePath::ReplaceExtension(const std::string &filename, const std::string &ext) {
        return MakePath(filename).replace_extension(ext).string();
    }

// Check if a file can be opened for reading.
    bool FilePath::Exists(const std::string &filename) { return Fs::exists(MakePath(filename)); }

// Check if a file is a directory
    bool FilePath::IsDir(const std::string &filename) {
        return Fs::is_directory(MakePath(filename));
    }

// Check if a file is a file
    bool FilePath::IsFile(const std::string &filename) {
        return Fs::is_regular_file(MakePath(filename));
    }

// Get file size in bytes
    size_t FilePath::SizeInBytes(const std::string &filename) {
        return Fs::file_size(MakePath(filename));
    }

// Get last write time
    time_t FilePath::LastWriteTime(const std::string &filename) {
        auto tp = Fs::last_write_time(MakePath(filename));
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                tp - Fs::file_time_type::clock::now()
                + std::chrono::system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    }

// List the contents of a directory
    std::vector<std::string> FilePath::ListDirectory(const std::string &filename) {
        auto entries = std::vector<std::string>{};
        for (const auto &entry: Fs::directory_iterator(MakePath(filename))) {
            entries.push_back(entry.path().string());
        }
        std::sort(entries.begin(), entries.end());
        return entries;
    }

// Create a directory and all missing parent directories if needed
    bool FilePath::MakeDirectory(const std::string &dirname, std::string &error) {
        if (Exists(dirname)) return true;
        try {
            return Fs::create_directories(MakePath(dirname));
        } catch (...) {
            error = dirname + ": cannot create directory";
            return false;
        }
    }

    bool FilePath::Remove(const std::string &dirname, std::string &error) {
        if (!Exists(dirname)) return true;
        try {
            return Fs::remove(MakePath(dirname));
        } catch (...) {
            error = dirname + ": cannot remove directory";
            return false;
        }
    }


    // Copy a file
    bool FilePath::Copy(const std::string &src, const std::string &dst, bool overwrite) {
        if (!Exists(src)) return false;
        if (!overwrite && Exists(dst)) return false;

        std::fstream src_file(src, std::ios::in | std::ios::binary);
        std::fstream dst_file(dst, std::ios::out | std::ios::binary);

        if (!src_file.is_open() || !dst_file.is_open()) return false;
        dst_file << src_file.rdbuf();
        src_file.close();
        dst_file.close();
        return true;
    }

    // Move a file
    bool FilePath::Move(const std::string &src, const std::string &dst, bool overwrite) {
        if (!Exists(src)) return false;
        if (!overwrite && Exists(dst)) return false;
        try {
            Fs::rename(MakePath(src), MakePath(dst));
            return true;
        } catch (...) {
            return false;
        }
    }

    // Load a text file
    bool FilePath::LoadText(const std::string &filename, std::string &text) {
        if (!Exists(filename)) return false;
        auto size = static_cast<std::streamsize>(SizeInBytes(filename));
        if (size == 0) return false;

        std::fstream fs(filename, std::ios::in);
        if (!fs.is_open()) return false;
        text = std::string(size, '\0');
        fs.read(text.data(), size);
        fs.close();
        return true;
    }

    // Save a text file
    bool FilePath::SaveText(const std::string &filename, const std::string &text) {
        std::fstream fs(filename, std::ios::out);
        if (!fs.is_open()) return false;
        fs.write(text.data(), static_cast<std::streamsize>(text.size()));
        fs.close();
        return true;
    }

    // Load a binary file
    bool FilePath::LoadBinary(const std::string &filename, void *data, size_t size_in_bytes) {
        if (!Exists(filename)) return false;
        std::fstream fs(filename, std::ios::in | std::ios::binary);
        if (!fs.is_open()) return false;
        fs.read((char *) data, static_cast<std::streamsize>(size_in_bytes));
        fs.close();
        return true;
    }

    // Save a binary file
    bool FilePath::SaveBinary(const std::string &filename, const void *data, size_t size_in_bytes) {
        std::fstream fs(filename, std::ios::out | std::ios::binary);
        if (!fs.is_open()) return false;
        fs.write((const char *) data, static_cast<std::streamsize>(size_in_bytes));
        fs.close();
        return true;
    }

// get the current directory
    std::string FilePath::Current() { return Fs::current_path().string(); }


    std::string FilePath::Separator() { return {Fs::path::preferred_separator}; }
}