#include "mainwindow.h"
#include "ui_mainwindow.h"

const double MainWindow::EPS=0.0001;

MainWindow::MainWindow(QWidget *parent) :
                                          QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    set_radius_for_labels(30);
    m_setting_file=QApplication::applicationDirPath()+"/settings.ini";
    m_selected_mode=Mode::joyStick;
    create_folder_for_settings();
    connect(ui->tableWidget->verticalHeader(), SIGNAL(sectionClicked(int)),
            this,SLOT(which_row_selected(int)));
    connect(ui->tableWidget->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&))
                                                   ,this,SLOT(which_row_selected(const QItemSelection &, const QItemSelection &)));
    m_NoteString=new QLabel(this);
    //update y value
    connect(ui->label_up,SIGNAL(mouseDouble()),this,SLOT(up_button_clicked()));
    connect(ui->label_down,SIGNAL(mouseDouble()),this,SLOT(down_button_clicked()));
    //update x value
    connect(ui->label_left,SIGNAL(mouseDouble()),this,SLOT(left_button_clicked()));
    connect(ui->label_right,SIGNAL(mouseDouble()),this,SLOT(right_button_clicked()));

    //when a button pressed for a while
    up_button_checker=new QTimer(this);
    down_button_checker=new QTimer(this);
    right_button_checker=new QTimer(this);
    left_button_checker=new QTimer(this);
    connect(ui->label_up,SIGNAL(mousePressed()),this,SLOT(up_button_pressed()));
    connect(ui->label_up,SIGNAL(mouseReleased()),this,SLOT(up_button_released()));
    connect(up_button_checker,SIGNAL(timeout()),this,SLOT(up_button_clicked()));
    connect(up_button_checker,SIGNAL(timeout()),this,SLOT(refresh_timer_value()));

    connect(ui->label_down,SIGNAL(mousePressed()),this,SLOT(down_button_pressed()));
    connect(ui->label_down,SIGNAL(mouseReleased()),this,SLOT(down_button_released()));
    connect(down_button_checker,SIGNAL(timeout()),this,SLOT(refresh_timer_value()));
    connect(down_button_checker,SIGNAL(timeout()),this,SLOT(down_button_clicked()));

    connect(ui->label_right,SIGNAL(mousePressed()),this,SLOT(right_button_pressed()));
    connect(ui->label_right,SIGNAL(mouseReleased()),this,SLOT(right_button_released()));
    connect(right_button_checker,SIGNAL(timeout()),this,SLOT(right_button_clicked()));
    connect(right_button_checker,SIGNAL(timeout()),this,SLOT(refresh_timer_value()));

    connect(ui->label_left,SIGNAL(mousePressed()),this,SLOT(left_button_pressed()));
    connect(ui->label_left,SIGNAL(mouseReleased()),this,SLOT(left_button_released()));
    connect(left_button_checker,SIGNAL(timeout()),this,SLOT(left_button_clicked()));
    connect(left_button_checker,SIGNAL(timeout()),this,SLOT(refresh_timer_value()));

    //set focus trigger for lineEdits
    connect(ui->lineEdit_X,SIGNAL(focusin()),this,SLOT(on_lineEditX_triggered()));
    connect(ui->lineEdit_Y,SIGNAL(focusin()),this,SLOT(on_lineEditY_triggered()));
    connect(ui->lineEdit_X,SIGNAL(focusout()),this,SLOT(on_lineEditX_turn_off()));
    connect(ui->lineEdit_Y,SIGNAL(focusout()),this,SLOT(on_lineEditY_turn_off()));
    load_setting();


    //this section is created for enabling ui responsivness
    size_of_main_windows=this->geometry();
    list_widget=ui->centralWidget->findChildren<QWidget*>();
    foreach (QWidget* w, list_widget) {
        size_of_all_objects.push_back(w->geometry());
    }

    connect(this,SIGNAL(X_value_signal()),this,SLOT(on_X_value_changed()));
    connect(this,SIGNAL(Y_value_signal()),this,SLOT(on_Y_value_changed()));

    reset_all_labels();
}

void MainWindow::on_X_value_changed()
{
    qDebug()<<"SIGNAL EMMITTED::X value has been changed"<<m_x<<endl;
}

