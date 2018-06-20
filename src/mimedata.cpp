#include <QtGui>
#include "mimedata.h"

MimeData::MimeData()
    : QMimeData()
{
}

QStringList MimeData::formats() const
{
    return QMimeData::formats() << "image/png";
}

QVariant MimeData::retrieveData(const QString &mimeType, QVariant::Type type)
         const
{
    emit dataRequested(mimeType);
    qDebug() << "mime data signal";

    return QMimeData::retrieveData(mimeType, type);
}
