#ifndef PARTED_WIDGET_H
#define PARTED_WIDGET_H

#include <parted/parted.h>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QMap>


class partition_item : public QWidget{
    Q_OBJECT
public:
    explicit partition_item(QWidget *parent = 0);
    ~partition_item();
private:
    QLabel      *Title;
};

typedef QMap<int,partition_item*> partitions_map;

class parted_widget : public QWidget{
    Q_OBJECT
public:
    explicit parted_widget(QWidget *parent = 0);
    ~parted_widget();
private:
    QVBoxLayout     *MainLayout;
    partition_item  *Item;
    partitions_map  *ItemsMap;
};

#endif // PARTED_WIDGET_H
