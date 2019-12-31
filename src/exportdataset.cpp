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

//open file function
//QDesktopServices::openUrl(QUrl("/home/realnc/test.pdf"));
//#ifdef Q_WS_WIN
//    if (QFileInfo(path).isDir())
//        QProcess::startDetached("explorer", QStringList(path));
//    else
//#endif
//        QDesktopServices::openUrl(QUrl(path));

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

                    if(tempAnnotation.tool == Annotation::draw_square)
                    {
                        QRect tempQRect = tempAnnotation.real.toRect();
                        stream << file_name << " "

                               << tempQRect.topLeft().x() << " "
                               << tempQRect.topLeft().y() << " "
                               << tempQRect.bottomRight().x() << " "
                               << tempQRect.bottomRight().y() << " "

                               << '"' << tempAnnotation.class_name << '"' << Annotation::draw_square << endl;
                    }

                    else if(tempAnnotation.tool == Annotation::draw_circle)
                    {
                        QRect tempQRect = tempAnnotation.real.toRect();
                        stream << file_name << " "

                               << tempQRect.topLeft().x() << " "
                               << tempQRect.topLeft().y() << " "
                               << tempQRect.bottomRight().x() << " "
                               << tempQRect.bottomRight().y() << " "

                               << '"' << tempAnnotation.class_name << '"' << Annotation::draw_circle << endl;
                    }

                    else if(tempAnnotation.tool == Annotation::draw_line) {
                        QPolygon tempPolygon = tempAnnotation.real.value<QPolygon>();
                        QString tempString = file_name;
                        tempString.append(" ");

                        for(int i = 0; i < tempPolygon.size()-1; i++)
                        {
                            qDebug() << "test1 x" << tempPolygon.toList();
                            tempString
                                    .append(QString::number(tempPolygon.point(i).x()))
                                    .append(" ")
                                    .append(QString::number(tempPolygon.point(i).y()))
                                    .append(" ");
                        }

                        tempString.append('"').append(tempAnnotation.class_name).append('"');
                        stream << tempString << Annotation::draw_line << endl;
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
