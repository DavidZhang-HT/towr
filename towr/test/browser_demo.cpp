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
 * @file browser_demo.cpp
 * @brief TOWR 浏览器可视化完整演示程序
 * 
 * 这个程序展示了TOWR的完整浏览器可视化功能，包括：
 * - 多种机器人类型的轨迹优化
 * - 交互式3D可视化
 * - 实时动画播放
 * - 目标点和路径可视化
 * - 用户友好的界面
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <map>

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

// Demo配置结构
struct DemoConfig {
  RobotModel::Robot robot_type;
  std::string robot_name;
  Eigen::Vector3d initial_position;
  Eigen::Vector3d target_position;
  double total_time;
  std::string description;
  Eigen::Vector3d camera_position;
  Eigen::Vector3d camera_target;
};

class TowrBrowserDemo {
public:
  TowrBrowserDemo() {
    SetupDemoConfigurations();
  }
  
  void RunInteractiveDemo();
  
private:
  std::vector<DemoConfig> demo_configs_;
  
  void SetupDemoConfigurations();
  void PrintWelcomeMessage();
  void PrintDemoMenu();
  int GetUserChoice();
  void RunSingleDemo(const DemoConfig& config);
  bool SolveTrajectoryOptimization(const DemoConfig& config, SplineHolder& solution);
  void VisualizeResults(const DemoConfig& config, const SplineHolder& solution);
  void PrintDemoResults(const DemoConfig& config, const SplineHolder& solution);
  void WaitForUserInput(const std::string& message);
};

void TowrBrowserDemo::SetupDemoConfigurations() {
  demo_configs_ = {
    {
      RobotModel::Monoped,
      "单腿跳跃机器人",
      Eigen::Vector3d(0.0, 0.0, 0.5),
      Eigen::Vector3d(1.5, 0.0, 0.5),
      2.0,
      "展示单腿机器人的跳跃运动，包含腾空和着陆相位",
      Eigen::Vector3d(2.0, -1.5, 1.0),
      Eigen::Vector3d(0.75, 0.0, 0.5)
    },
    {
      RobotModel::Biped,
      "双腿行走机器人", 
      Eigen::Vector3d(0.0, 0.0, 0.87),
      Eigen::Vector3d(2.0, 0.0, 0.87),
      3.0,
      "展示双腿机器人的行走步态，左右脚交替接触",
      Eigen::Vector3d(3.0, -2.0, 1.5),
      Eigen::Vector3d(1.0, 0.0, 0.87)
    },
    {
      RobotModel::Quadruped,
      "四腿奔跑机器人",
      Eigen::Vector3d(0.0, 0.0, 0.5),
      Eigen::Vector3d(2.5, 0.0, 0.5),
      2.5,
      "展示四腿机器人的奔跑步态，对角腿协调运动",
      Eigen::Vector3d(3.5, -2.5, 1.2),
      Eigen::Vector3d(1.25, 0.0, 0.5)
    }
  };
}

void TowrBrowserDemo::PrintWelcomeMessage() {
  std::cout << "\n";
  std::cout << "🌐 TOWR 浏览器可视化演示系统\n";
  std::cout << "=" << std::string(60, '=') << "\n";
  std::cout << "欢迎使用TOWR的交互式浏览器可视化系统！\n\n";
  std::cout << "🎯 功能特色：\n";
  std::cout << "  • 🤖 多种机器人类型（单腿、双腿、四腿）\n";
  std::cout << "  • 🌐 基于浏览器的3D可视化\n";
  std::cout << "  • 🎬 实时轨迹动画播放\n";
  std::cout << "  • 🎨 接触力和相位可视化\n";
  std::cout << "  • 🖱️ 交互式场景控制\n";
  std::cout << "  • 📊 详细的优化结果分析\n\n";
  
#ifdef TOWR_WITH_MESHCAT
  std::cout << "✅ MeshCat可视化已启用\n";
#else
  std::cout << "❌ MeshCat可视化未启用\n";
  std::cout << "💡 要启用可视化，请安装MeshCat-cpp并重新编译\n";
#endif
  std::cout << "\n";
}

