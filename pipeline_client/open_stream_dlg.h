#ifndef OPEN_STREAM_DLG_H
#define OPEN_STREAM_DLG_H

#include <QDialog>

namespace Ui {
class open_stream_dlg;
}

class open_stream_dlg : public QDialog
{
    Q_OBJECT

    QString m_inputUrl;
    int m_channel_num{1};
    bool m_isUseSameUrl{false};

    QString m_pixel_format;
    QString m_stream_format;
    int m_width;
    int m_height;

public:
    explicit open_stream_dlg(QWidget *parent = nullptr);
    ~open_stream_dlg();

    void GetInputUrl(QString &inputUrl, int& chan_num, bool &isUseSameUrl);
    void GetInputParams(QString &streamFormat, QString& pixel_format, int& width, int &height);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::open_stream_dlg *ui;
};

#endif // OPEN_STREAM_DLG_H
