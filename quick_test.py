#!/usr/bin/env python3
"""
TOWR MeshCat 快速测试脚本

这个脚本可以在没有完整TOWR环境的情况下快速验证：
1. 系统依赖是否满足
2. 基本的可视化概念演示
3. 文件结构是否正确
"""

import os
import sys
import subprocess
import platform

def print_header():
    print("🧪 TOWR MeshCat 快速测试")
    print("=" * 50)
    print("这个脚本将验证您的系统是否准备好运行TOWR MeshCat可视化")
    print()

def check_system_info():
    print("🖥️  系统信息:")
    print(f"   操作系统: {platform.system()} {platform.release()}")
    print(f"   Python版本: {sys.version.split()[0]}")
    print(f"   架构: {platform.machine()}")
    print()

def check_command(cmd, name):
    """检查命令是否可用"""
    try:
        result = subprocess.run([cmd, "--version"], 
                              capture_output=True, text=True, timeout=5)
        if result.returncode == 0:
            version = result.stdout.split('\n')[0]
            print(f"   ✅ {name}: {version}")
            return True
    except:
        pass
    
    try:
        result = subprocess.run(["which", cmd], 
                              capture_output=True, text=True, timeout=5)
        if result.returncode == 0:
            print(f"   ✅ {name}: 已安装")
            return True
    except:
        pass
    
    print(f"   ❌ {name}: 未找到")
    return False

def check_dependencies():
    print("🔧 检查系统依赖:")
    
    deps = [
        ("cmake", "CMake"),
        ("gcc", "GCC编译器"),
        ("g++", "G++编译器"),
        ("git", "Git"),
        ("pkg-config", "pkg-config")
    ]
    
    all_good = True
    for cmd, name in deps:
        if not check_command(cmd, name):
            all_good = False
    
    print()
    return all_good

def check_optional_deps():
    print("📦 检查可选依赖:")
    
    optional_deps = [
        ("python3", "Python3"),
        ("pip3", "pip3"),
        ("nano", "Nano编辑器"),
        ("vim", "Vim编辑器")
    ]
    
    for cmd, name in optional_deps:
        check_command(cmd, name)
    
    print()

def check_file_structure():
    print("📁 检查文件结构:")
    
    required_files = [
        "install_meshcat.sh",
        "MESHCAT_VISUALIZATION.md",
        "LOCAL_TESTING_GUIDE.md",
        "demo_meshcat_simple.py",
        "towr/CMakeLists.txt",
        "towr/cmake/FindMeshcatCpp.cmake",
        "towr/include/towr/visualization/meshcat_visualizer.h",
        "towr/src/meshcat_visualizer.cc",
        "towr/test/meshcat_demo.cpp"
    ]
    
    all_files_exist = True
    for file_path in required_files:
        if os.path.exists(file_path):
            print(f"   ✅ {file_path}")
        else:
            print(f"   ❌ {file_path} (缺失)")
            all_files_exist = False
    
    print()
    return all_files_exist

def check_permissions():
    print("🔐 检查文件权限:")
    
    executable_files = [
        "install_meshcat.sh",
        "demo_meshcat_simple.py",
        "quick_test.py"
    ]
    
    for file_path in executable_files:
        if os.path.exists(file_path):
            if os.access(file_path, os.X_OK):
                print(f"   ✅ {file_path} (可执行)")
            else:
                print(f"   ⚠️  {file_path} (不可执行，运行: chmod +x {file_path})")
        else:
            print(f"   ❌ {file_path} (文件不存在)")
    
    print()

def test_python_demo():
    print("🐍 测试Python演示:")
    
    if os.path.exists("demo_meshcat_simple.py"):
        try:
            print("   正在运行demo_meshcat_simple.py...")
            result = subprocess.run([sys.executable, "demo_meshcat_simple.py"], 
                                  input="n\n", text=True, capture_output=True, timeout=30)
            
            if result.returncode == 0:
                print("   ✅ Python演示运行成功")
                # 显示部分输出
                lines = result.stdout.split('\n')[:10]
                for line in lines:
                    if line.strip():
                        print(f"      {line}")
                if len(result.stdout.split('\n')) > 10:
                    print("      ...")
            else:
                print("   ❌ Python演示运行失败")
                print(f"      错误: {result.stderr}")
        except subprocess.TimeoutExpired:
            print("   ⚠️  Python演示运行超时")
        except Exception as e:
            print(f"   ❌ Python演示运行出错: {e}")
    else:
        print("   ❌ demo_meshcat_simple.py 文件不存在")
    
    print()

def provide_next_steps(deps_ok, files_ok):
    print("🎯 下一步操作:")
    
    if not deps_ok:
        print("   1. 安装缺失的系统依赖:")
        if platform.system() == "Linux":
            print("      sudo apt update")
            print("      sudo apt install cmake build-essential git pkg-config")
        elif platform.system() == "Darwin":
            print("      brew install cmake git pkg-config")
        print()
    
    if not files_ok:
        print("   2. 确保所有MeshCat文件都已正确放置")
        print("      参考 LOCAL_TESTING_GUIDE.md 中的文件结构")
        print()
    
    if deps_ok and files_ok:
        print("   ✅ 系统准备就绪！可以开始安装和测试:")
        print("      1. 运行安装脚本: ./install_meshcat.sh")
        print("      2. 编译TOWR: cd towr/build && cmake .. && make")
        print("      3. 运行演示: ./towr-meshcat-demo")
        print("      4. 在浏览器中查看: http://localhost:7000")
        print()
    
    print("📚 更多信息:")
    print("   • 详细安装指南: LOCAL_TESTING_GUIDE.md")
    print("   • 使用文档: MESHCAT_VISUALIZATION.md")
    print("   • 实现细节: IMPLEMENTATION_SUMMARY.md")

def main():
    print_header()
    check_system_info()
    
    deps_ok = check_dependencies()
    check_optional_deps()
    files_ok = check_file_structure()
    check_permissions()
    test_python_demo()
    
    provide_next_steps(deps_ok, files_ok)
    
    print()
    if deps_ok and files_ok:
        print("🎉 快速测试完成！您的系统已准备好运行TOWR MeshCat可视化！")
    else:
        print("⚠️  请解决上述问题后再进行完整测试")

if __name__ == "__main__":
    main()