void TowrBrowserDemo::PrintDemoMenu() {
  std::cout << "📋 可用演示列表：\n";
  std::cout << "─" << std::string(50, '─') << "\n";
  
  for (size_t i = 0; i < demo_configs_.size(); ++i) {
    const auto& config = demo_configs_[i];
    std::cout << "  " << (i + 1) << ". " << config.robot_name << "\n";
    std::cout << "     📍 " << config.description << "\n";
    std::cout << "     🎯 目标: (" << config.target_position.x() 
              << ", " << config.target_position.y() 
              << ", " << config.target_position.z() << ") 米\n";
    std::cout << "     ⏱️  时长: " << config.total_time << " 秒\n\n";
  }
  
  std::cout << "  " << (demo_configs_.size() + 1) << ". 🔄 运行所有演示\n";
  std::cout << "  " << (demo_configs_.size() + 2) << ". ❌ 退出程序\n\n";
}

int TowrBrowserDemo::GetUserChoice() {
  int choice;
  std::cout << "请选择演示 (1-" << (demo_configs_.size() + 2) << "): ";
  std::cin >> choice;
  return choice;
}

void TowrBrowserDemo::RunInteractiveDemo() {
  PrintWelcomeMessage();
  
  while (true) {
    PrintDemoMenu();
    int choice = GetUserChoice();
    
    if (choice >= 1 && choice <= static_cast<int>(demo_configs_.size())) {
      // 运行单个演示
      RunSingleDemo(demo_configs_[choice - 1]);
    }
    else if (choice == static_cast<int>(demo_configs_.size()) + 1) {
      // 运行所有演示
      std::cout << "\n🎬 开始运行所有演示...\n";
      for (const auto& config : demo_configs_) {
        RunSingleDemo(config);
        if (&config != &demo_configs_.back()) {
          WaitForUserInput("按回车键继续下一个演示...");
        }
      }
      std::cout << "\n🎉 所有演示完成！\n";
    }
    else if (choice == static_cast<int>(demo_configs_.size()) + 2) {
      // 退出
      std::cout << "\n👋 感谢使用TOWR浏览器可视化演示！\n";
      break;
    }
    else {
      std::cout << "\n❌ 无效选择，请重新输入。\n\n";
    }
  }
}

bool TowrBrowserDemo::SolveTrajectoryOptimization(const DemoConfig& config, SplineHolder& solution) {
  std::cout << "\n🔧 设置轨迹优化问题...\n";
  
  // 创建机器人模型
  RobotModel robot_model(config.robot_type);
  
  // 设置NLP问题
  NlpFormulation formulation;
  formulation.model_ = robot_model;
  
  // 设置初始和目标状态
  formulation.initial_base_.lin.at(kPos) = config.initial_position;
  formulation.initial_base_.ang.at(kPos) = Eigen::Vector3d::Zero();
  
  formulation.final_base_.lin.at(kPos) = config.target_position;
  formulation.final_base_.ang.at(kPos) = Eigen::Vector3d::Zero();
  
  // 设置初始脚位置
  int n_ee = robot_model.kinematic_model_->GetNumberOfEndeffectors();
  for (int ee = 0; ee < n_ee; ++ee) {
    Eigen::Vector3d initial_foot_pos = config.initial_position;
    initial_foot_pos.z() = 0.0;
    
    // 为不同的脚设置不同的初始位置
    if (n_ee == 2) {  // 双腿
      initial_foot_pos.y() += (ee == 0) ? 0.1 : -0.1;
    } else if (n_ee == 4) {  // 四腿
      initial_foot_pos.x() += (ee < 2) ? 0.2 : -0.2;
      initial_foot_pos.y() += (ee % 2 == 0) ? 0.15 : -0.15;
    }
    
    formulation.initial_ee_W_.push_back(initial_foot_pos);
  }
  
  // 设置地形
  formulation.terrain_ = std::make_shared<FlatGround>(0.0);
  
  // 设置参数
  formulation.params_.SetBasePolyDuration(0.5, config.total_time / 2.0);
  formulation.params_.SetEePolyDuration(0.25, config.total_time / 4.0);
  formulation.params_.SetTotalTime(config.total_time);
  
  // 构建优化问题
  ifopt::Problem nlp;
  
  for (auto c : formulation.GetVariableSets(solution))
    nlp.AddVariableSet(c);
  for (auto c : formulation.GetConstraints(solution))
    nlp.AddConstraintSet(c);
  for (auto c : formulation.GetCosts())
    nlp.AddCostSet(c);
  
  // 求解优化问题
  std::cout << "🚀 开始求解 " << config.robot_name << " 轨迹优化...\n";
  auto solver = std::make_shared<ifopt::IpoptSolver>();
  solver->Solve(nlp);
  
  std::cout << "✅ 优化完成！\n";
  return true;
}

