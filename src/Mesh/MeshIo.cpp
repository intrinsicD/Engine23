//
// Created by alex on 20.02.24.
//

#include "MeshIo.h"

#include "File/FilePath.h"
#include <map>
#include <iostream>
#include "MeshReaderOff.h"
#include "MeshReaderObj.h"
#include "MeshReaderStl.h"
#include "MeshReaderPly.h"
#include "MeshReaderPmp.h"
#include "MeshWriterObj.h"
#include "MeshWriterStl.h"
#include "MeshWriterOff.h"
#include "MeshWriterPly.h"
#include "MeshWriterPmp.h"

namespace Bcg {

    MeshIo::MeshIo(std::string filename, const MeshIoFlags &flags) : m_filename(
            std::move(filename)), m_flags(flags) {

    }

    bool MeshIo::read(Mesh &mesh) {
        // clear mesh before reading from file
        mesh.clear();

        // extract file extension
        std::string ext = FilePath::Extension(m_filename);
        if (!FilePath::Exists(m_filename))
            return false;

        // extension determines reader
        if (ext == ".off") {
            return read_off(mesh);
        } else if (ext == ".obj") {
            return read_obj(mesh);
        } else if (ext == ".stl") {
            return read_stl(mesh);
        } else if (ext == ".ply") {
            return read_ply(mesh);
        } else if (ext == ".pmp") {
            return read_pmp(mesh);
        }

        // we didn't find a reader module
        return false;
    }

    bool MeshIo::write(Mesh &mesh) {
        // extract file extension
        std::string ext = FilePath::Extension(m_filename);

        // extension determines reader
        if (ext == ".off") {
            return write_off(mesh);
        } else if (ext == ".obj") {
            return write_obj(mesh);
        } else if (ext == ".stl") {
            return write_stl(mesh);
        } else if (ext == ".ply") {
            return write_ply(mesh);
        } else if (ext == ".pmp") {
            return write_pmp(mesh);
        }

        // we didn't find a writer module
        return false;
    }

// helper function
    template<typename T>
    void TFWrite(FILE *out, const T &t) {
        size_t num_items = fwrite((char *) &t, 1, sizeof(t), out);
        if (num_items == 0) {
            std::cerr << "num_items should be > 0";
            assert(num_items > 0);
        }
    }

    bool MeshIo::read_off(Mesh &mesh) {
        MeshReaderOff readerOff(m_filename, mesh);
        return readerOff.read();
    }

    bool MeshIo::read_obj(Mesh &mesh) {
        MeshReaderObj readerObj(m_filename, mesh);
        return readerObj.read();
    }

    bool MeshIo::read_stl(Mesh &mesh) {
        MeshReaderStl readerStl(m_filename, mesh);
        return readerStl.read();
    }

    bool MeshIo::read_ply(Mesh &mesh) {
        MeshReaderPly readerPly(m_filename, mesh);
        return readerPly.read();
    }

    bool MeshIo::read_pmp(Mesh &mesh) {
        MeshReaderPmp readerPmp(m_filename, mesh);
        return readerPmp.read();
    }

    bool MeshIo::write_off(Mesh &mesh) {
        MeshWriterOff writerOff(m_filename, mesh);
        writerOff.use_binary = m_flags.use_binary;
        return writerOff.write();
    }

    bool MeshIo::write_obj(Mesh &mesh) {
        MeshWriterObj writerObj(m_filename, mesh);
        return writerObj.write();
    }

    bool MeshIo::write_stl(Mesh &mesh) {
        MeshWriterStl writerStl(m_filename, mesh);
        return writerStl.write();
    }

    bool MeshIo::write_ply(Mesh &mesh) {
        MeshWriterPly writerPly(m_filename, mesh);
        return writerPly.write();
    }

    bool MeshIo::write_pmp(Mesh &mesh) {
        MeshWriterPmp writerPmp(m_filename, mesh);
        return writerPmp.write();
    }

    std::vector<std::string> MeshIo::formats() {
        std::vector<std::string> names;
        names.emplace_back(".off");
        names.emplace_back(".obj");
        names.emplace_back(".stl");
        names.emplace_back(".ply");
        names.emplace_back(".pmp");
        return names;
    }

    bool MeshIo::has_file_extension(const std::string &filename) {
        auto known_formats = formats();
        auto ext = FilePath::Extension(filename);
        return std::find(known_formats.begin(), known_formats.end(), ext) != known_formats.end();
    }
}