void MainWindow::on_Y_value_changed()
{
    qDebug()<<"SIGNAL EMMITTED::Y value has been changed"<<m_y<<endl;
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QRect new_size=this->geometry();
    double width_scale=static_cast<double>(new_size.width())/size_of_main_windows.width();
    double height_scale=static_cast<double>(new_size.height())/size_of_main_windows.height ();
    int count=0;

    foreach (QWidget* w, list_widget) {
        w->setGeometry(change_geometry(size_of_all_objects[static_cast<size_t>(count)],width_scale,height_scale));
        count++;
    }




}

QRect MainWindow::change_geometry(QRect rect, double width_scale, double height_scale)
{
    int new_width=static_cast<int>( rect.width()*width_scale);
    int new_heigt=static_cast<int>( rect.height()*height_scale);
    int new_x=static_cast<int>( rect.x()*width_scale);
    int new_y=static_cast<int>(rect.y()*height_scale);
    rect.setX(new_x);
    rect.setY(new_y);
    rect.setWidth(new_width);
    rect.setHeight(new_heigt);
    return rect;
}
void MainWindow::create_folder_for_settings()
{
    m_joyStick_saved_folder=QApplication::applicationDirPath()+"/joyStick_setting";
    if(!QDir(m_joyStick_saved_folder).exists())
    {
        QDir().mkdir(m_joyStick_saved_folder);
    }
    else
    {
        //if path existed then load files to QTableWidget
        load_all_saved_setting();
    }

    m_Manaul_saved_folder=QApplication::applicationDirPath()+"/manual_setting";
    if(!QDir(m_Manaul_saved_folder).exists())
    {
        QDir().mkdir(m_Manaul_saved_folder);
    }
    else
    {
        //if path existed then load files to QTableWidget
        load_all_saved_setting();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_setting()
{
    QSettings settings(m_setting_file, QSettings::NativeFormat);
    QString XSTEP_string = settings.value("m_XSTEP", "").toString();
    QString YSTEP_string = settings.value("m_YSTEP", "").toString();
    QString X_string = settings.value("m_x", "").toString();
    QString Y_string = settings.value("m_Y", "").toString();

    ui->horizontalSlider_XSTEP->setValue(static_cast<int>(100*XSTEP_string.toDouble()));
    ui->horizontalSlider_YSTEP->setValue(static_cast<int>(100*YSTEP_string.toDouble()));

    set_m_x(X_string.toDouble());
    emit X_value_signal();
    QString XString="X : "+QString::number(read_m_x());
    ui->label_X->setText(XString);

    set_m_y(Y_string.toDouble());
    emit Y_value_signal();
    QString YString="Y : "+QString::number(read_m_y());
    ui->label_Y->setText(YString);

}

void MainWindow::save_setting(int var)
{
    QSettings settings(m_setting_file, QSettings::NativeFormat);
    switch (var)
    {
    case save_var::X_STEP_CHANGED:
    {
        QString XSTEP_string = QString::number(read_m_XSTEP());
        settings.setValue("m_XSTEP", XSTEP_string);
        break;
    }
    case save_var::Y_STEP_CHANGED:
    {
        QString YSTEP_string = QString::number(read_m_YSTEP());
        settings.setValue("m_YSTEP", YSTEP_string);
        break;
    }

    case save_var::Y_CHANGED:
    {
        QString Y_string = QString::number(read_m_y());
        settings.setValue("m_Y", Y_string);
        break;
    }
    case save_var::X_CHANGED:
    {
        QString X_string = QString::number(read_m_x());
        settings.setValue("m_x", X_string);
        break;
    }
    }
}



void MainWindow::note(QString NoteString)
{
    m_NoteString->setText(NoteString);
    m_NoteString->setStyleSheet("background-color:rgb(70, 71, 255);color:white;font: 75 16pt ""times"";");
    ui->statusBar->addWidget(m_NoteString);
}

void MainWindow::add_file_to_table(int row_number, QString fileName)
{
    item=new QTableWidgetItem;
    item->setText(fileName);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font;
    font.setFamily("times new roman");
    font.setPixelSize(24);
    item->setFont(font);
    ui->tableWidget->setItem(row_number,0,item);
}

void MainWindow::load_all_saved_setting()
{
    if(m_selected_mode==Mode::joyStick)
    {
        int file_number=0;
        m_file_names.resize(0);
        QString file_name=m_joyStick_saved_folder+"/JoyStick_"+QString::number(file_number)+".txt";
        QFileInfo file(file_name);
        while(file.exists())
        {
            if(ui->tableWidget->rowCount()<file_number+1)
            {
                ui->tableWidget->insertRow(file_number);
            }
            m_file_names.push_back(file_name);
            add_file_to_table(file_number,file.fileName());
            file_number++;
            file_name=m_joyStick_saved_folder+"/JoyStick_"+QString::number(file_number)+".txt";
            file.setFile(file_name);
        }
    }
    else if(m_selected_mode==Mode::Manual)
    {
        int file_number=0;
        m_file_names.resize(0);
        QString file_name=m_Manaul_saved_folder+"/Manual_"+QString::number(file_number)+".txt";
        QFileInfo file(file_name);
        while(file.exists())
        {
            if(ui->tableWidget->rowCount()<file_number+1)
            {
                ui->tableWidget->insertRow(file_number);
            }
            m_file_names.push_back(file_name);
            add_file_to_table(file_number,file.fileName());
            file_number++;
            file_name=m_Manaul_saved_folder+"/Manual_"+QString::number(file_number)+".txt";
            file.setFile(file_name);
        }
    }
}

void MainWindow::show_selected_config(int row_number)
{
    QFile file(m_file_names[static_cast<size_t>(row_number)]);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    QFileInfo file_info=m_file_names[static_cast<size_t>(row_number)];
    int i=0;
    ui->button_load_selected_setting->setText("load "+file_info.fileName());
    QString for_plain_text=file_info.fileName()+"\n";
    while(!in.atEnd())
    {
        if(m_selected_mode==joyStick)
        {
            QString line=in.readLine();
            switch (i) {
            case 0:{for_plain_text+="X : "+line +"\n" ;m_x_temp=line.toDouble();break;}
            case 1:{for_plain_text+="Y : "+line +"\n" ;m_y_temp=line.toDouble();break;}
            case 2:{for_plain_text+="X_STEP : "+line+"\n" ;m_XSTEP_temp=line.toDouble();break;}
            case 3:{for_plain_text+="Y_STEP : "+line+"\n" ;m_YSTEP_temp=line.toDouble();break;}
            }
        }
        else if (m_selected_mode==Manual) {
            QString line=in.readLine();
            switch (i) {
            case 0:{for_plain_text+="X : "+line +"\n" ;m_x_temp=line.toDouble();break;}
            case 1:{for_plain_text+="Y : "+line +"\n" ;m_y_temp=line.toDouble();break;}
            }
        }
        i++;
    }
    ui->plainTextEdit->setPlainText(for_plain_text);
}





void MainWindow::on_horizontalSlider_XSTEP_valueChanged(int value)
{
    set_m_XSTEP(value);  //save value to use later
    QString label_xstep_string="XSTEP\n"+QString::number(read_m_XSTEP());
    ui->label_XSTEP->setText(label_xstep_string);

    //call save_setting function to save new settings
    save_setting(save_var::X_STEP_CHANGED);
}

void MainWindow::on_horizontalSlider_YSTEP_valueChanged(int value)
{
    set_m_YSTEP(value);      //save value to use later
    QString label_ystep_string="YSTEP\n"+QString::number(read_m_YSTEP());
    ui->label_YSTEP->setText(label_ystep_string);

    //call save_setting function to save new settings
    save_setting(save_var::Y_STEP_CHANGED);
}

void MainWindow::up_button_clicked()
{
    if(abs(read_m_YSTEP()-0.0)>EPS)
    {
        set_m_y(read_m_y()+read_m_YSTEP());
        emit Y_value_signal();
        QString YString="Y : "+QString::number(read_m_y());
        ui->label_Y->setText(YString);
        //save updated Y value in file
        save_setting(save_var::Y_CHANGED);
    }
    else
    {
        note("   note that Y step is 0, please change y step first and try again");
    }
}
void MainWindow::reset_timer_value()
{
    timer_elapsed=250;
}

void MainWindow::reset_all_labels()
{
    ui->label_down->setStyleSheet(default_style_for_label);
    ui->label_up->setStyleSheet(default_style_for_label);
    ui->label_right->setStyleSheet(default_style_for_label);
    ui->label_left->setStyleSheet(default_style_for_label);
}

void MainWindow::up_button_pressed()
{
    ui->label_up->setStyleSheet(pressed_style_for_label);
    reset_timer_value();
    up_button_checker->start(timer_elapsed);
}

void MainWindow::up_button_released()
{
    reset_all_labels();
    up_button_checker->stop();
    temp_timer=0;
}

void MainWindow::down_button_pressed()
{
    ui->label_down->setStyleSheet(pressed_style_for_label);
    reset_timer_value();
    down_button_checker->start(timer_elapsed);
}

void MainWindow::down_button_released()
{
    reset_all_labels();
    down_button_checker->stop();
    temp_timer=0;
}

void MainWindow::right_button_pressed()
{
    ui->label_right->setStyleSheet(pressed_style_for_label);
    reset_timer_value();
    right_button_checker->start(timer_elapsed);
}

void MainWindow::right_button_released()
{
    reset_all_labels();
    right_button_checker->stop();
    temp_timer=0;
}

void MainWindow::left_button_pressed()
{
    ui->label_left->setStyleSheet(pressed_style_for_label);
    reset_timer_value();
    left_button_checker->start(timer_elapsed);
}

void MainWindow::left_button_released()
{
    reset_all_labels();
    left_button_checker->stop();
    temp_timer=0;
}
void MainWindow::refresh_timer_value()
{
    temp_timer++;
    if(temp_timer>2 && temp_timer<4)
    {
        set_all_timer_value(130);
    }
    else if(temp_timer>=4 && temp_timer<6)
    {
        set_all_timer_value(50);
    }
    else if(temp_timer>=6)
    {
        set_all_timer_value(20);
    }
}

void MainWindow::set_all_timer_value(int time)
{
    up_button_checker->setInterval(time);
    down_button_checker->setInterval(time);
    left_button_checker->setInterval(time);
    right_button_checker->setInterval(time);
}

void MainWindow::set_radius_for_labels(int radius)
{
    default_style_for_label=QString("border-width:4px;"
                                      "border-radius:%1px;"
                                      "border-color:rgb(70,70,90);"
                                      "padding:8px;"
                                      "background-color: rgb(50, 50, 100);"
                                      "color: rgb(255, 255, 255);"
                                      "border-color: rgb(0,160,200);").arg(radius);

    pressed_style_for_label=QString("border-width:2px;"
                                      "border-radius:%1px;"
                                      "border-color:rgb(70,70,90);"
                                      "padding:8px;"
                                      "background-color: rgb(50, 50, 100);"
                                      "color: rgb(255, 255, 255);"
                                      "border-color: rgb(240,160,0);").arg(radius);
}

void MainWindow::down_button_clicked()
{
    if(abs(read_m_YSTEP()-0)>EPS)
    {
        set_m_y(read_m_y()-read_m_YSTEP());
        emit Y_value_signal();
        QString YString="Y : "+QString::number(read_m_y());
        ui->label_Y->setText(YString);
        //save updated Y value in file
        save_setting(save_var::Y_CHANGED);
    }
    else
    {
        note("   note that Y step is 0, please change y step first and try again");
    }
}

void MainWindow::left_button_clicked()
{
    if(abs(read_m_XSTEP()-0)>EPS)
    {
        set_m_x(read_m_x()-read_m_XSTEP());
        emit X_value_signal();
        QString XString="X : "+QString::number(read_m_x());
        ui->label_X->setText(XString);
        //save updated X value in file
        save_setting(save_var::X_CHANGED);
    }
    else
    {
        note("   note that X step is 0, please change x step first and try again");
    }
}

void MainWindow::right_button_clicked()
{
    if(abs(read_m_XSTEP()-0.0)>EPS)
    {
        set_m_x(read_m_x()+read_m_XSTEP());
        emit X_value_signal();
        QString XString="X : "+QString::number(read_m_x());
        ui->label_X->setText(XString);
        //save updated X value in file
        save_setting(save_var::X_CHANGED);
    }
    else
    {
        note("   note that X step is 0, please change x step first and try again");
    }
}





void MainWindow::on_button_save_setting_clicked()
{
    if(m_selected_mode==Mode::joyStick)
    {
        int file_number=0;
        QString file_name=m_joyStick_saved_folder+"/JoyStick_"+QString::number(file_number)+".txt";
        QFile file(file_name);
        while(file.exists())
        {
            file_number++;
            QString file_name=m_joyStick_saved_folder+"/JoyStick_"+QString::number(file_number)+".txt";
            file.setFileName(file_name);
        }

        file.open(QIODevice::ReadWrite);
        QTextStream out(&file);
        out<<read_m_x()<<endl<<read_m_y()<<endl<<read_m_XSTEP()<<endl<<read_m_YSTEP();

        note("   Configuration saved in:   JoyStick_"+QString::number(file_number)+".txt");
    }
    else if(m_selected_mode==Mode::Manual)
    {
        int file_number=0;
        QString file_name=m_Manaul_saved_folder+"/Manual_"+QString::number(file_number)+".txt";
        QFile file(file_name);
        while(file.exists())
        {
            file_number++;
            QString file_name=m_Manaul_saved_folder+"/Manual_"+QString::number(file_number)+".txt";
            file.setFileName(file_name);
        }

        file.open(QIODevice::ReadWrite);
        QTextStream out(&file);
        out<<read_m_x()<<endl<<read_m_y()<<endl;

        note("   Configuration saved in:   Manual_"+QString::number(file_number)+".txt");
    }
    //update tableWidget
    load_all_saved_setting();
}

void MainWindow::which_row_selected(int row_selected)
{
    qDebug()<<row_selected;
}

void MainWindow::which_row_selected(const QItemSelection &in1, const QItemSelection &in2)
{

    try {
        QModelIndexList out=in1.indexes();
        if(out.size()>0)
        {
            m_row_selected=out[0].row();
            qDebug()<<m_row_selected;
            show_selected_config(m_row_selected);
        }
    }
    catch(...){

    }
}

void MainWindow::on_button_load_selected_setting_clicked()
{
    if(m_selected_mode==Mode::joyStick)
    {
        set_m_XSTEP(100*m_XSTEP_temp);
        set_m_YSTEP(100*m_YSTEP_temp);
        set_m_x(m_x_temp);emit X_value_signal();
        set_m_y(m_y_temp);emit Y_value_signal();

        ui->horizontalSlider_XSTEP->setValue(static_cast<int>(100*read_m_XSTEP()));
        ui->horizontalSlider_YSTEP->setValue(static_cast<int>(100*read_m_YSTEP()));

        QString XString="X : "+QString::number(read_m_x());
        ui->label_X->setText(XString);

        QString YString="Y : "+QString::number(read_m_y());
        ui->label_Y->setText(YString);
    }
    else if (m_selected_mode==Mode::Manual)
    {
        set_m_x(m_x_temp);emit X_value_signal();
        set_m_y(m_y_temp);emit Y_value_signal();
        ui->lineEdit_X->setText(QString::number(read_m_x()));
        ui->lineEdit_Y->setText(QString::number(read_m_y()));
        ui->label_->setText("x: "+QString::number(read_m_x()));
        ui->label__->setText("y: "+QString::number(read_m_y()));
    }
}

void MainWindow::on_button_manual_mode_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    if(m_selected_mode!=Mode::Manual)
    {
        m_selected_mode=Mode::Manual;
        ui->tableWidget->setRowCount(0);
        load_all_saved_setting();
    }
}

void MainWindow::on_button_Joystick_mode_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    if(m_selected_mode!=Mode::joyStick)
    {
        m_selected_mode=Mode::joyStick;
        ui->tableWidget->setRowCount(0);
        load_all_saved_setting();
    }
}

