#ifndef MIMEDATA_H
#define MIMEDATA_H

#include <QMimeData>

class MimeData : public QMimeData
{
    Q_OBJECT

public:
    MimeData();
    QStringList formats() const;

signals:
    void dataRequested(const QString &mimeType) const;

protected:
    QVariant retrieveData(const QString &mimetype, QVariant::Type type) const;
};

#endif // MIMEDATA_H
