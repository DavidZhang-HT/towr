#!/usr/bin/env python3
"""
TOWR轨迹可视化脚本
可视化单腿跳跃机器人的运动轨迹
"""

import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

def parse_towr_output():
    """解析TOWR输出数据（手动输入的示例数据）"""
    
    # 从demo输出中提取的数据
    time_points = [0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0]
    
    # 机器人本体位置 [x, y, z]
    base_positions = [
        [0.00, 0.00, 0.50],
        [0.04, 0.00, 0.54],
        [0.09, 0.00, 0.73],
        [0.13, 0.00, 0.73],
        [0.20, 0.00, 0.54],
        [0.38, 0.00, 0.69],
        [0.60, 0.00, 0.67],
        [0.78, 0.00, 0.49],
        [0.88, 0.00, 0.68],
        [0.95, 0.00, 0.72],
        [1.00, 0.00, 0.58]
    ]
    
    # 脚的位置 [x, y, z]
    foot_positions = [
        [0.00, 0.00, 0.00],
        [0.00, 0.00, 0.00],
        [0.00, 0.00, 0.00],
        [0.14, 0.00, 0.00],
        [0.14, 0.00, 0.00],
        [0.14, 0.00, 0.00],
        [0.82, 0.00, 0.00],
        [0.82, 0.00, 0.00],
        [0.82, 0.00, 0.00],
        [1.10, 0.00, 0.00],
        [1.10, 0.00, 0.00]
    ]
    
    # 接触状态
    contact_states = [True, True, True, False, True, True, False, True, True, False, True]
    
    # 接触力 [fx, fy, fz]
    contact_forces = [
        [53.39, 0.0, 240.14],
        [2.02, 0.0, 305.44],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [67.34, -0.01, 431.87],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [-54.64, 0.01, 458.66],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [-52.49, 0.0, 359.64]
    ]
    
    return {
        'time': time_points,
        'base_pos': np.array(base_positions),
        'foot_pos': np.array(foot_positions),
        'contact': contact_states,
        'forces': np.array(contact_forces)
    }

def plot_trajectory_3d(data):
    """绘制3D轨迹图"""
    fig = plt.figure(figsize=(15, 10))
    
    # 3D轨迹图
    ax1 = fig.add_subplot(221, projection='3d')
    
    base_pos = data['base_pos']
    foot_pos = data['foot_pos']
    contact = data['contact']
    
    # 绘制机器人本体轨迹
    ax1.plot(base_pos[:, 0], base_pos[:, 1], base_pos[:, 2], 
             'b-o', linewidth=2, markersize=6, label='机器人本体轨迹')
    
    # 绘制脚的轨迹，根据接触状态区分颜色
    for i in range(len(foot_pos)):
        color = 'red' if contact[i] else 'green'
        marker = 's' if contact[i] else '^'
        label = '脚接触地面' if contact[i] and i == 0 else '脚腾空' if not contact[i] and i == 3 else None
        ax1.scatter(foot_pos[i, 0], foot_pos[i, 1], foot_pos[i, 2], 
                   c=color, s=80, marker=marker, label=label)
    
    # 连接机器人本体和脚（表示腿）
    for i in range(len(base_pos)):
        ax1.plot([base_pos[i, 0], foot_pos[i, 0]], 
                [base_pos[i, 1], foot_pos[i, 1]], 
                [base_pos[i, 2], foot_pos[i, 2]], 
                'k--', alpha=0.5, linewidth=1)
    
    ax1.set_xlabel('X Position (m)')
    ax1.set_ylabel('Y Position (m)')
    ax1.set_zlabel('Z Position (m)')
    ax1.set_title('单腿跳跃机器人 - 3D轨迹')
    ax1.legend()
    ax1.grid(True)
    
    # 设置等比例
    ax1.set_box_aspect([1,1,1])
    
    # 时间-高度图
    ax2 = fig.add_subplot(222)
    ax2.plot(data['time'], base_pos[:, 2], 'b-o', linewidth=2, label='机器人本体高度')
    ax2.plot(data['time'], foot_pos[:, 2], 'r-s', linewidth=1, label='脚高度')
    ax2.set_xlabel('时间 (s)')
    ax2.set_ylabel('高度 (m)')
    ax2.set_title('高度随时间变化')
    ax2.legend()
    ax2.grid(True)
    
    # 时间-水平位置图
    ax3 = fig.add_subplot(223)
    ax3.plot(data['time'], base_pos[:, 0], 'b-o', linewidth=2, label='机器人本体X位置')
    ax3.plot(data['time'], foot_pos[:, 0], 'r-s', linewidth=1, label='脚X位置')
    ax3.set_xlabel('时间 (s)')
    ax3.set_ylabel('X位置 (m)')
    ax3.set_title('水平位置随时间变化')
    ax3.legend()
    ax3.grid(True)
    
    # 接触力图
    ax4 = fig.add_subplot(224)
    forces = data['forces']
    ax4.plot(data['time'], forces[:, 0], 'r-', linewidth=2, label='水平力 Fx')
    ax4.plot(data['time'], forces[:, 2], 'b-', linewidth=2, label='垂直力 Fz')
    ax4.set_xlabel('时间 (s)')
    ax4.set_ylabel('力 (N)')
    ax4.set_title('接触力随时间变化')
    ax4.legend()
    ax4.grid(True)
    
    plt.tight_layout()
    return fig

