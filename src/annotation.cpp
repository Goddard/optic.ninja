#include "annotation.h"

//template<typename T>
//QByteArray serialize(const T& x)
//{
//    QByteArray ba;
//    QDataStream s(&ba, QIODevice::WriteOnly);
//    s << x;
//    return ba;
//}

//template<typename T>
//T deserialize(QByteArray& ba)
//{
//    QDataStream s(&ba, QIODevice::ReadOnly);
//    T res; s >> res;
//    return res;
//}

Annotation::Annotation(QObject *parent) : QObject(parent)
{

}

//QDataStream &operator<<(QDataStream &out, const Annotation &annotation)
//{
//    out << annotation.id;
//    out << annotation.shape;
//    out << annotation.real;
//    out << annotation.tool;
//    out << annotation.selected;
//    out << annotation.class_name;
//    out << annotation.color;
//    return out;
//}

//QDataStream &operator>>(QDataStream &in, Annotation &annotation)
//{
//    QVariant tempVar;
//    in >> annotation.id;
//    in >> annotation.shape;
//    in >> annotation.real;
//    in >> tempVar;
//    tempVar.setValue(annotation.tool);
//    in >> annotation.selected;
//    in >> annotation.class_name;
//    in >> annotation.color;

//    return in;
//}

//QDebug operator<<(QDebug dbg, Annotation *object)
//{
////    dbg << object->metaObject()->className() << object->foo();
//    return dbg;
//}

//Q_DECLARE_METATYPE(Annotation *)
