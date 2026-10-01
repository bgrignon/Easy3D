/********************************************************************
 * Copyright (C) 2015 Liangliang Nan <liangliang.nan@gmail.com>
 * https://3d.bk.tudelft.nl/liangliang/
 *
 * This file is part of Easy3D. If it is useful in your research/work,
 * I would be grateful if you show your appreciation by citing it:
 * ------------------------------------------------------------------
 *      Liangliang Nan.
 *      Easy3D: a lightweight, easy-to-use, and efficient C++ library
 *      for processing and rendering 3D data.
 *      Journal of Open Source Software, 6(64), 3255, 2021.
 * ------------------------------------------------------------------
 *
 * Easy3D is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3
 * as published by the Free Software Foundation.
 *
 * Easy3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************/

#include <vector>
#include <iostream>
#include <easy3d/core/point_cloud.h>
#include <easy3d/fileio/point_cloud_io.h>
#include <easy3d/algo/point_cloud_normals.h>
#include <easy3d/algo/point_cloud_simplification.h>
#include <easy3d/algo/point_cloud_ransac.h>


using namespace easy3d;


void compute_normals(PointCloud* cloud, unsigned int k, bool compute_curvature) {

    PointCloudNormals::estimate(cloud, k, compute_curvature);
    PointCloudNormals::reorient(cloud, k);
    // auto normals = cloud->get_vertex_property<vec3>("v:normal");
    // auto curvature = cloud->get_vertex_property<vec3>("v:curvature");

}

void downsample(PointCloud* cloud, unsigned int target_num_points) {
    std::vector<PointCloud::Vertex> points_to_remove = PointCloudSimplification::uniform_simplification(cloud, target_num_points);
    for (const PointCloud::Vertex v: points_to_remove)
        cloud->delete_vertex(v);
    if (cloud->has_garbage()) {
        cloud->collect_garbage();
        std::cout<< "Removed " << points_to_remove.size() << " points" << std::endl;
    }
}

void compute_planes(PointCloud* cloud, unsigned int min_support, float dist_threshold,
    float bitmap_resolution, float normal_threshold, float overlook_probability) {

    
    PrimitivesRansac ransac;
    ransac.add_primitive_type(PrimitivesRansac::PLANE);

    const int num_planes = ransac.detect(cloud, min_support, dist_threshold,
        bitmap_resolution, normal_threshold, overlook_probability);

    if (num_planes > 0)
        std::cout << num_planes << " planes extracted" << std::endl;
}

int main(int argc, char **argv) {

    if (argc > 1) {
        const std::string input = argv[1];

        // harcoded for now
        std::string output = input;
        auto dot_pos = input.find_last_of('.');
        if (dot_pos != std::string::npos)
            output = input.substr(0, dot_pos);
        output += ".vg";

        unsigned int k = 16;
        bool compute_curvature = false;

        unsigned int target_num_points = 30000;

        unsigned int min_support = 200;
        float dist_threshold = 0.005f;
        float bitmap_resolution = 0.02f;
        float normal_threshold = 0.8f;
        float overlook_probability = 0.001f;


        PointCloud* cloud = PointCloudIO::load(input);

        compute_normals(cloud, k, compute_curvature);

        downsample(cloud, target_num_points);

        compute_planes(cloud, min_support, dist_threshold, bitmap_resolution,
            normal_threshold, overlook_probability);

        PointCloudIO::save(output, cloud);
    }

    return 0;
}

