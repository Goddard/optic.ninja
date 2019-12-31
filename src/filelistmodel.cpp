#include "filelistmodel.h"

#include <QApplication>
#include <QBrush>
#include <QDir>
#include <QPalette>

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

//![4]
int FileListModel::rowCount(const QModelIndex & /* parent */) const
{
    return fileCount;
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= fileList.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        return fileList.at(index.row());
    } else if (role == Qt::BackgroundRole) {
        int batch = (index.row() / 100) % 2;
        if (batch == 0)
            return qApp->palette().base();
        else
            return qApp->palette().alternateBase();
    }
    return QVariant();
}
//![4]

//![1]
bool FileListModel::canFetchMore(const QModelIndex & /* index */) const
{
    if (fileCount < fileList.size())
        return true;
    else
        return false;
}
//![1]

//![2]
void FileListModel::fetchMore(const QModelIndex & /* index */)
{
    int remainder = fileList.size() - fileCount;
    int itemsToFetch = qMin(100, remainder);

    beginInsertRows(QModelIndex(), fileCount, fileCount+itemsToFetch-1);

    fileCount += itemsToFetch;

    endInsertRows();

    emit numberPopulated(itemsToFetch);

    if(fileCount > 300){
        beginRemoveRows(QModelIndex(), 0, 100);
        for (int row = 0; row < 100; ++row) {
            fileList.removeAt(row);
        }
        endRemoveRows();
    }
}
//![2]

//![0]
void FileListModel::setDirPath(const QString &path)
{
    QDir dir(path);

    beginResetModel();
    fileList = dir.entryList();
    fileCount = 0;
    endResetModel();
}
//![0]

