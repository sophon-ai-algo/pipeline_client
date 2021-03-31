#include "system_settings_dlg.h"

system_settings_dlg::system_settings_dlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}

system_settings_dlg::~system_settings_dlg()
{
}

void system_settings_dlg::on_buttonBox_accepted()
{
    accept();
}

void system_settings_dlg::on_buttonBox_rejected()
{
    reject();
}

void system_settings_dlg::set_data(stream_demo_system_settings &settings)
{
    ui.leRtspUrl->setText(QString::fromStdString(settings.m_strRtspUrl));
}

const stream_demo_system_settings& system_settings_dlg::get_data()
{
    auto text = ui.leRtspUrl->text();
    m_settings.m_strRtspUrl = text.toStdString();

    return m_settings;
}