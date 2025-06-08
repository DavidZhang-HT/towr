# TOWR MeshCat 3D可视化功能

本文档介绍如何在TOWR中使用基于MeshCat的3D可视化功能。

## 🌟 功能特性

- **实时3D可视化**: 在浏览器中查看机器人轨迹
- **交互式场景**: 支持鼠标拖拽旋转、缩放视角
- **多机器人支持**: 支持单腿、双腿、四腿机器人
- **动态播放**: 实时播放轨迹动画
- **力可视化**: 显示接触力向量
- **相位显示**: 区分接触和腾空相位
- **地形可视化**: 显示地形高度图

## 🚀 快速开始

### 1. 安装MeshCat-cpp

使用提供的自动安装脚本：

```bash
# 运行安装脚本
./install_meshcat.sh
```

或者手动安装：

```bash
# 安装系统依赖 (Ubuntu/Debian)
sudo apt install cmake pkg-config build-essential ninja-build git \
                 libssl-dev libuv1-dev libz-dev libboost-dev

# 下载并编译MeshCat-cpp
git clone https://github.com/ami-iit/meshcat-cpp.git
cd meshcat-cpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
sudo make install
```

### 2. 重新编译TOWR

```bash
cd towr/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

如果MeshCat-cpp安装成功，您应该看到：
```
-- MeshCat-cpp found - enabling MeshCat visualization
-- MeshCat demo will be built: towr-meshcat-demo
```

### 3. 运行演示

```bash
# 运行MeshCat可视化演示
./towr-meshcat-demo
```

程序会显示类似以下的输出：
```
🚀 TOWR MeshCat 可视化演示
==================================================
🌐 MeshCat可视化器已启动
📱 请在浏览器中打开: http://localhost:7000
```

在浏览器中打开显示的URL即可查看3D可视化。

## 📊 可视化元素说明

### 机器人模型
- **蓝色方块**: 机器人本体
- **红色球**: 脚部接触地面时
- **绿色球**: 脚部腾空时
- **灰色线**: 腿部连接线

### 轨迹显示
- **蓝色轨迹线**: 机器人本体运动路径
- **红色轨迹线**: 脚部运动路径

### 力可视化
- **黄色箭头**: 接触力向量
- 箭头长度表示力的大小
- 箭头方向表示力的方向

### 地形
- **灰色网格**: 地形高度图

## 🎮 交互操作

- **鼠标左键拖拽**: 旋转视角
- **鼠标滚轮**: 缩放场景
- **鼠标右键拖拽**: 平移视角

## 🔧 编程接口

### 基本使用

```cpp
#include <towr/visualization/meshcat_visualizer.h>

// 创建可视化器
auto visualizer = std::make_shared<MeshcatVisualizer>(7000);

// 初始化机器人模型
RobotModel robot_model(RobotModel::Monoped);
visualizer->Initialize(robot_model);

// 设置地形
visualizer->SetTerrain(terrain_ptr, {-1.0, 3.0}, {-1.0, 1.0}, 0.1);

// 可视化轨迹
visualizer->VisualizeTrajectory(splines, 0.02);

// 播放动画
visualizer->PlayTrajectory(splines, 1.0, 0.05, true);
```

### 主要方法

#### `MeshcatVisualizer(int port = 7000)`
创建MeshCat可视化器，指定服务器端口。

#### `Initialize(const RobotModel& robot_model)`
初始化机器人模型，设置机器人几何体。

#### `SetTerrain(terrain, x_range, y_range, resolution)`
设置地形可视化。
- `terrain`: 地形高度图
- `x_range`, `y_range`: 可视化范围
- `resolution`: 网格分辨率

#### `VisualizeTrajectory(splines, dt, total_duration)`
显示完整轨迹路径。
- `splines`: 轨迹样条数据
- `dt`: 采样时间间隔
- `total_duration`: 总时长

#### `PlayTrajectory(splines, playback_speed, dt, loop)`
播放轨迹动画。
- `playback_speed`: 播放速度倍数
- `dt`: 动画时间间隔
- `loop`: 是否循环播放

#### `VisualizeState(t, base_state, ee_positions, ee_forces, contact_states)`
可视化单个时刻的机器人状态。

## 🛠️ 故障排除

### 编译错误

**问题**: `MeshcatCpp not found`
**解决**: 确保MeshCat-cpp正确安装，检查CMake能否找到库：
```bash
pkg-config --exists MeshcatCpp && echo "Found" || echo "Not found"
```

**问题**: 链接错误
**解决**: 确保所有依赖库都已安装：
```bash
# Ubuntu/Debian
sudo apt install libssl-dev libuv1-dev libz-dev libboost-dev

# macOS
brew install openssl libuv boost
```

### 运行时错误

**问题**: 浏览器无法连接
**解决**: 
1. 检查端口7000是否被占用
2. 尝试使用不同端口：`MeshcatVisualizer(7001)`
3. 检查防火墙设置

**问题**: 可视化显示异常
**解决**:
1. 刷新浏览器页面
2. 清除浏览器缓存
3. 尝试不同的浏览器

## 📝 示例程序

### 1. 基础演示 (`towr-meshcat-demo`)
展示单腿机器人的完整轨迹优化和可视化流程。

### 2. 自定义可视化
```cpp
// 创建自定义可视化
auto visualizer = std::make_shared<MeshcatVisualizer>();
visualizer->Initialize(robot_model);

// 逐帧更新
for (double t = 0; t <= total_time; t += dt) {
    // 获取机器人状态
    BaseState base_state = GetBaseState(t);
    std::vector<Vector3d> ee_pos = GetEEPositions(t);
    std::vector<Vector3d> ee_forces = GetEEForces(t);
    std::vector<bool> contact = GetContactStates(t);
    
    // 更新可视化
    visualizer->VisualizeState(t, base_state, ee_pos, ee_forces, contact);
    
    // 控制播放速度
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
```

## 🔗 相关链接

- [MeshCat-cpp GitHub](https://github.com/ami-iit/meshcat-cpp)
- [MeshCat原版 (Python)](https://github.com/rdeits/meshcat-python)
- [TOWR项目主页](https://github.com/ethz-adrl/towr)

## 🤝 贡献

欢迎提交Issue和Pull Request来改进MeshCat可视化功能！

## 📄 许可证

本可视化功能遵循TOWR项目的BSD-3-Clause许可证。
