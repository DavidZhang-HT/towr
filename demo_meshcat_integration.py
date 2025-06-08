#!/usr/bin/env python3
"""
TOWR MeshCat集成演示脚本

这个脚本演示了如何将TOWR与MeshCat可视化集成，
即使在MeshCat-cpp库未安装的情况下也能展示集成效果。
"""

import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import time

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
        time.sleep(0.5)
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
    t = np.linspace(0, 2.7, 100)
    
    # 机器人本体轨迹 (跳跃运动)
    base_x = t * 0.37  # 向前移动
    base_y = np.zeros_like(t)
    base_z = 0.5 + 0.3 * np.sin(3 * np.pi * t) * np.exp(-0.5 * t)  # 跳跃高度
    
    # 脚部轨迹 (接触-腾空循环)
    foot_x = np.zeros_like(t)
    foot_y = np.zeros_like(t)
    foot_z = np.zeros_like(t)
    
    # 模拟步态：接触-腾空-接触-腾空
    for i, time_val in enumerate(t):
        phase = (time_val % 0.9)  # 0.9秒一个周期
        if phase < 0.4:  # 接触相位
            foot_x[i] = base_x[i]
            foot_z[i] = 0.0
        else:  # 腾空相位
            foot_x[i] = base_x[i] + 0.1 * np.sin(np.pi * (phase - 0.4) / 0.5)
            foot_z[i] = 0.1 * np.sin(np.pi * (phase - 0.4) / 0.5)
    
    # 接触状态
    contact_states = []
    for time_val in t:
        phase = (time_val % 0.9)
        contact_states.append(phase < 0.4)
    
    # 接触力 (只在接触时有力)
    forces = []
    for i, is_contact in enumerate(contact_states):
        if is_contact:
            fx = 50 * np.sin(0.5 * t[i])  # 水平推进力
            fz = 300 + 100 * np.sin(2 * t[i])  # 垂直支撑力
            forces.append([fx, 0, fz])
        else:
            forces.append([0, 0, 0])
    
    return {
        'time': t,
        'base_pos': np.column_stack([base_x, base_y, base_z]),
        'foot_pos': np.column_stack([foot_x, foot_y, foot_z]),
        'contact_states': contact_states,
        'forces': np.array(forces)
    }