void TowrBrowserDemo::RunSingleDemo(const DemoConfig& config) {
  std::cout << "\n" << std::string(60, '=') << "\n";
  std::cout << "🤖 " << config.robot_name << " 演示\n";
  std::cout << std::string(60, '=') << "\n";
  std::cout << "📝 " << config.description << "\n";
  
  SplineHolder solution;
  if (!SolveTrajectoryOptimization(config, solution)) {
    std::cout << "❌ 轨迹优化失败！\n";
    return;
  }
  
  PrintDemoResults(config, solution);
  
#ifdef TOWR_WITH_MESHCAT
  VisualizeResults(config, solution);
#else
  std::cout << "\n⚠️  MeshCat可视化未启用，跳过可视化部分\n";
#endif
}

void TowrBrowserDemo::PrintDemoResults(const DemoConfig& config, const SplineHolder& solution) {
  std::cout << "\n📊 优化结果分析：\n";
  std::cout << "─" << std::string(40, '─') << "\n";
  
  double total_time = solution.base_linear_->GetTotalTime();
  std::cout << "⏱️  总运动时间: " << total_time << " 秒\n";
  
  // 计算最大高度
  double max_height = 0.0;
  for (double t = 0.0; t <= total_time; t += 0.01) {
    double height = solution.base_linear_->GetPoint(t).p().z();
    max_height = std::max(max_height, height);
  }
  std::cout << "📏 最大运动高度: " << max_height << " 米\n";
  
  // 计算平均速度
  double distance = (config.target_position - config.initial_position).norm();
  std::cout << "📐 总移动距离: " << distance << " 米\n";
  std::cout << "🏃 平均移动速度: " << distance / total_time << " 米/秒\n";
  
  // 计算末端执行器数量
  int n_ee = solution.ee_motion_.size();
  std::cout << "🦶 末端执行器数量: " << n_ee << " 个\n";
}

void TowrBrowserDemo::VisualizeResults(const DemoConfig& config, const SplineHolder& solution) {
  std::cout << "\n🌐 启动浏览器可视化...\n";
  
  // 创建可视化器
  auto visualizer = std::make_shared<MeshcatVisualizer>(7000);
  
  // 初始化机器人模型
  RobotModel robot_model(config.robot_type);
  visualizer->Initialize(robot_model);
  
  // 设置相机视角
  visualizer->SetCameraView(config.camera_position, config.camera_target);
  
  // 添加目标点标记
  visualizer->AddTargetMarker(config.target_position, "target", 
                             Eigen::Vector3d(1.0, 0.0, 0.0), 0.05);
  
  // 添加起始点标记
  visualizer->AddTargetMarker(config.initial_position, "start", 
                             Eigen::Vector3d(0.0, 1.0, 0.0), 0.03);
  
  std::cout << "📱 浏览器可视化已启动: " << visualizer->GetUrl() << "\n";
  std::cout << "💡 请在浏览器中打开上述链接查看3D可视化\n";
  
  WaitForUserInput("准备好后按回车键开始播放轨迹...");
  
  // 显示完整轨迹
  std::cout << "📊 显示完整轨迹路径...\n";
  visualizer->VisualizeTrajectory(solution, 0.02);
  
  WaitForUserInput("按回车键开始播放动画...");
  
  // 播放动画
  std::cout << "▶️  播放轨迹动画...\n";
  std::cout << "🎮 浏览器控制说明：\n";
  std::cout << "   • 鼠标左键拖拽: 旋转视角\n";
  std::cout << "   • 鼠标滚轮: 缩放场景\n";
  std::cout << "   • 鼠标右键拖拽: 平移视角\n\n";
  
  visualizer->PlayTrajectory(solution, 1.0, 0.05, false);
  
  std::cout << "\n🎉 " << config.robot_name << " 演示完成！\n";
  
  WaitForUserInput("按回车键继续...");
}

void TowrBrowserDemo::WaitForUserInput(const std::string& message) {
  std::cout << message;
  std::cin.ignore();
  std::cin.get();
}

int main() {
  try {
    TowrBrowserDemo demo;
    demo.RunInteractiveDemo();
    
    std::cout << "\n🎯 演示程序成功完成！\n";
    return 0;
    
  } catch (const std::exception& e) {
    std::cerr << "❌ 错误: " << e.what() << std::endl;
    return -1;
  }
}
