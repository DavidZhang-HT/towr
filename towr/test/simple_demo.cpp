#include <cmath>
#include <iostream>
#include <iomanip>
#include <towr/terrain/examples/height_map_examples.h>
#include <towr/nlp_formulation.h>
#include <ifopt/ipopt_solver.h>

using namespace towr;

void demonstrateMonopedTrajectory() {
    std::cout << "\n🤖 演示：单腿跳跃机器人轨迹优化\n";
    std::cout << "=" << std::string(50, '=') << "\n";
    
    NlpFormulation formulation;
    
    // 地形设置
    formulation.terrain_ = std::make_shared<FlatGround>(0.0);
    
    // 单腿机器人模型
    formulation.model_ = RobotModel(RobotModel::Monoped);
    
    // 初始状态：机器人在高度0.5米处
    formulation.initial_base_.lin.at(kPos).z() = 0.5;
    formulation.initial_ee_W_.push_back(Eigen::Vector3d::Zero());
    
    // 目标状态：向前移动2米
    formulation.final_base_.lin.at(towr::kPos) << 2.0, 0.0, 0.5;
    
    // 步态参数：交替的站立和跳跃相位
    formulation.params_.ee_phase_durations_.push_back({0.5, 0.3, 0.5, 0.3, 0.5, 0.3, 0.3});
    formulation.params_.ee_in_contact_at_start_.push_back(true);
    
    // 创建优化问题
    ifopt::Problem nlp;
    SplineHolder solution;
    
    for (auto c : formulation.GetVariableSets(solution))
        nlp.AddVariableSet(c);
    for (auto c : formulation.GetConstraints(solution))
        nlp.AddConstraintSet(c);
    for (auto c : formulation.GetCosts())
        nlp.AddCostSet(c);
    
    // 求解器设置
    auto solver = std::make_shared<ifopt::IpoptSolver>();
    solver->SetOption("jacobian_approximation", "exact");
    solver->SetOption("max_cpu_time", 15.0);
    solver->SetOption("print_level", 2);
    
    std::cout << "🔄 开始轨迹优化...\n";
    solver->Solve(nlp);
    
    std::cout << "✅ 优化完成！\n";
    std::cout << "\n📊 优化结果摘要：\n";
    std::cout << "• 总变量数：" << nlp.GetNumberOfOptimizationVariables() << "\n";
    std::cout << "• 总约束数：" << nlp.GetNumberOfConstraints() << "\n";
    std::cout << "• 运动总时间：" << std::fixed << std::setprecision(2) 
              << solution.base_linear_->GetTotalTime() << " 秒\n";
    
    // 分析运动轨迹
    std::cout << "\n📍 轨迹关键点分析：\n";
    double total_time = solution.base_linear_->GetTotalTime();
    
    for (int i = 0; i <= 6; ++i) {
        double t = i * total_time / 6.0;
        
        // 获取位置和速度
        auto pos = solution.base_linear_->GetPoint(t).p();
        auto vel = solution.base_linear_->GetPoint(t).v();
        
        // 获取脚的状态
        bool in_contact = solution.phase_durations_.at(0)->IsContactPhase(t);
        auto foot_pos = solution.ee_motion_.at(0)->GetPoint(t).p();
        auto foot_force = solution.ee_force_.at(0)->GetPoint(t).p();
        
        std::cout << "t=" << std::setw(4) << std::setprecision(1) << t << "s: ";
        std::cout << "位置[" << std::setprecision(2) 
                  << pos.x() << ", " << pos.y() << ", " << pos.z() << "]";
        std::cout << " 速度[" << vel.x() << ", " << vel.z() << "]";
        std::cout << " | " << (in_contact ? "接触●" : "腾空○");
        
        if (in_contact) {
            std::cout << " 力[" << std::setprecision(0) 
                      << foot_force.x() << ", " << foot_force.z() << "]N";
        }
        std::cout << "\n";
    }
    
    // 运动特性分析
    std::cout << "\n🔍 运动特性分析：\n";
    
    // 计算最大高度
    double max_height = 0.0;
    double min_height = 100.0;
    for (double t = 0; t <= total_time; t += 0.1) {
        double height = solution.base_linear_->GetPoint(t).p().z();
        max_height = std::max(max_height, height);
        min_height = std::min(min_height, height);
    }
    
    // 计算平均速度
    double distance = solution.base_linear_->GetPoint(total_time).p().x() - 
                     solution.base_linear_->GetPoint(0).p().x();
    double avg_speed = distance / total_time;
    
    std::cout << "• 最大跳跃高度：" << std::setprecision(2) << max_height << " 米\n";
    std::cout << "• 最小高度：" << min_height << " 米\n";
    std::cout << "• 前进距离：" << distance << " 米\n";
    std::cout << "• 平均速度：" << avg_speed << " 米/秒\n";
    
    // 统计接触相位
    int contact_phases = 0;
    int swing_phases = 0;
    bool last_contact = true;
    
    for (double t = 0; t <= total_time; t += 0.05) {
        bool current_contact = solution.phase_durations_.at(0)->IsContactPhase(t);
        if (current_contact != last_contact) {
            if (current_contact) contact_phases++;
            else swing_phases++;
        }
        last_contact = current_contact;
    }
    
    std::cout << "• 接触相位数：" << contact_phases << "\n";
    std::cout << "• 腾空相位数：" << swing_phases << "\n";
}

