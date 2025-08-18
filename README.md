# LaserControl

一个基于 OpenCV 和 SFML 的激光控制系统，具有目标跟踪、激光控制和实时图像处理功能。

## 项目概述

LaserControl 是一个智能激光控制系统，能够通过摄像头进行目标检测和跟踪，并控制激光器和步进电机进行精确定位。该系统提供了直观的图形界面，支持多种工作模式。

### 主要功能

- **实时图像处理**: 基于 OpenCV 的目标检测和 ROI（感兴趣区域）提取
- **目标跟踪**: 自动检测和跟踪纸张中心点等目标物体
- **激光控制**: 精确控制激光器功率和位置
- **电机控制**: 支持步进电机和舵机的精确控制
- **多工作模式**: 手动模式、跟踪模式、绘图模式
- **PID控制**: 内置 PID 控制器确保精确定位
- **串口通信**: 通过串口与硬件设备通信
- **图形界面**: 基于 ImGui 的直观控制面板

## 系统架构

### 核心模块

1. **LaserControl**: 主控制类，管理整个系统的运行
2. **Tracker**: 目标跟踪模块，处理图像识别和目标检测
3. **ROIExtractor**: ROI 提取器，从图像中提取感兴趣区域
4. **PIDController**: PID 控制器，用于精确的电机控制
5. **Application**: GUI 应用程序界面

### 工作模式

- **Manual (手动模式)**: 用户手动控制激光器和电机
- **Track (跟踪模式)**: 自动跟踪检测到的目标
- **Draw (绘图模式)**: 激光绘图功能

## 技术栈

- **C++20**: 现代 C++ 标准
- **OpenCV**: 计算机视觉和图像处理
- **SFML**: 图形和窗口管理
- **ImGui**: 即时模式图形界面
- **spdlog**: 高性能日志记录
- **fmt**: 现代字符串格式化
- **Serial**: 串口通信库
- **CMake**: 构建系统

## 系统要求

### 开发环境
- CMake 3.31 或更高版本
- C++20 兼容的编译器
- vcpkg 包管理器

### 硬件要求
- 支持的摄像头设备
- 串口连接的控制板
- 步进电机/舵机
- 激光器模块

## 编译和安装

### 1. 安装依赖

首先确保安装了 vcpkg，然后安装必要的依赖包：

```bash
vcpkg install opencv
vcpkg install sfml
vcpkg install imgui
vcpkg install imgui-sfml
vcpkg install fmt
vcpkg install spdlog
```

### 2. 编译项目

```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

### 3. 打包发布

```bash
cpack
```

## 使用方法

### 启动程序

运行编译生成的 `LaserControl.exe`

### 界面操作

1. **选择串口**: 点击 "Refresh" 刷新可用串口，选择正确的设备
2. **打开连接**: 点击 "OK" 建立串口连接
3. **选择模式**: 在下拉菜单中选择工作模式
4. **启动系统**: 点击 "START" 开始运行
5. **停止系统**: 点击 "STOP" 停止运行

### 参数调节

- **MotorX/MotorY**: 调节电机位置
- **Laser Power**: 控制激光功率 (0-1)
- **Motor Step**: 设置电机步进值

## 配置说明

### 摄像头配置

在 `Tracker` 构造函数中可以调整摄像头参数：

```cpp
m_cap.set(cv::CAP_PROP_FRAME_WIDTH, 1200);
m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 800);
m_cap.set(cv::CAP_PROP_FPS, 60);
m_cap.set(cv::CAP_PROP_EXPOSURE, -5);
```

### ROI 检测参数

在 `ROIExtractor` 中可以调整检测阈值：

```cpp
extractROI(m_frame, &m_draw, 80);  // 灰度阈值
```

### PID 参数

可以在 `LaserControl` 中配置 PID 控制参数以获得最佳控制效果。

## API 参考

### LaserControl 类

```cpp
// 单例获取
LaserControl& getInstance();

// 串口操作
int OpenSerialPort(const std::string &port, uint32_t baud);
static std::string EnumeratePortToString();

// 任务控制
int CreateTask(uint32_t interval);
int StopTask();

// 电机控制
size_t SetMotorPulse(MotorIndex motor, uint32_t speed, int32_t pulse);
```

### Tracker 类

```cpp
// 单例获取
static Tracker& getInstance(const int index);

// 图像处理
void m_opencv_task();
void getObjectList(std::vector<ObjectInfo>& list);
```

## 故障排除

### 常见问题

1. **摄像头无法打开**
   - 检查摄像头是否被其他程序占用
   - 确认摄像头索引是否正确

2. **串口连接失败**
   - 检查串口号是否正确
   - 确认波特率设置
   - 检查设备驱动程序

3. **目标检测不准确**
   - 调整光照条件
   - 修改 ROI 检测阈值
   - 检查摄像头焦距和位置

## 开发计划

- [ ] 线程池优化图像处理性能
- [ ] 激光轨迹记录和回放
- [ ] 更多目标检测算法
- [ ] 网络远程控制功能
- [ ] 参数配置文件支持

## 贡献指南

1. Fork 本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

## 联系方式

- 开发者: AHPU
- 项目主页: [GitHub Repository]
- 版本: 1.0.0

## 致谢

感谢以下开源项目的支持：
- [OpenCV](https://opencv.org/)
- [SFML](https://www.sfml-dev.org/)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [spdlog](https://github.com/gabime/spdlog)
- [fmt](https://github.com/fmtlib/fmt)

---

**注意**: 使用激光设备时请注意安全，佩戴适当的防护设备。