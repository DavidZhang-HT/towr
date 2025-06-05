#!/bin/bash

# TOWR 自动化构建脚本
# 用于简化TOWR轨迹优化库的编译安装过程

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印函数
print_header() {
    echo -e "${BLUE}════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BLUE}    TOWR (Trajectory Optimization for Walking Robots) 构建器    ${NC}"
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

# 检测操作系统
detect_os() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        MAKE_JOBS=$(sysctl -n hw.ncpu)
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        MAKE_JOBS=$(nproc)
    else
        OS="unknown"
        MAKE_JOBS=4
    fi
    print_info "检测到操作系统: $OS (使用 $MAKE_JOBS 个并行编译任务)"
}

# 检查依赖
check_dependencies() {
    print_step "检查系统依赖..."
    
    local missing_deps=()
    
    # 检查基本工具
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
    fi
    
    if ! command -v make &> /dev/null; then
        missing_deps+=("make")
    fi
    
    if ! command -v git &> /dev/null; then
        missing_deps+=("git")
    fi
    
    # 检查编译器
    if ! command -v gcc &> /dev/null && ! command -v clang &> /dev/null; then
        missing_deps+=("编译器(gcc或clang)")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "缺少以下依赖: ${missing_deps[*]}"
        echo ""
        print_info "请安装缺少的依赖:"
        if [[ "$OS" == "macos" ]]; then
            echo "  brew install cmake eigen ipopt"
        elif [[ "$OS" == "linux" ]]; then
            echo "  sudo apt-get install cmake libeigen3-dev coinor-libipopt-dev build-essential"
        fi
        exit 1
    fi
    
    print_success "所有依赖已满足"
}

# 设置环境变量
setup_environment() {
    print_step "设置编译环境..."
    
    # macOS特殊设置
    if [[ "$OS" == "macos" ]]; then
        export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
        print_info "已设置macOS动态库路径"
    fi
}

# 构建ifopt依赖
build_ifopt() {
    print_step "构建ifopt依赖库..."
    
    if [ ! -d "ifopt" ]; then
        print_info "克隆ifopt仓库..."
        git clone https://github.com/ethz-adrl/ifopt.git
    else
        print_info "ifopt目录已存在，跳过克隆"
    fi
    
    cd ifopt
    
    if [ ! -d "build" ]; then
        mkdir build
    fi
    
    cd build
    
    print_info "配置ifopt构建..."
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    
    print_info "编译ifopt (使用 $MAKE_JOBS 个并行任务)..."
    make -j$MAKE_JOBS
    
    print_info "安装ifopt..."
    if command -v sudo &> /dev/null; then
        sudo make install
    else
        make install
    fi
    
    # 更新库缓存 (Linux)
    if [[ "$OS" == "linux" ]]; then
        sudo ldconfig 2>/dev/null || true
    fi
    
    cd ../../
    print_success "ifopt构建完成"
}

# 构建TOWR核心库
build_towr() {
    print_step "构建TOWR核心库..."
    
    cd towr
    
    if [ ! -d "build" ]; then
        mkdir build
    fi
    
    cd build
    
    print_info "配置TOWR构建..."
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    
    print_info "编译TOWR (使用 $MAKE_JOBS 个并行任务)..."
    make -j$MAKE_JOBS
    
    cd ../..
    print_success "TOWR构建完成"
}

# 运行测试
run_tests() {
    print_step "运行构建验证..."
    
    cd towr/build
    
    # 检查生成的可执行文件
    local executables=("towr-example" "towr-test")
    for exe in "${executables[@]}"; do
        if [ -f "$exe" ]; then
            print_success "✓ $exe 已生成"
        else
            print_error "✗ $exe 未找到"
        fi
    done
    
    # 运行单元测试
    if [ -f "towr-test" ]; then
        print_info "运行单元测试..."
        if ./towr-test > /dev/null 2>&1; then
            print_success "单元测试通过"
        else
            print_error "单元测试失败"
        fi
    fi
    
    cd ../..
}

# 生成使用说明
generate_usage() {
    print_step "生成使用说明..."
    
    cat > towr/build/运行说明.txt << EOF
TOWR 构建完成！

运行演示程序：
================

1. 基础跳跃演示：
   cd towr/build
   ./towr-example

2. 详细分析演示：
   ./towr-simple-demo

3. 高级多机器人演示：
   ./towr-advanced-demo

4. 单元测试：
   ./towr-test

macOS用户注意：
===============
如果遇到库加载问题，请运行：
export DYLD_LIBRARY_PATH=/usr/local/lib:\$DYLD_LIBRARY_PATH

更多信息：
==========
• 查看 INSTALLATION_GUIDE.md 获取详细文档
• 查看 QUICK_START.md 获取快速开始指南
• 访问 https://github.com/ethz-adrl/towr 获取最新信息

EOF
    
    print_success "使用说明已生成: towr/build/运行说明.txt"
}

# 主函数
main() {
    print_header
    
    # 获取脚本参数
    CLEAN_BUILD=false
    SKIP_TESTS=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --clean)
                CLEAN_BUILD=true
                shift
                ;;
            --skip-tests)
                SKIP_TESTS=true
                shift
                ;;
            -h|--help)
                echo "用法: $0 [选项]"
                echo "选项:"
                echo "  --clean      清理之前的构建"
                echo "  --skip-tests 跳过测试验证"
                echo "  -h, --help   显示帮助信息"
                exit 0
                ;;
            *)
                print_error "未知选项: $1"
                exit 1
                ;;
        esac
    done
    
    # 清理构建
    if $CLEAN_BUILD; then
        print_step "清理之前的构建..."
        rm -rf ifopt/build towr/build
        print_info "构建目录已清理"
    fi
    
    # 执行构建步骤
    detect_os
    check_dependencies
    setup_environment
    build_ifopt
    build_towr
    
    if ! $SKIP_TESTS; then
        run_tests
    fi
    
    generate_usage
    
    echo ""
    print_success "🎉 TOWR构建完成！"
    print_info "请查看 towr/build/运行说明.txt 了解如何运行演示程序"
    echo ""
}

# 运行主函数
main "$@" 