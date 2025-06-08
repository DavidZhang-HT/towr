# TOWR MeshCat 本地测试指南

本指南将帮助您在本地机器上测试TOWR的MeshCat 3D可视化功能。

## 🎯 测试目标

- 验证MeshCat-cpp库的安装
- 编译带有MeshCat支持的TOWR
- 运行3D可视化演示
- 在浏览器中查看机器人轨迹动画

## 📋 系统要求

### 支持的操作系统
- Ubuntu 18.04+ / Debian 10+
- macOS 10.15+
- 其他Linux发行版 (CentOS, Fedora等)

### 必需的软件
- C++编译器 (GCC 7+ 或 Clang 6+)
- CMake 3.1+
- Git
- 现代浏览器 (Chrome, Firefox, Safari, Edge)

## 🚀 快速开始

### 第一步：获取代码

#### 选项A: 克隆原始项目并添加MeshCat功能
```bash
# 1. 克隆TOWR项目
git clone https://github.com/ethz-adrl/towr.git
cd towr

# 2. 创建新分支用于测试
git checkout -b meshcat-visualization

# 3. 下载MeshCat功能文件 (见下方文件列表)
```

#### 选项B: 使用我们的完整包
```bash
# 如果您有完整的代码包
tar -xzf towr_meshcat_package.tar.gz
cd towr
```

### 第二步：添加MeshCat文件

您需要创建以下文件和目录结构：

```
towr/
├── cmake/
│   └── FindMeshcatCpp.cmake          # CMake查找模块
├── include/towr/visualization/
│   └── meshcat_visualizer.h          # 可视化器头文件
├── src/
│   └── meshcat_visualizer.cc         # 可视化器实现
├── test/
│   └── meshcat_demo.cpp              # 演示程序
├── CMakeLists.txt                    # 更新的构建配置
├── MESHCAT_VISUALIZATION.md          # 使用文档
└── install_meshcat.sh               # 安装脚本
```

### 第三步：安装MeshCat-cpp

```bash
# 使用自动安装脚本 (推荐)
chmod +x install_meshcat.sh
./install_meshcat.sh

# 或者手动安装
sudo apt update
sudo apt install cmake pkg-config build-essential ninja-build git \
                 libssl-dev libuv1-dev libz-dev libboost-dev

git clone https://github.com/ami-iit/meshcat-cpp.git
cd meshcat-cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
cd ../..
```

### 第四步：安装TOWR依赖

```bash
# Ubuntu/Debian
sudo apt install libeigen3-dev coinor-libipopt-dev

# macOS
brew install eigen ipopt

# 安装ifopt
git clone https://github.com/ethz-adrl/ifopt.git
cd ifopt
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

**期望输出**：
```
-- MeshCat-cpp found - enabling MeshCat visualization
-- MeshCat demo will be built: towr-meshcat-demo
-- Configuring done
-- Generating done
-- Build files have been written to: /path/to/towr/build
```

### 第六步：运行测试

```bash
# 1. 运行基础TOWR演示
./towr-example

# 2. 运行MeshCat可视化演示
./towr-meshcat-demo
```

## 🎬 演示程序说明

### towr-meshcat-demo

这是主要的MeshCat演示程序，运行后会：

1. **轨迹优化**: 求解单腿机器人的跳跃轨迹
2. **启动服务器**: 在端口7000启动MeshCat服务器
3. **显示URL**: 提示您在浏览器中打开可视化链接
4. **静态轨迹**: 首先显示完整的轨迹路径
5. **动画播放**: 播放3次实时动画

**期望输出**：
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

在浏览器中打开 `http://localhost:7000`，您将看到：

- **3D机器人模型**: 蓝色方块表示机器人本体
- **脚部状态**: 红色球(接触)，绿色球(腾空)
- **轨迹路径**: 蓝色线条显示运动轨迹
- **接触力**: 黄色箭头显示力的大小和方向
- **交互控制**: 鼠标拖拽旋转，滚轮缩放

## 🛠️ 故障排除

### 编译问题

**问题**: `MeshcatCpp not found`
```bash
# 检查安装
pkg-config --exists MeshcatCpp && echo "Found" || echo "Not found"

# 重新安装
./install_meshcat.sh
```

**问题**: `ifopt not found`
```bash
# 安装ifopt
git clone https://github.com/ethz-adrl/ifopt.git
cd ifopt && mkdir build && cd build
cmake .. && make -j4 && sudo make install
```

**问题**: `Eigen not found`
```bash
# Ubuntu/Debian
sudo apt install libeigen3-dev

# macOS
brew install eigen
```

### 运行时问题

**问题**: 端口7000被占用
```bash
# 检查端口占用
lsof -i :7000

# 修改端口 (编辑meshcat_demo.cpp)
auto visualizer = std::make_shared<MeshcatVisualizer>(8000);
```

**问题**: 浏览器无法连接
- 检查防火墙设置
- 尝试 `http://127.0.0.1:7000`
- 使用不同的浏览器

**问题**: 可视化显示异常
- 刷新浏览器页面
- 清除浏览器缓存
- 检查控制台错误信息

## 🧪 测试验证

### 基础功能测试

1. **编译测试**:
   ```bash
   cd towr/build
   make towr-meshcat-demo
   echo $?  # 应该输出 0
   ```

2. **运行测试**:
   ```bash
   ./towr-meshcat-demo
   # 检查是否显示MeshCat URL
   ```

3. **可视化测试**:
   - 在浏览器中打开显示的URL
   - 验证3D场景是否正常显示
   - 测试鼠标交互功能

### 高级功能测试

1. **不同机器人类型**:
   修改 `meshcat_demo.cpp` 中的机器人类型：
   ```cpp
   RobotModel robot_model(RobotModel::Biped);  // 双腿机器人
   ```

2. **自定义参数**:
   修改目标位置、时间等参数测试

3. **性能测试**:
   监控CPU和内存使用情况

## 📊 性能基准

在标准配置下的期望性能：

- **编译时间**: 2-5分钟 (取决于CPU)
- **优化时间**: 5-10秒
- **内存使用**: ~100MB
- **浏览器响应**: <100ms延迟

## 🎯 下一步

测试成功后，您可以：

1. **修改参数**: 尝试不同的机器人配置
2. **添加功能**: 实现新的可视化元素
3. **集成项目**: 将MeshCat集成到您的项目中
4. **贡献代码**: 向TOWR项目提交改进

## 📞 获取帮助

如果遇到问题：

1. 查看 `MESHCAT_VISUALIZATION.md` 详细文档
2. 检查 `IMPLEMENTATION_SUMMARY.md` 技术细节
3. 运行 `demo_meshcat_simple.py` 查看演示效果
4. 在GitHub上提交Issue

## 🎉 成功标志

测试成功的标志：

- ✅ 编译无错误，生成 `towr-meshcat-demo`
- ✅ 程序运行显示优化结果和MeshCat URL
- ✅ 浏览器中显示3D机器人模型
- ✅ 可以看到流畅的轨迹动画
- ✅ 鼠标交互正常工作

恭喜！您已经成功在本地测试了TOWR的MeshCat 3D可视化功能！
