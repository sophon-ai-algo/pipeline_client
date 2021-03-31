#pragma once

#include <QDialog>
#include "ui_system_settings_dlg.h"

class stream_demo_system_settings {
public:
    stream_demo_system_settings(){}
    ~stream_demo_system_settings(){}

    std::string m_strRtspUrl;
};


class system_settings_dlg : public QDialog
{
    Q_OBJECT

public:
    system_settings_dlg(QWidget *parent = Q_NULLPTR);
    ~system_settings_dlg();

    void set_data(stream_demo_system_settings &settings);
    const stream_demo_system_settings& get_data();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    stream_demo_system_settings m_settings;
    Ui::system_settings_dlg ui;
};
