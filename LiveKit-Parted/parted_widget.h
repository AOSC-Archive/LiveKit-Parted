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
#include <QScrollArea>
#include <QDebug>

#define PARTED_WIDGET_WIDTH     500
#define PARTITION_HEIGTH        26
#define PARTITION_PATH_LENGTH   150
#define PARTITION_FS_NAME_LENGTH 100
#define PARTITION_SIZE_LENGTH   80
#define DISK_HEIGTH             20

class partition_item : public QWidget{
    Q_OBJECT
public:
    explicit    partition_item(QWidget *parent = 0,PedPartition *Part = NULL,PedDevice *Dev = NULL);
    ~partition_item();
    void        mousePressEvent(QMouseEvent *);
    void        unClicked(void);
signals:
    void        clicked(partition_item*);
private:
    QLabel      *Path;
    QLabel      *FsName;
    QLabel      *Size;
    QFont        Font;
    QHBoxLayout *MainLayout;
    PedPartition*Partition;
    PedDevice   *Device;
};

typedef QMap<int,partition_item*>   p_map_t;
class disk_item : public QWidget{
    Q_OBJECT
public:
    explicit    disk_item(QWidget *parent = 0, PedDisk *disk = NULL,PedDevice *dev = NULL);
    ~disk_item();
    void        InsertPartitions(partition_item* Item,int order = -1);
    void        mousePressEvent(QMouseEvent *);
    void        clearClickedStatus();
    int         getSize(void);
    bool        spreaded(void);
signals:
    void        ItemClicked(partition_item*);
    void        diskClicked(disk_item*,bool spreaded);
    void        partitionClicked(disk_item*,partition_item*);
public slots:
    void        onItemClicked(partition_item*);
private:
    QLabel          *Title;
    QFont            TitleFont;
    QVBoxLayout     *MainLayout;
    partition_item  *Item;
    p_map_t         *PartitionsMap;
    bool             shown;
    PedDisk         *Disk;
    PedDevice       *Device;
};
typedef QMap<int,disk_item*>        d_map_t;

class partition_select : public QWidget{
    Q_OBJECT
public:
    explicit    partition_select(QWidget *parent = 0);
    ~partition_select();
    void        insertDisk(disk_item*,int order = -1);
public slots:
    void        onDiskClicked(disk_item*,bool);
    void        onPartitionClicked(disk_item*, partition_item*);
    void        refreshSelect(void);
    void        clearSelect(void);
private:
    d_map_t         *DiskMap;
    QVBoxLayout     *MainLayout;
};

#endif // PARTED_WIDGET_H
