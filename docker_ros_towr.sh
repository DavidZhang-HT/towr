#!/bin/bash

# TOWR ROS Docker 运行脚本
# 用于在macOS上通过Docker运行完整的ROS版本TOWR

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_header() {
    echo -e "${BLUE}════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}         TOWR ROS Docker 运行器 (macOS 解决方案)                ${NC}"
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

# 检查Docker状态
check_docker() {
    print_step "检查Docker环境..."
    
    if ! command -v docker &> /dev/null; then
        print_error "Docker未安装，请先安装Docker Desktop for Mac"
        exit 1
    fi
    
    if ! docker info &> /dev/null; then
        print_error "Docker未运行，请启动Docker Desktop"
        exit 1
    fi
    
    print_success "Docker环境正常"
}

# 准备X11转发 (用于GUI显示)
setup_x11() {
    print_step "设置X11转发..."
    
    # 检查XQuartz
    if ! command -v xhost &> /dev/null; then
        print_info "需要安装XQuartz来显示GUI"
        print_info "请下载安装: https://www.xquartz.org/"
        print_info "安装后重启Terminal并重新运行此脚本"
        
        read -p "已安装XQuartz? (y/n): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
    
    # 启用X11转发
    print_info "配置X11访问权限..."
    xhost +localhost 2>/dev/null || true
    
    print_success "X11转发配置完成"
}

# 创建Dockerfile
create_dockerfile() {
    print_step "创建TOWR ROS Docker镜像..."
    
    cat > Dockerfile.towr_ros << 'EOF'
# TOWR ROS Docker镜像
FROM ubuntu:20.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive
ENV DISPLAY=host.docker.internal:0
ENV TZ=Asia/Shanghai

# 设置时区
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# 安装基础包和ROS
RUN apt-get update && apt-get install -y \
    curl \
    lsb-release \
    && sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list' \
    && curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | apt-key add - \
    && apt-get update && apt-get install -y \
    ros-noetic-desktop-full \
    python3-rosdep \
    python3-rosinstall \
    python3-rosinstall-generator \
    python3-wstool \
    build-essential \
    cmake \
    libeigen3-dev \
    coinor-libipopt-dev \
    libncurses5-dev \
    xterm \
    git \
    python3-catkin-tools \
    && rm -rf /var/lib/apt/lists/*

# 初始化rosdep
RUN rosdep init && rosdep update

# 创建catkin工作空间
WORKDIR /catkin_ws
RUN mkdir -p src

# 克隆依赖
WORKDIR /catkin_ws/src
RUN git clone https://github.com/ethz-adrl/ifopt.git
RUN git clone https://github.com/DavidZhang-HT/towr.git

# 初始化工作空间
RUN /bin/bash -c "source /opt/ros/noetic/setup.bash && catkin_init_workspace"

# 编译
WORKDIR /catkin_ws
RUN /bin/bash -c "source /opt/ros/noetic/setup.bash && catkin_make_isolated -DCMAKE_BUILD_TYPE=Release"

# 设置环境
RUN echo "source /opt/ros/noetic/setup.bash" >> ~/.bashrc
RUN echo "source /catkin_ws/devel_isolated/setup.bash" >> ~/.bashrc

# 创建启动脚本
RUN echo '#!/bin/bash\n\
source /opt/ros/noetic/setup.bash\n\
source /catkin_ws/devel_isolated/setup.bash\n\
echo "🚀 TOWR ROS 环境已就绪!"\n\
echo "运行命令: roslaunch towr_ros towr_ros.launch"\n\
echo "在xterm终端中按 '"'"'o'"'"' 开始优化"\n\
echo "按 Ctrl+C 退出"\n\
exec "$@"' > /entrypoint.sh && chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
CMD ["bash"]
EOF

    print_info "Dockerfile已创建"
}

# 构建Docker镜像
build_image() {
    print_step "构建TOWR ROS Docker镜像..."
    print_info "这可能需要几分钟时间..."
    
    docker build -f Dockerfile.towr_ros -t towr_ros:latest . || {
        print_error "Docker镜像构建失败"
        exit 1
    }
    
    print_success "Docker镜像构建完成"
}

# 运行TOWR ROS
run_towr_ros() {
    print_step "启动TOWR ROS容器..."
    
    print_info "启动交互式容器..."
    print_info "在容器中运行: roslaunch towr_ros towr_ros.launch"
    print_info "在弹出的xterm窗口中按 'o' 开始优化"
    echo ""
    
    docker run -it --rm \
        -e DISPLAY=host.docker.internal:0 \
        -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
        --name towr_ros_container \
        towr_ros:latest
}

# 快速启动函数
quick_launch() {
    print_step "快速启动TOWR ROS..."
    
    docker run -it --rm \
        -e DISPLAY=host.docker.internal:0 \
        -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
        --name towr_ros_container \
        towr_ros:latest \
        bash -c "roslaunch towr_ros towr_ros.launch"
}

# 主函数
main() {
    print_header
    
    # 检查参数
    if [[ "$1" == "--quick" ]]; then
        check_docker
        setup_x11
        print_info "使用快速启动模式..."
        quick_launch
        return
    fi
    
    if [[ "$1" == "--build-only" ]]; then
        check_docker
        create_dockerfile
        build_image
        print_success "Docker镜像构建完成! 使用 '$0' 来运行"
        return
    fi
    
    if [[ "$1" == "--help" ]]; then
        echo "TOWR ROS Docker 运行器"
        echo ""
        echo "用法:"
        echo "  $0                运行完整的交互式容器"
        echo "  $0 --quick        直接启动roslaunch (需要已构建镜像)"
        echo "  $0 --build-only   仅构建Docker镜像"
        echo "  $0 --help         显示帮助信息"
        echo ""
        echo "注意:"
        echo "  1. 需要安装并运行Docker Desktop"
        echo "  2. 需要安装XQuartz来显示GUI: https://www.xquartz.org/"
        echo "  3. 首次运行会自动构建Docker镜像"
        return
    fi
    
    # 完整流程
    check_docker
    setup_x11
    
    # 检查镜像是否存在
    if ! docker image inspect towr_ros:latest &> /dev/null; then
        print_info "TOWR ROS镜像不存在，开始构建..."
        create_dockerfile
        build_image
    else
        print_info "TOWR ROS镜像已存在，跳过构建"
    fi
    
    run_towr_ros
}

# 清理函数
cleanup() {
    print_info "清理临时文件..."
    rm -f Dockerfile.towr_ros
}

# 设置清理trap
trap cleanup EXIT

# 运行主函数
main "$@" 