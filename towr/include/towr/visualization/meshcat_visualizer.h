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

#ifndef TOWR_VISUALIZATION_MESHCAT_VISUALIZER_H_
#define TOWR_VISUALIZATION_MESHCAT_VISUALIZER_H_

#ifdef TOWR_WITH_MESHCAT

#include <memory>
#include <vector>
#include <string>

#include <towr/models/robot_model.h>
#include <towr/variables/spline_holder.h>
#include <towr/variables/state.h>

// Forward declarations to avoid including MeshCat headers in this header
namespace MeshcatCpp {
  class Meshcat;
  class Material;
}

namespace towr {

/**
 * @brief MeshCat-based visualizer for TOWR trajectory optimization results.
 * 
 * This class provides 3D visualization of robot trajectories using the MeshCat
 * web-based visualizer. It can display:
 * - Robot body trajectory
 * - End-effector (foot) trajectories  
 * - Contact phases and forces
 * - Terrain visualization
 * - Real-time trajectory playback
 */
class MeshcatVisualizer {
public:
  using Ptr = std::shared_ptr<MeshcatVisualizer>;
  using Vector3d = Eigen::Vector3d;
  using VectorXd = Eigen::VectorXd;
  
  /**
   * @brief Constructor.
   * @param port Port number for MeshCat server (default: 7000)
   */
  explicit MeshcatVisualizer(int port = 7000);
  
  /**
   * @brief Destructor.
   */
  ~MeshcatVisualizer();
  
  /**
   * @brief Initialize the visualizer with robot model.
   * @param robot_model The robot model to visualize
   */
  void Initialize(const RobotModel& robot_model);
  
  /**
   * @brief Set the terrain for visualization.
   * @param terrain_height_map Height map of the terrain
   * @param x_range X-axis range for terrain visualization
   * @param y_range Y-axis range for terrain visualization  
   * @param resolution Grid resolution for terrain mesh
   */
  void SetTerrain(const HeightMap::Ptr& terrain_height_map,
                  const std::pair<double, double>& x_range = {-1.0, 3.0},
                  const std::pair<double, double>& y_range = {-1.0, 1.0},
                  double resolution = 0.1);
  
  /**
   * @brief Visualize a single state of the robot.
   * @param t Time of the state
   * @param base_state State of the robot base
   * @param ee_positions Positions of all end-effectors
   * @param ee_forces Contact forces at end-effectors
   * @param contact_states Contact states for each end-effector
   */
  void VisualizeState(double t,
                      const BaseState& base_state,
                      const std::vector<Vector3d>& ee_positions,
                      const std::vector<Vector3d>& ee_forces,
                      const std::vector<bool>& contact_states);
  
  /**
   * @brief Visualize complete trajectory from spline holder.
   * @param splines Spline holder containing all trajectory data
   * @param dt Time step for trajectory sampling
   * @param total_duration Total duration of the trajectory
   */
  void VisualizeTrajectory(const SplineHolder& splines,
                          double dt = 0.01,
                          double total_duration = -1.0);
  
  /**
   * @brief Play trajectory animation.
   * @param splines Spline holder containing trajectory data
   * @param playback_speed Speed multiplier for playback (1.0 = real-time)
   * @param dt Time step for animation
   * @param loop Whether to loop the animation
   */
  void PlayTrajectory(const SplineHolder& splines,
                     double playback_speed = 1.0,
                     double dt = 0.02,
                     bool loop = true);
  
  /**
   * @brief Clear all visualizations.
   */
  void Clear();
  
  /**
   * @brief Get the URL of the MeshCat visualizer.
   * @return URL string
   */
  std::string GetUrl() const;
  
  /**
   * @brief Keep the visualizer running (blocking call).
   */
  void Join();

private:
  std::unique_ptr<MeshcatCpp::Meshcat> meshcat_;
  std::unique_ptr<MeshcatCpp::Material> robot_material_;
  std::unique_ptr<MeshcatCpp::Material> foot_material_;
  std::unique_ptr<MeshcatCpp::Material> force_material_;
  std::unique_ptr<MeshcatCpp::Material> terrain_material_;
  
  RobotModel robot_model_;
  HeightMap::Ptr terrain_;
  
  int n_ee_;  // Number of end-effectors
  
  // Helper methods
  void CreateRobotGeometry();
  void CreateTerrainMesh(const std::pair<double, double>& x_range,
                        const std::pair<double, double>& y_range,
                        double resolution);
  void UpdateRobotVisualization(const BaseState& base_state,
                               const std::vector<Vector3d>& ee_positions,
                               const std::vector<bool>& contact_states);
  void UpdateForceVisualization(const std::vector<Vector3d>& ee_positions,
                               const std::vector<Vector3d>& ee_forces,
                               const std::vector<bool>& contact_states);
  void SetupMaterials();
  
  // Convert Eigen matrix to MeshCat transform format
  std::array<double, 16> EigenToMeshcatTransform(const Vector3d& position,
                                                const Eigen::Matrix3d& rotation = Eigen::Matrix3d::Identity());
};

} // namespace towr

#endif // TOWR_WITH_MESHCAT

#endif // TOWR_VISUALIZATION_MESHCAT_VISUALIZER_H_
