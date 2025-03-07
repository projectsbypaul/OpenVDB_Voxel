#include "MeshDataHandling.h"
#include <CGAL/IO/STL.h>
#include <CGAL/IO/PLY.h>
#include <CGAL/IO/OBJ.h>
#include <CGAL/IO/OFF.h>
#include <CGAL/IO/GOCAD.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <filesystem>
#include <string>
#include <algorithm>

namespace MDH {

std::string getFileExtension(const std::string& path)
{
    return std::filesystem::path(path).extension().string();
}


//Read Write for different FileExtensions

// STL Format
void readSTL(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::read_STL(*path, *mesh)) {
        std::cout << "Can't create the mesh" << std::endl;
    }
    else {
        std::cout << "Reading: " << *path << std::endl;
    }
}

void writeSTL(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::write_STL(*path, *mesh)) {
        std::cout << "Error: Unable to write STL file" << std::endl;
    }
    else {
        std::cout << "Successfully wrote: " << *path << std::endl;
    }
}

// PLY Format
void readPLY(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::read_PLY(*path, *mesh)) {
        std::cout << "Error: Can't read PLY file" << std::endl;
    }
    else {
        std::cout << "Successfully read: " << *path << std::endl;
    }
}

void writePLY(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::write_PLY(*path, *mesh)) {
        std::cout << "Error: Unable to write PLY file" << std::endl;
    }
    else {
        std::cout << "Successfully wrote: " << *path << std::endl;
    }
}

// OBJ Format
void readOBJ(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::read_OBJ(*path, *mesh)) {
        std::cout << "Error: Can't read OBJ file" << std::endl;
    }
    else {
        std::cout << "Successfully read: " << *path << std::endl;
    }
}

void writeOBJ(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::write_OBJ(*path, *mesh)) {
        std::cout << "Error: Unable to write OBJ file" << std::endl;
    }
    else {
        std::cout << "Successfully wrote: " << *path << std::endl;
    }
}

// OFF Format
void readOFF(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::read_OFF(*path, *mesh)) {
        std::cout << "Error: Can't read OFF file" << std::endl;
    }
    else {
        std::cout << "Successfully read: " << *path << std::endl;
    }
}

void writeOFF(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::write_OFF(*path, *mesh)) {
        std::cout << "Error: Unable to write OFF file" << std::endl;
    }
    else {
        std::cout << "Successfully wrote: " << *path << std::endl;
    }
}

// GOCAD Format
void readGOCAD(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::read_GOCAD(*path, *mesh)) {
        std::cout << "Error: Can't read GOCAD file" << std::endl;
    }
    else {
        std::cout << "Successfully read: " << *path << std::endl;
    }
}

void writeGOCAD(std::string* path, Surface_mesh* mesh)
{
    if (!CGAL::IO::write_GOCAD(*path, *mesh)) {
        std::cout << "Error: Unable to write GOCAD file" << std::endl;
    }
    else {
        std::cout << "Successfully wrote: " << *path << std::endl;
    }
}

// Main Handling Functions
bool readMesh(std::string* path, Surface_mesh* mesh)
{
    std::string ext = getFileExtension(*path);
    std::cout << "Fileextension: " << ext << std::endl;

    if (ext == ".stl" || ext == ".STL") {
        readSTL(path, mesh);
        return true;
    }
    else if (ext == ".ply" || ext == ".PLY") {
        readPLY(path, mesh);
        return true;
    }
    else if (ext == ".obj" || ext == ".OBJ") {
        readOBJ(path, mesh);
        return true;
    }
    else if (ext == ".off" || ext == ".OFF") {
        readOFF(path, mesh);
        return true;
    }
    else if (ext == ".ts" || ext == ".TS") {
        readGOCAD(path, mesh);
        return true;
    }
    else {
        std::cout << "Error: handling for filetype " << ext << " not implemented" << std::endl;
        return false;
    }
}

void writeMesh(std::string* path, Surface_mesh* mesh)
{
    std::string ext = getFileExtension(*path);
    std::cout << "Fileextension: " << ext << std::endl;

    if (ext == ".stl" || ext == ".STL") {
        writeSTL(path, mesh);
    }
    else if (ext == ".ply" || ext == ".PLY") {
        writePLY(path, mesh);
    }
    else if (ext == ".obj" || ext == ".OBJ") {
        writeOBJ(path, mesh);
    }
    else if (ext == ".off" || ext == ".OFF") {
        writeOFF(path, mesh);
    }
    else if (ext == ".ts" || ext == ".TS") {
        writeGOCAD(path, mesh);
    }
    else {
        std::cout << "Error: handling for filetype " << ext << " not implemented" << std::endl;
    }
}

}