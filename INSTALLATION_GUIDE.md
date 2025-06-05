# TOWR 编译安装指南

## 📋 概述

本文档提供了TOWR (Trajectory Optimization for Walking Robots) 的完整编译安装指南。TOWR是一个用于腿式机器人轨迹优化的轻量级C++库。

## 🖥️ 系统要求

### 支持的操作系统
- **Linux**: Ubuntu 18.04+, CentOS 7+
- **macOS**: 10.14+
- **Windows**: Windows 10 (使用WSL或MinGW)

### 编译器要求
- **GCC**: 7.0+
- **Clang**: 6.0+
- **C++标准**: C++11或更高

## 📦 依赖安装

### 必需依赖

#### 1. CMake (3.5+)
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# CentOS/RHEL
sudo yum install cmake3
```

#### 2. Eigen3 线性代数库
```bash
# Ubuntu/Debian
sudo apt-get install libeigen3-dev

# macOS
brew install eigen

# CentOS/RHEL
sudo yum install eigen3-devel
```

#### 3. IPOPT 优化求解器
```bash
# Ubuntu/Debian
sudo apt-get install coinor-libipopt-dev

# macOS
brew install ipopt

# CentOS/RHEL
sudo yum install coin-or-Ipopt-devel
```

### 可选依赖（用于完整功能）

#### 4. ROS (用于可视化和GUI)
```bash
# Ubuntu 18.04
sudo apt-get install ros-melodic-desktop-full ros-melodic-xpp

# Ubuntu 20.04
sudo apt-get install ros-noetic-desktop-full ros-noetic-xpp

# macOS (不推荐，建议使用Linux虚拟机)
```

#### 5. 其他工具
```bash
# Ubuntu/Debian
sudo apt-get install libncurses5-dev xterm

# macOS
brew install ncurses
```

## 🔧 编译安装步骤

### 方法1：仅核心库编译 (推荐快速开始)

#### 步骤1：克隆项目
```bash
git clone https://github.com/ethz-adrl/towr.git
cd towr
```

#### 步骤2：安装ifopt依赖
```bash
# 克隆ifopt库
git clone https://github.com/ethz-adrl/ifopt.git
cd ifopt
mkdir build && cd build

# 配置编译
cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
make -j$(nproc)
sudo make install

# 返回项目根目录
cd ../../
```

#### 步骤3：编译TOWR核心库
```bash
cd towr
mkdir build && cd build

# 配置编译
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5

# 编译
make -j$(nproc)

# 可选：安装到系统
sudo make install
```

### 方法2：使用ROS编译 (完整功能)

#### 步骤1：设置catkin工作空间
```bash
# 创建catkin工作空间
mkdir -p ~/towr_ws/src
cd ~/towr_ws/src

# 初始化工作空间
catkin_init_workspace
```

#### 步骤2：克隆依赖和项目
```bash
# 克隆依赖
git clone https://github.com/ethz-adrl/ifopt.git
git clone https://github.com/ethz-adrl/towr.git

# 返回工作空间根目录
cd ..
```

#### 步骤3：编译整个工作空间
```bash
# 使用catkin编译
catkin_make_isolated -DCMAKE_BUILD_TYPE=Release

# 或者使用catkin build (如果安装了catkin tools)
catkin build

# 设置环境变量
source ./devel_isolated/setup.bash
```

## 🏃‍♂️ 运行Demo

### 基础单腿跳跃Demo
```bash
# 设置库路径 (macOS需要)
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH

# 运行基础demo
cd towr/build
./towr-example
```

### 详细分析Demo
```bash
# 运行详细分析demo
./towr-simple-demo
```

### 高级多机器人Demo
```bash
# 运行高级demo
./towr-advanced-demo
```

### ROS可视化Demo (如果安装了ROS)
```bash
# 启动ROS节点
roslaunch towr_ros towr_ros.launch

# 在弹出的xterm终端中按 'o' 开始优化
```

## 📊 验证安装

### 检查编译结果
```bash
# 检查生成的可执行文件
ls -la towr/build/towr-*

# 应该看到以下文件：
# - towr-example
# - towr-simple-demo  
# - towr-advanced-demo
# - towr-test
```

### 运行测试
```bash
# 运行单元测试
cd towr/build
./towr-test

# 运行基础示例测试
make test
```

### 期望输出
成功安装后，运行demo应该看到类似输出：
```
************************************************************
 TOWR - Trajectory Optimization for Walking Robots (v1.4)
                © Alexander W. Winkler
           https://github.com/ethz-adrl/towr
************************************************************

Total number of variables............................:      313
Total number of equality constraints.................:      265
Total number of inequality constraints...............:      134

Number of Iterations....: 6
EXIT: Optimal Solution Found.
```

## 🐛 常见问题和解决方案

### 问题1：CMake版本过旧
**错误信息**: `Compatibility with CMake < 3.5 has been removed`
**解决方案**: 
```bash
# 添加策略版本参数
cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

### 问题2：找不到ifopt库
**错误信息**: `Could not find a package configuration file provided by "ifopt"`
**解决方案**:
```bash
# 确保ifopt已正确安装
sudo ldconfig  # Linux
# 或重新安装ifopt
```

### 问题3：macOS动态库路径问题
**错误信息**: `Library not loaded: @rpath/libifopt_ipopt.dylib`
**解决方案**:
```bash
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

### 问题4：编译警告
**警告信息**: `non-void function does not return a value`
**解决方案**: 这些是库内部的警告，不影响功能，可以忽略。

### 问题5：双足机器人优化失败
**错误信息**: `Assertion failed: (row>=0 && row<rows()...)`
**解决方案**: 
- 调整初始参数
- 使用更保守的步态配置
- 增加求解器时间限制

## 🎯 性能优化建议

### 编译优化
```bash
# 使用Release模式编译获得最佳性能
cmake .. -DCMAKE_BUILD_TYPE=Release

# 启用本地架构优化
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native"
```

### 求解器优化
```cpp
// 在代码中调整IPOPT参数
solver->SetOption("max_cpu_time", 20.0);          // 增加求解时间
solver->SetOption("tol", 1e-4);                   // 调整容差
solver->SetOption("print_level", 0);              // 减少输出提高速度
```

## 📚 下一步

安装完成后，你可以：

1. **学习示例**: 查看 `towr/test/` 目录中的示例代码
2. **阅读文档**: 访问 [官方文档](http://docs.ros.org/kinetic/api/towr/html/)
3. **自定义机器人**: 参考 `towr/include/towr/models/examples/` 
4. **可视化结果**: 使用提供的Python脚本或ROS工具
5. **添加约束**: 扩展约束集合满足特定需求

## 🔗 相关资源

- **项目主页**: https://github.com/ethz-adrl/towr
- **学术论文**: [Gait and Trajectory Optimization for Legged Systems](https://ieeexplore.ieee.org/document/8283570/)
- **API文档**: http://docs.ros.org/kinetic/api/towr/html/
- **问题报告**: https://github.com/ethz-adrl/towr/issues

## 📄 许可证

TOWR使用BSD-3-Clause许可证。详见项目根目录的LICENSE文件。

---

**编译日期**: $(date +%Y-%m-%d)  
**文档版本**: 1.0  
**适用TOWR版本**: 1.4+ 