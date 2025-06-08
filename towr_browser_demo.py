#!/usr/bin/env python3
"""
TOWR 浏览器可视化演示系统

这个脚本提供了一个完整的基于浏览器的TOWR可视化演示，
包括多种机器人类型、交互式控制和实时动画播放。

功能特色：
- 🤖 多种机器人类型演示
- 🌐 基于MeshCat的浏览器可视化
- 🎬 实时轨迹动画播放
- 🎨 丰富的可视化元素
- 🖱️ 交互式场景控制
"""

import os
import sys
import time
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import webbrowser
import threading
import http.server
import socketserver
from urllib.parse import urlparse

class TowrBrowserDemo:
    def __init__(self):
        self.demo_configs = self.setup_demo_configurations()
        self.server_port = 8080
        self.meshcat_port = 7000
        
    def setup_demo_configurations(self):
        """设置演示配置"""
        return [
            {
                'robot_type': 'Monoped',
                'robot_name': '单腿跳跃机器人',
                'initial_pos': np.array([0.0, 0.0, 0.5]),
                'target_pos': np.array([1.5, 0.0, 0.5]),
                'total_time': 2.0,
                'description': '展示单腿机器人的跳跃运动，包含腾空和着陆相位',
                'n_ee': 1,
                'color': [0.2, 0.6, 1.0]
            },
            {
                'robot_type': 'Biped',
                'robot_name': '双腿行走机器人',
                'initial_pos': np.array([0.0, 0.0, 0.87]),
                'target_pos': np.array([2.0, 0.0, 0.87]),
                'total_time': 3.0,
                'description': '展示双腿机器人的行走步态，左右脚交替接触',
                'n_ee': 2,
                'color': [1.0, 0.4, 0.2]
            },
            {
                'robot_type': 'Quadruped',
                'robot_name': '四腿奔跑机器人',
                'initial_pos': np.array([0.0, 0.0, 0.5]),
                'target_pos': np.array([2.5, 0.0, 0.5]),
                'total_time': 2.5,
                'description': '展示四腿机器人的奔跑步态，对角腿协调运动',
                'n_ee': 4,
                'color': [0.8, 0.2, 0.8]
            }
        ]
    
    def print_welcome_message(self):
        """打印欢迎信息"""
        print("\n🌐 TOWR 浏览器可视化演示系统")
        print("=" * 60)
        print("欢迎使用TOWR的交互式浏览器可视化系统！\n")
        print("🎯 功能特色：")
        print("  • 🤖 多种机器人类型（单腿、双腿、四腿）")
        print("  • 🌐 基于浏览器的3D可视化")
        print("  • 🎬 实时轨迹动画播放")
        print("  • 🎨 接触力和相位可视化")
        print("  • 🖱️ 交互式场景控制")
        print("  • 📊 详细的优化结果分析\n")
        
        # 检查依赖
        try:
            import meshcat
            print("✅ MeshCat Python库已安装")
        except ImportError:
            print("⚠️  MeshCat Python库未安装，将使用matplotlib可视化")
        
        print()
    
    def print_demo_menu(self):
        """打印演示菜单"""
        print("📋 可用演示列表：")
        print("─" * 50)
        
        for i, config in enumerate(self.demo_configs):
            print(f"  {i + 1}. {config['robot_name']}")
            print(f"     📍 {config['description']}")
            print(f"     🎯 目标: ({config['target_pos'][0]:.1f}, {config['target_pos'][1]:.1f}, {config['target_pos'][2]:.1f}) 米")
            print(f"     ⏱️  时长: {config['total_time']} 秒\n")
        
        print(f"  {len(self.demo_configs) + 1}. 🔄 运行所有演示")
        print(f"  {len(self.demo_configs) + 2}. 🌐 启动浏览器可视化服务器")
        print(f"  {len(self.demo_configs) + 3}. ❌ 退出程序\n")
    
    def get_user_choice(self):
        """获取用户选择"""
        try:
            choice = int(input(f"请选择演示 (1-{len(self.demo_configs) + 3}): "))
            return choice
        except ValueError:
            return -1
    
    def generate_trajectory_data(self, config):
        """生成轨迹数据"""
        print(f"\n🔧 生成 {config['robot_name']} 轨迹数据...")
        
        # 时间序列
        dt = 0.02
        t = np.arange(0, config['total_time'] + dt, dt)
        n_points = len(t)
        
        # 机器人本体轨迹
        start_pos = config['initial_pos']
        end_pos = config['target_pos']
        
        # 生成平滑的轨迹
        base_trajectory = np.zeros((n_points, 3))
        for i, time_val in enumerate(t):
            progress = time_val / config['total_time']
            
            # X方向线性插值
            base_trajectory[i, 0] = start_pos[0] + progress * (end_pos[0] - start_pos[0])
            
            # Y方向保持不变
            base_trajectory[i, 1] = start_pos[1]
            
            # Z方向添加跳跃效果
            if config['robot_type'] == 'Monoped':
                # 单腿跳跃
                jump_height = 0.3 * np.sin(np.pi * progress) * np.exp(-0.5 * progress)
                base_trajectory[i, 2] = start_pos[2] + jump_height
            elif config['robot_type'] == 'Biped':
                # 双腿行走，轻微上下运动
                walk_height = 0.05 * np.sin(4 * np.pi * progress)
                base_trajectory[i, 2] = start_pos[2] + walk_height
            else:  # Quadruped
                # 四腿奔跑，中等跳跃
                gallop_height = 0.15 * np.sin(2 * np.pi * progress)
                base_trajectory[i, 2] = start_pos[2] + gallop_height
        
        # 生成末端执行器轨迹
        ee_trajectories = []
        contact_states = []
        forces = []
        
        for ee in range(config['n_ee']):
            ee_traj = np.zeros((n_points, 3))
            ee_contact = np.zeros(n_points, dtype=bool)
            ee_force = np.zeros((n_points, 3))
            
            for i, time_val in enumerate(t):
                progress = time_val / config['total_time']
                
                # 基础位置跟随机器人本体
                base_x = base_trajectory[i, 0]
                
                if config['robot_type'] == 'Monoped':
                    # 单腿机器人
                    phase = (progress * 2) % 1.0  # 两个周期
                    if phase < 0.3:  # 接触相位
                        ee_traj[i] = [base_x, 0, 0]
                        ee_contact[i] = True
                        ee_force[i] = [50 * np.sin(progress * np.pi), 0, 300 + 100 * np.sin(4 * np.pi * progress)]
                    else:  # 腾空相位
                        swing_height = 0.1 * np.sin(np.pi * (phase - 0.3) / 0.7)
                        ee_traj[i] = [base_x + 0.1 * np.sin(np.pi * (phase - 0.3) / 0.7), 0, swing_height]
                        ee_contact[i] = False
                        ee_force[i] = [0, 0, 0]
                
                elif config['robot_type'] == 'Biped':
                    # 双腿机器人
                    phase = (progress * 4) % 1.0  # 四个周期
                    y_offset = 0.1 if ee == 0 else -0.1  # 左右脚分开
                    
                    # 交替步态
                    if (ee == 0 and phase < 0.5) or (ee == 1 and phase >= 0.5):
                        # 支撑相位
                        ee_traj[i] = [base_x, y_offset, 0]
                        ee_contact[i] = True
                        ee_force[i] = [25, 0, 200]
                    else:
                        # 摆动相位
                        swing_progress = (phase - 0.5) if ee == 0 else (phase if phase < 0.5 else phase - 0.5)
                        swing_height = 0.08 * np.sin(np.pi * swing_progress * 2)
                        swing_forward = 0.15 * swing_progress * 2
                        ee_traj[i] = [base_x + swing_forward, y_offset, swing_height]
                        ee_contact[i] = False
                        ee_force[i] = [0, 0, 0]
                
                else:  # Quadruped
                    # 四腿机器人
                    phase = (progress * 3) % 1.0  # 三个周期
                    
                    # 设置脚的位置偏移
                    if ee == 0:  # 前左
                        x_offset, y_offset = 0.2, 0.15
                    elif ee == 1:  # 前右
                        x_offset, y_offset = 0.2, -0.15
                    elif ee == 2:  # 后左
                        x_offset, y_offset = -0.2, 0.15
                    else:  # 后右
                        x_offset, y_offset = -0.2, -0.15
                    
                    # 对角步态
                    if (ee in [0, 3] and phase < 0.5) or (ee in [1, 2] and phase >= 0.5):
                        # 支撑相位
                        ee_traj[i] = [base_x + x_offset, y_offset, 0]
                        ee_contact[i] = True
                        ee_force[i] = [20, 0, 150]
                    else:
                        # 摆动相位
                        swing_progress = (phase - 0.5) if ee in [0, 3] else (phase if phase < 0.5 else phase - 0.5)
                        swing_height = 0.06 * np.sin(np.pi * swing_progress * 2)
                        ee_traj[i] = [base_x + x_offset, y_offset, swing_height]
                        ee_contact[i] = False
                        ee_force[i] = [0, 0, 0]
            
            ee_trajectories.append(ee_traj)
            contact_states.append(ee_contact)
            forces.append(ee_force)
        
        return {
            'time': t,
            'base_trajectory': base_trajectory,
            'ee_trajectories': ee_trajectories,
            'contact_states': contact_states,
            'forces': forces,
            'config': config
        }
    
    def visualize_with_matplotlib(self, trajectory_data):
        """使用matplotlib进行可视化"""
        print("📊 使用matplotlib生成可视化...")
        
        config = trajectory_data['config']
        base_traj = trajectory_data['base_trajectory']
        ee_trajs = trajectory_data['ee_trajectories']
        contact_states = trajectory_data['contact_states']
        forces = trajectory_data['forces']
        t = trajectory_data['time']
        
        # 创建图形
        fig = plt.figure(figsize=(16, 12))
        fig.suptitle(f'TOWR {config["robot_name"]} 轨迹可视化', fontsize=16, fontweight='bold')
        
        # 3D轨迹图
        ax1 = fig.add_subplot(221, projection='3d')
        
        # 绘制机器人本体轨迹
        ax1.plot(base_traj[:, 0], base_traj[:, 1], base_traj[:, 2], 
                'b-', linewidth=3, label='机器人本体轨迹', alpha=0.8)
        
        # 绘制末端执行器轨迹
        colors = ['red', 'green', 'orange', 'purple']
        for i, (ee_traj, contact) in enumerate(zip(ee_trajs, contact_states)):
            color = colors[i % len(colors)]
            
            # 分别绘制接触和腾空阶段
            for j in range(len(ee_traj)):
                marker = 'o' if contact[j] else '^'
                size = 30 if contact[j] else 15
                alpha = 0.8 if contact[j] else 0.4
                ax1.scatter(ee_traj[j, 0], ee_traj[j, 1], ee_traj[j, 2],
                           c=color, s=size, marker=marker, alpha=alpha)
        
        # 标记起始和目标位置
        ax1.scatter(*config['initial_pos'], c='green', s=100, marker='s', label='起始位置')
        ax1.scatter(*config['target_pos'], c='red', s=100, marker='*', label='目标位置')
        
        ax1.set_xlabel('X Position (m)')
        ax1.set_ylabel('Y Position (m)')
        ax1.set_zlabel('Z Position (m)')
        ax1.set_title('3D轨迹可视化')
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # 高度随时间变化
        ax2 = fig.add_subplot(222)
        ax2.plot(t, base_traj[:, 2], 'b-', linewidth=2, label='机器人本体高度')
        for i, ee_traj in enumerate(ee_trajs):
            ax2.plot(t, ee_traj[:, 2], '--', linewidth=1, label=f'脚{i+1}高度', alpha=0.7)
        ax2.set_xlabel('时间 (s)')
        ax2.set_ylabel('高度 (m)')
        ax2.set_title('高度随时间变化')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        # 接触相位图
        ax3 = fig.add_subplot(223)
        for i, contact in enumerate(contact_states):
            y_offset = i * 1.2
            contact_array = contact.astype(float) + y_offset
            ax3.fill_between(t, y_offset, contact_array, alpha=0.6, 
                           label=f'脚{i+1}接触', color=colors[i % len(colors)])
        ax3.set_xlabel('时间 (s)')
        ax3.set_ylabel('接触状态')
        ax3.set_title('接触相位时序图')
        ax3.legend()
        ax3.grid(True, alpha=0.3)
        
        # 接触力图
        ax4 = fig.add_subplot(224)
        for i, force in enumerate(forces):
            force_magnitude = np.linalg.norm(force, axis=1)
            ax4.plot(t, force_magnitude, linewidth=2, label=f'脚{i+1}接触力', 
                    color=colors[i % len(colors)])
        ax4.set_xlabel('时间 (s)')
        ax4.set_ylabel('力 (N)')
        ax4.set_title('接触力随时间变化')
        ax4.legend()
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def create_browser_visualization(self, trajectory_data):
        """创建浏览器可视化"""
        print("🌐 创建浏览器可视化...")
        
        try:
            import meshcat
            import meshcat.geometry as g
            import meshcat.transformations as tf
            
            # 创建MeshCat可视化器
            vis = meshcat.Visualizer()
            print(f"📱 MeshCat服务器已启动: {vis.url()}")
            
            config = trajectory_data['config']
            base_traj = trajectory_data['base_trajectory']
            ee_trajs = trajectory_data['ee_trajectories']
            contact_states = trajectory_data['contact_states']
            
            # 设置场景
            vis.delete()
            
            # 添加地面
            vis["ground"].set_object(g.Box([5, 5, 0.01]), 
                                   g.MeshLambertMaterial(color=0x808080, opacity=0.5))
            vis["ground"].set_transform(tf.translation_matrix([0, 0, -0.005]))
            
            # 添加机器人本体
            body_color = int(config['color'][0] * 255) << 16 | \
                        int(config['color'][1] * 255) << 8 | \
                        int(config['color'][2] * 255)
            
            if config['robot_type'] == 'Monoped':
                body_geom = g.Box([0.2, 0.1, 0.1])
            elif config['robot_type'] == 'Biped':
                body_geom = g.Box([0.3, 0.15, 0.1])
            else:  # Quadruped
                body_geom = g.Box([0.4, 0.2, 0.1])
            
            vis["robot/body"].set_object(body_geom, 
                                       g.MeshLambertMaterial(color=body_color, opacity=0.8))
            
            # 添加脚部
            for i in range(config['n_ee']):
                vis[f"robot/foot_{i}"].set_object(g.Sphere(0.03), 
                                                g.MeshLambertMaterial(color=0xff0000))
            
            # 添加目标和起始标记
            vis["markers/start"].set_object(g.Sphere(0.05), 
                                          g.MeshLambertMaterial(color=0x00ff00))
            vis["markers/start"].set_transform(tf.translation_matrix(config['initial_pos']))
            
            vis["markers/target"].set_object(g.Sphere(0.05), 
                                           g.MeshLambertMaterial(color=0xff0000))
            vis["markers/target"].set_transform(tf.translation_matrix(config['target_pos']))
            
            # 播放动画
            print("▶️  播放轨迹动画...")
            for i in range(0, len(base_traj), 2):  # 每隔一帧播放
                # 更新机器人本体位置
                vis["robot/body"].set_transform(tf.translation_matrix(base_traj[i]))
                
                # 更新脚部位置和颜色
                for j in range(config['n_ee']):
                    foot_pos = ee_trajs[j][i]
                    is_contact = contact_states[j][i]
                    
                    vis[f"robot/foot_{j}"].set_transform(tf.translation_matrix(foot_pos))
                    
                    # 根据接触状态改变颜色
                    color = 0xff0000 if is_contact else 0x00ff00
                    vis[f"robot/foot_{j}"].set_object(g.Sphere(0.03), 
                                                    g.MeshLambertMaterial(color=color))
                
                time.sleep(0.05)  # 控制播放速度
            
            print("✅ 浏览器动画播放完成！")
            print(f"🌐 可视化链接: {vis.url()}")
            return vis.url()
            
        except ImportError:
            print("⚠️  MeshCat库未安装，无法创建浏览器可视化")
            return None
    
    def run_single_demo(self, config):
        """运行单个演示"""
        print(f"\n{'=' * 60}")
        print(f"🤖 {config['robot_name']} 演示")
        print('=' * 60)
        print(f"📝 {config['description']}")
        
        # 生成轨迹数据
        trajectory_data = self.generate_trajectory_data(config)
        
        # 打印结果分析
        self.print_demo_results(trajectory_data)
        
        # 可视化
        print("\n🎨 生成可视化...")
        
        # matplotlib可视化
        fig = self.visualize_with_matplotlib(trajectory_data)
        
        # 浏览器可视化
        browser_url = self.create_browser_visualization(trajectory_data)
        
        # 显示图表
        plt.show()
        
        if browser_url:
            print(f"\n🌐 浏览器可视化已准备就绪: {browser_url}")
            print("💡 您可以在浏览器中查看交互式3D可视化")
        
        input("\n按回车键继续...")
    
    def print_demo_results(self, trajectory_data):
        """打印演示结果"""
        config = trajectory_data['config']
        base_traj = trajectory_data['base_trajectory']
        
        print(f"\n📊 {config['robot_name']} 优化结果分析：")
        print("─" * 40)
        
        print(f"⏱️  总运动时间: {config['total_time']} 秒")
        
        # 计算最大高度
        max_height = np.max(base_traj[:, 2])
        print(f"📏 最大运动高度: {max_height:.3f} 米")
        
        # 计算总距离
        distance = np.linalg.norm(config['target_pos'] - config['initial_pos'])
        print(f"📐 总移动距离: {distance:.3f} 米")
        print(f"🏃 平均移动速度: {distance / config['total_time']:.3f} 米/秒")
        
        print(f"🦶 末端执行器数量: {config['n_ee']} 个")
    
    def run_interactive_demo(self):
        """运行交互式演示"""
        self.print_welcome_message()
        
        while True:
            self.print_demo_menu()
            choice = self.get_user_choice()
            
            if 1 <= choice <= len(self.demo_configs):
                # 运行单个演示
                self.run_single_demo(self.demo_configs[choice - 1])
            
            elif choice == len(self.demo_configs) + 1:
                # 运行所有演示
                print("\n🎬 开始运行所有演示...")
                for config in self.demo_configs:
                    self.run_single_demo(config)
                print("\n🎉 所有演示完成！")
            
            elif choice == len(self.demo_configs) + 2:
                # 启动浏览器可视化服务器
                self.start_browser_server()
            
            elif choice == len(self.demo_configs) + 3:
                # 退出
                print("\n👋 感谢使用TOWR浏览器可视化演示！")
                break
            
            else:
                print("\n❌ 无效选择，请重新输入。\n")
    
    def start_browser_server(self):
        """启动浏览器可视化服务器"""
        print(f"\n🌐 启动浏览器可视化服务器...")
        print(f"📱 服务器端口: {self.server_port}")
        print(f"🔗 访问地址: http://localhost:{self.server_port}")
        print("💡 这将启动一个本地服务器用于浏览器可视化")
        print("⏹️  按 Ctrl+C 停止服务器")
        
        try:
            # 创建简单的HTTP服务器
            handler = http.server.SimpleHTTPRequestHandler
            with socketserver.TCPServer(("", self.server_port), handler) as httpd:
                print(f"✅ 服务器已启动在端口 {self.server_port}")
                
                # 尝试自动打开浏览器
                try:
                    webbrowser.open(f'http://localhost:{self.server_port}')
                    print("🌐 已尝试自动打开浏览器")
                except:
                    print("⚠️  无法自动打开浏览器，请手动访问上述地址")
                
                httpd.serve_forever()
                
        except KeyboardInterrupt:
            print("\n⏹️  服务器已停止")
        except Exception as e:
            print(f"❌ 服务器启动失败: {e}")

def main():
    """主函数"""
    try:
        demo = TowrBrowserDemo()
        demo.run_interactive_demo()
        
    except KeyboardInterrupt:
        print("\n\n👋 程序被用户中断，再见！")
    except Exception as e:
        print(f"\n❌ 程序运行出错: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
