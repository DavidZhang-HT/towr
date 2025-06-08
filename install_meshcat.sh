#!/bin/bash

# TOWR MeshCat-cpp 安装脚本
# 这个脚本将自动安装MeshCat-cpp库以支持TOWR的3D可视化功能

set -e  # 遇到错误时退出

echo "🚀 TOWR MeshCat-cpp 安装脚本"
echo "=" $(printf '=%.0s' {1..50})
echo "这个脚本将安装MeshCat-cpp库以启用TOWR的3D可视化功能"
echo ""

# 检查操作系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    echo "🐧 检测到Linux系统"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    echo "🍎 检测到macOS系统"
else
    echo "❌ 不支持的操作系统: $OSTYPE"
    exit 1
fi

# 检查是否有sudo权限
if ! sudo -n true 2>/dev/null; then
    echo "⚠️  这个脚本需要sudo权限来安装系统依赖"
    echo "请输入您的密码："
fi

# 安装系统依赖
echo ""
echo "📦 安装系统依赖..."

if [[ "$OS" == "linux" ]]; then
    # Ubuntu/Debian
    if command -v apt-get &> /dev/null; then
        echo "使用apt-get安装依赖..."
        sudo apt-get update
        sudo apt-get install -y \
            cmake \
            pkg-config \
            build-essential \
            ninja-build \
            git \
            libssl-dev \
            libuv1-dev \
            libz-dev \
            libboost-dev
    # CentOS/RHEL/Fedora
    elif command -v yum &> /dev/null; then
        echo "使用yum安装依赖..."
        sudo yum install -y \
            cmake \
            pkgconfig \
            gcc-c++ \
            ninja-build \
            git \
            openssl-devel \
            libuv-devel \
            zlib-devel \
            boost-devel
    elif command -v dnf &> /dev/null; then
        echo "使用dnf安装依赖..."
        sudo dnf install -y \
            cmake \
            pkgconfig \
            gcc-c++ \
            ninja-build \
            git \
            openssl-devel \
            libuv-devel \
            zlib-devel \
            boost-devel
    else
        echo "❌ 无法检测到包管理器 (apt-get, yum, 或 dnf)"
        echo "请手动安装以下依赖: cmake, pkg-config, build-essential, git, libssl-dev, libuv1-dev, libz-dev, libboost-dev"
        exit 1
    fi
elif [[ "$OS" == "macos" ]]; then
    # macOS with Homebrew
    if command -v brew &> /dev/null; then
        echo "使用Homebrew安装依赖..."
        brew install cmake pkg-config ninja git openssl libuv boost
    else
        echo "❌ 未找到Homebrew"
        echo "请先安装Homebrew: https://brew.sh/"
        echo "然后运行: brew install cmake pkg-config ninja git openssl libuv boost"
        exit 1
    fi
fi

echo "✅ 系统依赖安装完成"

# 创建临时目录
TEMP_DIR=$(mktemp -d)
echo ""
echo "📁 使用临时目录: $TEMP_DIR"

# 下载并编译MeshCat-cpp
echo ""
echo "⬇️  下载MeshCat-cpp源码..."
cd "$TEMP_DIR"
git clone https://github.com/ami-iit/meshcat-cpp.git
cd meshcat-cpp

echo ""
echo "🔧 编译MeshCat-cpp..."
mkdir build && cd build

# 配置CMake
if [[ "$OS" == "macos" ]]; then
    # macOS可能需要指定OpenSSL路径
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_INSTALL_PREFIX=/usr/local \
             -DOPENSSL_ROOT_DIR=$(brew --prefix openssl)
else
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_INSTALL_PREFIX=/usr/local
fi

# 编译
cmake --build . --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "📦 安装MeshCat-cpp..."
sudo cmake --install .

# 清理临时文件
echo ""
echo "🧹 清理临时文件..."
cd /
rm -rf "$TEMP_DIR"

echo ""
echo "✅ MeshCat-cpp安装完成！"
echo ""
echo "🎯 下一步："
echo "1. 重新编译TOWR项目："
echo "   cd towr/build"
echo "   cmake .. -DCMAKE_BUILD_TYPE=Release"
echo "   make -j4"
echo ""
echo "2. 运行MeshCat演示："
echo "   ./towr-meshcat-demo"
echo ""
echo "3. 在浏览器中打开显示的URL查看3D可视化"
echo ""
echo "🔗 更多信息："
echo "   • MeshCat-cpp: https://github.com/ami-iit/meshcat-cpp"
echo "   • TOWR: https://github.com/ethz-adrl/towr"
echo ""
echo "🎉 安装完成！享受3D可视化吧！"
