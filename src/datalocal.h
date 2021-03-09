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
#include <QJsonArray>

#include <QDebug>

#include <QColor>
#include <QDataStream>
#include <QRect>

#include <QDataStream>

struct Annotation
{
    //removed enum because it is annoying to serialize
    static const int draw_square = 0;
    static const int draw_circle = 1;
    static const int draw_line = 2;

    qint64 id = -1;
    QVariant shape;
    //represents the real annotation dimensions without zooming
    QVariant real;
    qint8 tool;
    bool selected = false;
    QString class_name = "None";
    QColor color = Qt::red;
};
Q_DECLARE_METATYPE(Annotation)


class DataLocal : public QObject
{
    Q_OBJECT
public:
  explicit DataLocal(QObject *parent = nullptr, QString set_name = nullptr, QString home_path = nullptr);
  ~DataLocal();
  bool open();
  bool destroy();
  bool exists();
  void close();

  QString getDBPath();
  QSqlError lastError();

  QStringList getPath(QString path);
  QStringList getClass(int class_id);
  QStringList getClass(QString class_name);
  QList<Annotation> getAnnotation(int object_id);

  QMap<QString, QString> getPaths();
  QMap<QString, QString> getClasses();
  QMap<QString, QString> getAnnotations();

  QSqlQueryModel* getPathsModel(QSqlQueryModel *object_path_model);
  QSqlQueryModel* getClassesModel(QSqlQueryModel *classes_model);
  QSqlQueryModel* getAnnotationsModel(QSqlQueryModel *annotations_model, int object_id);

  int insertObjectPath(QString object_path);
  int insertClass(QString class_name, QString class_color);
  int insertAnnotation(Annotation annotation, int object_id, int class_id = -1);

  int updateObjectPath(int object_id, QString object_path);
  int updateClass(int class_id, QString class_name, int class_color);
  int updateAnnotation(Annotation annotation, int annotation_id, int class_id);

  bool removeClass(QString class_name);
  bool removeAnnotation(int object_id, int annotation_id);

  friend QDataStream &operator<<(QDataStream &out, const Annotation &annotation);
  friend QDataStream &operator>>(QDataStream &in, Annotation &annotation);

private:
  QSqlDatabase db;
  bool dbExists;
  QString set_name;
  QString sets_path;
  void createSchema();
};

#endif // DATA_LOCAL_H
