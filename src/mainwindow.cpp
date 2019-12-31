#include "mainwindow.h"
#include "ui_mainwindow.h"
// Qt


#include "objectdetection.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup UI
    ui->setupUi(this);

    // Set start tab as blank
    QLabel *newTab = new QLabel(ui->tabWidget);
    newTab->setText("No camera connected.");
    newTab->setAlignment(Qt::AlignCenter);
    ui->tabWidget->addTab(newTab, "");
    ui->tabWidget->setTabsClosable(false);

    // Add "Connect to Camera" button to tab
    connectToCameraButton = new QPushButton();
    connectToCameraButton->setText("Connect to Camera");
    ui->tabWidget->setCornerWidget(connectToCameraButton, Qt::TopLeftCorner);
    connect(connectToCameraButton,SIGNAL(released()),this, SLOT(connectToCamera()));

    // Add "Play Video" button to tab
    playVideoButton = new QPushButton();
    playVideoButton->setText("Play Video");
    ui->tabWidget->setCornerWidget(playVideoButton, Qt::TopRightCorner);
    connect(playVideoButton,SIGNAL(released()),this, SLOT(playVideoFile()));
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this, SLOT(disconnectCamera(int)));

    // Set focus on button
    connectToCameraButton->setFocus();

    // Connect other signals/slots
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionFullScreen, SIGNAL(toggled(bool)), this, SLOT(setFullScreen(bool)));

    //set settings
    appSettingsController = new appSettings();

    //lets make sure we have our app folder
    if(appSettingsController->permission != true)
    {
        QMessageBox::information(this, "Permission Issue", QString("It appears we can't access our application folder : %1").arg(appSettingsController->settingsPath));
    }

    ui->setDirectoryTextEdit->setText(this->appSettingsController->getSetsPath());
    ui->setImageExtensionTextEdit->setText(this->appSettingsController->getSetImageExtensions());

    //create set controller
    this->setController = ui->SetViewWidget;
    this->setController->initalize(appSettingsController);

    // add sets to ui combo selection drop down
    // only need to call this function on initalization because it also
    // initalizes the class view drop down due to the nature of using the slot
    // on_index_changed fro Qt framework
    ui->setComboBox->addItems(this->setController->sets);
//    this->setController->getSetDirectories();
    //add set classes to ui combo selection drop down
//    ui->viewComboBox->addItems(this->setController->getSetClassDirectories());
//    this->setController->getSetClassDirectories();

//    this->setController->setSetName(ui->setComboBox->currentText());
//    this->setController->setViewName(ui->viewComboBox->currentText());
    //add items to set list widget
    this->setController->getSetFiles();

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setAlignment(Qt::AlignHCenter);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    scrollArea->setGeometry(ui->ImageViewLayout->geometry());
    scrollArea->setWidget(this->setController->getImageView());
    connect(ui->setImagespinBox, SIGNAL(valueChanged(int)), this->setController->getImageView(), SLOT(zoomChanged(int)));

    //add image view widget to main window
    ui->ImageViewLayout->addWidget(scrollArea);

    //setup shortcut keys
    ui->saveImageButton->setShortcut(QKeySequence::fromString("CTRL+S"));

//    if(ui->listWidget->count() > 0)
//    {
//      ui->listWidget->item(1)->setSelected(true);
//    }
//    ui->listWidget->setFocus();

    this->setController->repaint();
