# TOWR 快速开始指南

## 🚀 5分钟快速上手

本指南帮助你快速搭建和运行TOWR轨迹优化演示。

## ✅ 前提条件检查

```bash
# 检查必需工具
which cmake     # 应该显示cmake路径
which make      # 应该显示make路径
which git       # 应该显示git路径

# 检查编译器
gcc --version   # 或 clang --version
```

## 📦 一键安装依赖

### macOS用户
```bash
# 使用Homebrew安装所有依赖
brew install cmake eigen ipopt
```

### Ubuntu/Debian用户
```bash
# 安装所有必需依赖
sudo apt-get update
sudo apt-get install cmake libeigen3-dev coinor-libipopt-dev build-essential
```

## 🔧 编译安装 (已验证流程)

### 第1步：克隆并进入项目
```bash
git clone https://github.com/ethz-adrl/towr.git
cd towr
```

### 第2步：安装ifopt依赖
```bash
# 克隆ifopt
git clone https://github.com/ethz-adrl/ifopt.git
cd ifopt && mkdir build && cd build

# 编译安装ifopt
cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
make -j4
sudo make install

# 返回项目根目录
cd ../../
```

### 第3步：编译TOWR核心库
```bash
cd towr && mkdir build && cd build

# 配置和编译
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
make -j4
```

## 🎯 运行Demo验证

### Demo 1: 基础跳跃机器人
```bash
# macOS用户需要设置库路径
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH

# 运行基础demo
./towr-example
```

**期望输出**:
```
TOWR - Trajectory Optimization for Walking Robots (v1.4)
...
Number of Iterations....: 6
EXIT: Optimal Solution Found.
```

### Demo 2: 详细轨迹分析
```bash
./towr-simple-demo
```

**期望输出**:
```
🚀 TOWR 轨迹优化演示系统
• 总变量数：423
• 总约束数：501
• 运动总时间：2.70 秒
• 最大跳跃高度：0.86 米
• 平均速度：0.74 米/秒
```

### Demo 3: 多机器人类型 (可选)
```bash
./towr-advanced-demo
```

## ✨ 快速定制示例

### 修改目标距离
编辑 `test/simple_demo.cpp`:
```cpp
// 将目标距离从2米改为3米
formulation.final_base_.lin.at(towr::kPos) << 3.0, 0.0, 0.5;
```

### 修改跳跃高度
```cpp
// 修改初始高度从0.5米到0.8米
formulation.initial_base_.lin.at(kPos).z() = 0.8;
```

重新编译并运行：
```bash
make -j4
./towr-simple-demo
```

## 🐛 快速故障排除

### 问题：CMake版本错误
```bash
# 解决方案：添加策略参数
cmake .. -DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

### 问题：找不到库文件 (macOS)
```bash
# 解决方案：设置库路径
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
echo 'export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH' >> ~/.zshrc
```

### 问题：权限错误
```bash
# 解决方案：确保有sudo权限
sudo make install
```

## 📊 性能基准

在标准配置下，你应该看到：

| 指标 | 期望值 |
|------|---------|
| 编译时间 | < 2分钟 |
| 基础demo运行时间 | < 5秒 |
| 轨迹优化迭代次数 | 6-15次 |
| 内存使用 | < 50MB |

## 🎓 下一步学习

1. **查看源码**: `towr/test/` 目录中的示例
2. **修改参数**: 尝试不同的机器人配置
3. **添加约束**: 学习添加自定义约束
4. **可视化**: 运行Python可视化脚本

## 📞 获取帮助

- **常见问题**: 查看 `INSTALLATION_GUIDE.md`
- **问题报告**: [GitHub Issues](https://github.com/ethz-adrl/towr/issues)
- **学术论文**: [TOWR论文](https://ieeexplore.ieee.org/document/8283570/)

---

**最后更新**: 2024-12-06  
**测试环境**: macOS Sequoia, Ubuntu 20.04  
**TOWR版本**: 1.4 