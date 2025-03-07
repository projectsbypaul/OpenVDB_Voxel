#pragma once
#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Surface_mesh = CGAL::Surface_mesh<K::Point_3>;

namespace MDH {


	bool readMesh(std::string* path, Surface_mesh* mesh);
	void writeMesh(std::string* path, Surface_mesh* mesh);

}