//    this->setController->getImageView()->repaint();

    this->drawTool = Annotation::draw_square;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setClassComboBox()
{
    ui->classComboBox->clear();

    QMap<QString, QString> map = this->setController->db->getClasses();
    int index = 0;
    for(QMap<QString, QString>::const_iterator iter = map.begin(); iter != map.end(); ++iter) {
      QPixmap pixmap(25, 25);
      pixmap.fill(QColor(iter.value()));
      QIcon icon(pixmap);

      QVariant tempVariant = iter.key();
      ui->classComboBox->insertItem(index, icon, tempVariant.toString());
      index++;
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event){
    qDebug("\nkey press : %i", event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
    qDebug("\nkey release: %i", event->key());
}

void MainWindow::showAboutDialog()
{
    QMessageBox::information(this, "About", QString("Created by Ryein C. Goddard\n\nContact: ryein@goddardlabs.com\nWebsite: www.goddardlabs.com\n\nVersion: %1").arg(APP_VERSION));
}

bool MainWindow::removeFromMapByTabIndex(QMap<int, int> &map, int tabIndex)
{
    QMutableMapIterator<int, int> i(map);
    while (i.hasNext())
    {
         i.next();
         if(i.value()==tabIndex)
         {
             i.remove();
             return true;
         }
    }
    return false;
}

void MainWindow::updateMapValues(QMap<int, int> &map, int tabIndex)
{
    QMutableMapIterator<int, int> i(map);
    while (i.hasNext())
    {
        i.next();
        if(i.value()>tabIndex)
            i.setValue(i.value()-1);
    }
}

void MainWindow::setTabCloseToolTips(QTabWidget *tabs, QString tooltip)
{
    QList<QAbstractButton*> allPButtons = tabs->findChildren<QAbstractButton*>();
    for (int ind = 0; ind < allPButtons.size(); ind++)
    {
        QAbstractButton* item = allPButtons.at(ind);
        if (item->inherits("CloseButton"))
            item->setToolTip(tooltip);
    }
}

void MainWindow::setFullScreen(bool input)
{
    if(input)
        this->showFullScreen();
    else
        this->showNormal();
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(index.row());
}

void MainWindow::on_saveAppSettingsButton_clicked()
{
    this->appSettingsController->setSetsPath(ui->setDirectoryTextEdit->text());
    this->appSettingsController->setSetImageExtensions(ui->setImageExtensionTextEdit->text());
}

//void MainWindow::on_createSetButton_clicked()
//{
//    createSetDialog *setDialog = new createSetDialog(this);
//    connect(setDialog, SIGNAL(sendString(QString)), this, SLOT(recieveSetText(QString)));
//    setDialog->show();
//}

void MainWindow::recieveSetText(const QString &newText)
{
    //ui->imageListWidget->clear();
    ui->setComboBox->clear();
    QDir dir(appSettingsController->getSetsPath());
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    QStringList files = dir.entryList();
    ui->setComboBox->addItems(files);
}

void MainWindow::on_setComboBox_currentIndexChanged(const QString &arg1)
{
    //clear the set class combo dropdown box
//    ui->viewComboBox->clear();
    this->setController->setSetName(arg1);
//    ui->viewComboBox->addItems(this->setController->getSetClassDirectories());
    //removed because now the set files needs to be taken when the class view is changed
//    this->setController->getSetFiles(currentSet); //, currentView
    this->setClassComboBox();

    this->setController->setViewName("images");
    this->setController->getSetFiles();
}


void MainWindow::on_deleteImageButton_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Set File Delete Confirmation");
    msgBox.setText("Are you sure you want to delete this set image?");
    msgBox.setStandardButtons(QMessageBox::Apply | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Apply);

    if(msgBox.exec() == QMessageBox::Apply)
    {
      this->setController->deleteImage();
    }
}

//void MainWindow::on_positiveImageRadioButton_clicked()
//{
////    this->setController->setImageStatus("positive");
//}

//void MainWindow::on_negativeImageRadioButton_clicked()
//{
////    this->setController->setImageStatus("negative");
//}

void MainWindow::on_saveImageButton_clicked()
{
    this->setController->saveImage();
    //bool returnValue = this->setController->getImageView()->imageBuffer.last().save(appSettingsController->getSetsPath() + "\\" + ui->setComboBox->currentText() + "\\" + ui->imageListWidget->currentItem()->text());
    //this->setController->currentItem()->setIcon(QIcon(this->appSettingsController->getSetsPath() + "\\" + ui->setComboBox->currentText() + "\\" + this->setController->currentItem()->text()));
}

void MainWindow::on_copySetImageButton_clicked()
{
    this->setController->copyImage();
}

void MainWindow::on_actionList_View_triggered()
{
    this->setController->setViewMode(QListView::ListMode);
    this->appSettingsController->setSetViewMode(QListView::ListMode);
}

void MainWindow::on_actionIcon_View_triggered()
{
    this->setController->setViewMode(QListView::IconMode);
    this->appSettingsController->setSetViewMode(QListView::IconMode);
}

void MainWindow::on_squareDrawRadioButton_clicked()
{
//    this->drawTool = draw_square;
    this->setController->getImageView()->setDrawingTool(Annotation::draw_square);
}

void MainWindow::on_lineDrawRadioButton_clicked()
{
//    this->drawTool = draw_line;
    this->setController->getImageView()->setDrawingTool(Annotation::draw_line);
}

void MainWindow::on_circalDrawRadioButton_clicked()
{
//    this->drawTool = draw_circle;
    this->setController->getImageView()->setDrawingTool(Annotation::draw_circle);
}

void MainWindow::on_addClassButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
    {
//        colorDialogOptionsWidget = new DialogOptionsWidget;
//        colorDialogOptionsWidget->addCheckBox(doNotUseNativeDialog, QColorDialog::DontUseNativeDialog);
//        colorDialogOptionsWidget->addCheckBox(tr("Show alpha channel") , QColorDialog::ShowAlphaChannel);
//        colorDialogOptionsWidget->addCheckBox(tr("No buttons") , QColorDialog::NoButtons);

        qDebug() << "CLASS NAME " << text;

//        const QColorDialog::ColorDialogOptions options = QFlag(colorDialogOptionsWidget->value());
        const QColor color = QColorDialog::getColor(Qt::green, this, "Select Color");

        if (color.isValid()) {
//            colorLabel->setText(color.name());
//            colorLabel->setPalette(QPalette(color));
//            colorLabel->setAutoFillBackground(true);
            if(this->setController->db->insertClass(text, color.name()) == 0)
                qDebug() << "INSERT CLASS FAILED";
            else
                qDebug() << "INSERT CLASS SUCCESS";

            qDebug() << "CLASS COLOR " << color.name();

            this->setClassComboBox();
        }
    }
//        textLabel->setText(text);
}

