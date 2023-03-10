# 如何在SE5盒子上运行pipeline client

## 目录

* [1. 设备准备](#1-设备准备)
* [2. 编译并运行pipeline_client](#2-编译并运行pipeline_client)
* [3. 使用方法](#3-使用方法)
  * [3.1 播放本地视频](#31-播放本地视频)
  * [3.2 推流播放](#32-推流播放)
* [4. FAQ](#4-faq)
* [5. 待优化](#5-待优化)


## 1. 设备准备
确保显示器通过hdmi连接，鼠标和键盘通过usb连接。将SE5盒子上电，此时显示器将显示盒子参数信息(例如网关、IP信息等)。

## 2. 编译并运行pipeline_client
通过串口或ssh连接盒子，然后执行：
```bash
# 下载项目源码
git clone https://github.com/sophon-ai-algo/pipeline_client.git
# 查看overplay下剩余内存，至少保证有1GB的内存
df -hl
# 安装依赖库
sudo apt install qtbase5-dev
sudo apt install libopencv-dev
sudo apt install ffmpeg
sudo apt install libavfilter-dev
# 编译项目
mkdir build   
cd build
cmake ..   
make -j4
# 安装sophon-qt
git clone https://github.com/sophon-ai-algo/sophon-qt.git
cd sophon-qt/qt-lib
tar -xvf qtbase-5.14.tgz  
```  
进入执行：
```bash
# 执行, 注意将${prefix_path}替换为正确的路径
sudo su
export QT_PLUGIN_PATH=${prefix_path}/sophon-qt/qt-lib/install/plugins
export QT_QPA_PLATFORM=linuxfb
cd ${prefix_path}/pipeline_client/build/bin/
./pipeline_client
```  
执行完最后一步，可以看到显示器已经显示pipeline client的画面。

## 3. 使用方法
### 3.1 播放本地视频
打开pipeline客户端，点击右上角的绿色+号，通过choose file按钮打开选择文件菜单，选择要播放的视频文件，点击"OK"。  
### 3.2 推流播放
以rtsp流为例, 通过[rtsp server下载链接](https://objects.githubusercontent.com/github-production-release-asset-2e65be/230660669/57f105d8-1465-4e67-bb3b-f46e6da113ea?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20230309%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20230309T070855Z&X-Amz-Expires=300&X-Amz-Signature=392609ec68d27130b3ca4253d75ce04d284a9900df87a3cd6f7888062f349749&X-Amz-SignedHeaders=host&actor_id=45344941&key_id=0&repo_id=230660669&response-content-disposition=attachment%3B%20filename%3Drtsp-simple-server_v0.21.5_linux_arm64v8.tar.gz&response-content-type=application%2Foctet-stream)下载rtsp server, 将其拷贝到盒子任意目录下面。
```bash
# 解压
tar -xvf rtsp-simple-server_v0.21.5_linux_arm64v8.tar.gz
# 运行
./rtsp-simple-server
# 注意：如提示8888端口被占用，则需要终止占用其的进程
sudo lsof -i :8888
# 将刚刚看到的进程终止
kill ${PID}
```  
新建一个终端，通过ffmpeg推流(示例)：
```bash
# 查看盒子的ip
ip a 
# 将${video_name}替换为推流的视频路径，${ip_address}为盒子的ip地址
ffmpeg -re -an -c:v h264_bm -i ${video_name} -an -c:v h264_bm -enc-params "gop_preset=2" -f rtsp rtsp://${ip_address}:8554/test_0
```  
打开pipeline客户端，点击右上角的绿色+号，在"input url or file path"一栏输入: rtsp://${ip_address}:8554/test(注意这里的test不用加"_0")，点击"OK"。

## 4. FAQ
1. 显示器被其他进程占用  
提示Failed to open framebuffer /dev/fl2000-0 (Device or resource busy)。  
一般是因为显示器被盒子的开机界面进程占用，将进程终止即可：
```bash
# 查看占用/dev/fl2000-0的进程PID
lsof /dev/fl2000-0
# 将刚刚看到的进程终止
kill ${PID}
```  
2. 客户端没有任何字  
提示QFontDatabase: Cannot find font directory /lib/fonts.  
Note that Qt no longer ships fonts. Deploy some (from https://dejavu-fonts.github.io/ for example) or switch to fontconfig。  
这是因为QT插件没有找到合适的字体，可以从https://dejavu-fonts.github.io下载字体，或者从/usr/share/fonts/truetype/dejavu/路径下面找默认字体的ttf文件，然后修改pipeline_client/pipeline_client/main.cpp，修改示例:
```bash
#include "stream_demo_qt.h"
#include <QtWidgets/QApplication>
#include <QFontDatabase>
#include <qstylefactory.h>
#include <fstream>
#include "json/json.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    
    QApplication::setStyle("Windows");
    QFontDatabase::addApplicationFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    QFont font("DejaVu Sans", 10);
    stream_demo_qt w;
    w.setFont(font);
    w.show();
    return a.exec();
}
```  
修改完成后，保存一下，然后进入pipeline_client/build目录下面重新编译一下：
```bash
make -j4
```

## 5. 待优化
1. QDialog窗口的边界框没有显示，可能的原因是：  
在Linux上，Qt应用程序的窗口边框可能会由窗口管理器负责绘制，而不是由Qt本身绘制。这可能会导致Qt应用程序的窗口边框在Linux上无法正确显示。

