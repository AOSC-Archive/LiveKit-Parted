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
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QMessageBox>
#include <stdint.h>

#define     PARTED_WIDGET_WIDTH             550
#define     PARTED_WIDGET_HEIGTH            200
#define     PARTITION_HEIGTH                20
#define     PARTITION_PATH_LENGTH           150
#define     PARTITION_FS_NAME_LENGTH        100
#define     PARTITION_SIZE_LENGTH           80
#define     PARTITION_FORMAT_LENGTH         120
#define     PARTITION_MOUNT_POINT_LENGTH    120
#define     DISK_HEIGTH                     20
#define     PARTITION_SPACING               40
#define     DISK_SPACING                    25


#define     INSTALLER_MOUNT_POINT_NONE      0
#define     INSTALLER_MOUNT_POINT_ROOT      1
#define     INSTALLER_MOUNT_POINT_HOME      2
#define     INSTALLER_MOUNT_POINT_USR       3
#define     INSTALLER_MOUNT_POINT_BOOT      4

#define     INSTALLER_FILESYSTEM_NONE       -2
#define     INSTALLER_FILESYSTEM_FREESPACE  -1
#define     INSTALLER_FILESYSTEM_EXT2       0
#define     INSTALLER_FILESYSTEM_EXT3       1
#define     INSTALLER_FILESYSTEM_EXT4       2
#define     INSTALLER_FILESYSTEM_NTFS       3
#define     INSTALLER_FILESYSTEM_FAT32      4

#define     INSTALLER_WORKTYPE_ADD          1
#define     INSTALLER_WORKTYPE_CHANGE       2

#define     PARTITION_DELETED               0x0001
#define     PARTITION_NEW                   0x0110
#define     PARTITION_FORMAT                0x0100
#define     PARTITION_CHANGED               0x1000

#define     PARTITION_FORCE_FORMAT          0x0003
#define     PARTITION_DO_FORMAT             0x0002
#define     PARTITION_DONT_FORMAT           0x0001
#define     PARTITION_CANNOT_FORMAT         0x0000

class partition_item : public QWidget{
    Q_OBJECT
public:
    explicit        partition_item(QWidget *parent = 0,PedPartition *Part = NULL,PedDevice *Dev = NULL);
                    ~partition_item();
    void            mousePressEvent(QMouseEvent *);
    void            unClicked(void);
    void            set_partition(PedPartition *Part = NULL, PedDevice *Dev = NULL);
    PedPartition*   getPartition(void);
    partition_item* getPrev(void);
    partition_item* getNext(void);
    PedDevice   *   getDevice(void);
    void            setMountPoint(int MountPointFlag);
    void            setFormatStatus(int);
    void            setPrev(partition_item*);
    void            setNext(partition_item*);
    int             getMountPoint(void);
    int             getFormatStatus(void);
    u_int16_t       Flag;
signals:
    void            clicked(partition_item*);
public slots:
    void            onFormatCheckBoxClicked(bool);
private:
    QLabel          *Path;
    QLabel          *FsName;
    QLabel          *Size;
    QLabel          *MountPoint;
    QCheckBox       *doFormat;
    int              flagMountPoint;
    int              formatStatus;
    QFont            Font;
    PedPartition    *Partition;
    PedDevice       *Device;
    partition_item  *prev;
    partition_item  *next;
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
    void            removeItem(partition_item*);
    int             getSize(void);
    bool            spreaded(void);
    PedDisk  *      getDisk(void);
    PedDevice*      getDevice(void);
    partition_item* getNextPartition(partition_item* Item = NULL);
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
    void            removePartitionFromDisk(partition_item*,disk_item*);
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

class partition_modification_dialog : public QWidget{
    Q_OBJECT
public:
    explicit        partition_modification_dialog(QWidget *parent = 0);
                   ~partition_modification_dialog();

    void    setCurrentPartition(PedPartition Partition, PedDisk Disk, PedDevice Device, int, int WorkType,int _formatStatus);
signals:
    void    MountPointChangeApplied(int);     // MountPoint;
    void    FormatStatusChanged(int);
    void    FilesystemChanged(int);
public slots:
    void    ApplyButtonClicked(void);
    void    CancelButtonClicked(void);
    void    FileSystemSelectChanged(int);
    void    FormatDone(int);
private:
    PedPartition     CurrentPartition;
    PedDisk          CurrentDisk;
    PedDevice        CurrentDevice;
    int              OriginMountPoint;
    int              OriginFileSystem;
    int              WorkType;
    QPushButton     *ApplyButton;
    QPushButton     *CancelButton;
    QLabel          *PartitionPath;
    QLabel          *FileSystemLabel;
    QLabel          *MountPointLabel;
    QLabel          *DoFormatLabel;
    QComboBox       *FileSystemSelect;
    QComboBox       *MountPointSelect;
    QCheckBox       *DoFormatCheckBox;
    QSpinBox        *PartitionSize;
    QLabel          *PartitionSizeLabel;

    QString         CurrentFileSystem;
    QString         CurrentMountPoint;
};

class partition_controllor : public QWidget{
    Q_OBJECT
public:
    explicit        partition_controllor(QWidget *parent = 0);
                    ~partition_controllor();
public slots:
    void            onDiskClicked(disk_item*);
    void            onPartitionClicked(disk_item*, partition_item*);
    void            onModificationButtonClicked(void);
    void            onDeleteButtonClicked(void);
    void            onAddButtonClicked(void);
    void            onMountPointChanged(int);
    void            onFormatStatusChanged(int);
private:
    QScrollArea     *MainArea;
    partition_select*Select;
    QPushButton     *AddButton;
    QPushButton     *DelButton;
    QPushButton     *ChangeButton;
    QFont           DefaultFont;
    partition_item  *currentlySelectedPartition;
    disk_item       *currentlySelectedDisk;
    partition_modification_dialog   *modification_dialog;
};

#endif // PARTED_WIDGET_H
