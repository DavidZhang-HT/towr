/******************************************************************************
Copyright (c) 2024, TOWR Contributors. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <towr/visualization/meshcat_visualizer.h>

#ifdef TOWR_WITH_MESHCAT

#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>

#include <MeshcatCpp/Meshcat.h>
#include <MeshcatCpp/Material.h>
#include <MeshcatCpp/Shape.h>

#include <towr/terrain/height_map.h>
#include <towr/variables/euler_converter.h>

namespace towr {

MeshcatVisualizer::MeshcatVisualizer(int port) 
  : meshcat_(std::make_unique<MeshcatCpp::Meshcat>(port)),
    robot_material_(std::make_unique<MeshcatCpp::Material>()),
    foot_material_(std::make_unique<MeshcatCpp::Material>()),
    force_material_(std::make_unique<MeshcatCpp::Material>()),
    terrain_material_(std::make_unique<MeshcatCpp::Material>()),
    n_ee_(0) {
  
  SetupMaterials();
  
  std::cout << "🌐 MeshCat可视化器已启动" << std::endl;
  std::cout << "📱 请在浏览器中打开: " << GetUrl() << std::endl;
}

MeshcatVisualizer::~MeshcatVisualizer() = default;

void MeshcatVisualizer::Initialize(const RobotModel& robot_model) {
  robot_model_ = robot_model;
  n_ee_ = robot_model_.kinematic_model_->GetNumberOfEndeffectors();

  CreateRobotGeometry();

  // Determine robot type based on number of end-effectors
  std::string robot_type_name = "Unknown";
  if (n_ee_ == 1) {
    robot_type_name = "Monoped";
  } else if (n_ee_ == 2) {
    robot_type_name = "Biped";
  } else if (n_ee_ == 4) {
    robot_type_name = "Quadruped";
  }

  std::cout << "🤖 机器人模型已初始化: " << robot_type_name
            << " (末端执行器数量: " << n_ee_ << ")" << std::endl;
}

void MeshcatVisualizer::SetupMaterials() {
  // Robot body material - blue
  robot_material_->set_color(66, 133, 244);
  robot_material_->set_opacity(0.8);
  
  // Foot material - red for contact, green for swing
  foot_material_->set_color(234, 67, 53);
  foot_material_->set_opacity(0.9);
  
  // Force vector material - yellow
  force_material_->set_color(251, 188, 5);
  force_material_->set_opacity(1.0);
  
  // Terrain material - gray
  terrain_material_->set_color(128, 128, 128);
  terrain_material_->set_opacity(0.6);
}

void MeshcatVisualizer::CreateRobotGeometry() {
  // Create robot body (box representation)
  double body_length = 0.4;
  double body_width = 0.2;
  double body_height = 0.1;
  
  // Adjust size based on robot type
  if (n_ee_ == 1) {  // Monoped
    body_length = 0.2;
    body_width = 0.1;
  } else if (n_ee_ == 2) {  // Biped
    body_length = 0.3;
    body_width = 0.15;
  } else if (n_ee_ == 4) {  // Quadruped
    body_length = 0.5;
    body_width = 0.25;
  }
  
  meshcat_->set_object("robot/body", 
                      MeshcatCpp::Box(body_length, body_width, body_height), 
                      *robot_material_);
  
  // Create end-effector spheres
  for (int ee = 0; ee < n_ee_; ++ee) {
    std::string foot_name = "robot/foot_" + std::to_string(ee);
    meshcat_->set_object(foot_name, 
                        MeshcatCpp::Sphere(0.03), 
                        *foot_material_);
  }
}

std::array<double, 16> MeshcatVisualizer::EigenToMeshcatTransform(
    const Vector3d& position, const Eigen::Matrix3d& rotation) {
  
  std::array<double, 16> transform;
  
  // Column-major order for MeshCat
  for (int col = 0; col < 3; ++col) {
    for (int row = 0; row < 3; ++row) {
      transform[col * 4 + row] = rotation(row, col);
    }
    transform[col * 4 + 3] = position(col);
  }
  
  // Last row: [0, 0, 0, 1]
  transform[12] = 0.0;
  transform[13] = 0.0;
  transform[14] = 0.0;
  transform[15] = 1.0;
  
  return transform;
}

void MeshcatVisualizer::VisualizeState(double t,
                                      const BaseState& base_state,
                                      const std::vector<Vector3d>& ee_positions,
                                      const std::vector<Vector3d>& ee_forces,
                                      const std::vector<bool>& contact_states) {
  
  UpdateRobotVisualization(base_state, ee_positions, contact_states);
  UpdateForceVisualization(ee_positions, ee_forces, contact_states);
}

void MeshcatVisualizer::UpdateRobotVisualization(const BaseState& base_state,
                                                const std::vector<Vector3d>& ee_positions,
                                                const std::vector<bool>& contact_states) {
  
  // Update robot body
  EulerConverter euler_converter;
  Eigen::Matrix3d rotation = euler_converter.GetRotationMatrixBaseToWorld(base_state.ang.p);
  
  auto body_transform = EigenToMeshcatTransform(base_state.lin.p, rotation);
  meshcat_->set_transform("robot/body", body_transform);
  
  // Update end-effectors
  for (int ee = 0; ee < std::min(n_ee_, static_cast<int>(ee_positions.size())); ++ee) {
    std::string foot_name = "robot/foot_" + std::to_string(ee);
    
    // Change color based on contact state
    auto material = std::make_unique<MeshcatCpp::Material>();
    if (ee < contact_states.size() && contact_states[ee]) {
      material->set_color(234, 67, 53);  // Red for contact
    } else {
      material->set_color(52, 168, 83);  // Green for swing
    }
    material->set_opacity(0.9);
    
    meshcat_->set_object(foot_name, MeshcatCpp::Sphere(0.03), *material);
    
    auto foot_transform = EigenToMeshcatTransform(ee_positions[ee]);
    meshcat_->set_transform(foot_name, foot_transform);
    
    // Draw leg connection (line from body to foot)
    // This is a simplified representation using a thin cylinder
    Vector3d leg_vector = ee_positions[ee] - base_state.lin.p;
    double leg_length = leg_vector.norm();
    
    if (leg_length > 1e-6) {
      Vector3d leg_center = base_state.lin.p + 0.5 * leg_vector;
      
      // Create rotation matrix to align cylinder with leg direction
      Vector3d z_axis = leg_vector.normalized();
      Vector3d x_axis = (std::abs(z_axis.dot(Vector3d::UnitX())) < 0.9) ? 
                        Vector3d::UnitX() : Vector3d::UnitY();
      Vector3d y_axis = z_axis.cross(x_axis).normalized();
      x_axis = y_axis.cross(z_axis);
      
      Eigen::Matrix3d leg_rotation;
      leg_rotation.col(0) = x_axis;
      leg_rotation.col(1) = y_axis;
      leg_rotation.col(2) = z_axis;
      
      std::string leg_name = "robot/leg_" + std::to_string(ee);
      auto leg_material = std::make_unique<MeshcatCpp::Material>();
      leg_material->set_color(100, 100, 100);
      leg_material->set_opacity(0.7);
      
      meshcat_->set_object(leg_name, 
                          MeshcatCpp::Cylinder(0.01, leg_length), 
                          *leg_material);
      
      auto leg_transform = EigenToMeshcatTransform(leg_center, leg_rotation);
      meshcat_->set_transform(leg_name, leg_transform);
    }
  }
}

void MeshcatVisualizer::UpdateForceVisualization(const std::vector<Vector3d>& ee_positions,
                                                 const std::vector<Vector3d>& ee_forces,
                                                 const std::vector<bool>& contact_states) {
  
  for (int ee = 0; ee < std::min(n_ee_, static_cast<int>(ee_forces.size())); ++ee) {
    std::string force_name = "forces/force_" + std::to_string(ee);
    
    if (ee < contact_states.size() && contact_states[ee] && 
        ee < ee_forces.size() && ee_forces[ee].norm() > 1e-3) {
      
      Vector3d force = ee_forces[ee];
      double force_magnitude = force.norm();
      Vector3d force_direction = force.normalized();
      
      // Scale force visualization (1N = 0.001m in visualization)
      double force_scale = 0.001;
      double arrow_length = force_magnitude * force_scale;
      
      if (arrow_length > 1e-6) {
        Vector3d arrow_start = ee_positions[ee];
        Vector3d arrow_end = arrow_start + arrow_length * force_direction;
        Vector3d arrow_center = arrow_start + 0.5 * arrow_length * force_direction;
        
        // Create rotation matrix for arrow
        Vector3d z_axis = force_direction;
        Vector3d x_axis = (std::abs(z_axis.dot(Vector3d::UnitX())) < 0.9) ? 
                          Vector3d::UnitX() : Vector3d::UnitY();
        Vector3d y_axis = z_axis.cross(x_axis).normalized();
        x_axis = y_axis.cross(z_axis);
        
        Eigen::Matrix3d arrow_rotation;
        arrow_rotation.col(0) = x_axis;
        arrow_rotation.col(1) = y_axis;
        arrow_rotation.col(2) = z_axis;
        
        meshcat_->set_object(force_name, 
                            MeshcatCpp::Cylinder(0.005, arrow_length), 
                            *force_material_);
        
        auto force_transform = EigenToMeshcatTransform(arrow_center, arrow_rotation);
        meshcat_->set_transform(force_name, force_transform);
      }
    } else {
      // Hide force arrow when not in contact
      meshcat_->delete_object(force_name);
    }
  }
}

std::string MeshcatVisualizer::GetUrl() const {
  return meshcat_->web_url();
}

void MeshcatVisualizer::Clear() {
  meshcat_->delete_object("robot");
  meshcat_->delete_object("forces");
  meshcat_->delete_object("terrain");
}

void MeshcatVisualizer::SetTerrain(const HeightMap::Ptr& terrain_height_map,
                                  const std::pair<double, double>& x_range,
                                  const std::pair<double, double>& y_range,
                                  double resolution) {
  terrain_ = terrain_height_map;
  CreateTerrainMesh(x_range, y_range, resolution);
}

void MeshcatVisualizer::CreateTerrainMesh(const std::pair<double, double>& x_range,
                                         const std::pair<double, double>& y_range,
                                         double resolution) {
  if (!terrain_) return;

  // Create a simple terrain representation using boxes
  for (double x = x_range.first; x <= x_range.second; x += resolution) {
    for (double y = y_range.first; y <= y_range.second; y += resolution) {
      double height = terrain_->GetHeight(x, y);

      if (std::abs(height) > 1e-6) {  // Only show non-zero height terrain
        std::string terrain_name = "terrain/tile_" +
                                  std::to_string(static_cast<int>(x * 100)) + "_" +
                                  std::to_string(static_cast<int>(y * 100));

        meshcat_->set_object(terrain_name,
                            MeshcatCpp::Box(resolution, resolution, std::abs(height)),
                            *terrain_material_);

        Vector3d position(x, y, height / 2.0);
        auto terrain_transform = EigenToMeshcatTransform(position);
        meshcat_->set_transform(terrain_name, terrain_transform);
      }
    }
  }
}

void MeshcatVisualizer::VisualizeTrajectory(const SplineHolder& splines,
                                           double dt,
                                           double total_duration) {
  if (total_duration < 0) {
    total_duration = splines.base_linear_->GetTotalTime();
  }

  std::cout << "📊 可视化轨迹 - 总时长: " << total_duration << "秒" << std::endl;

  // Sample trajectory and create static visualization
  std::vector<Vector3d> base_trajectory;
  std::vector<std::vector<Vector3d>> ee_trajectories(n_ee_);

  for (double t = 0.0; t <= total_duration; t += dt) {
    // Base trajectory
    Vector3d base_pos = splines.base_linear_->GetPoint(t).p();
    base_trajectory.push_back(base_pos);

    // End-effector trajectories
    for (int ee = 0; ee < n_ee_; ++ee) {
      Vector3d ee_pos = splines.ee_motion_.at(ee)->GetPoint(t).p();
      ee_trajectories[ee].push_back(ee_pos);
    }
  }

  // Visualize base trajectory as a line
  for (size_t i = 1; i < base_trajectory.size(); ++i) {
    Vector3d start = base_trajectory[i-1];
    Vector3d end = base_trajectory[i];
    Vector3d segment = end - start;
    double length = segment.norm();

    if (length > 1e-6) {
      Vector3d center = start + 0.5 * segment;
      Vector3d direction = segment.normalized();

      // Create rotation matrix
      Vector3d z_axis = direction;
      Vector3d x_axis = (std::abs(z_axis.dot(Vector3d::UnitX())) < 0.9) ?
                        Vector3d::UnitX() : Vector3d::UnitY();
      Vector3d y_axis = z_axis.cross(x_axis).normalized();
      x_axis = y_axis.cross(z_axis);

      Eigen::Matrix3d rotation;
      rotation.col(0) = x_axis;
      rotation.col(1) = y_axis;
      rotation.col(2) = z_axis;

      std::string segment_name = "trajectory/base_segment_" + std::to_string(i);
      auto traj_material = std::make_unique<MeshcatCpp::Material>();
      traj_material->set_color(66, 133, 244);
      traj_material->set_opacity(0.5);

      meshcat_->set_object(segment_name,
                          MeshcatCpp::Cylinder(0.005, length),
                          *traj_material);

      auto segment_transform = EigenToMeshcatTransform(center, rotation);
      meshcat_->set_transform(segment_name, segment_transform);
    }
  }

  // Visualize end-effector trajectories
  for (int ee = 0; ee < n_ee_; ++ee) {
    for (size_t i = 1; i < ee_trajectories[ee].size(); ++i) {
      Vector3d start = ee_trajectories[ee][i-1];
      Vector3d end = ee_trajectories[ee][i];
      Vector3d segment = end - start;
      double length = segment.norm();

      if (length > 1e-6) {
        Vector3d center = start + 0.5 * segment;
        Vector3d direction = segment.normalized();

        Vector3d z_axis = direction;
        Vector3d x_axis = (std::abs(z_axis.dot(Vector3d::UnitX())) < 0.9) ?
                          Vector3d::UnitX() : Vector3d::UnitY();
        Vector3d y_axis = z_axis.cross(x_axis).normalized();
        x_axis = y_axis.cross(z_axis);

        Eigen::Matrix3d rotation;
        rotation.col(0) = x_axis;
        rotation.col(1) = y_axis;
        rotation.col(2) = z_axis;

        std::string segment_name = "trajectory/ee" + std::to_string(ee) +
                                  "_segment_" + std::to_string(i);
        auto ee_traj_material = std::make_unique<MeshcatCpp::Material>();
        ee_traj_material->set_color(234, 67, 53);
        ee_traj_material->set_opacity(0.3);

        meshcat_->set_object(segment_name,
                            MeshcatCpp::Cylinder(0.003, length),
                            *ee_traj_material);

        auto segment_transform = EigenToMeshcatTransform(center, rotation);
        meshcat_->set_transform(segment_name, segment_transform);
      }
    }
  }
}

void MeshcatVisualizer::PlayTrajectory(const SplineHolder& splines,
                                      double playback_speed,
                                      double dt,
                                      bool loop) {
  double total_duration = splines.base_linear_->GetTotalTime();

  std::cout << "▶️  播放轨迹动画 - 播放速度: " << playback_speed << "x" << std::endl;
  std::cout << "⏱️  总时长: " << total_duration << "秒" << std::endl;

  do {
    for (double t = 0.0; t <= total_duration; t += dt) {
      // Get robot state at time t
      BaseState base_state;
      base_state.lin = splines.base_linear_->GetPoint(t);
      base_state.ang = splines.base_angular_->GetPoint(t);

      // Get end-effector positions and forces
      std::vector<Vector3d> ee_positions(n_ee_);
      std::vector<Vector3d> ee_forces(n_ee_);
      std::vector<bool> contact_states(n_ee_);

      for (int ee = 0; ee < n_ee_; ++ee) {
        ee_positions[ee] = splines.ee_motion_.at(ee)->GetPoint(t).p();
        ee_forces[ee] = splines.ee_force_.at(ee)->GetPoint(t).p();

        // Determine contact state based on force magnitude
        contact_states[ee] = (ee_forces[ee].norm() > 1.0);  // Threshold for contact
      }

      // Update visualization
      VisualizeState(t, base_state, ee_positions, ee_forces, contact_states);

      // Sleep for real-time playback
      std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(dt * 1000.0 / playback_speed)));
    }
  } while (loop);
}

void MeshcatVisualizer::Join() {
  meshcat_->join();
}

} // namespace towr

#endif // TOWR_WITH_MESHCAT