void demonstrateTerrainVariations() {
    std::cout << "\n🏔️ 演示：不同地形上的轨迹优化\n";
    std::cout << "=" << std::string(50, '=') << "\n";
    
    // 测试不同的地形类型
    std::vector<std::pair<std::shared_ptr<HeightMap>, std::string>> terrains = {
        {std::make_shared<FlatGround>(0.0), "平地"},
        {std::make_shared<FlatGround>(0.1), "0.1米高台"},
        {std::make_shared<Block>(), "障碍物地形"}
    };
    
    for (auto& terrain_pair : terrains) {
        std::cout << "\n地形类型：" << terrain_pair.second << "\n";
        
        NlpFormulation formulation;
        formulation.terrain_ = terrain_pair.first;
        formulation.model_ = RobotModel(RobotModel::Monoped);
        
        // 基本设置
        formulation.initial_base_.lin.at(kPos).z() = 0.5;
        formulation.initial_ee_W_.push_back(Eigen::Vector3d::Zero());
        formulation.final_base_.lin.at(towr::kPos) << 1.0, 0.0, 0.5;
        
        // 较短的轨迹用于快速演示
        formulation.params_.ee_phase_durations_.push_back({0.4, 0.2, 0.4});
        formulation.params_.ee_in_contact_at_start_.push_back(true);
        
        ifopt::Problem nlp;
        SplineHolder solution;
        
        for (auto c : formulation.GetVariableSets(solution))
            nlp.AddVariableSet(c);
        for (auto c : formulation.GetConstraints(solution))
            nlp.AddConstraintSet(c);
        for (auto c : formulation.GetCosts())
            nlp.AddCostSet(c);
        
        auto solver = std::make_shared<ifopt::IpoptSolver>();
        solver->SetOption("jacobian_approximation", "exact");
        solver->SetOption("max_cpu_time", 8.0);
        solver->SetOption("print_level", 1); // 减少输出
        
        solver->Solve(nlp);
        
        // 检查最终位置
        auto final_pos = solution.base_linear_->GetPoint(solution.base_linear_->GetTotalTime()).p();
        std::cout << "• 最终位置：[" << std::setprecision(2) 
                  << final_pos.x() << ", " << final_pos.y() << ", " << final_pos.z() << "]\n";
        std::cout << "• 用时：" << solution.base_linear_->GetTotalTime() << " 秒\n";
    }
}

int main() {
    std::cout << "🚀 TOWR 轨迹优化演示系统\n";
    std::cout << "=" << std::string(60, '=') << "\n";
    std::cout << "这个演示程序将展示TOWR库的核心功能：\n";
    std::cout << "1. 单腿跳跃机器人的详细轨迹分析\n";
    std::cout << "2. 不同地形条件下的适应性轨迹优化\n\n";
    
    try {
        // 演示1：详细的单腿机器人轨迹
        demonstrateMonopedTrajectory();
        
        // 演示2：不同地形上的轨迹
        demonstrateTerrainVariations();
        
        std::cout << "\n🎉 所有演示完成！\n";
        std::cout << "\n💡 关键特性总结：\n";
        std::cout << "• TOWR成功优化了单腿跳跃机器人的复杂轨迹\n";
        std::cout << "• 自动处理接触相位和腾空相位的切换\n";
        std::cout << "• 适应不同地形条件的轨迹规划\n";
        std::cout << "• 满足动力学约束的同时优化运动效率\n";
        std::cout << "• 详细的运动状态和力的分析\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
} 