void MainWindow::on_lineEditX_triggered()
{
    m_temp=0;
    qDebug()<<m_temp;
}

void MainWindow::on_lineEditY_triggered()
{
    m_temp=1;
    qDebug()<<m_temp;
}

void MainWindow::on_lineEditX_turn_off()
{

}

void MainWindow::on_lineEditY_turn_off()
{

}

void MainWindow::on_button_0_clicked()
{
    process_numbers(0);
}

void MainWindow::on_button_1_clicked()
{
    process_numbers(1);
}

void MainWindow::on_button_2_clicked()
{
    process_numbers(2);
}

void MainWindow::on_button_3_clicked()
{
    process_numbers(3);
}

void MainWindow::on_button_4_clicked()
{
    process_numbers(4);
}

void MainWindow::on_button_5_clicked()
{
    process_numbers(5);
}

void MainWindow::on_button_6_clicked()
{
    process_numbers(6);
}

void MainWindow::on_button_7_clicked()
{
    process_numbers(7);
}

void MainWindow::on_button_8_clicked()
{
    process_numbers(8);
}

void MainWindow::on_button_9_clicked()
{
    process_numbers(9);
}

void MainWindow::on_button_dot_clicked()
{

    if(m_temp==0)
    {
        QString X_STRING=ui->lineEdit_X->text();
        if(X_STRING.contains("."))
        {
            note("  syntax is not correct   ");
        }
        else
        {
            ui->lineEdit_X->setText(ui->lineEdit_X->text()+".");
        }
    }
    else if(m_temp==1)
    {
        QString Y_STRING=ui->lineEdit_Y->text();
        if(Y_STRING.contains("."))
        {
            note("  syntax is not correct   ");
        }
        else
        {
            ui->lineEdit_Y->setText(ui->lineEdit_Y->text()+".");
        }

    }
}

