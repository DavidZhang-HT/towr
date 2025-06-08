#!/usr/bin/env python3
"""
TOWR MeshCat集成演示脚本 (简化版)

这个脚本演示了如何将TOWR与MeshCat可视化集成，
不依赖额外的Python库，纯文本展示集成效果。
"""

import time
import math

def print_welcome():
    """打印欢迎信息"""
    print("\n🚀 TOWR MeshCat 集成演示")
    print("=" * 60)
    print("本演示展示了TOWR与MeshCat 3D可视化的集成效果")
    print("包括以下功能：")
    print("• 实时3D机器人模型可视化")
    print("• 轨迹动画播放")
    print("• 接触力向量显示")
    print("• 交互式场景浏览")
    print("• 多机器人类型支持")
    print()

def simulate_towr_optimization():
    """模拟TOWR轨迹优化过程"""
    print("🤖 模拟单腿跳跃机器人轨迹优化...")
    
    # 模拟优化过程
    for i in range(5):
        print(f"   迭代 {i+1}/5: 优化中...", end="", flush=True)
        time.sleep(0.3)
        print(" ✓")
    
    print("✅ 轨迹优化完成！")
    print("📊 优化结果:")
    print("   • 总变量数: 423")
    print("   • 总约束数: 501") 
    print("   • 运动总时间: 2.70 秒")
    print("   • 最大跳跃高度: 0.86 米")
    print("   • 平均速度: 0.74 米/秒")
    print()

def generate_sample_trajectory():
    """生成示例轨迹数据"""
    trajectory_points = []
    
    for i in range(27):  # 2.7秒，每0.1秒一个点
        t = i * 0.1
        
        # 机器人本体位置 (跳跃运动)
        base_x = t * 0.37  # 向前移动
        base_y = 0.0
        base_z = 0.5 + 0.3 * math.sin(3 * math.pi * t) * math.exp(-0.5 * t)
        
        # 脚部位置 (接触-腾空循环)
        phase = (t % 0.9)  # 0.9秒一个周期
        if phase < 0.4:  # 接触相位
            foot_x = base_x
            foot_z = 0.0
            contact = True
            force_x = 50 * math.sin(0.5 * t)
            force_z = 300 + 100 * math.sin(2 * t)
        else:  # 腾空相位
            foot_x = base_x + 0.1 * math.sin(math.pi * (phase - 0.4) / 0.5)
            foot_z = 0.1 * math.sin(math.pi * (phase - 0.4) / 0.5)
            contact = False
            force_x = 0
            force_z = 0
        
        trajectory_points.append({
            'time': t,
            'base_pos': [base_x, base_y, base_z],
            'foot_pos': [foot_x, 0.0, foot_z],
            'contact': contact,
            'force': [force_x, 0.0, force_z]
        })
    
    return trajectory_points

def visualize_ascii_trajectory(trajectory_points):
    """ASCII艺术可视化轨迹"""
    print("🎨 ASCII轨迹可视化:")
    print()
    
    # 创建ASCII图表显示高度变化
    print("机器人高度变化 (Z轴):")
    print("高度(m) |")
    
    max_height = max(point['base_pos'][2] for point in trajectory_points)
    min_height = min(point['base_pos'][2] for point in trajectory_points)
    
    for height_level in [0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0]:
        line = f"  {height_level:.1f}   |"
        for point in trajectory_points:
            if abs(point['base_pos'][2] - height_level) < 0.05:
                line += "●"
            elif point['contact'] and height_level < 0.1:
                line += "■"  # 地面接触
            else:
                line += " "
        print(line)
    
    print("        +" + "-" * len(trajectory_points))
    print("         " + "".join([str(int(p['time'])) if int(p['time']) == p['time'] else " " 
                                for p in trajectory_points]))
    print("                                时间(s)")
    print()

def show_contact_phases(trajectory_points):
    """显示接触相位"""
    print("🦶 接触相位可视化:")
    print("接触: ■  腾空: □")
    print()
    
    contact_line = "接触状态: "
    force_line =   "接触力:   "
    
    for point in trajectory_points:
        if point['contact']:
            contact_line += "■"
            force_magnitude = (point['force'][0]**2 + point['force'][2]**2)**0.5
            if force_magnitude > 200:
                force_line += "▲"  # 大力
            elif force_magnitude > 100:
                force_line += "△"  # 中力
            else:
                force_line += "·"  # 小力
        else:
            contact_line += "□"
            force_line += " "
    
    print(contact_line)
    print(force_line)
    print("时间轴:   " + "".join([str(int(p['time'])) if int(p['time']) == p['time'] else "·" 
                                for p in trajectory_points]))
    print()

