#!/bin/bash

# TOWR 快速ROS演示脚本
# 提供多种运行ROS版本TOWR的方法

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_header() {
    echo -e "${BLUE}════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}              TOWR ROS 快速演示启动器                          ${NC}"
    echo -e "${BLUE}════════════════════════════════════════════════════════════════${NC}"
    echo ""
}

print_step() {
    echo -e "${GREEN}[步骤] $1${NC}"
}

print_info() {
    echo -e "${YELLOW}[信息] $1${NC}"
}

print_error() {
    echo -e "${RED}[错误] $1${NC}"
}

print_success() {
    echo -e "${GREEN}[成功] $1${NC}"
}

# 方案1: 使用现有的核心库运行简化ROS演示
run_simple_ros_demo() {
    print_step "运行简化的ROS风格演示..."
    
    if [ ! -f "towr/build/towr-simple-demo" ]; then
        print_error "请先编译TOWR核心库: ./build_towr.sh"
        exit 1
    fi
    
    cd towr/build
    
    print_info "启动TOWR轨迹优化演示..."
    print_info "这个演示模拟了ROS节点的功能"
    echo ""
    
    # 设置macOS库路径
    export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
    
    # 创建一个模拟ROS launch的脚本
    cat > ros_style_demo.sh << 'EOF'
#!/bin/bash

echo "🚀 模拟 roslaunch towr_ros towr_ros.launch"
echo "════════════════════════════════════════════"
echo ""
echo "正在启动TOWR轨迹优化节点..."
echo ""

# 运行实际的TOWR演示
./towr-simple-demo

echo ""
echo "演示完成！在真实的ROS环境中，你可以："
echo "1. 在rviz中可视化轨迹"
echo "2. 通过键盘接口修改参数"
echo "3. 实时调整目标状态和运动类型"
EOF

    chmod +x ros_style_demo.sh
    ./ros_style_demo.sh
    
    cd ../..
}

# 方案2: 使用Docker (如果可用)
run_docker_ros() {
    print_step "尝试启动Docker ROS演示..."
    
    if ! command -v docker &> /dev/null; then
        print_error "Docker未安装"
        return 1
    fi
    
    if ! docker info &> /dev/null; then
        print_error "Docker未运行，请启动Docker Desktop"
        return 1
    fi
    
    print_info "使用Docker运行完整的ROS版本..."
    ./docker_ros_towr.sh --quick
}

# 方案3: 显示如何在Linux虚拟机中运行
show_vm_instructions() {
    print_step "Linux虚拟机运行指南..."
    
    cat << 'EOF'
🐧 在Linux虚拟机中运行完整ROS版本:

1. 安装虚拟机软件 (推荐):
   - VMware Fusion (商业)
   - Parallels Desktop (商业)  
   - VirtualBox (免费)

2. 下载Ubuntu 20.04 LTS:
   https://ubuntu.com/download/desktop

3. 在虚拟机中安装ROS Noetic:
   sudo apt-get install ros-noetic-desktop-full

4. 克隆并编译TOWR:
   mkdir -p ~/catkin_ws/src && cd ~/catkin_ws/src
   git clone https://github.com/ethz-adrl/ifopt.git
   git clone https://github.com/DavidZhang-HT/towr.git
   cd .. && catkin_make_isolated

5. 运行完整ROS版本:
   source devel_isolated/setup.bash
   roslaunch towr_ros towr_ros.launch

6. 在弹出的xterm窗口中按 'o' 开始优化
EOF
}

# 方案4: 显示在线演示视频
show_online_demo() {
    print_step "在线演示资源..."
    
    cat << 'EOF'
📺 TOWR在线演示和文档:

1. 官方演示视频:
   - YouTube: "TOWR Trajectory Optimization"
   - 搜索: "legged robot trajectory optimization"

2. 项目文档:
   - GitHub: https://github.com/ethz-adrl/towr
   - 文档: http://docs.ros.org/kinetic/api/towr/html/

3. 学术论文:
   - IEEE RA-L: "Gait and Trajectory Optimization for Legged Systems"
   - DOI: 10.1109/LRA.2018.2798285

4. 交互式在线演示:
   - ROS官网可能有在线演示
   - Jupyter notebook演示 (如果有)
EOF
}

# 主菜单
show_menu() {
    echo ""
    echo "选择运行方式:"
    echo "1) 简化ROS风格演示 (使用核心库)"
    echo "2) Docker完整ROS演示 (需要Docker)"
    echo "3) Linux虚拟机运行指南"
    echo "4) 查看在线演示资源"
    echo "5) 退出"
    echo ""
    read -p "请选择 (1-5): " choice
}

main() {
    print_header
    
    # 检查核心库是否已编译
    if [ ! -d "towr/build" ]; then
        print_info "检测到TOWR核心库未编译"
        print_info "运行编译脚本: ./build_towr.sh"
        echo ""
        read -p "是否现在编译? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            ./build_towr.sh
        fi
    fi
    
    while true; do
        show_menu
        
        case $choice in
            1)
                run_simple_ros_demo
                ;;
            2)
                if run_docker_ros; then
                    break
                else
                    print_error "Docker方案失败，请选择其他方案"
                fi
                ;;
            3)
                show_vm_instructions
                ;;
            4)
                show_online_demo
                ;;
            5)
                print_info "退出"
                break
                ;;
            *)
                print_error "无效选择，请输入1-5"
                ;;
        esac
        
        echo ""
        read -p "按Enter继续..." 
    done
}

main "$@" 