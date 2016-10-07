#ifndef MESSAGEOBJECT_H
#define MESSAGEOBJECT_H

#include <QObject>

class MessageObject : public QObject
{
    Q_OBJECT
public:
    explicit MessageObject(QObject *parent = 0);

signals:

public slots:
};

#endif // MESSAGEOBJECT_H
