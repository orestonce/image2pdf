#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "image2pdf.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStringList getImageList();
private slots:
    void on_pushButton_addImage_clicked();

    void on_pushButton_deleteImage_clicked();

    void on_pushButton_imageMoveUp_clicked();

    void on_pushButton_imageMoveDown_clicked();

    void on_pushButton_beginConv_clicked();

    void on_pushButton_endConv_clicked();

    void on_pushButton_setOutputFileName_clicked();

    void on_pushButton_clearImageList_clicked();

private:
    Ui::MainWindow *ui;
    RunOnUiThread  m_syncUi;
    QTimer *m_updateUi;
};

#endif // MAINWINDOW_H
