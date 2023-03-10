#include "stream_demo_qt.h"
#include "stream_controller.h"
#include "system_settings_dlg.h"
#include "open_stream_dlg.h"
#include <qinputdialog.h>
#include <qlineedit.h>
#include <otl_string.h>
#include <fstream>
#include "json/json.h"

stream_demo_qt::stream_demo_qt(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    statusBar()->hide();

    //initGUITheme();
    m_strTitle = "Pipeline Client Demo";
    setWindowTitle(QString::fromStdString(m_strTitle));
    loadConfig();

    setupToolbar();
    setupLeftView();
    setupContainerView();

    m_settings.m_strRtspUrl = "rtsp://10.32.129.248:8554/test";


    
}

void stream_demo_qt::loadConfig() {
    std::ifstream fs("pipeline_client.json");
    if (!fs.is_open()) {
        return;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(fs, root, false)) {
        flog(LOG_ERROR, "Parse cofnig file failed!");
        fs.close();
        return;
    }

    m_strTitle = root["title"].asString();

    fs.close();
}

void stream_demo_qt::setupToolbar()
{
    QToolBar* toolBar = new QToolBar(tr("TaskToolBar"), this);
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    //m_menuActionConnect = m_toolBar->addAction(QIcon(":/images/UserAvailable.png"), tr("Connect"), this, SLOT(onMenuSetupCall()));
    //m_toolBar->addAction(QIcon(":/images/UserUnavailable.png"), tr("Disconnect"), this, SLOT(onMenuStopCall()));

    toolBar->addAction(QIcon(":/images/addwidget.png"), tr("Add Widget"), this, SLOT(onMenuAddWidget()));
    toolBar->addAction(QIcon(":/images/delwidget.png"), tr("Remove Widget"), this, SLOT(onMenuRemoveWidget()));

    //m_toolBar->addAction(QIcon(":/images/cmdSystem.png"), tr("SystemSettings"), this, SLOT(onSystemSettings()));
    //m_toolBar->addAction(QIcon(":/images/up_arrow.png"), tr("Increase buffer"), this, SLOT(onIncraseBufferNum()));
    //m_toolBar->addAction(QIcon(":/images/down_arrow.png"), tr("Decrease buffer"), this, SLOT(onDecreaseBufferNum()));

    m_lblTitle = new QLabel(toolBar);
    m_lblTitle->setText(QString::fromStdString(m_strTitle));
    QRect rc = m_lblTitle->geometry();
    rc.setWidth(this->width()-130);
    m_lblTitle->setGeometry(rc);

    QFont font1;
    font1.setFamily("宋体");
    m_lblTitle->setStyleSheet("font-size:28px");
    m_lblTitle->setFont(font1);
    m_lblTitle->setAlignment(Qt::AlignCenter);
    toolBar->addWidget(m_lblTitle);

    addToolBar(toolBar);
    
}

void stream_demo_qt::setupLeftView()
{
    //m_pUserListWidget = new user_list_widget(this);
    //m_pDeviceListDockWidget = new QDockWidget(tr("Users"), this);
    //m_pDeviceListDockWidget->setWidget(m_pUserListWidget);
    //addDockWidget(Qt::RightDockWidgetArea, m_pDeviceListDockWidget);

}


void stream_demo_qt::setupContainerView()
{
    if (nullptr == m_pVideoContainerWidget) {
        m_pVideoContainerWidget = new container_widget(this);
        setCentralWidget(m_pVideoContainerWidget);
    }


}


