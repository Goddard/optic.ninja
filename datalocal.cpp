#include "datalocal.h"

DataLocal::DataLocal(QObject *parent, QString set_name, QString home_path) : QObject(parent)
{
    this->set_name = set_name;
    this->sets_path = home_path;

    if(this->open()) {
        qDebug("Database opened successfully");
    }

    else
    {
        qDebug("Database opening failed");
    }

    this->createSchema();
}

DataLocal::~DataLocal()
{
    db.close();
    qDebug("Database closed");
}

void DataLocal::close()
{
    db.close();
}

QString DataLocal::getDBPath()
{
    //    QString path(QDir::home().path());
    QString path(this->sets_path);
    path.append(QDir::separator()).append(this->set_name);
    path.append(QDir::separator()).append(this->set_name).append(".db.sqlite");
    path = QDir::toNativeSeparators(path);

    qDebug() << path;

    return path;
}

bool DataLocal::exists()
{
    if(QFile::exists(this->getDBPath()))
    {
        return true;
    }

    else
    {
        return false;
    }
}

bool DataLocal::open()
{
    this->dbExists = false;
    if(exists())
        this->dbExists = true;

    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName(this->getDBPath());

    // Open databasee
    return db.open();
}

QSqlError DataLocal::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

bool DataLocal::destroy()
{
    // Close database
    db.close();

    return QFile::remove(this->getDBPath());
}

void DataLocal::createSchema()
{
    if(!this->dbExists)
    {
        QSqlQuery query;
        query.exec("create table object_paths(id INTEGER PRIMARY KEY, object_path TEXT, modified DATETIME)");

        query.exec("create table classes(id INTEGER PRIMARY KEY, class_name VARCHAR(60) UNIQUE, class_color VARCHAR(7), modified DATETIME)");

        query.exec("create table annotations(id INTEGER PRIMARY KEY, object_id INTEGER, class_id INTEGER, annotation BLOB, modified DATETIME)");

        qDebug("Database schema created");
    }

    else
    {
        qDebug("Database schema already exists");
    }
}

QStringList DataLocal::getPath(QString path)
{
    QStringList path_data;
    QSqlQuery path_query;

    path_query.prepare("SELECT * FROM object_paths WHERE object_path = :object_path");
    path_query.bindValue(":object_path", path);

    bool exec = path_query.exec();
    if(!exec)
    {
        qDebug() << "GET PATH FAILED";
    }

    else
    {
        qDebug() << "GET PATH SUCCESS";

        QSqlRecord record = path_query.record();
        int id = record.indexOf("id");
        int object_path = record.indexOf("object_path");
        int modified = record.indexOf("modified");

        while(path_query.next()) {
            QString id_value = path_query.value(id).toString();
            QString object_path_value = path_query.value(object_path).toString();
            QString modified_value = path_query.value(modified).toString();

            path_data.append(id_value);
            path_data.append(object_path_value);
            path_data.append(modified_value);
        }
    }

    return path_data;
}

QStringList DataLocal::getClass(int class_id)
{
    QStringList class_data;
    QSqlQuery class_query;

    class_query.prepare("SELECT * FROM classes WHERE id = :class_id");
    class_query.bindValue(":class_id", class_id);

    bool exec = class_query.exec();
    if(!exec)
    {
        qDebug() << "GET CLASSES FAILED";
    }

    else
    {
        qDebug() << "GET CLASSES SUCCESS";

        QSqlRecord record = class_query.record();
        int id = record.indexOf("id");
        int class_name = record.indexOf("class_name");
        int class_color = record.indexOf("class_color");
        int modified = record.indexOf("modified");

        while(class_query.next()) {
            QString id_value = class_query.value(id).toString();
            QString class_name_value = class_query.value(class_name).toString();
            QString class_color_value = class_query.value(class_color).toString();
            QString modified_value = class_query.value(modified).toString();

            class_data.append(id_value);
            class_data.append(class_name_value);
            class_data.append(class_color_value);
            class_data.append(modified_value);
        }
    }

    return class_data;
}

