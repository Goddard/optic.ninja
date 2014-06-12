#ifndef CAMERACONNECTDIALOG_H
#define CAMERACONNECTDIALOG_H

// Qt
#include <QDialog>
// Local
#include "config.h"

namespace Ui {
class CameraConnectDialog;
}

class CameraConnectDialog : public QDialog
{
    Q_OBJECT
    
    public:
        explicit CameraConnectDialog(QWidget *parent=0, bool isStreamSyncEnabled=false);
        ~CameraConnectDialog();
        void setDeviceNumber();
        void setImageBufferSize();
        int getDeviceNumber();
        int getResolutionWidth();
        int getResolutionHeight();
        int getImageBufferSize();
        bool getDropFrameCheckBoxState();
        int getCaptureThreadPrio();
        int getProcessingThreadPrio();
        QString getTabLabel();
        bool getEnableFrameProcessingCheckBoxState();

    private:
        Ui::CameraConnectDialog *ui;

    public slots:
        void resetToDefaults();
};

#endif // CAMERACONNECTDIALOG_H
