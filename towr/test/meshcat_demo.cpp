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

/**
 * @file meshcat_demo.cpp
 * @brief MeshCat可视化演示程序
 * 
 * 这个程序演示如何使用MeshCat可视化TOWR的轨迹优化结果。
 * 它展示了单腿跳跃机器人的运动轨迹，包括：
 * - 3D机器人模型可视化
 * - 实时轨迹播放
 * - 接触力可视化
 * - 接触相位显示
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include <towr/nlp_formulation.h>
#include <towr/variables/spline_holder.h>
#include <towr/models/robot_model.h>
#include <towr/terrain/height_map_examples.h>
#include <towr/variables/state.h>

#ifdef TOWR_WITH_MESHCAT
#include <towr/visualization/meshcat_visualizer.h>
#endif

#include <ifopt/ipopt_solver.h>

using namespace towr;

void PrintWelcomeMessage() {
  std::cout << "\n";
  std::cout << "🚀 TOWR MeshCat 可视化演示\n";
  std::cout << "=" << std::string(50, '=') << "\n";
  std::cout << "这个演示程序将展示：\n";
  std::cout << "• 单腿跳跃机器人的3D轨迹优化\n";
  std::cout << "• 基于MeshCat的实时可视化\n";
  std::cout << "• 接触力和相位的动态显示\n";
  std::cout << "• 交互式3D场景浏览\n\n";
}

void DemonstrateMonopedWithMeshCat() {
  std::cout << "🤖 开始单腿机器人轨迹优化...\n";
  
  // 1. 设置机器人模型
  RobotModel robot_model(RobotModel::Monoped);
  
  // 2. 设置NLP问题
  NlpFormulation formulation;
  formulation.model_ = robot_model;
  
  // 设置初始和目标状态
  formulation.initial_base_.lin.at(kPos) = Eigen::Vector3d(0.0, 0.0, 0.5);
  formulation.initial_base_.ang.at(kPos) = Eigen::Vector3d(0.0, 0.0, 0.0);
  
  formulation.final_base_.lin.at(kPos) = Eigen::Vector3d(1.0, 0.0, 0.5);
  formulation.final_base_.ang.at(kPos) = Eigen::Vector3d(0.0, 0.0, 0.0);
  
  // 设置初始脚位置
  formulation.initial_ee_W_.push_back(Eigen::Vector3d(0.0, 0.0, 0.0));
  
  // 设置地形
  formulation.terrain_ = std::make_shared<FlatGround>(0.0);
  
  // 设置参数
  formulation.params_.SetBasePolyDuration(0.5, 1.0);
  formulation.params_.SetEePolyDuration(0.25, 1.0);
  formulation.params_.SetTotalTime(2.0);
  
  // 3. 构建优化问题
  SplineHolder solution;
  ifopt::Problem nlp;
  
  for (auto c : formulation.GetVariableSets(solution))
    nlp.AddVariableSet(c);
  for (auto c : formulation.GetConstraints(solution))
    nlp.AddConstraintSet(c);
  for (auto c : formulation.GetCosts())
    nlp.AddCostSet(c);
  
  // 4. 求解优化问题
  std::cout << "🔧 正在求解优化问题...\n";
  auto solver = std::make_shared<ifopt::IpoptSolver>();
  solver->Solve(nlp);
  
  std::cout << "✅ 优化完成！\n";
  std::cout << "📊 优化结果统计：\n";
  std::cout << "   • 总变量数: " << nlp.GetNumberOfOptimizationVariables() << "\n";
  std::cout << "   • 总约束数: " << nlp.GetNumberOfConstraints() << "\n";
  std::cout << "   • 运动总时间: " << solution.base_linear_->GetTotalTime() << " 秒\n";
  
  // 计算一些统计信息
  double max_height = 0.0;
  double total_time = solution.base_linear_->GetTotalTime();
  for (double t = 0.0; t <= total_time; t += 0.01) {
    double height = solution.base_linear_->GetPoint(t).p().z();
    max_height = std::max(max_height, height);
  }
  std::cout << "   • 最大跳跃高度: " << max_height << " 米\n";
  std::cout << "   • 平均速度: " << 1.0 / total_time << " 米/秒\n\n";

#ifdef TOWR_WITH_MESHCAT
  // 5. MeshCat可视化
  std::cout << "🌐 启动MeshCat可视化...\n";
  
  auto visualizer = std::make_shared<MeshcatVisualizer>(7000);
  visualizer->Initialize(robot_model);
  
  // 设置地形可视化
  visualizer->SetTerrain(formulation.terrain_, {-0.5, 1.5}, {-0.5, 0.5}, 0.2);
  
  std::cout << "📱 请在浏览器中打开: " << visualizer->GetUrl() << "\n";
  std::cout << "⏳ 等待5秒让您打开浏览器...\n";
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  // 显示静态轨迹
  std::cout << "📊 显示完整轨迹...\n";
  visualizer->VisualizeTrajectory(solution, 0.02);
  
  std::cout << "⏳ 等待3秒查看静态轨迹...\n";
  std::this_thread::sleep_for(std::chrono::seconds(3));
  
  // 播放动画
  std::cout << "▶️  开始播放动画 (播放3次)...\n";
  for (int i = 0; i < 3; ++i) {
    std::cout << "   播放第 " << (i+1) << " 次...\n";
    visualizer->PlayTrajectory(solution, 1.0, 0.05, false);
    if (i < 2) {
      std::cout << "   ⏸️  暂停2秒...\n";
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }
  
  std::cout << "\n🎉 演示完成！\n";
  std::cout << "💡 提示：\n";
  std::cout << "   • 您可以在浏览器中拖拽鼠标旋转视角\n";
  std::cout << "   • 滚轮可以缩放场景\n";
  std::cout << "   • 蓝色轨迹：机器人本体运动路径\n";
  std::cout << "   • 红色轨迹：脚部运动路径\n";
  std::cout << "   • 红色球：脚接触地面\n";
  std::cout << "   • 绿色球：脚腾空状态\n";
  std::cout << "   • 黄色箭头：接触力向量\n";
  std::cout << "   • 灰色线：腿部连接\n\n";
  
  std::cout << "按回车键退出...";
  std::cin.get();
  
#else
  std::cout << "❌ MeshCat支持未启用\n";
  std::cout << "💡 要启用MeshCat可视化，请：\n";
  std::cout << "   1. 安装MeshCat-cpp库\n";
  std::cout << "   2. 重新编译TOWR\n";
  std::cout << "   3. 确保CMake能找到MeshcatCpp\n\n";
#endif
}

int main() {
  try {
    PrintWelcomeMessage();
    DemonstrateMonopedWithMeshCat();
    
    std::cout << "🎯 演示程序成功完成！\n";
    return 0;
    
  } catch (const std::exception& e) {
    std::cerr << "❌ 错误: " << e.what() << std::endl;
    return -1;
  }
}
