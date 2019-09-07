#ifndef IMAGEPROCESSINGSETTINGSDIALOG_H
#define IMAGEPROCESSINGSETTINGSDIALOG_H

// Qt
#include <QDialog>
#include <QAbstractButton>
#include <QRegExp>
#include <QRegExpValidator>

// Local
#include "structures.h"
#include "config.h"

namespace Ui {
class ImageProcessingSettingsDialog;
}

class ImageProcessingSettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit ImageProcessingSettingsDialog(QWidget *parent = 0);
        ~ImageProcessingSettingsDialog();
        void updateDialogSettingsFromStored();

    private:
        Ui::ImageProcessingSettingsDialog *ui;
        ImageProcessingSettings imageProcessingSettings;

    public slots:
        void resetAllDialogToDefaults();
        void updateStoredSettingsFromDialog();

    private slots:
        void resetSmoothDialogToDefaults();
        void resetDilateDialogToDefaults();
        void resetErodeDialogToDefaults();
        void resetFlipDialogToDefaults();
        void resetCannyDialogToDefaults();
        void validateDialog();
        void smoothTypeChange(QAbstractButton *);

    signals:
        void newImageProcessingSettings(struct ImageProcessingSettings p_settings);
};

#endif // IMAGEPROCESSINGSETTINGSDIALOG_H