QStringList DataLocal::getClass(QString class_name)
{
    QStringList class_data;
    QSqlQuery class_query;

    class_query.prepare("SELECT * FROM classes WHERE class_name = :class_name");
    class_query.bindValue(":class_name", class_name);

    bool exec = class_query.exec();
    if(!exec)
    {
        qDebug() << "GET CLASSES FAILED";
    }

    else
    {
        qDebug() << "GET CLASSES SUCCESS";

        QSqlRecord record = class_query.record();
        int id = record.indexOf("id");
        int class_name = record.indexOf("class_name");
        int class_color = record.indexOf("class_color");
        int modified = record.indexOf("modified");

        while(class_query.next()) {
            QString id_value = class_query.value(id).toString();
            QString class_name_value = class_query.value(class_name).toString();
            QString class_color_value = class_query.value(class_color).toString();
            QString modified_value = class_query.value(modified).toString();

            class_data.append(id_value);
            class_data.append(class_name_value);
            class_data.append(class_color_value);
            class_data.append(modified_value);
        }
    }

    return class_data;
}

QDataStream &operator<<(QDataStream &out, const Annotation &annotation)
{
  out << annotation.id;
  out << annotation.shape;
  out << annotation.real;
  out << annotation.tool;
  out << annotation.selected;
  out << annotation.class_name;
  out << annotation.color;
  return out;
}

QDataStream &operator>>(QDataStream &in, Annotation &annotation)
{
    in >> annotation.id;
    in >> annotation.shape;
    in >> annotation.real;
    in >> annotation.tool;
    in >> annotation.selected;
    in >> annotation.class_name;
    in >> annotation.color;

  return in;
}

//int DataLocal::getClassId(QString class_name)
//{
//    int id = -1;
//    QSqlQuery class_query;

//    class_query.prepare("SELECT id FROM classes WHERE class_name = :class_name");
//    class_query.bindValue(":class_name", class_name);

//    bool exec = class_query.exec();
//    if(!exec)
//    {
//        qDebug() << "GET CLASS ID FAILED";
//    }

//    else
//    {
//        qDebug() << "GET CLASS ID SUCCESS";

//        QSqlRecord record = class_query.record();
//        int class_id = record.indexOf("id");

//        int id = class_query.value(class_id).toString();
//    }

//    return id;
//}


QList<Annotation> DataLocal::getAnnotation(int object_id)
{
    QList<Annotation> annotation_data;

    QSqlQuery annotation_query;
    annotation_query.prepare("SELECT * FROM annotations WHERE object_id = :object_id");
    annotation_query.bindValue(":object_id", object_id);

    bool exec = annotation_query.exec();
    if(!exec)
    {
        qDebug() << "GET ANNOTATION FAILED";
    }

    else
    {
//        qDebug() << "GET ANNOTATION SUCCESS";

        QSqlRecord record = annotation_query.record();
        int id = record.indexOf("id");
        int object_id = record.indexOf("object_id");
        int class_id = record.indexOf("class_id");
        int annotation = record.indexOf("annotation");
        int modified = record.indexOf("modified");

        while(annotation_query.next())
        {
            QString id_value = annotation_query.value(id).toString();
            QString object_id_value = annotation_query.value(object_id).toString();
            QString class_id_value = annotation_query.value(class_id).toString();
            QByteArray annotation_value = annotation_query.value(annotation).toByteArray();
            QString modified_value = annotation_query.value(modified).toString();

            QDataStream in(&annotation_value, QIODevice::ReadOnly);
            Annotation annotation_deserialized;
            in >> annotation_deserialized;

            qDebug() << "ID VALUE FOR DB " << id_value;
            qDebug() << "OBJECT ID VALUE FOR DB " << object_id_value;
            qDebug() << "CLASS ID VALUE FOR DB " << class_id_value;
            qDebug() << "CLASS NAME IN GET ANNOTATION " << annotation_deserialized.class_name;
            qDebug() << "TOOL ID IN GET ANNOTATION " << annotation_deserialized.tool;
            qDebug() << "ID IN GET ANNOTATION " << annotation_deserialized.id;

//            annotation_data.append(id_value);
//            annotation_data.append(object_id_value);
//            annotation_data.append(class_id_value);
            annotation_data.append(annotation_deserialized);
//            annotation_data.append(modified_value);
        }
        return annotation_data;
    }
    return {};
}

