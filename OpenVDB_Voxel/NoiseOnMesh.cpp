#include "NoiseOnMesh.h"

//created with aid of ChatGPT 
//inspired by https://github.com/sol-prog/Perlin_Noise
//and https://doc.cgal.org/latest/Polygon_mesh_processing/index.html#fig__Elephants

namespace NoiseOnMesh {

    namespace util {
        double swirl_noise(double param_1, double param_2, double x, double y, double z) {
            double swirl = std::sin(param_1 * std::sqrt(x * x + y * y)) + std::cos(param_2 * z);
            return 0.5 + 0.5 * std::sin(swirl); // Normalize to [0,1]
        }
    }

    namespace CGALbased {

        int applySwirlyNoise(Surface_mesh* mesh_ptr, double param_1, double param_2, double threshold, int random_seed) {

            Surface_mesh& mesh = *mesh_ptr; // Fix: reference, not a copy

            auto vnoise = mesh.add_property_map<Surface_mesh::Vertex_index, double>("v:noise", 0.0).first;
            for (auto v : mesh.vertices()) {
                const auto& p = mesh.point(v);
                vnoise[v] = util::swirl_noise(param_1, param_2, p.x(), p.y(), p.z());
            }

            std::vector<Surface_mesh::Face_index> faces_to_remove;
            for (auto f : mesh.faces()) {
                double avg_noise = 0.0;
                int count = 0;
                for (auto v : vertices_around_face(mesh.halfedge(f), mesh)) {
                    avg_noise += vnoise[v];
                    ++count;
                }
                avg_noise /= count;

                if (avg_noise < threshold) {
                    faces_to_remove.push_back(f);
                }
            }

            for (auto f : faces_to_remove) {
                CGAL::Euler::remove_face(mesh.halfedge(f), mesh);
            }

            mesh.collect_garbage();

            return faces_to_remove.size();
        }

    }

}