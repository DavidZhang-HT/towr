#!/bin/bash

# macOS 原生 ROS 安装脚本
# 注意：这是一个实验性的安装方法，可能会遇到兼容性问题

set -e

print_header() {
    echo "════════════════════════════════════════════════════════════════"
    echo "         macOS 原生 ROS Noetic 安装脚本                        "
    echo "════════════════════════════════════════════════════════════════"
    echo ""
    echo "⚠️  注意：ROS在macOS上的支持有限，推荐使用Docker方案"
    echo ""
}

install_dependencies() {
    echo "[步骤] 安装必要依赖..."
    
    # 安装Python 3
    if ! command -v python3 &> /dev/null; then
        brew install python@3.9
    fi
    
    # 安装其他依赖
    brew install cmake pkg-config eigen boost console_bridge urdfdom
    
    # 安装Python包
    pip3 install -U rosdep rosinstall_generator wstool rosinstall six vcstools
    
    echo "✅ 依赖安装完成"
}

setup_ros_workspace() {
    echo "[步骤] 设置ROS工作空间..."
    
    # 创建catkin工作空间
    mkdir -p ~/ros_catkin_ws
    cd ~/ros_catkin_ws
    
    # 初始化rosdep
    if [ ! -f /etc/ros/rosdep/sources.list.d/20-default.list ]; then
        sudo rosdep init
    fi
    rosdep update
    
    echo "✅ ROS工作空间设置完成"
}

install_ros_core() {
    echo "[步骤] 安装ROS核心包..."
    cd ~/ros_catkin_ws
    
    # 生成ROS包列表
    rosinstall_generator desktop --rosdistro noetic --deps --tar > noetic-desktop.rosinstall
    wstool init src noetic-desktop.rosinstall
    
    # 安装依赖
    rosdep install --from-paths src --ignore-src --rosdistro noetic -y
    
    # 编译 (这可能需要很长时间)
    echo "⏳ 开始编译ROS (可能需要1-2小时)..."
    ./src/catkin/bin/catkin_make_isolated --install -DCMAKE_BUILD_TYPE=Release
    
    echo "✅ ROS核心安装完成"
}

setup_environment() {
    echo "[步骤] 设置环境变量..."
    
    echo "source ~/ros_catkin_ws/install_isolated/setup.bash" >> ~/.bash_profile
    echo "source ~/ros_catkin_ws/install_isolated/setup.bash" >> ~/.zshrc
    
    echo "✅ 环境设置完成"
}

install_towr() {
    echo "[步骤] 安装TOWR..."
    
    source ~/ros_catkin_ws/install_isolated/setup.bash
    
    # 创建TOWR工作空间
    mkdir -p ~/towr_ws/src
    cd ~/towr_ws/src
    
    # 克隆依赖
    git clone https://github.com/ethz-adrl/ifopt.git
    git clone https://github.com/DavidZhang-HT/towr.git
    
    cd ..
    catkin_make_isolated -DCMAKE_BUILD_TYPE=Release
    
    echo "✅ TOWR安装完成"
}

main() {
    print_header
    
    read -p "确定要在macOS上原生安装ROS吗？这可能需要2-3小时 (y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "建议使用Docker方案: ./docker_ros_towr.sh"
        exit 1
    fi
    
    install_dependencies
    setup_ros_workspace
    install_ros_core
    setup_environment
    install_towr
    
    echo ""
    echo "🎉 安装完成！"
    echo "重启Terminal后运行："
    echo "  cd ~/towr_ws"
    echo "  source devel_isolated/setup.bash"
    echo "  roslaunch towr_ros towr_ros.launch"
}

main "$@" 