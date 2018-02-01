#ifndef DATA_LOCAL_H
#define DATA_LOCAL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include <QFile>
#include <QDir>
#include <QSqlRelationalTableModel>
#include <QDateTime>

#include <QDebug>

class DataLocal : public QObject
{
    Q_OBJECT

public:
  explicit DataLocal(QObject *parent = 0, QString set_name = NULL, QString home_path = NULL);
  ~DataLocal();
  bool open();
  bool destroy();
  bool exists();
  void close();

  QString getPath();
  QSqlError lastError();

  QMap<QString, QString> getClasses();

  QSqlQueryModel* getPathsModel(QSqlQueryModel *object_path_model);
  QSqlQueryModel* getClassesModel(QSqlQueryModel *classes_model);
  QSqlQueryModel* getAnnotationsModel(QSqlQueryModel *annotations_model, int object_id);

  int insertObjectPath(QString object_path);
  int insertClass(QString class_name, QString class_color);
  int insertAnnotation(QString annotation, int object_id, int class_id);

  int updateObjectPath(int object_id, QString object_path);
  int updateClass(int class_id, QString class_name, int class_color);
  int updateAnnotation(int annotation_id, int class_id, QString annotation);

private:
  QSqlDatabase db;
  bool dbExists;
  QString set_name;
  QString sets_path;
  void createSchema();

};

#endif // DATA_LOCAL_H
