#pragma once
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_set>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Surface_mesh;

//created with aid of ChatGPT 
//inspired by https://github.com/sol-prog/Perlin_Noise
//and https://doc.cgal.org/latest/Polygon_mesh_processing/index.html#fig__Elephants

namespace NoiseOnMesh{

	namespace util {
		double swirl_noise(double param_1,double param_2, double x, double y, double z);
	}

	namespace CGALbased {
		int applySwirlyNoise(Surface_mesh* mesh_ptr, double param_1, double param_2, double threshold, int random_seed);
	}
}