void stream_demo_qt::onMenuAddWidget()
{
    bool ok;
    open_stream_dlg dlg(this);
    auto ret = dlg.exec();
    if (ret != QDialog::Accepted){
        return;
    }

    QString inputUrl;
    int chann_num = 1;
    bool isUseSameUrl = false;
    QString streamFormat;
    QString pixelFormat;
    int w, h;

    dlg.GetInputUrl(inputUrl, chann_num, isUseSameUrl);
    dlg.GetInputParams(streamFormat, pixelFormat, w, h);

    for(int i = 0; i < chann_num; ++i){
        std::string real_url;
        video_widget *pWidget = m_pVideoContainerWidget->addChildWnd();
        if (!isUseSameUrl) {
            if (inputUrl.startsWith("rtsp://")) {
                real_url = String::format("%s_%d", inputUrl.toLocal8Bit().data(), i);
            }else if(inputUrl.startsWith("udp://")){
                auto iports = String::split(inputUrl.toStdString().substr(6), ":");
                int port_base = std::atoi(iports[1].c_str());
                real_url = String::format("udp://%s:%d?overrun_nonfatal=1&fifo_size=5000000", iports[0].c_str(), port_base + i);
            }else if(inputUrl.startsWith("tcp://")){
                auto iports = String::split(inputUrl.toStdString().substr(6), ":");
                int port_base = std::atoi(iports[1].c_str());
                real_url = String::format("tcp://%s:%d?listen&tcp_nodelay=1", iports[0].c_str(), port_base + i);
            }
			else {
				// New feat: 接受本地路径输入
				std::ifstream f(inputUrl.toStdString());
				if (f.good() && inputUrl.toStdString().find_last_of(".") != inputUrl.toStdString().npos) {
					real_url = inputUrl.toStdString();
					streamFormat = QString::fromStdString(real_url.substr(real_url.find_last_of(".") + 1));
					/*
					if (real_url.find_last_of(".") == real_url.npos || real_url.substr(real_url.find_last_of(".") + 1) != streamFormat.toStdString()) {
					 QMessageBox::information(this, windowTitle(), "Video file format is different from the currently selected stream format!");
					 return;
					}
					*/
				}
				else {
					QMessageBox::information(this, windowTitle(), "Stream format not supported or wrong file path!");
					return;
				}
            }

        }else{
            real_url = inputUrl.toStdString();
        }
        StreamController *controller = new StreamController(pWidget, real_url, i);
        //Load all configuration
        controller->set_frame_bufferd_num(m_frameBufferedNum);
        m_mapWnd2Ctrler[pWidget] = controller;
        controller->start_stream(streamFormat.toStdString(), pixelFormat.toStdString(), w, h);
    }

}

void stream_demo_qt::onMenuRemoveWidget()
{
    auto wnd = m_pVideoContainerWidget->getSelectedWidget();
    auto it = m_mapWnd2Ctrler.find(wnd);
    if (it != m_mapWnd2Ctrler.end()) {

        it->second->stop_stream();

        m_pVideoContainerWidget->removeChildWindow(wnd);
        delete it->second;

        m_mapWnd2Ctrler.erase(it);
    }

    m_pVideoContainerWidget->UpdateWidgetLayout();
}

void stream_demo_qt::onSystemSettings()
{
    system_settings_dlg dlg(this);
    dlg.set_data(m_settings);
    int ret = dlg.exec();
    if (ret != QDialog::Accepted) {
        m_settings = dlg.get_data();
    }

}

void stream_demo_qt::onIncraseBufferNum()
{
    m_frameBufferedNum++;
    for (auto it : m_mapWnd2Ctrler) {
        it.second->set_frame_bufferd_num(m_frameBufferedNum);
    }
    //m_lblFrameBufferedNum->setNum(m_frameBufferedNum);

}

void stream_demo_qt::onDecreaseBufferNum()
{
    m_frameBufferedNum--;
    for (auto it : m_mapWnd2Ctrler) {
        it.second->set_frame_bufferd_num(m_frameBufferedNum);
    }
    //m_lblFrameBufferedNum->setNum(m_frameBufferedNum);
}

void stream_demo_qt::closeEvent(QCloseEvent* e)
{
    for (auto it : m_mapWnd2Ctrler) {
        it.second->stop_stream();
        m_pVideoContainerWidget->removeChildWindow(dynamic_cast<video_widget*>(it.first));
        delete it.second;
    }
}

void stream_demo_qt::keyPressEvent(QKeyEvent *e)
{
    return QMainWindow::keyPressEvent(e);
}

void stream_demo_qt::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_F5) {
        if (!isFullScreen()) {
            showFullScreen();
        }else{
            showNormal();
        }
    }

    return QMainWindow::keyReleaseEvent(e);
}

void stream_demo_qt::resizeEvent(QResizeEvent *) {
    QRect rc = m_lblTitle->geometry();
    m_lblTitle->setGeometry(rc.x(), rc.y(), this->width()-130, rc.height());
}
