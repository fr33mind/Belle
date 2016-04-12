#ifndef OBJECTSVIEW_H
#define OBJECTSVIEW_H

#include "properties_widget.h"
#include "object.h"

class ObjectsView : public PropertiesWidget
{
    Q_OBJECT

public:
    explicit ObjectsView(QWidget *parent = 0);
    void addObjects(const QList<Object*>&);

protected:
    QStandardItem* itemFromObject(Object*);

signals:
    void objectSelected(Object*);

public slots:
    void addObject(Object*);
    void removeObject(QObject*);
    void removeObject(Object*);
    void itemClicked(const QModelIndex&);

private slots:
    void objectNameChanged(const QString&);

};

#endif // OBJECTSVIEW_H
