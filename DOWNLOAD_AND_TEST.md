# 📦 TOWR MeshCat 本地下载和测试指南

## 🎯 概述

这个指南将帮助您将TOWR MeshCat可视化功能下载到本地机器并进行测试。

## 📥 获取代码的方法

### 方法1: 从GitHub克隆 (推荐)

```bash
# 1. 克隆原始TOWR项目
git clone https://github.com/ethz-adrl/towr.git
cd towr

# 2. 创建新分支用于MeshCat功能
git checkout -b meshcat-visualization
```

然后您需要手动添加我们创建的文件（见下方文件列表）。

### 方法2: 下载完整包

如果您有访问权限，可以下载我们创建的完整包：
```bash
# 解压完整包
tar -xzf towr_meshcat_package.tar.gz
cd towr
```

### 方法3: 手动创建文件

您可以根据下面的文件内容手动创建所有文件。

## 📋 需要创建的文件列表

### 1. 项目根目录文件

#### `install_meshcat.sh` (可执行)
```bash
#!/bin/bash
# TOWR MeshCat-cpp 安装脚本
# [完整内容见原文件]
```

#### `MESHCAT_VISUALIZATION.md`
```markdown
# TOWR MeshCat 3D可视化功能
# [完整内容见原文件]
```

#### `LOCAL_TESTING_GUIDE.md`
```markdown
# TOWR MeshCat 本地测试指南
# [完整内容见原文件]
```

#### `demo_meshcat_simple.py` (可执行)
```python
#!/usr/bin/env python3
# [完整内容见原文件]
```

#### `quick_test.py` (可执行)
```python
#!/usr/bin/env python3
# [完整内容见原文件]
```

### 2. TOWR项目内文件

#### `towr/cmake/FindMeshcatCpp.cmake`
```cmake
# FindMeshcatCpp.cmake
# [完整内容见原文件]
```

#### `towr/include/towr/visualization/meshcat_visualizer.h`
```cpp
// MeshCat可视化器头文件
// [完整内容见原文件]
```

#### `towr/src/meshcat_visualizer.cc`
```cpp
// MeshCat可视化器实现
// [完整内容见原文件]
```

#### `towr/test/meshcat_demo.cpp`
```cpp
// MeshCat演示程序
// [完整内容见原文件]
```

### 3. 修改现有文件

#### `towr/CMakeLists.txt`
需要添加以下内容：

在第4-5行后添加：
```cmake
# Find MeshCat-cpp for visualization (optional)
find_package(MeshcatCpp QUIET)
if(MeshcatCpp_FOUND)
  message(STATUS "MeshCat-cpp found - enabling MeshCat visualization")
  add_definitions(-DTOWR_WITH_MESHCAT)
else()
  message(STATUS "MeshCat-cpp not found - MeshCat visualization disabled")
endif()
```

在源文件列表中添加：
```cmake
# visualization (optional)
$<$<BOOL:${MeshcatCpp_FOUND}>:src/meshcat_visualizer.cc>
```

在链接库中添加：
```cmake
$<$<BOOL:${MeshcatCpp_FOUND}>:MeshcatCpp::MeshcatCpp>
```

在测试部分添加：
```cmake
# MeshCat visualization demo (only if MeshCat-cpp is available)
if(MeshcatCpp_FOUND)
  add_executable(${PROJECT_NAME}-meshcat-demo 
    test/meshcat_demo.cpp
  )
  target_link_libraries(${PROJECT_NAME}-meshcat-demo  
    PRIVATE
      ${PROJECT_NAME} 
      ifopt::ifopt_ipopt
  )
  add_test(${PROJECT_NAME}-meshcat-demo ${PROJECT_NAME}-meshcat-demo)
endif()
```

#### `README.md`
在Features部分添加：
```markdown
:heavy_check_mark: **NEW**: Interactive 3D visualization using [MeshCat] (web-based, cross-platform).
```

在Run部分添加MeshCat选项。

## 🚀 本地测试步骤

### 第一步：系统准备

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install cmake build-essential git pkg-config \
                 libeigen3-dev coinor-libipopt-dev \
                 libssl-dev libuv1-dev libz-dev libboost-dev

# macOS
brew install cmake git pkg-config eigen ipopt openssl libuv boost
```

### 第二步：快速验证

```bash
# 运行快速测试脚本
python3 quick_test.py
```

这会检查：
- 系统依赖是否满足
- 文件结构是否正确
- Python演示是否能运行

### 第三步：安装ifopt

```bash
git clone https://github.com/ethz-adrl/ifopt.git
cd ifopt
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
cd ../..
```

### 第四步：安装MeshCat-cpp

```bash
# 使用自动安装脚本
chmod +x install_meshcat.sh
./install_meshcat.sh

# 或者手动安装
git clone https://github.com/ami-iit/meshcat-cpp.git
cd meshcat-cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
cd ../..
```

### 第五步：编译TOWR

```bash
cd towr
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**期望看到**：
```
-- MeshCat-cpp found - enabling MeshCat visualization
-- MeshCat demo will be built: towr-meshcat-demo
```

### 第六步：运行演示

```bash
# 基础演示
./towr-example

# MeshCat可视化演示
./towr-meshcat-demo
```

### 第七步：浏览器查看

打开浏览器，访问程序显示的URL（通常是 `http://localhost:7000`）

## 🎬 期望的演示效果

### 控制台输出
```
🚀 TOWR MeshCat 可视化演示
==================================================
🤖 开始单腿机器人轨迹优化...
🔧 正在求解优化问题...
✅ 优化完成！
📊 优化结果统计：
   • 总变量数: 423
   • 总约束数: 501
   • 运动总时间: 2.70 秒
🌐 启动MeshCat可视化...
📱 请在浏览器中打开: http://localhost:7000
```

### 浏览器可视化
- 3D机器人模型（蓝色方块）
- 脚部状态（红色/绿色球）
- 运动轨迹（蓝色线条）
- 接触力向量（黄色箭头）
- 流畅的动画播放

## 🛠️ 常见问题

### Q: 编译时找不到MeshCat-cpp
A: 确保运行了 `./install_meshcat.sh` 并且没有错误

### Q: 浏览器无法连接
A: 检查端口7000是否被占用，尝试使用其他端口

### Q: 动画不流畅
A: 这是正常的，可以调整播放速度参数

### Q: 缺少某些依赖
A: 运行 `quick_test.py` 检查所有依赖

## 📞 获取帮助

1. 查看 `LOCAL_TESTING_GUIDE.md` 详细指南
2. 运行 `python3 demo_meshcat_simple.py` 查看演示
3. 检查 `MESHCAT_VISUALIZATION.md` 使用文档

## 🎉 成功标志

- ✅ `quick_test.py` 显示所有检查通过
- ✅ 编译时显示 "MeshCat-cpp found"
- ✅ 程序运行显示MeshCat URL
- ✅ 浏览器中看到3D机器人动画

恭喜！您已经成功在本地运行了TOWR的MeshCat 3D可视化功能！