QMap<QString, QString> DataLocal::getPaths()
{
    QMap<QString, QString> path_data;
    QSqlQuery paths_query;

    bool exec = paths_query.exec("SELECT * FROM object_paths");
    if(!exec)
    {
        qDebug() << "GET OBJECT PATHS FAILED";
    }

    else
    {
        qDebug() << "GET OBJECT PATHS SUCCESS";

        QSqlRecord record = paths_query.record();
        int id = record.indexOf("id");
        int object_path = record.indexOf("object_path");

        while(paths_query.next()) {
            QString id_value = paths_query.value(id).toString();
            QString object_path_value = paths_query.value(object_path).toString();

            path_data.insert(id_value, object_path_value);
        }
    }

    return path_data;
}

QMap<QString, QString> DataLocal::getClasses()
{
    QMap<QString, QString> class_data;
    QSqlQuery class_query;

    bool exec = class_query.exec("SELECT * FROM classes");
    if(!exec)
    {
        qDebug() << "GET CLASSES FAILED";
    }

    else
    {
        qDebug() << "GET CLASSES SUCCESS";

        QSqlRecord record = class_query.record();
        int class_name = record.indexOf("class_name");
        int class_color = record.indexOf("class_color");

        while(class_query.next()) {
            QString key = class_query.value(class_name).toString();
            QString value = class_query.value(class_color).toString();

            class_data.insert(key, value);
        }
    }

    return class_data;
}

QMap<QString, QString> DataLocal::getAnnotations()
{
    QMap<QString, QString> annotation_data;
    QSqlQuery annotation_query;

    bool exec = annotation_query.exec("SELECT * FROM annotations");
    if(!exec)
    {
        qDebug() << "GET ANNOTATIONS FAILED";
    }

    else
    {
        qDebug() << "GET ANNOTATIONS SUCCESS";

        QSqlRecord record = annotation_query.record();
//        QJsonArray class_name = record.indexOf("class_name");
        int class_color = record.indexOf("class_color");

        while(annotation_query.next()) {
//            QString key = annotation_query.value(class_name).toString();
            QString value = annotation_query.value(class_color).toString();

//            annotation_data.insert(key, value);
        }
    }

    return annotation_data;
}

//QSqlQuery* DataLocal::getSetsQuery()
//{
//    QSqlQuery *set_query = new QSqlQuery("SELECT * FROM sets");
//    set_query->exec();
//    return set_query;
////    while (query.next()) {
////        QSqlRecord record = query.record();
////        qDebug() << "Date : " << record.value(date).toString();
////    }
//}

QSqlQueryModel* DataLocal::getPathsModel(QSqlQueryModel *object_path_model)
{
    object_path_model->setQuery("SELECT * FROM object_paths");
    object_path_model->query();
    return object_path_model;
}

QSqlQueryModel* DataLocal::getClassesModel(QSqlQueryModel *classes_model)
{
    classes_model->setQuery("SELECT * FROM classes");
    classes_model->query();
    return classes_model;
}

QSqlQueryModel* DataLocal::getAnnotationsModel(QSqlQueryModel *annotations_model, int object_id)
{
    annotations_model->setQuery("SELECT * FROM annotations WHERE object_id = " + QString::number(object_id));
    annotations_model->query();
    return annotations_model;
}

int DataLocal::insertObjectPath(QString object_path)
{
    QSqlQuery add_object_path;
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    add_object_path.prepare("INSERT INTO object_paths (object_path, modified) VALUES (:object_path, :modified)");
    add_object_path.bindValue(":object_path", object_path);
    add_object_path.bindValue(":modified", date_time);

    if(add_object_path.exec())
    {
        qDebug("Object Path Added Successfully");
        if(add_object_path.lastInsertId().type() != QMetaType::QVariant)
            return add_object_path.lastInsertId().toInt();
        else
            return 0;
    }

    else
    {
        return 0;
    }
}

int DataLocal::insertClass(QString class_name, QString class_color)
{
    QSqlQuery add_class;
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    add_class.prepare("INSERT INTO classes (class_name, class_color, modified) VALUES (:class_name, :class_color, :modified)");
    add_class.bindValue(":class_name", class_name);
    add_class.bindValue(":class_color", class_color);
    add_class.bindValue(":modified", date_time);

    if(add_class.exec())
    {
        qDebug("Object Path Added Successfully");
        if(add_class.lastInsertId().type() != QMetaType::QVariant)
            return add_class.lastInsertId().toInt();
        else
            return 0;
    }

    else
    {
        return 0;
    }
}

