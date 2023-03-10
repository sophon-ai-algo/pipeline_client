#include "open_stream_dlg.h"
#include "ui_open_stream_dlg.h"

open_stream_dlg::open_stream_dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::open_stream_dlg)
{
    ui->setupUi(this);
    QStringList inputUrls;
    inputUrls 
        << "rtsp://127.0.0.1:8554/test_sei"
        << "udp://0.0.0.0:9000"
        << "tcp://0.0.0.0:9000"
        << "rtsp://192.168.1.110:8554/test_sei"
            << "rtsp://admin:hk123456@192.168.1.100/test"
            << "rtsp://admin:hk123456@192.168.1.101/test"
    << "rtsp://admin:hk123456@192.168.1.102/test"
    << "rtsp://admin:hk123456@192.168.1.103/test"
	<< "D:\\Users\\test_01.mp4"
	<< "D:\\Users\\test_01.h264"
	<< "D:\\Users\\test_01.h265";
    ui->cbxInputUrls->addItems(inputUrls);
    ui->cbxInputUrls->setCurrentIndex(0);
    ui->cbxInputUrls->setEditable(true);

    ui->leChanNum->setText(QString::number(m_channel_num));
    ui->ckUseSameUrl->setChecked(m_isUseSameUrl);

    ui->leWidth->setText(QString::number(1920));
    ui->leHeight->setText(QString::number(1080));
    ui->lePixelFormat->setText("yuv420p");
    ui->leStreamFormat->setText("h264");

	QObject::connect(ui->chooseFileButton, &QPushButton::clicked, this, &open_stream_dlg::choose_file);
}

open_stream_dlg::~open_stream_dlg()
{
    delete ui;
}

void open_stream_dlg::on_buttonBox_accepted()
{
     m_inputUrl = ui->cbxInputUrls->currentText();
     m_channel_num = ui->leChanNum->text().toInt();
     m_isUseSameUrl = ui->ckUseSameUrl->isChecked();
     m_pixel_format = ui->lePixelFormat->text();
     m_stream_format = ui->leStreamFormat->text();
     m_width = ui->leWidth->text().toInt();
     m_height = ui->leHeight->text().toInt();
}

void open_stream_dlg::on_buttonBox_rejected()
{
     reject();
}

void open_stream_dlg::GetInputUrl(QString &inputUrl, int& chan_num, bool &isUseSameUrl)
{
    inputUrl = m_inputUrl;
    chan_num = m_channel_num;
    isUseSameUrl = m_isUseSameUrl;
}

void open_stream_dlg::GetInputParams(QString &streamFormat, QString& pixel_format, int& width, int &height)
{
    streamFormat = m_stream_format;
    pixel_format = m_pixel_format;
    width = m_width;
    height = m_height;
}

void open_stream_dlg::choose_file() {
    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("请选择视频文件"), "C:", QStringLiteral("视频文件(*mp4 *h264 *h265 *mkv *flv *avi *mov);;""全部类型文件(*)"));
	if (!fileName.isEmpty()) {
		ui->cbxInputUrls->setEditText(fileName);
	}
}
