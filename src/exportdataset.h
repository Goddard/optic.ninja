#ifndef EXPORTDATASET_H
#define EXPORTDATASET_H

#include <QObject>
#include <datalocal.h>
#include <QPolygon>

class ExportDataSet : public QObject
{
    Q_OBJECT
public:
    explicit ExportDataSet(QObject *parent = nullptr);
    bool setDatabase(DataLocal *db);
    void setBasePath(QString path);
    QString writeCSVData();

private:
    //might not need
    QString base_path;
    DataLocal *db;

signals:

public slots:
};

#endif // EXPORTDATASET_H