int DataLocal::insertAnnotation(Annotation annotation, int object_id, int class_id)
{
    QByteArray annotation_buffer;
    QDataStream annotation_out(&annotation_buffer, QIODevice::WriteOnly);
    annotation_out << annotation;

    QSqlQuery add_annotation;
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    add_annotation.prepare("INSERT INTO annotations (annotation, object_id, class_id, modified) VALUES (:annotation, :object_id, :class_id, :modified)");
    add_annotation.bindValue(":annotation", annotation_buffer);
    add_annotation.bindValue(":object_id", object_id);
    add_annotation.bindValue(":class_id", class_id);
    add_annotation.bindValue(":modified", date_time);

    if(add_annotation.exec())
    {
        qDebug("Object Path Added Successfully");
        if(add_annotation.lastInsertId().type() != QMetaType::QVariant)
            return add_annotation.lastInsertId().toInt();
        else
            return 0;
    }

    else
    {
        return 0;
    }
}

int DataLocal::updateObjectPath(int object_id, QString object_path)
{
    QSqlQuery update_object_path;
    QString modified = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    update_object_path.prepare("UPDATE object_paths SET object_path = :object_path, modified = :modified WHERE id = :object_id");
    update_object_path.bindValue(":object_id", object_id);
    update_object_path.bindValue(":object_path", object_path);
    update_object_path.bindValue(":modified", modified);

    if(update_object_path.exec())
    {
        qDebug("Object Path Updated Successfully : %s", qPrintable(modified));
    }

    else
    {
        qDebug(qPrintable(update_object_path.lastError().text()));
    }
}

int DataLocal::updateClass(int class_id, QString class_name, int class_color)
{
    QSqlQuery update_class;
    QString modified = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    update_class.prepare("UPDATE classes SET class_name = :class_name, class_color = :class_color, modified = :modified WHERE id = :class_id");
    update_class.bindValue(":class_id", class_id);
    update_class.bindValue(":class_name", class_name);
    update_class.bindValue(":class_color", class_color);
    update_class.bindValue(":modified", modified);

    if(update_class.exec())
    {
        qDebug("Class Updated Successfully : %s", qPrintable(modified));
    }

    else
    {
        qDebug(qPrintable(update_class.lastError().text()));
    }
}

int DataLocal::updateAnnotation(Annotation annotation, int annotation_id, int class_id)
{
    QByteArray annotation_buffer;
    QDataStream annotation_out(&annotation_buffer, QIODevice::WriteOnly);
    annotation_out << annotation;

    QSqlQuery update_annotation;
    QString modified = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    update_annotation.prepare("UPDATE annotations SET annotation = :annotation, class_id = :class_id, modified = :modified WHERE id = :annotation_id");
    update_annotation.bindValue(":annotation_id", annotation_id);
    update_annotation.bindValue(":class_id", class_id);
    update_annotation.bindValue(":annotation", annotation_buffer);
    update_annotation.bindValue(":modified", modified);

    if(update_annotation.exec())
    {
        qDebug("Annotation Updated Successfully : %s", qPrintable(modified));
    }

    else
    {
        qDebug(qPrintable(update_annotation.lastError().text()));
    }
}

bool DataLocal::removeClass(QString class_name)
{
    QSqlQuery remove_class;
    remove_class.prepare("DELETE FROM classes WHERE class_name = :class_name");
    remove_class.bindValue(":class_name", class_name);

    if(remove_class.exec())
    {
        qDebug("Class Removed Successfully");
        return true;
    }

    else
    {
        qDebug(qPrintable(remove_class.lastError().text()));
    }

    return false;
}

bool DataLocal::removeAnnotation(int object_id, int annotation_id)
{
    qDebug () << "OBJECT ID FOR DELETE " << object_id;
    qDebug() << "ANNOTATION ID FOR DELETE " << annotation_id;
    QSqlQuery remove_annotation;
    remove_annotation.prepare("DELETE FROM annotations WHERE object_id = :object_id AND id = :annotation_id");
    remove_annotation.bindValue(":object_id", object_id);
    remove_annotation.bindValue(":annotation_id", annotation_id);

    if(remove_annotation.exec())
    {
        qDebug("Annotation Removed Successfully");
        return true;
    }

    else
    {
        qDebug(qPrintable(remove_annotation.lastError().text()));
    }

    return false;
}
