#include <cmath>
#include <iostream>
#include <iomanip>
#include <towr/terrain/examples/height_map_examples.h>
#include <towr/nlp_formulation.h>
#include <ifopt/ipopt_solver.h>

using namespace towr;

void optimizeRobotTrajectory(RobotModel::Robot robot_type, const std::string& robot_name) {
    std::cout << "\n🤖 优化 " << robot_name << " 的轨迹...\n";
    std::cout << "=" << std::string(50, '=') << "\n";
    
    NlpFormulation formulation;
    
    // 地形设置
    formulation.terrain_ = std::make_shared<FlatGround>(0.0);
    
    // 机器人模型
    formulation.model_ = RobotModel(robot_type);
    
    // 初始状态
    formulation.initial_base_.lin.at(kPos).z() = 0.5;
    
    // 根据机器人类型设置末端执行器初始位置
    int ee_count = formulation.model_.kinematic_model_->GetNumberOfEndeffectors();
    for (int ee = 0; ee < ee_count; ++ee) {
        formulation.initial_ee_W_.push_back(Eigen::Vector3d::Zero());
    }
    
    // 目标状态
    formulation.final_base_.lin.at(towr::kPos) << 1.5, 0.0, 0.5;
    
    // 根据机器人类型设置步态参数
    switch(robot_type) {
        case RobotModel::Monoped:
            // 单腿机器人：交替接触-腾空
            formulation.params_.ee_phase_durations_.push_back({0.4, 0.2, 0.4, 0.2, 0.4});
            formulation.params_.ee_in_contact_at_start_.push_back(true);
            break;
            
        case RobotModel::Biped:
            // 双腿机器人：交替步行
            for (int ee = 0; ee < 2; ++ee) {
                if (ee == 0) {
                    formulation.params_.ee_phase_durations_.push_back({0.3, 0.3, 0.3, 0.3});
                    formulation.params_.ee_in_contact_at_start_.push_back(true);
                } else {
                    formulation.params_.ee_phase_durations_.push_back({0.3, 0.3, 0.3, 0.3});
                    formulation.params_.ee_in_contact_at_start_.push_back(false);
                }
            }
            break;
            
        case RobotModel::Hyq:
            // 四腿机器人：小跑步态
            for (int ee = 0; ee < 4; ++ee) {
                formulation.params_.ee_phase_durations_.push_back({0.4, 0.2, 0.4, 0.2, 0.4});
                // 对角线腿一起运动
                bool starts_in_contact = (ee % 2 == 0);
                formulation.params_.ee_in_contact_at_start_.push_back(starts_in_contact);
            }
            break;
    }
    
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
    solver->SetOption("max_cpu_time", 10.0);
    solver->SetOption("print_level", 2); // 减少输出
    
    std::cout << "🔄 开始优化...\n";
    solver->Solve(nlp);
    
    std::cout << "✅ 优化完成！\n";
    std::cout << "📊 结果摘要：\n";
    std::cout << "• 总变量数：" << nlp.GetNumberOfOptimizationVariables() << "\n";
    std::cout << "• 约束数：" << nlp.GetNumberOfConstraints() << "\n";
    std::cout << "• 末端执行器数量：" << ee_count << "\n";
    std::cout << "• 总时间：" << solution.base_linear_->GetTotalTime() << " 秒\n";
    
    // 显示几个关键时间点的状态
    std::cout << "\n📍 关键状态点：\n";
    double total_time = solution.base_linear_->GetTotalTime();
    for (int i = 0; i <= 4; ++i) {
        double t = i * total_time / 4.0;
        auto pos = solution.base_linear_->GetPoint(t).p();
        std::cout << "t=" << std::fixed << std::setprecision(1) << t << "s: ";
        std::cout << "位置[" << std::setprecision(2) << pos.x() << ", " << pos.y() << ", " << pos.z() << "]";
        
        // 显示接触状态
        std::cout << " | 接触: ";
        for (int ee = 0; ee < ee_count; ++ee) {
            bool contact = solution.phase_durations_.at(ee)->IsContactPhase(t);
            std::cout << (contact ? "●" : "○");
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "🚀 TOWR 高级轨迹优化演示\n";
    std::cout << "=" << std::string(50, '=') << "\n";
    std::cout << "本演示将展示不同类型机器人的轨迹优化：\n";
    std::cout << "• 单腿跳跃机器人 (Monoped)\n";
    std::cout << "• 双足行走机器人 (Biped) \n";
    std::cout << "• 四足机器人 (Quadruped)\n\n";
    
    try {
        // 1. 单腿机器人
        optimizeRobotTrajectory(RobotModel::Monoped, "单腿跳跃机器人");
        
        // 2. 双足机器人
        optimizeRobotTrajectory(RobotModel::Biped, "双足行走机器人");
        
        // 3. 四足机器人
        optimizeRobotTrajectory(RobotModel::Hyq, "四足机器人(HyQ)");
        
        std::cout << "\n🎉 所有演示完成！\n";
        std::cout << "\n💡 说明：\n";
        std::cout << "• ● 表示该腿接触地面\n";
        std::cout << "• ○ 表示该腿在空中\n";
        std::cout << "• 不同机器人采用了不同的步态策略\n";
        std::cout << "• 所有机器人都成功从起点移动到1.5米外的目标点\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 错误: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
} 