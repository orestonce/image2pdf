#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_updateUi = new QTimer(this);
    connect(m_updateUi, &QTimer::timeout, [this](){
        auto resp = GetStatus();
        ui->lineEdit_pdfOutput->setEnabled(!resp.IsRunning);
        ui->pushButton_addImage->setEnabled(!resp.IsRunning);
        ui->pushButton_deleteImage->setEnabled(!resp.IsRunning);
        ui->pushButton_imageMoveUp->setEnabled(!resp.IsRunning);
        ui->pushButton_imageMoveDown->setEnabled(!resp.IsRunning);
        ui->pushButton_beginConv->setEnabled(!resp.IsRunning);
        ui->pushButton_endConv->setEnabled(resp.IsRunning);
        ui->pushButton_setOutputFileName->setEnabled(!resp.IsRunning);
        ui->pushButton_clearImageList->setEnabled(!resp.IsRunning);

        if(!resp.IsRunning)
            resp.Percent = 0;
        ui->progressBar->setValue(resp.Percent);
    });
    m_updateUi->start(100);
}

MainWindow::~MainWindow()
{
    m_updateUi->stop();
    delete ui;
}

QStringList MainWindow::getImageList()
{
    QStringList list;
    for(int i=0; i < ui->listWidget_image->count(); i++)
    {
        list.append(ui->listWidget_image->item(i)->text());
    }
    return list;
}

void MainWindow::on_pushButton_addImage_clicked()
{
    QString filter = "*.png;*.jpg;*.jpeg;*.gif;*.tiff;*.bmp";

    QSet<QString> fileSet = QFileDialog::getOpenFileNames(this,
                                                          "",
                                                          "",
                                                          filter).toSet();
    if(fileSet.isEmpty())
        return;

    for(auto one: getImageList())
    {
        fileSet.remove(one);
    }
    if(fileSet.isEmpty())
        return;

    QStringList fileList = fileSet.toList();
    std::sort(fileList.begin(), fileList.end());

    int index = ui->listWidget_image->currentIndex().row();
    if(index < 0)
        ui->listWidget_image->addItems(fileList);
    else
        ui->listWidget_image->insertItems(index, fileList);

    if(ui->lineEdit_pdfOutput->text().isEmpty())
    {
        QFileInfo info(fileList.at(0));
        QString savePath = info.absoluteDir().absolutePath() + "/image2pdf.pdf";
        ui->lineEdit_pdfOutput->setPlaceholderText(savePath);
    }
}

void MainWindow::on_pushButton_deleteImage_clicked()
{
    int index = ui->listWidget_image->currentIndex().row();
    if(index < 0)
        return;
    ui->listWidget_image->takeItem(index);
}

void MainWindow::on_pushButton_imageMoveUp_clicked()
{
    int index = ui->listWidget_image->currentIndex().row();
    if(index <= 0)
        return;
    auto item = ui->listWidget_image->takeItem(index);
    ui->listWidget_image->insertItem(index-1, item);
    ui->listWidget_image->setCurrentItem(item);
}

void MainWindow::on_pushButton_imageMoveDown_clicked()
{
    int index = ui->listWidget_image->currentIndex().row();
    if(index >= ui->listWidget_image->count())
        return;
    auto item = ui->listWidget_image->takeItem(index);
    ui->listWidget_image->insertItem(index+1, item);
    ui->listWidget_image->setCurrentItem(item);
}

void MainWindow::on_pushButton_beginConv_clicked()
{
    Image2PDF_Req req;
    for(auto one: getImageList())
    {
        req.FileList.push_back(one.toStdString());
    }
    req.PdfOut = ui->lineEdit_pdfOutput->text().toStdString();
    if(req.PdfOut.empty())
        req.PdfOut = ui->lineEdit_pdfOutput->placeholderText().toStdString();

    m_syncUi.AddRunFnOn_OtherThread([this,req](){
        QString errMsg = QString::fromStdString(Image2PDF(req));
        m_syncUi.AddRunFnOn_UiThread([=]{
            if(errMsg.isEmpty())
                Toast::Instance()->SetSuccess("成功!");
            else if(!IsRequestStop())
                QMessageBox::warning(this, "错误", errMsg);
        });
    });
}

void MainWindow::on_pushButton_endConv_clicked()
{
    EndConv();
}

void MainWindow::on_pushButton_setOutputFileName_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if(!fileName.isEmpty())
        ui->lineEdit_pdfOutput->setText(fileName);
}

void MainWindow::on_pushButton_clearImageList_clicked()
{
    QMessageBox box;
    box.setInformativeText("是否清空图片列表?");
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::Yes);
    if(box.exec() != QMessageBox::Yes)
        return;
    ui->listWidget_image->clear();
}
