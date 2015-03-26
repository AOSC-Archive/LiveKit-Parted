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
#include <QFont>

#define PARTED_WIDGET_WIDTH     300
#define PARTITION_HEIGTH        28


class partition_item : public QWidget{
    Q_OBJECT
public:
    explicit    partition_item(QWidget *parent = 0);
    ~partition_item();
    void        mousePressEvent(QMouseEvent *);
    void        unClicked(void);
signals:
    void        clicked(partition_item*);
private:
    QLabel      *Title;
    QFont        TitleFont;
    QHBoxLayout *MainLayout;
};

typedef QMap<int,partition_item*> p_map_t;

class parted_widget : public QWidget{
    Q_OBJECT
public:
    explicit    parted_widget(QWidget *parent = 0);
    ~parted_widget();
    void        InsertPartitions(partition_item* Item,int order = -1);
    void        mousePressEvent(QMouseEvent *);
public slots:
    void    onItemClicked(partition_item*);
private:
    QLabel          *Title;
    QFont            TitleFont;
    QVBoxLayout     *MainLayout;
    partition_item  *Item;
    p_map_t         *PartitionsMap;
    bool            shown;
};

#endif // PARTED_WIDGET_H
