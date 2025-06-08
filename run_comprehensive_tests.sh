#!/bin/bash

# TOWR 综合测试脚本
# 运行所有测试并生成详细报告
# 更新日期: 2025年1月

set -e

echo "🧪 TOWR 综合测试脚本"
echo "============================================================"

# 检查构建目录
if [ ! -d "towr/build" ]; then
    echo "❌ 构建目录不存在，请先运行 ./build_towr.sh"
    exit 1
fi

cd towr/build

# 设置库路径
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH

# 测试结果统计
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# 测试结果记录
TEST_RESULTS=()

run_test() {
    local test_name=$1
    local test_command=$2
    local description=$3
    
    echo ""
    echo "🔍 运行测试: $test_name"
    echo "📝 描述: $description"
    echo "⚡ 命令: $test_command"
    echo "----------------------------------------"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if eval $test_command; then
        echo "✅ $test_name: 通过"
        TEST_RESULTS+=("✅ $test_name: 通过")
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        echo "❌ $test_name: 失败"
        TEST_RESULTS+=("❌ $test_name: 失败")
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

# 运行单元测试
run_test "单元测试" "./towr-test" "核心算法单元测试，验证基础功能"

# 运行基础示例
run_test "基础示例" "./towr-example" "单腿跳跃机器人轨迹优化示例"

# 运行简单演示
run_test "简单演示" "./towr-simple-demo" "详细的轨迹分析和输出演示"

# 运行高级演示（可能失败）
echo ""
echo "⚠️ 注意: 高级演示可能在双足机器人部分失败，这是已知问题"
run_test "高级演示" "./towr-advanced-demo" "多机器人类型演示（已知双足机器人问题）" || true

# 运行 CMake 测试
echo ""
echo "🔍 运行 CMake 测试套件..."
if command -v ctest &> /dev/null; then
    run_test "CMake测试" "ctest --verbose" "CMake 测试套件"
else
    echo "⚠️ ctest 不可用，跳过 CMake 测试"
fi

# 返回根目录运行 Python 测试
cd ../..

# 运行 Python 可视化测试
echo ""
echo "🐍 运行 Python 可视化测试..."
run_test "Python可视化" "/opt/homebrew/bin/python3 demo_meshcat_integration.py" "MeshCat 集成可视化演示"

# 生成测试报告
echo ""
echo "📊 测试报告"
echo "============================================================"
echo "📈 总体统计:"
echo "   总测试数: $TOTAL_TESTS"
echo "   通过测试: $PASSED_TESTS"
echo "   失败测试: $FAILED_TESTS"
echo "   成功率: $(( PASSED_TESTS * 100 / TOTAL_TESTS ))%"

echo ""
echo "📋 详细结果:"
for result in "${TEST_RESULTS[@]}"; do
    echo "   $result"
done

# 生成建议
echo ""
echo "💡 建议和下一步:"
echo "----------------------------------------"

if [ $FAILED_TESTS -eq 0 ]; then
    echo "🎉 所有测试通过！项目状态良好。"
    echo "✨ 可以开始使用 TOWR 进行轨迹优化开发。"
elif [ $PASSED_TESTS -ge 3 ]; then
    echo "✅ 核心功能正常，项目基本可用。"
    echo "🔧 建议修复失败的测试以获得完整功能。"
else
    echo "⚠️ 多个测试失败，建议检查依赖安装。"
    echo "📖 查看 INSTALLATION_GUIDE.md 获取帮助。"
fi

echo ""
echo "📚 相关文档:"
echo "   - PROJECT_STATUS.md: 项目状态详情"
echo "   - MESHCAT_VISUALIZATION.md: 可视化功能说明"
echo "   - INSTALLATION_GUIDE.md: 安装指南"

# 保存测试报告到文件
REPORT_FILE="test_report_$(date +%Y%m%d_%H%M%S).txt"
{
    echo "TOWR 测试报告 - $(date)"
    echo "============================================================"
    echo "总测试数: $TOTAL_TESTS"
    echo "通过测试: $PASSED_TESTS"
    echo "失败测试: $FAILED_TESTS"
    echo "成功率: $(( PASSED_TESTS * 100 / TOTAL_TESTS ))%"
    echo ""
    echo "详细结果:"
    for result in "${TEST_RESULTS[@]}"; do
        echo "$result"
    done
} > "$REPORT_FILE"

echo ""
echo "📄 测试报告已保存到: $REPORT_FILE"
