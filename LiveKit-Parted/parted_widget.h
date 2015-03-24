#ifndef PARTED_WIDGET_H
#define PARTED_WIDGET_H

#include <parted/parted.h>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel>
#include <QWidget>
#include <QMap>

#define PARTED_WIDGET_WIDTH     300
#define PARTITION_HEIGTH        50


class partition_item : public QWidget{
    Q_OBJECT
public:
    explicit partition_item(QWidget *parent = 0);
    ~partition_item();
    void mouseMoveEvent(QMouseEvent *Event);
    void mousePressEvent(QMouseEvent *);
private:
    QLabel      *Title;
};

typedef QMap<int,partition_item*> p_map_t;

class parted_widget : public QWidget{
    Q_OBJECT
public:
    explicit    parted_widget(QWidget *parent = 0);
    ~parted_widget();
    void        InsertPartitions(partition_item* Item,int order = -1);
private:
    QVBoxLayout     *MainLayout;
    partition_item  *Item;
    p_map_t         *PartitionsMap;
};

#endif // PARTED_WIDGET_H
