# TOWR 构建与安装文档

## 📚 文档指南

本目录包含了TOWR (Trajectory Optimization for Walking Robots) 的完整编译安装文档系统。

### 📖 文档结构

| 文档 | 描述 | 适用场景 |
|------|------|----------|
| [`INSTALLATION_GUIDE.md`](./INSTALLATION_GUIDE.md) | 详细安装指南 | 需要完整了解安装过程的用户 |
| [`QUICK_START.md`](./QUICK_START.md) | 5分钟快速上手 | 希望快速开始的用户 |
| [`build_towr.sh`](./build_towr.sh) | 自动化构建脚本 | 一键自动编译安装 |
| [`README_BUILD.md`](./README_BUILD.md) | 本文档，构建指南总览 | 选择合适的安装方式 |

## 🚀 三种安装方式

### 方式1: 自动化脚本安装 (推荐)
```bash
# 一键安装，自动检测系统并安装所有依赖
./build_towr.sh

# 清理重新构建
./build_towr.sh --clean

# 跳过测试验证
./build_towr.sh --skip-tests
```

### 方式2: 快速手动安装
```bash
# 参考快速开始指南
cat QUICK_START.md

# 核心步骤 (5分钟)
brew install cmake eigen ipopt  # macOS
git clone https://github.com/ethz-adrl/ifopt.git
# ... 详见QUICK_START.md
```

### 方式3: 详细手动安装
```bash
# 参考完整安装指南
cat INSTALLATION_GUIDE.md

# 包含完整的系统要求、故障排除等
```

## ✅ 验证安装

安装完成后，你应该能够运行以下命令：

```bash
cd towr/build

# 基础演示
./towr-example

# 详细分析演示  
./towr-simple-demo

# 高级多机器人演示
./towr-advanced-demo

# 单元测试
./towr-test
```

## 🎯 期望结果

成功安装后，你应该看到：

```
TOWR - Trajectory Optimization for Walking Robots (v1.4)
...
Total number of variables............................:      313
Total number of equality constraints.................:      265
Total number of inequality constraints...............:      134

Number of Iterations....: 6
EXIT: Optimal Solution Found.
```

## 🔧 系统要求

### 最低要求
- **操作系统**: macOS 10.14+, Ubuntu 18.04+, CentOS 7+
- **编译器**: GCC 7.0+ 或 Clang 6.0+
- **CMake**: 3.5+
- **依赖**: Eigen3, IPOPT

### 推荐配置
- **CPU**: 4核或更多 (加速编译)
- **内存**: 4GB+ (编译时需要)
- **磁盘**: 1GB+ 可用空间

## 🐛 常见问题

| 问题 | 解决方案 | 文档 |
|------|----------|------|
| CMake版本过旧 | 添加 `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` | INSTALLATION_GUIDE.md |
| 找不到库文件 (macOS) | 设置 `DYLD_LIBRARY_PATH` | QUICK_START.md |
| 编译警告 | 可以忽略，不影响功能 | 所有文档 |
| 权限错误 | 使用 `sudo make install` | build_towr.sh |

## 📊 性能基准

在标准硬件上的预期性能：

| 指标 | 期望值 |
|------|--------|
| 编译时间 | < 2分钟 |
| 演示运行时间 | < 5秒 |
| 内存占用 | < 50MB |
| 优化迭代次数 | 6-15次 |

## 🎓 下一步

安装完成后，推荐的学习路径：

1. **运行基础演示**: `./towr-example`
2. **查看源码**: `towr/test/` 目录
3. **修改参数**: 尝试不同的机器人配置
4. **学习API**: 查看头文件和文档
5. **自定义约束**: 添加特定应用的约束

## 📞 获取帮助

- **构建问题**: 查看本目录的文档
- **API问题**: 访问 [官方文档](http://docs.ros.org/kinetic/api/towr/html/)
- **Bug报告**: [GitHub Issues](https://github.com/ethz-adrl/towr/issues)
- **学术支持**: [TOWR论文](https://ieeexplore.ieee.org/document/8283570/)

## 📋 文档维护

- **创建日期**: 2024-12-06
- **最后更新**: 2024-12-06  
- **维护者**: TOWR构建文档团队
- **测试环境**: macOS Sequoia, Ubuntu 20.04
- **TOWR版本**: 1.4+

---

**选择建议**: 
- 新用户推荐使用自动化脚本 `./build_towr.sh`
- 有经验用户可参考快速开始指南
- 遇到问题时查阅详细安装指南 