def demonstrate_meshcat_features():
    """演示MeshCat功能特性"""
    print("🎮 MeshCat可视化功能特性:")
    print()
    
    features = [
        ("🌐 Web浏览器可视化", "在任何现代浏览器中查看3D场景"),
        ("🖱️ 交互式控制", "鼠标拖拽旋转、滚轮缩放、右键平移"),
        ("🎬 实时动画", "流畅的轨迹播放和状态更新"),
        ("🤖 多机器人支持", "单腿、双腿、四腿机器人模型"),
        ("⚡ 力向量显示", "实时显示接触力的大小和方向"),
        ("🎨 相位区分", "不同颜色区分接触和腾空状态"),
        ("🗺️ 地形可视化", "支持复杂地形高度图显示"),
        ("📊 数据导出", "支持轨迹数据导出和分析")
    ]
    
    for feature, description in features:
        print(f"   {feature}: {description}")
        time.sleep(0.2)
    
    print()

def show_integration_code():
    """展示集成代码示例"""
    print("💻 TOWR MeshCat集成代码示例:")
    print()
    
    code_example = '''
// C++ 集成示例
#include <towr/visualization/meshcat_visualizer.h>

int main() {
    // 1. 创建TOWR轨迹优化
    NlpFormulation formulation;
    formulation.model_ = RobotModel(RobotModel::Monoped);
    formulation.initial_base_.lin.at(kPos) = Vector3d(0.0, 0.0, 0.5);
    formulation.final_base_.lin.at(kPos) = Vector3d(1.0, 0.0, 0.5);
    
    // 2. 求解优化问题
    ifopt::Problem nlp;
    SplineHolder solution;
    for (auto c : formulation.GetVariableSets(solution))
        nlp.AddVariableSet(c);
    for (auto c : formulation.GetConstraints(solution))
        nlp.AddConstraintSet(c);
    solver->Solve(nlp);
    
    // 3. 启动MeshCat可视化
    auto visualizer = std::make_shared<MeshcatVisualizer>(7000);
    visualizer->Initialize(formulation.model_);
    
    // 4. 显示轨迹
    visualizer->VisualizeTrajectory(solution, 0.02);
    
    // 5. 播放动画
    visualizer->PlayTrajectory(solution, 1.0, 0.05, true);
    
    return 0;
}
'''
    
    print(code_example)
    print()

def simulate_meshcat_animation(trajectory_points):
    """模拟MeshCat动画播放"""
    print("🎬 模拟MeshCat动画播放:")
    print("(在实际MeshCat中，这将是流畅的3D动画)")
    print()
    
    for i, point in enumerate(trajectory_points[::3]):  # 每3个点显示一次
        # 清屏效果 (简化)
        if i > 0:
            print("\033[F" * 8)  # 向上移动8行
        
        print(f"时间: {point['time']:.1f}s")
        print(f"机器人位置: ({point['base_pos'][0]:.2f}, {point['base_pos'][1]:.2f}, {point['base_pos'][2]:.2f})")
        print(f"脚部位置:   ({point['foot_pos'][0]:.2f}, {point['foot_pos'][1]:.2f}, {point['foot_pos'][2]:.2f})")
        print(f"接触状态:   {'接触地面' if point['contact'] else '腾空中'}")
        print(f"接触力:     ({point['force'][0]:.0f}, {point['force'][1]:.0f}, {point['force'][2]:.0f}) N")
        print()
        
        # 简单的机器人状态可视化
        robot_height = int(point['base_pos'][2] * 10)  # 缩放到合适范围
        foot_contact = "■" if point['contact'] else "□"
        
        print("机器人状态:")
        for h in range(8, 0, -1):
            if h == robot_height:
                print("    🤖")  # 机器人本体
            elif h == 1 and point['contact']:
                print(f"    {foot_contact}")  # 脚部
            else:
                print("     ")
        print("  ━━━━━━━━  地面")
        
        time.sleep(0.5)
    
    print("\n🎉 动画播放完成！")
    print()

def main():
    """主函数"""
    print_welcome()
    
    # 模拟TOWR优化过程
    simulate_towr_optimization()
    
    # 生成示例轨迹
    print("📊 生成示例轨迹数据...")
    trajectory_points = generate_sample_trajectory()
    print("✅ 轨迹数据生成完成")
    print()
    
    # ASCII可视化
    visualize_ascii_trajectory(trajectory_points)
    
    # 显示接触相位
    show_contact_phases(trajectory_points)
    
    # 演示MeshCat功能
    demonstrate_meshcat_features()
    
    # 显示集成代码
    show_integration_code()
    
    # 模拟动画
    print("是否要观看模拟动画? (y/n): ", end="")
    try:
        response = input().lower()
        if response == 'y' or response == 'yes':
            simulate_meshcat_animation(trajectory_points)
    except:
        print("跳过动画演示")
    
    print("🎯 下一步操作:")
    print("1. 安装MeshCat-cpp: ./install_meshcat.sh")
    print("2. 重新编译TOWR: cd towr/build && cmake .. && make")
    print("3. 运行演示: ./towr-meshcat-demo")
    print("4. 在浏览器中查看: http://localhost:7000")
    print()
    
    print("📚 更多信息请查看: MESHCAT_VISUALIZATION.md")
    print()
    
    print("🎉 演示完成！感谢您的关注！")

if __name__ == "__main__":
    main()
