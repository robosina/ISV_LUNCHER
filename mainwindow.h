#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSettings>
#include <QToolButton>
#include <QLabel>
#include <QPointer>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTableWidgetItem>
using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //*****************************************************************
    //***********functions for saving and loading our settings*********
    //*****************************************************************
    void load_setting();
    void save_setting(int var);
    //save the settings in this file e.x: /home/saeed/Desktop/settings.ini
    QString m_setting_file;      //this is latest config(when a variable change this file will be update)
    void create_folder_for_settings();
    QString m_joyStick_saved_folder,m_Manaul_saved_folder; // user can save manual settings
    //declare which variable has been changed,then try to save it
    enum save_var{
        X_STEP_CHANGED,
        Y_STEP_CHANGED,
        X_CHANGED,
        Y_CHANGED,
    };
    //add note to widget
    void note(QString NoteString);
    void add_file_to_table(int row_number,QString fileName);
    vector<QString> m_file_names;
    void load_all_saved_setting();
    void show_selected_config(int row_number);


private slots:
    void on_horizontalSlider_XSTEP_valueChanged(int value);
    void on_horizontalSlider_YSTEP_valueChanged(int value);
    //slots for joystick
    void up_button_clicked();
    void down_button_clicked();
    void left_button_clicked();
    void right_button_clicked();

    void on_button_save_setting_clicked();
    void which_row_selected(int row_selected);
    void which_row_selected(const QItemSelection& in1, const QItemSelection&in2);
    void on_button_load_selected_setting_clicked();
    void on_button_manual_mode_clicked();
    void on_button_Joystick_mode_clicked();
    //slot when lineEdit focus has triggered
    void on_lineEditX_triggered();
    void on_lineEditY_triggered();
    void on_lineEditX_turn_off();
    void on_lineEditY_turn_off();
    void on_button_0_clicked();
    void on_button_1_clicked();
    void on_button_2_clicked();
    void on_button_3_clicked();
    void on_button_4_clicked();
    void on_button_5_clicked();
    void on_button_6_clicked();
    void on_button_7_clicked();
    void on_button_8_clicked();
    void on_button_9_clicked();
    void on_button_clear_clicked();

    void process_numbers(int i);

    void on_button_apply_XCHANGE_clicked();

    void on_button_apply_YCHANGE_clicked();

private:
    Ui::MainWindow *ui;
    //VALUE FOR XSTEP,YSTEP
    int m_XSTEP{0},m_YSTEP{0};
    int m_x{0},m_y{0};
    QPointer<QLabel> m_NoteString;
    QTableWidgetItem * item;
    int m_row_selected;
    int m_selected_mode{0};
    enum Mode{
        joyStick,  //0
        Manual     //1
    };

    int m_temp{-1}; //which line edit is selected in manual mode
};

#endif // MAINWINDOW_H
