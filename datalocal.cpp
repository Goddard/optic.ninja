#include "datalocal.h"

DataLocal::DataLocal(QObject *parent, QString set_name, QString home_path) : QObject(parent)
{
    this->set_name = set_name;
    this->home_path = home_path;

    if(this->open())
        qDebug("Database opened successfully");
    else
        qDebug("Database opening was unsucessful");

    this->createSchema();
}

DataLocal::~DataLocal()
{
    db.close();
    qDebug("Database closed");

//    delete this->set_name;
//    delete this->home_path;
}

QString DataLocal::getPath()
{
    // NOTE: We have to store database file into user home folder in Linux
//    QString path(QDir::home().path());
    QString path(this->home_path);
    path.append(QDir::separator()).append("optic.ninja");
    path.append(QDir::separator()).append(this->set_name);
    path.append(QDir::separator()).append(this->set_name).append(".db.sqlite");
    path = QDir::toNativeSeparators(path);
    return path;
}

bool DataLocal::exists()
{
    if(QFile::exists(this->getPath()))
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

    db.setDatabaseName(this->getPath());

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

    return QFile::remove(this->getPath());
}

void DataLocal::createSchema()
{
    if(!this->dbExists)
    {
        QSqlQuery query;
        query.exec("create table sets(id INTEGER PRIMARY KEY, set_name VARCHAR(60) UNIQUE, modified DATETIME)");

        query.exec("create table object_paths(id INTEGER PRIMARY KEY, set_id INTEGER, object_path TEXT, modified DATETIME)");

        query.exec("create table classes(id INTEGER PRIMARY KEY, set_id INTEGER, class_name VARCHAR(60) UNIQUE, class_color VARCHAR(6), modified DATETIME)");

        query.exec("create table annotations(id INTEGER PRIMARY KEY, object_id INTEGER, class_id INTEGER, annotation TEXT, modified DATETIME)");

        qDebug("Database schema created");
    }

    else
    {
        qDebug("Database schema already exists");
    }
}

QSqlTableModel* DataLocal::getSetsModel(QSqlTableModel *client_model)
{
    client_model->setTable("sets");
    client_model->select();
    return client_model;
}

QSqlQueryModel* DataLocal::getPathsModel(QSqlQueryModel *object_path_model, int set_id)
{
    object_path_model->setQuery("SELECT * FROM object_paths WHERE set_id = " + QString::number(set_id));
    object_path_model->query();
    return object_path_model;
}

QSqlQueryModel* DataLocal::getClassesModel(QSqlQueryModel *classes_model, int set_id)
{
    classes_model->setQuery("SELECT * FROM classes WHERE set_id = " + QString::number(set_id));
    classes_model->query();
    return classes_model;
}

QSqlQueryModel* DataLocal::getAnnotationsModel(QSqlQueryModel *annotations_model, int object_id)
{
    annotations_model->setQuery("SELECT * FROM annotations WHERE object_id = " + QString::number(object_id));
    annotations_model->query();
    return annotations_model;
}

int DataLocal::insertSet(QString set_name)
{
    QSqlQuery add_set;
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    add_set.prepare("INSERT INTO sets (set_name, modified) VALUES (:set_name, :modified)");
    add_set.bindValue(":set_name", set_name);
    add_set.bindValue(":modified", date_time);

    if(add_set.exec())
    {
        qDebug("Object Path Added Successfully");
        if(add_set.lastInsertId().type() != QMetaType::QVariant)
            return add_set.lastInsertId().toInt();
        else
            return 0;
    }

    else
    {
        return 0;
    }
}

int DataLocal::insertObjectPath(QString object_path, int set_id)
{
    QSqlQuery add_object_path;
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    add_object_path.prepare("INSERT INTO object_paths (object_path, set_id, modified) VALUES (:object_path, :set_id, :modified)");
    add_object_path.bindValue(":object_path", object_path);
    add_object_path.bindValue(":set_id", set_id);
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

int DataLocal::insertClass(QString class_name, int set_id)
{
    QSqlQuery add_class;
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    add_class.prepare("INSERT INTO object_paths (object_path, set_id, modified) VALUES (:class_name, :set_id, :modified)");
    add_class.bindValue(":object_path", class_name);
    add_class.bindValue(":set_id", set_id);
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

int DataLocal::insertAnnotation(QString annotation, int object_id, int class_id)
{
    QSqlQuery add_annotation;
    QString date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    add_annotation.prepare("INSERT INTO annotations (annotation, object_id, class_id, modified) VALUES (:class_name, :object_id, :class_id, :modified)");
    add_annotation.bindValue(":annotation", annotation);
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

int DataLocal::updateSet(int set_id, QString set_name)
{
    QSqlQuery update_set;
    QString modified = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    update_set.prepare("UPDATE sets SET set_name = :set_name, modified = :modified WHERE id = :set_id");
    update_set.bindValue(":set_id", set_id);
    update_set.bindValue(":set_name", set_name);
    update_set.bindValue(":modified", modified);

    if(update_set.exec())
    {
        qDebug("Set Updated Successfully : %s", qPrintable(modified));
    }

    else
    {
        qDebug(qPrintable(update_set.lastError().text()));
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

int DataLocal::updateClass(int class_id, QString class_name)
{
    QSqlQuery update_class;
    QString modified = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    update_class.prepare("UPDATE classes SET class_name = :class_name, modified = :modified WHERE id = :class_id");
    update_class.bindValue(":class_id", class_id);
    update_class.bindValue(":class_name", class_name);
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

int DataLocal::updateAnnotation(int annotation_id, int class_id, QString annotation)
{
    QSqlQuery update_class;
    QString modified = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    update_class.prepare("UPDATE classes SET annotation = :annotation, class_id = :class_id, modified = :modified WHERE id = :annotation_id");
    update_class.bindValue(":annotation_id", annotation_id);
    update_class.bindValue(":class_id", class_id);
    update_class.bindValue(":annotation", annotation);
    update_class.bindValue(":modified", modified);

    if(update_class.exec())
    {
        qDebug("Annotation Updated Successfully : %s", qPrintable(modified));
    }

    else
    {
        qDebug(qPrintable(update_class.lastError().text()));
    }
}