def visualize_meshcat_integration(trajectory_data):
    """可视化MeshCat集成效果"""
    print("🌐 启动MeshCat可视化集成演示...")
    print("📱 在实际应用中，您将在浏览器中看到: http://localhost:7000")
    print()
    
    # 创建3D可视化
    fig = plt.figure(figsize=(16, 12))
    
    # 主要3D场景
    ax1 = fig.add_subplot(221, projection='3d')
    
    base_pos = trajectory_data['base_pos']
    foot_pos = trajectory_data['foot_pos']
    contact_states = trajectory_data['contact_states']
    forces = trajectory_data['forces']
    
    # 绘制机器人本体轨迹
    ax1.plot(base_pos[:, 0], base_pos[:, 1], base_pos[:, 2], 
             'b-', linewidth=3, label='机器人本体轨迹', alpha=0.8)
    
    # 绘制脚部轨迹，根据接触状态着色
    for i in range(len(foot_pos)):
        color = 'red' if contact_states[i] else 'green'
        marker = 'o' if contact_states[i] else '^'
        size = 60 if contact_states[i] else 30
        ax1.scatter(foot_pos[i, 0], foot_pos[i, 1], foot_pos[i, 2],
                   c=color, s=size, marker=marker, alpha=0.7)
    
    # 绘制腿部连接
    for i in range(0, len(base_pos), 5):  # 每5个点绘制一次
        ax1.plot([base_pos[i, 0], foot_pos[i, 0]], 
                [base_pos[i, 1], foot_pos[i, 1]], 
                [base_pos[i, 2], foot_pos[i, 2]], 
                'k--', alpha=0.3, linewidth=1)
    
    # 绘制接触力向量
    for i in range(0, len(forces), 10):  # 每10个点绘制一次力向量
        if contact_states[i] and np.linalg.norm(forces[i]) > 10:
            force_scale = 0.001
            start = foot_pos[i]
            end = start + forces[i] * force_scale
            ax1.quiver(start[0], start[1], start[2],
                      end[0]-start[0], end[1]-start[1], end[2]-start[2],
                      color='yellow', arrow_length_ratio=0.1, linewidth=2)
    
    ax1.set_xlabel('X Position (m)')
    ax1.set_ylabel('Y Position (m)')
    ax1.set_zlabel('Z Position (m)')
    ax1.set_title('MeshCat 3D可视化效果预览\n(实际效果为交互式3D场景)')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 时间-高度图
    ax2 = fig.add_subplot(222)
    ax2.plot(trajectory_data['time'], base_pos[:, 2], 'b-', linewidth=2, label='机器人本体高度')
    ax2.plot(trajectory_data['time'], foot_pos[:, 2], 'r-', linewidth=1, label='脚部高度')
    ax2.set_xlabel('时间 (s)')
    ax2.set_ylabel('高度 (m)')
    ax2.set_title('高度随时间变化')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # 接触相位图
    ax3 = fig.add_subplot(223)
    contact_array = np.array(contact_states, dtype=int)
    ax3.fill_between(trajectory_data['time'], 0, contact_array, 
                     alpha=0.3, color='red', label='接触相位')
    ax3.fill_between(trajectory_data['time'], contact_array, 1, 
                     alpha=0.3, color='green', label='腾空相位')
    ax3.plot(trajectory_data['time'], base_pos[:, 2] / max(base_pos[:, 2]), 
             'b-', linewidth=2, label='归一化高度')
    ax3.set_xlabel('时间 (s)')
    ax3.set_ylabel('相位状态')
    ax3.set_title('接触相位和运动高度')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 接触力图
    ax4 = fig.add_subplot(224)
    ax4.plot(trajectory_data['time'], forces[:, 0], 'r-', linewidth=2, label='水平力 Fx')
    ax4.plot(trajectory_data['time'], forces[:, 2], 'b-', linewidth=2, label='垂直力 Fz')
    ax4.set_xlabel('时间 (s)')
    ax4.set_ylabel('力 (N)')
    ax4.set_title('接触力随时间变化')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    return fig

def demonstrate_meshcat_features():
    """演示MeshCat功能特性"""
    print("🎮 MeshCat可视化功能特性演示:")
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
        time.sleep(0.3)
    
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
    // ... 设置优化参数 ...
    
    // 2. 求解优化问题
    ifopt::Problem nlp;
    // ... 添加变量和约束 ...
    solver->Solve(nlp);
    
    // 3. 启动MeshCat可视化
    auto visualizer = std::make_shared<MeshcatVisualizer>(7000);
    visualizer->Initialize(formulation.model_);
    
    // 4. 显示轨迹
    visualizer->VisualizeTrajectory(solution, 0.02);
    
    // 5. 播放动画
    visualizer->PlayTrajectory(solution, 1.0, 0.05, true);
    
    // 6. 保持可视化运行
    visualizer->Join();
    
    return 0;
}
'''
    
    print(code_example)
    print()

def main():
    """主函数"""
    print_welcome()
    
    # 模拟TOWR优化过程
    simulate_towr_optimization()
    
    # 生成示例轨迹
    print("📊 生成示例轨迹数据...")
    trajectory_data = generate_sample_trajectory()
    print("✅ 轨迹数据生成完成")
    print()
    
    # 演示MeshCat功能
    demonstrate_meshcat_features()
    
    # 显示集成代码
    show_integration_code()
    
    # 可视化效果
    print("🎨 生成可视化效果预览...")
    fig = visualize_meshcat_integration(trajectory_data)
    
    print("📈 可视化图表说明:")
    print("   • 左上: 3D轨迹场景 (MeshCat实际效果为交互式)")
    print("   • 右上: 高度变化曲线")
    print("   • 左下: 接触相位时序图")
    print("   • 右下: 接触力变化")
    print()
    
    print("🎯 下一步操作:")
    print("1. 安装MeshCat-cpp: ./install_meshcat.sh")
    print("2. 重新编译TOWR: cd towr/build && cmake .. && make")
    print("3. 运行演示: ./towr-meshcat-demo")
    print("4. 在浏览器中查看: http://localhost:7000")
    print()
    
    print("📚 更多信息请查看: MESHCAT_VISUALIZATION.md")
    print()
    
    plt.show()

if __name__ == "__main__":
    main()
