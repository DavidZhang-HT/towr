#!/bin/bash

# 模拟 roslaunch towr_ros towr_ros.launch 的脚本
# 提供与ROS版本相似的交互体验

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# 清屏并显示ROS启动信息
clear
echo -e "${BLUE}████████████████████████████████████████████████████████████████${NC}"
echo -e "${BLUE}█                                                              █${NC}"
echo -e "${BLUE}█               🚀 模拟 roslaunch towr_ros                     █${NC}"
echo -e "${BLUE}█                                                              █${NC}"
echo -e "${BLUE}████████████████████████████████████████████████████████████████${NC}"
echo ""

# 模拟ROS启动消息
echo -e "${GREEN}[roslaunch] 启动launch文件 [towr_ros/launch/towr_ros.launch]${NC}"
echo -e "${YELLOW}[INFO] 正在初始化ROS节点...${NC}"
echo -e "${YELLOW}[INFO] 启动参数服务器...${NC}"
echo -e "${YELLOW}[INFO] 加载机器人模型...${NC}"
echo -e "${YELLOW}[INFO] 初始化rviz可视化...${NC}"
echo -e "${YELLOW}[INFO] 启动TOWR轨迹优化节点...${NC}"
echo ""

# 设置环境
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH

# 检查是否在正确目录
if [ ! -f "towr/build/towr-example" ]; then
    echo -e "${RED}[ERROR] TOWR可执行文件未找到！${NC}"
    echo "请确保在项目根目录运行此脚本"
    exit 1
fi

# 创建模拟的GUI交互
show_gui_interface() {
    echo -e "${CYAN}╔══════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${CYAN}║                   TOWR 控制界面                             ║${NC}"
    echo -e "${CYAN}║                 (模拟ROS GUI)                               ║${NC}"
    echo -e "${CYAN}╠══════════════════════════════════════════════════════════════╣${NC}"
    echo -e "${CYAN}║                                                              ║${NC}"
    echo -e "${CYAN}║  🎮 控制选项:                                               ║${NC}"
    echo -e "${CYAN}║                                                              ║${NC}"
    echo -e "${CYAN}║  [1] 单腿跳跃演示 (monoped)                                ║${NC}"
    echo -e "${CYAN}║  [2] 详细轨迹分析                                          ║${NC}"
    echo -e "${CYAN}║  [3] 可视化轨迹 (Python)                                   ║${NC}"
    echo -e "${CYAN}║  [4] 运行单元测试                                          ║${NC}"
    echo -e "${CYAN}║  [q] 退出程序                                              ║${NC}"
    echo -e "${CYAN}║                                                              ║${NC}"
    echo -e "${CYAN}║  💡 在真实ROS环境中，这里会显示:                          ║${NC}"
    echo -e "${CYAN}║      - rviz 3D可视化界面                                   ║${NC}"
    echo -e "${CYAN}║      - 实时轨迹动画                                        ║${NC}"
    echo -e "${CYAN}║      - 参数调节滑块                                        ║${NC}"
    echo -e "${CYAN}║      - 机器人状态监控                                      ║${NC}"
    echo -e "${CYAN}║                                                              ║${NC}"
    echo -e "${CYAN}╚══════════════════════════════════════════════════════════════╝${NC}"
    echo ""
}

# 运行单腿跳跃演示
run_monoped_demo() {
    echo -e "${GREEN}[TOWR] 启动单腿跳跃轨迹优化...${NC}"
    echo -e "${YELLOW}[INFO] 目标: 从(0,0,0.5m)跳跃到(1,0,0.5m)${NC}"
    echo ""
    
    cd towr/build
    ./towr-example
    cd ../..
    
    echo ""
    echo -e "${GREEN}✅ 单腿跳跃轨迹优化完成！${NC}"
    echo -e "${CYAN}💡 在ROS环境中，你现在可以在rviz中看到3D轨迹动画${NC}"
}

# 运行详细分析
run_detailed_analysis() {
    echo -e "${GREEN}[TOWR] 启动详细轨迹分析...${NC}"
    echo ""
    
    if [ -f "towr/build/towr-simple-demo" ]; then
        cd towr/build
        ./towr-simple-demo
        cd ../..
    else
        echo -e "${YELLOW}详细演示未编译，运行基础演示...${NC}"
        run_monoped_demo
    fi
    
    echo ""
    echo -e "${GREEN}✅ 详细轨迹分析完成！${NC}"
}

# 运行可视化
run_visualization() {
    echo -e "${GREEN}[TOWR] 启动Python轨迹可视化...${NC}"
    echo ""
    
    if [ -f "visualize_trajectory.py" ]; then
        python3 visualize_trajectory.py
    else
        echo -e "${YELLOW}可视化脚本未找到，显示轨迹数据...${NC}"
        run_monoped_demo
    fi
    
    echo ""
    echo -e "${GREEN}✅ 可视化完成！${NC}"
    echo -e "${CYAN}💡 在ROS环境中，rviz会提供实时的3D可视化${NC}"
}

# 运行测试
run_tests() {
    echo -e "${GREEN}[TOWR] 运行单元测试...${NC}"
    echo ""
    
    cd towr/build
    if [ -f "towr-test" ]; then
        ./towr-test
    else
        echo -e "${YELLOW}测试程序未找到，运行基础演示验证...${NC}"
        ./towr-example > /dev/null 2>&1 && echo "✅ 基础功能正常" || echo "❌ 基础功能异常"
    fi
    cd ../..
    
    echo ""
    echo -e "${GREEN}✅ 测试完成！${NC}"
}

# 主循环
main() {
    echo -e "${PURPLE}[roslaunch] 所有节点启动完成${NC}"
    echo -e "${PURPLE}[INFO] ROS系统就绪，等待用户交互...${NC}"
    echo ""
    
    while true; do
        show_gui_interface
        echo -e "${YELLOW}请选择操作 (1-4, q退出): ${NC}"
        read -p "> " choice
        
        case $choice in
            1)
                echo ""
                run_monoped_demo
                ;;
            2)
                echo ""
                run_detailed_analysis
                ;;
            3)
                echo ""
                run_visualization
                ;;
            4)
                echo ""
                run_tests
                ;;
            q|Q)
                echo ""
                echo -e "${YELLOW}[INFO] 正在关闭ROS节点...${NC}"
                echo -e "${YELLOW}[INFO] 清理资源...${NC}"
                echo -e "${GREEN}[roslaunch] 程序正常退出${NC}"
                echo ""
                echo -e "${BLUE}谢谢使用TOWR！${NC}"
                exit 0
                ;;
            *)
                echo -e "${RED}无效选择，请输入1-4或q${NC}"
                ;;
        esac
        
        echo ""
        echo -e "${CYAN}按Enter继续...${NC}"
        read
        clear
    done
}

# 检查依赖
check_requirements() {
    if [ ! -d "towr/build" ]; then
        echo -e "${RED}[ERROR] TOWR未编译！${NC}"
        echo "请先运行: ./build_towr.sh"
        exit 1
    fi
}

# 启动程序
echo -e "${YELLOW}[INFO] 检查系统要求...${NC}"
check_requirements

echo -e "${YELLOW}[INFO] 模拟ROS master启动...${NC}"
sleep 1

echo -e "${YELLOW}[INFO] 连接TOWR节点...${NC}"
sleep 1

main 