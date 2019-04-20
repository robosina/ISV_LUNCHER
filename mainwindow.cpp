#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
                                          QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    ui->label->setLayoutDirection(Qt::RightToLeft);


//    ui->label->setAlignment(Qt::AlignRight);


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

    //set focus trigger for lineEdits
    connect(ui->lineEdit_X,SIGNAL(focusin()),this,SLOT(on_lineEditX_triggered()));
    connect(ui->lineEdit_Y,SIGNAL(focusin()),this,SLOT(on_lineEditY_triggered()));
    connect(ui->lineEdit_X,SIGNAL(focusout()),this,SLOT(on_lineEditX_turn_off()));
    connect(ui->lineEdit_Y,SIGNAL(focusout()),this,SLOT(on_lineEditY_turn_off()));
    load_setting();

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

    ui->horizontalSlider_XSTEP->setValue(XSTEP_string.toInt());
    ui->horizontalSlider_YSTEP->setValue(YSTEP_string.toInt());

    m_x=X_string.toInt();
    QString XString="X : "+QString::number(m_x);
    ui->label_X->setText(XString);

    m_y=Y_string.toInt();
    QString YString="Y : "+QString::number(m_y);
    ui->label_Y->setText(YString);

}

void MainWindow::save_setting(int var)
{
    QSettings settings(m_setting_file, QSettings::NativeFormat);
    switch (var)
    {
    case save_var::X_STEP_CHANGED:
    {
        QString XSTEP_string = QString::number(m_XSTEP);
        settings.setValue("m_XSTEP", XSTEP_string);
        break;
    }
    case save_var::Y_STEP_CHANGED:
    {
        QString YSTEP_string = QString::number(m_YSTEP);
        settings.setValue("m_YSTEP", YSTEP_string);
        break;
    }

    case save_var::Y_CHANGED:
    {
        QString Y_string = QString::number(m_y);
        settings.setValue("m_Y", Y_string);
        break;
    }
    case save_var::X_CHANGED:
    {
        QString X_string = QString::number(m_x);
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
            case 0:{for_plain_text+="X : "+line +"\n" ;m_x=line.toInt();break;}
            case 1:{for_plain_text+="Y : "+line +"\n" ;m_y=line.toInt();break;}
            case 2:{for_plain_text+="X_STEP : "+line+"\n" ;m_XSTEP=line.toInt();break;}
            case 3:{for_plain_text+="Y_STEP : "+line+"\n" ;m_YSTEP=line.toInt();break;}
            }
        }
        else if (m_selected_mode==Manual) {
            QString line=in.readLine();
            switch (i) {
            case 0:{for_plain_text+="X : "+line +"\n" ;m_x=line.toInt();break;}
            case 1:{for_plain_text+="Y : "+line +"\n" ;m_y=line.toInt();break;}
            }
        }
        i++;
    }
    ui->plainTextEdit->setPlainText(for_plain_text);
}

void MainWindow::on_horizontalSlider_XSTEP_valueChanged(int value)
{
    m_XSTEP=value;   //save value to use later
    QString label_xstep_string="XSTEP\n"+QString::number(m_XSTEP);
    ui->label_XSTEP->setText(label_xstep_string);

    //call save_setting function to save new settings
    save_setting(save_var::X_STEP_CHANGED);
}

void MainWindow::on_horizontalSlider_YSTEP_valueChanged(int value)
{
    m_YSTEP=value;  //save value to use later
    QString label_ystep_string="YSTEP\n"+QString::number(m_YSTEP);
    ui->label_YSTEP->setText(label_ystep_string);

    //call save_setting function to save new settings
    save_setting(save_var::Y_STEP_CHANGED);
}

void MainWindow::up_button_clicked()
{
    if(m_YSTEP!=0)
    {
        m_y+=m_YSTEP;
        QString YString="Y : "+QString::number(m_y);
        ui->label_Y->setText(YString);

        //save updated Y value in file
        save_setting(save_var::Y_CHANGED);
    }
    else
    {
        note("   note that Y step is 0, please change y step first and try again");
    }
}

void MainWindow::down_button_clicked()
{
    if(m_YSTEP!=0)
    {
        m_y-=m_YSTEP;
        QString YString="Y : "+QString::number(m_y);
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
    if(m_XSTEP!=0)
    {
        m_x-=m_XSTEP;
        QString XString="X : "+QString::number(m_x);
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
    if(m_XSTEP!=0)
    {
        m_x+=m_XSTEP;
        QString XString="X : "+QString::number(m_x);
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
        out<<m_x<<endl<<m_y<<endl<<m_XSTEP<<endl<<m_YSTEP;

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
        out<<m_x<<endl<<m_y<<endl;

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
        ui->horizontalSlider_XSTEP->setValue(m_XSTEP);
        ui->horizontalSlider_YSTEP->setValue(m_YSTEP);

        QString XString="X : "+QString::number(m_x);
        ui->label_X->setText(XString);

        QString YString="Y : "+QString::number(m_y);
        ui->label_Y->setText(YString);
    }
    else if (m_selected_mode==Mode::Manual)
    {
        ui->lineEdit_X->setText(QString::number(m_x));
        ui->lineEdit_Y->setText(QString::number(m_y));
        ui->label_->setText("x: "+QString::number(m_x));
        ui->label__->setText("y: "+QString::number(m_y));
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
    int number=XSTRING.toInt(&validInt);
    if(validInt)
    {
        note("  X changes to :"+QString::number(number));
        m_x=number;
        ui->label_->setText("x: "+QString::number(number));
    }
}

void MainWindow::on_button_apply_YCHANGE_clicked()
{
    QString YSTRING=ui->lineEdit_Y->text();
    bool validInt;
    int number=YSTRING.toInt(&validInt);
    if(validInt)
    {
        note("  Y changes to :"+QString::number(number));
        m_y=number;
        ui->label__->setText("y: "+QString::number(number));
    }
}
