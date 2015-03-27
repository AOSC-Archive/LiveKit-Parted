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
#include <QStyle>
#include <QPushButton>

#define PARTED_WIDGET_WIDTH             500
#define PARTITION_HEIGTH                26
#define PARTITION_PATH_LENGTH           150
#define PARTITION_FS_NAME_LENGTH        100
#define PARTITION_SIZE_LENGTH           80
#define PARTITION_MOUNT_POINT_LENGTH    120
#define DISK_HEIGTH                     20
#define PARTITION_SPACING               40
#define DISK_SPACING                    25

class partition_item : public QWidget{
    Q_OBJECT
public:
    explicit        partition_item(QWidget *parent = 0,PedPartition *Part = NULL,PedDevice *Dev = NULL);
                    ~partition_item();
    void            mousePressEvent(QMouseEvent *);
    void            unClicked(void);
    void            set_partition(PedPartition *Part = NULL, PedDevice *Dev = NULL);
    PedPartition*   getPartition(void);
    PedDevice   *   getDevice(void);
signals:
    void            clicked(partition_item*);
private:
    QLabel      *Path;
    QLabel      *FsName;
    QLabel      *Size;
    QLabel      *MountPoint;
    QFont        Font;
    PedPartition*Partition;
    PedDevice   *Device;
};

typedef QMap<int,partition_item*>   p_map_t;
class disk_item : public QWidget{
    Q_OBJECT
public:
    explicit        disk_item(QWidget *parent = 0, PedDisk *disk = NULL,PedDevice *dev = NULL);
                    ~disk_item();
    void            set_disk(PedDisk *disk = NULL, PedDevice *dev = NULL);
    void            InsertPartitions(partition_item* Item,int order = -1);
    void            mousePressEvent(QMouseEvent *);
    void            clearClickedStatus();
    int             getSize(void);
    bool            spreaded(void);
    PedDisk  *      getDisk(void);
    PedDevice*      getDevice(void);
signals:
    void            ItemClicked(partition_item*);
    void            diskClicked(disk_item*,bool spreaded);
    void            partitionClicked(disk_item*,partition_item*);
public slots:
    void            onItemClicked(partition_item*);
private:
    QLabel          *Title;
    QFont            TitleFont;
    p_map_t         *PartitionsMap;
    bool             shown;
    PedDisk         *Disk;
    PedDevice       *Device;
};
typedef QMap<int,disk_item*>        d_map_t;

class partition_select : public QWidget{
    Q_OBJECT
public:
    explicit        partition_select(QWidget *parent = 0);
                    ~partition_select();
    void            insertDisk(disk_item*,int order = -1);
signals:
    void            diskClicked(disk_item*);
    void            partitionClicked(disk_item*, partition_item*);
public slots:
    void            onDiskClicked(disk_item*,bool);
    void            onPartitionClicked(disk_item*, partition_item*);
    void            refreshSelect(void);
    void            clearSelect(void);
private:
    d_map_t         *DiskMap;
};


class partition_controllor : public QWidget{
    Q_OBJECT
public:
    explicit        partition_controllor(QWidget *parent = 0);
                    ~partition_controllor();
public slots:
    void            onDiskClicked(disk_item*);
    void            onPartitionClicked(disk_item*, partition_item*);
private:
    QScrollArea     *MainArea;
    partition_select*Select;
    QPushButton     *AddButton;
    QPushButton     *DelButton;
    QPushButton     *ChangeButton;
    QFont           DefaultFont;
    PedPartition    *currenlytSelected;
};

#endif // PARTED_WIDGET_H
