#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtSql>
#include<QDebug>
#include<QFileInfo>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_login_button_clicked();

    void on_register_button_clicked();

    void on_login_page_quit_clicked();

    void on_training_page_quit_clicked();

    void on_testing_page_quit_clicked();

    void on_training_page_training_clicked();

    void on_training_page_testing_clicked();

    //void on_testing_page_training_clicked();

    void on_testing_page_testing_clicked();

    void on_training_positive_button_clicked();

    void on_training_negative_button_clicked();

    void on_train_button_clicked();

    void on_training_page_quit_2_clicked();

    void on_training_page_quit_3_clicked();

    void timerEvent(QTimerEvent *event);

    void on_training_positive_directory_textChanged(const QString &arg1);

    void on_training_negative_video_textChanged(const QString &arg1);

    void on_training_camera_selection_clicked();

    void on_negative_duration_valueChanged(int value);

    void on_testing_video_button_clicked();

    void on_testing_model_button_clicked();

    void on_testing_model_path_textChanged(const QString &arg1);

    void on_testing_video_path_textChanged(const QString &arg1);

    void on_testing_camera_selection_clicked();

    void on_test_button_clicked();

    void on_test_stop_button_clicked();

    void on_about_back_clicked();

    void on_about_button_clicked();

    void on_training_help_button_clicked();

    void on_testing_help_button_clicked();

    void on_help_back_clicked();

    void on_login_username_textChanged(const QString &arg1);

    void on_login_password_textChanged(const QString &arg1);

    void on_about_back1_clicked();

    void on_about_button1_clicked();

    void on_sample_clicked();

    void on_camera_clicked();

    void on_test_butt_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