def plot_contact_phases(data):
    """绘制接触相位图"""
    fig, ax = plt.subplots(figsize=(12, 6))
    
    time = data['time']
    contact = data['contact']
    
    # 创建接触相位的可视化
    for i in range(len(time)-1):
        if contact[i]:
            ax.axvspan(time[i], time[i+1], alpha=0.3, color='red', label='接触相位' if i == 0 else "")
        else:
            ax.axvspan(time[i], time[i+1], alpha=0.3, color='green', label='腾空相位' if i == 3 else "")
    
    # 叠加机器人高度曲线
    base_height = data['base_pos'][:, 2]
    ax.plot(time, base_height, 'b-o', linewidth=3, markersize=8, label='机器人本体高度')
    
    ax.set_xlabel('时间 (s)', fontsize=12)
    ax.set_ylabel('高度 (m)', fontsize=12)
    ax.set_title('单腿跳跃机器人的接触相位和高度变化', fontsize=14)
    ax.legend(fontsize=12)
    ax.grid(True, alpha=0.3)
    
    # 添加文本说明
    ax.text(0.2, 0.6, '接触相位\n(红色区域)', ha='center', va='center', 
           bbox=dict(boxstyle="round,pad=0.3", facecolor='red', alpha=0.3))
    ax.text(0.7, 0.75, '腾空相位\n(绿色区域)', ha='center', va='center', 
           bbox=dict(boxstyle="round,pad=0.3", facecolor='green', alpha=0.3))
    
    return fig

def main():
    print("🤖 TOWR轨迹可视化工具")
    print("=" * 50)
    
    # 解析数据
    data = parse_towr_output()
    
    # 生成可视化图表
    fig1 = plot_trajectory_3d(data)
    fig2 = plot_contact_phases(data)
    
    print("📊 生成的图表：")
    print("1. 3D轨迹图 - 显示机器人的完整运动轨迹")
    print("2. 接触相位图 - 显示步态变化")
    print("\n💡 图表说明：")
    print("• 蓝色线：机器人本体轨迹")
    print("• 红色方块：脚接触地面时的位置")  
    print("• 绿色三角：脚腾空时的位置")
    print("• 虚线：连接本体和脚的腿部")
    print("\n✨ 观察要点：")
    print("• 机器人通过跳跃从x=0移动到x=1米")
    print("• 整个过程包含多个stance-swing循环")
    print("• 在腾空阶段，脚离开地面，接触力为0")
    print("• 在接触阶段，脚产生支撑力推动身体前进")
    
    plt.show()

if __name__ == "__main__":
    main() 