void MainWindow::on_removeClassButton_clicked()
{
    qDebug() << "REMOVE ID " << ui->classComboBox->currentText();
    this->setController->db->removeClass(ui->classComboBox->currentText());
    this->setClassComboBox();
}

void MainWindow::on_classComboBox_currentIndexChanged(const QString &arg1)
{
    QStringList class_data = this->setController->db->getClass(arg1);

    if(class_data.count() > 0)
    {
        this->setController->getImageView()->current_class_id = QString(class_data[0]).toInt();
        this->setController->getImageView()->current_class = class_data[1];
        this->setController->getImageView()->current_color = class_data[2];
    }
}

void MainWindow::on_actionExport_Dataset_triggered()
{
    ExportDataSet *exportData = new ExportDataSet();
    if(exportData->setDatabase(this->setController->db)) {
        exportData->setBasePath(this->setController->getSetPath());
        qDebug () << "SET PATH YO " << this->setController->getSetPath();
        //QString(appSettingsController->getSetsPath()).append("/").append(setControl.setName)
        QString write_success = exportData->writeCSVData();
        if(write_success != "fail")
        {
            const QString title = "Export Data";
            const QString message = QString("Your data set data has been exported to a file located at : \n").append(write_success);
            QMessageBox::information(
                        this,
                        title,
                        message);
        }

        else
        {
            const QString title = "Export Data";
            const QString message = QString("Data export failed.");
            QMessageBox::information(
                        this,
                        title,
                        message);
        }
    }
}
