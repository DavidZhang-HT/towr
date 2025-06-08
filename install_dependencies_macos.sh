#!/bin/bash

# TOWR MacOS 依赖安装脚本
# 适用于 Apple Silicon 和 Intel Mac
# 更新日期: 2025年1月

set -e  # 遇到错误时退出

echo "🚀 TOWR MacOS 依赖安装脚本"
echo "============================================================"

# 检测系统架构
ARCH=$(uname -m)
echo "📱 检测到系统架构: $ARCH"

# 检查是否安装了 Homebrew
if ! command -v brew &> /dev/null; then
    echo "❌ Homebrew 未安装，正在安装..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    # 为 Apple Silicon Mac 添加 Homebrew 到 PATH
    if [[ "$ARCH" == "arm64" ]]; then
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zshrc
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
else
    echo "✅ Homebrew 已安装"
fi

# 更新 Homebrew
echo "🔄 更新 Homebrew..."
brew update

# 安装基础开发工具
echo "🛠️ 安装基础开发工具..."
brew install cmake
brew install pkg-config
brew install git

# 安装数学和优化库
echo "📐 安装数学和优化库..."
brew install eigen
brew install ipopt
brew install cppunit

# 安装 Python 和可视化依赖
echo "🐍 安装 Python 和可视化依赖..."
brew install python3
brew install python-matplotlib
brew install numpy

# 使用 pip 安装额外的 Python 包
echo "📦 安装额外的 Python 包..."
/opt/homebrew/bin/python3 -m pip install --upgrade pip
/opt/homebrew/bin/python3 -m pip install numpy matplotlib scipy

# 检查安装状态
echo "🔍 检查依赖安装状态..."

check_dependency() {
    local name=$1
    local command=$2
    
    if command -v $command &> /dev/null; then
        local version=$($command --version 2>/dev/null | head -n1 || echo "已安装")
        echo "✅ $name: $version"
        return 0
    else
        echo "❌ $name: 未找到"
        return 1
    fi
}

check_brew_package() {
    local name=$1
    local package=$2
    
    if brew list $package &> /dev/null; then
        local version=$(brew list --versions $package | head -n1)
        echo "✅ $name: $version"
        return 0
    else
        echo "❌ $name: 未安装"
        return 1
    fi
}

echo ""
echo "📋 依赖检查报告:"
echo "----------------------------------------"

check_dependency "CMake" "cmake"
check_dependency "Git" "git"
check_dependency "Python3" "python3"

check_brew_package "Eigen3" "eigen"
check_brew_package "IPOPT" "ipopt"
check_brew_package "CPPUnit" "cppunit"
check_brew_package "matplotlib" "python-matplotlib"

# 检查 Python 包
echo ""
echo "🐍 Python 包检查:"
echo "----------------------------------------"

check_python_package() {
    local package=$1
    if /opt/homebrew/bin/python3 -c "import $package" 2>/dev/null; then
        local version=$(/opt/homebrew/bin/python3 -c "import $package; print($package.__version__)" 2>/dev/null || echo "已安装")
        echo "✅ $package: $version"
    else
        echo "❌ $package: 未安装"
    fi
}

check_python_package "numpy"
check_python_package "matplotlib"

echo ""
echo "🎯 安装完成！"
echo "============================================================"
echo "📝 下一步:"
echo "1. 运行 ./build_towr.sh 构建项目"
echo "2. 运行测试验证安装"
echo "3. 查看 PROJECT_STATUS.md 了解项目状态"
echo ""
echo "💡 提示:"
echo "- 如果遇到权限问题，请确保有管理员权限"
echo "- 如果 Python 包安装失败，尝试使用 pyenv 管理 Python 版本"
echo "- 查看 INSTALLATION_GUIDE.md 获取详细说明"