void MainWindow::on_button_clear_clicked()
{
    if(m_temp==0)
    {
        QString XString=ui->lineEdit_X->text();
        ui->lineEdit_X->setText(XString.left(XString.length()-1));
    }
    else if(m_temp==1)
    {
        QString YString=ui->lineEdit_Y->text();
        ui->lineEdit_Y->setText(YString.left(YString.length()-1));
    }
}

void MainWindow::process_numbers(int i)
{
    if(m_temp==0)
    {
        ui->lineEdit_X->setText(ui->lineEdit_X->text()+QString::number(i));
    }
    else if(m_temp==1)
    {
        ui->lineEdit_Y->setText(ui->lineEdit_Y->text()+QString::number(i));
    }


}

void MainWindow::on_button_apply_XCHANGE_clicked()
{
    QString XSTRING=ui->lineEdit_X->text();
    bool validInt;
    double number=XSTRING.toDouble(&validInt);
    if(validInt)
    {
        note("  X changes to :"+QString::number(number));
        set_m_x(number);emit X_value_signal();
        ui->label_->setText("x: "+QString::number(number));
    }
}

void MainWindow::on_button_apply_YCHANGE_clicked()
{
    QString YSTRING=ui->lineEdit_Y->text();
    bool validInt;
    double number=YSTRING.toDouble(&validInt);
    if(validInt)
    {
        note("  Y changes to :"+QString::number(number));
        set_m_y(number);emit Y_value_signal();emit Y_value_signal();
        ui->label__->setText("y: "+QString::number(number));
    }
}

void MainWindow::set_m_XSTEP(double xstep)
{
    m_XSTEP=xstep;
}

void MainWindow::set_m_YSTEP(double ystep)
{
    m_YSTEP=ystep;
}

void MainWindow::set_m_x(double x)
{
    m_x=x;
    if(m_x<=0)
    {
        m_x=0;
    }
    else if (m_x>360)
    {
        m_x=360;
    }
}

void MainWindow::set_m_y(double y)
{
    m_y=y;
    if(m_y<=4)
    {
        m_y=4;
    }
    else if(m_y>=45)
    {
        m_y=45;
    }
}

double MainWindow::read_m_x()
{
    return m_x;
}

double MainWindow::read_m_y()
{
    return m_y;
}

double MainWindow::read_m_XSTEP()
{
    return m_XSTEP/100;
}

double MainWindow::read_m_YSTEP()
{
    return m_YSTEP/100;
}

