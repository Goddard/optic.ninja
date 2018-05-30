#include "exportdataset.h"

ExportDataSet::ExportDataSet(QObject *parent) : QObject(parent)
{

}

bool ExportDataSet::setDatabase(DataLocal *db) {
    this->db = db;

    return this->db->exists();
}

void ExportDataSet::setBasePath(QString path) {
    this->base_path = path;
}

QString ExportDataSet::writeCSVData() {
    QString path = this->base_path;
    path.append(QDir::separator()).append("labels.csv");
    path = QDir::toNativeSeparators(path);

    QFile file(path);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);

        QMap<QString, QString> paths = this->db->getPaths();
        for(QMap<QString, QString>::const_iterator iter = paths.begin(); iter != paths.end(); ++iter)
        {
            QString file_name = QString("images/").append(QFileInfo(iter.value()).fileName());

            QVariant tempVariant = iter.key();
            QList<Annotation> annotations = this->db->getAnnotation(tempVariant.toInt());
            if(annotations.size() > 0)
            {
                for(QList<Annotation>::iterator iter2 = annotations.begin(); iter2 != annotations.end(); ++iter2)
                {
                    Annotation &tempAnnotation = *iter2;

                    if(tempAnnotation.shape.type() == QMetaType::QRect)
                    {
                        QRect tempQRect = tempAnnotation.real.toRect();
                        stream << file_name << " " << tempQRect.topLeft().x() << " " << tempQRect.topLeft().y() << " " << tempQRect.bottomRight().x() << " " << tempQRect.bottomRight().y() << " " << '"' << tempAnnotation.class_name << '"' << endl;
                    }
                }
            }

            else
            {
                QString class_name = "none";
                //we will just put zeros for size since no annotation exists.
                stream << file_name << " " << 0 << " " << 0 << " " << 0 << " " << 0 << " " << '"' << class_name << '"' << endl;
            }

            file_name = "";
        }
        return path;
    }

    return QString("fail");
}
