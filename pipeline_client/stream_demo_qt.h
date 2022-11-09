#pragma once

#include <QtWidgets/QMainWindow>
#include <QToolBar>
#include <QTableWidget>
#include <QDockWidget>
#include <QtCore>
#include <QStatusBar>
#include <QMessageBox>
#include <QKeyEvent>
#include <unordered_map>
#include "ui_stream_demo_qt.h"
#include "container_widget.h"
#include "system_settings_dlg.h"

class StreamController;
class stream_demo_qt : public QMainWindow
{
    Q_OBJECT

public:
    stream_demo_qt(QWidget *parent = Q_NULLPTR);
private:
    // Functions
    void setupToolbar();
    void setupLeftView();
    void setupContainerView();
    void loadConfig();
private slots:
    // for internal testing
    void onMenuAddWidget();
    void onMenuRemoveWidget();
    void onSystemSettings();
    void onIncraseBufferNum();
    void onDecreaseBufferNum();

protected:
    void closeEvent(QCloseEvent* e) override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void resizeEvent(QResizeEvent*) override;
private:
    std::string m_rtspUrl;
    stream_demo_system_settings m_settings;
    std::unordered_map<QWidget*, StreamController*> m_mapWnd2Ctrler;
    int m_frameBufferedNum{0};

    QAction *m_menuActionConnect{0};
    container_widget *m_pVideoContainerWidget{ 0 };
private:
    //QLabel *m_lblFrameBufferedNum;
    Ui::stream_demo_qtClass ui;
    QLabel *m_lblTitle;
    std::string m_strTitle;
};
