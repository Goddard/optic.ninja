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

    // Create SharedImageBuffer object
    sharedImageBuffer = new bufferThread();

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
    this->setController = new setControl(appSettingsController);
    ui->SetViewLayout->addWidget(this->setController);

    //add sets to ui combo selection drop down
    // only need to call this function on initalization because it also
    // initalizes the class view drop down due to the nature of using the slot
    // on_index_changed fro Qt framework
    ui->setComboBox->addItems(this->setController->getSetDirectories());
//    this->setController->getSetDirectories();
    //add set classes to ui combo selection drop down
//    ui->viewComboBox->addItems(this->setController->getSetClassDirectories());
    this->setController->getSetClassDirectories();

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

    if(ui->listWidget->count() > 0)
    {
      ui->listWidget->item(1)->setSelected(true);
    }
    ui->listWidget->setFocus();

    this->setController->repaint();
    this->setController->getImageView()->repaint();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event){
    qDebug("\nkey press : %i", event->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent * event)
{
    qDebug("\nkey release: %i", event->key());
}

void MainWindow::playVideoFile()
{
    // We cannot connect to a camera if devices are already connected and stream synchronization is in progress
    if(ui->actionSynchronizeStreams->isChecked() && deviceNumberMap.size()>0 && sharedImageBuffer->getSyncEnabled())
    {
        // Prompt user
        QMessageBox::warning(this, tr("qt-opencv-multithreaded"),
                                    tr("Stream synchronization is in progress.\n\n"
                                       "Please close all currently open streams before attempting to open a new stream."),
                                        QMessageBox::Ok);
    }
    // Attempt to connect to camera
    else
    {
        // Get next tab index
        int nextTabIndex = (deviceNumberMap.size()==0) ? 0 : ui->tabWidget->count();
        playVideoDialog *playVideoFileDialog = new playVideoDialog(this);
        if(playVideoFileDialog->exec()==QDialog::Accepted)
        {
            // Save user-defined device number (currently fake) add in reverse counter here starting from 999
            int deviceNumber = 999;//cameraConnectDialog->getDeviceNumber();
            // Check if this camera is already connected
            if(!deviceNumberMap.contains(deviceNumber))
            {
                // Create ImageBuffer with user-defined size
                tbuffer<Mat> *imageBuffer = new tbuffer<Mat>(999);
                // Add created ImageBuffer to SharedImageBuffer object
                sharedImageBuffer->add(deviceNumber, imageBuffer, ui->actionSynchronizeStreams->isChecked());
                // Create CameraView
                cameraViewMap[deviceNumber] = new CameraView(ui->tabWidget, deviceNumber, sharedImageBuffer);

                // Check if stream synchronization is enabled
                if(ui->actionSynchronizeStreams->isChecked())
                {
                    // Prompt user
                    int ret = QMessageBox::question(this, tr("Vision"),
                                                    tr("Stream synchronization is enabled.\n\n"
                                                       "Do you want to start processing?\n\n"
                                                       "Choose 'No' if you would like to open additional streams."),
                                                    QMessageBox::Yes | QMessageBox::No,
                                                    QMessageBox::Yes);
                    // Start processing
                    if(ret==QMessageBox::Yes)
                        sharedImageBuffer->setSyncEnabled(true);
                    // Defer processing
                    else
                        sharedImageBuffer->setSyncEnabled(false);
                }

                // Attempt to connect to camera
                if(cameraViewMap[deviceNumber]->connectToCamera(false, 6, 6, true, 1920, 1080, playVideoFileDialog->getFileSelected()))
                {
                    // Add to map
                    deviceNumberMap[deviceNumber] = nextTabIndex;
                    // Save tab label
                    QString tabLabel = "video";
                    // Allow tabs to be closed
                    ui->tabWidget->setTabsClosable(true);
                    // If start tab, remove
                    if(nextTabIndex==0)
                        ui->tabWidget->removeTab(0);
                    // Add tab
                    ui->tabWidget->addTab(cameraViewMap[deviceNumber], tabLabel + " [" + QString::number(deviceNumber) + "]");
                    ui->tabWidget->setCurrentWidget(cameraViewMap[deviceNumber]);
                    // Set tooltips
                    setTabCloseToolTips(ui->tabWidget, "Disconnect Camera");
                    // Prevent user from enabling/disabling stream synchronization after a camera has been connected
                    ui->actionSynchronizeStreams->setEnabled(false);
                }

                // Could not connect to camera
                else
                {
                    // Display error message
                    QMessageBox::warning(this,"ERROR:","Could not connect to camera. Please check device number.");
                    // Explicitly delete widget
                    delete cameraViewMap[deviceNumber];
                    cameraViewMap.remove(deviceNumber);
                    // Remove from shared buffer
                    sharedImageBuffer->removeByDeviceNumber(deviceNumber);
                    // Explicitly delete ImageBuffer object
                    delete imageBuffer;
                }
            }
            // Display error message
            else
                QMessageBox::warning(this,"ERROR:","Could not connect to camera. Already connected.");
        }
        // Delete dialog
        delete playVideoFileDialog;
    }
}

void MainWindow::connectToCamera()
{
    // We cannot connect to a camera if devices are already connected and stream synchronization is in progress
    if(ui->actionSynchronizeStreams->isChecked() && deviceNumberMap.size()>0 && sharedImageBuffer->getSyncEnabled())
    {
        // Prompt user
        QMessageBox::warning(this, tr("qt-opencv-multithreaded"),
                                    tr("Stream synchronization is in progress.\n\n"
                                       "Please close all currently open streams before attempting to open a new stream."),
                                        QMessageBox::Ok);
    }
    // Attempt to connect to camera
    else
    {
        // Get next tab index
        int nextTabIndex = (deviceNumberMap.size()==0) ? 0 : ui->tabWidget->count();
        // Show dialog
        CameraConnectDialog *cameraConnectDialog = new CameraConnectDialog(this, ui->actionSynchronizeStreams->isChecked());
        if(cameraConnectDialog->exec()==QDialog::Accepted)
        {
            // Save user-defined device number
            int deviceNumber = cameraConnectDialog->getDeviceNumber();
            // Check if this camera is already connected
            if(!deviceNumberMap.contains(deviceNumber))
            {
                // Create ImageBuffer with user-defined size
                tbuffer<Mat> *imageBuffer = new tbuffer<Mat>(cameraConnectDialog->getImageBufferSize());
                // Add created ImageBuffer to SharedImageBuffer object
                sharedImageBuffer->add(deviceNumber, imageBuffer, ui->actionSynchronizeStreams->isChecked());
                // Create CameraView
                cameraViewMap[deviceNumber] = new CameraView(ui->tabWidget, deviceNumber, sharedImageBuffer);

                // Check if stream synchronization is enabled
                if(ui->actionSynchronizeStreams->isChecked())
                {
                    // Prompt user
                    int ret = QMessageBox::question(this, tr("Vision"),
                                                    tr("Stream synchronization is enabled.\n\n"
                                                       "Do you want to start processing?\n\n"
                                                       "Choose 'No' if you would like to open additional streams."),
                                                    QMessageBox::Yes | QMessageBox::No,
                                                    QMessageBox::Yes);
                    // Start processing
                    if(ret==QMessageBox::Yes)
                        sharedImageBuffer->setSyncEnabled(true);
                    // Defer processing
                    else
                        sharedImageBuffer->setSyncEnabled(false);
                }

                // Attempt to connect to camera
                if(cameraViewMap[deviceNumber]->connectToCamera(cameraConnectDialog->getDropFrameCheckBoxState(),
                                               cameraConnectDialog->getCaptureThreadPrio(),
                                               cameraConnectDialog->getProcessingThreadPrio(),
                                               cameraConnectDialog->getEnableFrameProcessingCheckBoxState(),
                                               cameraConnectDialog->getResolutionWidth(),
                                               cameraConnectDialog->getResolutionHeight()))
                {
                    // Add to map
                    deviceNumberMap[deviceNumber] = nextTabIndex;
                    // Save tab label
                    QString tabLabel = cameraConnectDialog->getTabLabel();
                    // Allow tabs to be closed
                    ui->tabWidget->setTabsClosable(true);
                    // If start tab, remove
                    if(nextTabIndex==0)
                        ui->tabWidget->removeTab(0);
                    // Add tab
                    ui->tabWidget->addTab(cameraViewMap[deviceNumber], tabLabel + " [" + QString::number(deviceNumber) + "]");
                    ui->tabWidget->setCurrentWidget(cameraViewMap[deviceNumber]);
                    // Set tooltips
                    setTabCloseToolTips(ui->tabWidget, "Disconnect Camera");
                    // Prevent user from enabling/disabling stream synchronization after a camera has been connected
                    ui->actionSynchronizeStreams->setEnabled(false);
                }
                // Could not connect to camera
                else
                {
                    // Display error message
                    QMessageBox::warning(this,"ERROR:","Could not connect to camera. Please check device number.");
                    // Explicitly delete widget
                    delete cameraViewMap[deviceNumber];
                    cameraViewMap.remove(deviceNumber);
                    // Remove from shared buffer
                    sharedImageBuffer->removeByDeviceNumber(deviceNumber);
                    // Explicitly delete ImageBuffer object
                    delete imageBuffer;
                }
            }
            // Display error message
            else
                QMessageBox::warning(this,"ERROR:","Could not connect to camera. Already connected.");
        }
        // Delete dialog
        delete cameraConnectDialog;
    }
}

void MainWindow::disconnectCamera(int index)
{
    // Local variable(s)
    bool doDisconnect=true;

    // Check if stream synchronization is enabled, more than 1 camera connected, and frame processing is not in progress
    if(ui->actionSynchronizeStreams->isChecked() && cameraViewMap.size()>1 && !sharedImageBuffer->getSyncEnabled())
    {
        // Prompt user
        int ret = QMessageBox::question(this, tr("qt-opencv-multithreaded"),
                                        tr("Stream synchronization is enabled.\n\n"
                                           "Disconnecting this camera will cause frame processing to begin on other streams.\n\n"
                                           "Do you wish to proceed?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::Yes);
        // Disconnect
        if(ret==QMessageBox::Yes)
            doDisconnect=true;
        // Do not disconnect
        else
            doDisconnect=false;
    }

    // Disconnect camera
    if(doDisconnect)
    {
        // Save number of tabs
        int nTabs = ui->tabWidget->count();
        // Close tab
        ui->tabWidget->removeTab(index);

        // Delete widget (CameraView) contained in tab
        delete cameraViewMap[deviceNumberMap.key(index)];
        cameraViewMap.remove(deviceNumberMap.key(index));

        // Remove from map
        removeFromMapByTabIndex(deviceNumberMap, index);
        // Update map (if tab closed is not last)
        if(index!=(nTabs-1))
            updateMapValues(deviceNumberMap, index);

        // If start tab, set tab as blank
        if(nTabs==1)
        {
            QLabel *newTab = new QLabel(ui->tabWidget);
            newTab->setText("No camera connected.");
            newTab->setAlignment(Qt::AlignCenter);
            ui->tabWidget->addTab(newTab, "");
            ui->tabWidget->setTabsClosable(false);
            ui->actionSynchronizeStreams->setEnabled(true);
        }
    }
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

void MainWindow::on_createSetButton_clicked()
{
    createSetDialog *setDialog = new createSetDialog(this);
    connect(setDialog, SIGNAL(sendString(QString)), this, SLOT(recieveSetText(QString)));
    setDialog->show();
}

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
    ui->viewComboBox->clear();
//    this->currentSet = arg1;
    this->setController->setSetName(arg1);
//    this->setController->setSetName(this->currentSet);

    ui->viewComboBox->addItems(this->setController->getSetClassDirectories());
    //removed because now the set files needs to be taken when the class view is changed
//    this->setController->getSetFiles(currentSet); //, currentView
}

void MainWindow::on_viewComboBox_currentIndexChanged(const QString &arg1)
{
//    this->currentView = arg1;
    this->setController->setViewName(arg1);
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

void MainWindow::on_positiveImageRadioButton_clicked()
{
//    this->setController->setImageStatus("positive");
}

void MainWindow::on_negativeImageRadioButton_clicked()
{
//    this->setController->setImageStatus("negative");
}

void MainWindow::on_saveImageButton_clicked()
{
    this->setController->saveImage();
    //bool returnValue = this->setController->getImageView()->imageBuffer.last().save(appSettingsController->getSetsPath() + "\\" + ui->setComboBox->currentText() + "\\" + ui->imageListWidget->currentItem()->text());
    //this->setController->currentItem()->setIcon(QIcon(this->appSettingsController->getSetsPath() + "\\" + ui->setComboBox->currentText() + "\\" + this->setController->currentItem()->text()));
}

void MainWindow::on_viewComboBox_activated(const QString &arg1)
{
//    this->currentView = arg1;
//    this->currentSet = ui->setComboBox->currentText();
//    setController->getSetFiles(); //currentSet, currentView
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
