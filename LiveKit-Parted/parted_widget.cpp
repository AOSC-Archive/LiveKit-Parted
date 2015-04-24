#include "parted_widget.h"

//
// Partition Items
//
//
//
partition_item::partition_item(QWidget *parent, PedPartition *Part, PedDevice *Dev):
    QWidget(parent){
    this->Path      = new QLabel(this);
    this->FsName    = new QLabel(this);
    this->Size      = new QLabel(this);
    this->MountPoint= new QLabel(this);
    this->prev      = NULL;
    this->next      = NULL;
    this->Font.setPointSize(10);
    this->Path->setFont(this->Font);
    this->FsName->setFont(this->Font);
    this->Size->setFont(this->Font);
    this->Path->setText(tr("/dev/sda12"));
    this->FsName->setText(tr("EXT3"));
    this->Size->setText(tr("100G"));
    int _x = 0;
    this->Path->setGeometry(0,0,PARTITION_PATH_LENGTH,PARTITION_HEIGTH);
    _x+=PARTITION_PATH_LENGTH;
    this->FsName->setGeometry(_x,0,PARTITION_FS_NAME_LENGTH,PARTITION_HEIGTH);
    _x+=PARTITION_FS_NAME_LENGTH;
    this->Size->setGeometry(_x,0,PARTITION_SIZE_LENGTH,PARTITION_HEIGTH);
    _x+=PARTITION_SIZE_LENGTH;
    this->MountPoint->setGeometry(_x,0,PARTITION_MOUNT_POINT_LENGTH,PARTITION_HEIGTH);
    this->MountPoint->setText(tr("None"));
    this->setStyleSheet("background-color:white;");
    this->resize(PARTED_WIDGET_WIDTH,PARTITION_HEIGTH);

    this->set_partition(Part,Dev);
    this->setMountPoint(INSTALLER_MOUNT_POINT_NONE);
}

partition_item::~partition_item(){
    if(this->Partition != NULL)
        delete this->Partition;
    if(this->Device != NULL)
        delete this->Device;
    delete  this->Path;
    delete  this->FsName;
    delete  this->Size;
    delete  this->MountPoint;
}

void partition_item::set_partition(PedPartition *Part, PedDevice *Dev){
    if(Part != NULL && Dev != NULL){
        this->Partition = new PedPartition;
        this->Device    = new PedDevice;
        memcpy(this->Partition,Part,sizeof(PedPartition));
        memcpy(Device,Dev,sizeof(PedDevice));
        char TmpBuffer[64] = {0};
        long long partition_size = (Partition->geom.length * Device->sector_size)/1024;
        if(partition_size < 1024)
            sprintf(TmpBuffer,"%lld KB",partition_size);
        else if(partition_size < 1024*1024)
            sprintf(TmpBuffer,"%lld MB",partition_size/(1024));
        else if(partition_size < 1024*1024*1024)
            sprintf(TmpBuffer,"%lld GB",partition_size/(1024*1024));
        else
            sprintf(TmpBuffer,"%lld TB",partition_size/(1024*1024*1024));
        this->Size->setText(tr(TmpBuffer));
        if(this->Partition->type & PED_PARTITION_FREESPACE){
            this->FsName->setText(tr("Free Space"));
            this->Path->setText(tr("Free Space"));
        }else{
            this->FsName->setText(tr(this->Partition->fs_type->name));
            this->Path->setText(tr(ped_partition_get_path(Partition)));
        }
    }else if(Part != NULL && Dev == NULL){
        memcpy(this->Partition,Part,sizeof(PedPartition));
        char TmpBuffer[64] = {0};
        long long partition_size = (Partition->geom.length * Device->sector_size)/1024;
        if(partition_size < 1024)
            sprintf(TmpBuffer,"%lld KB",partition_size);
        else if(partition_size < 1024*1024)
            sprintf(TmpBuffer,"%lld MB",partition_size/(1024));
        else if(partition_size < 1024*1024*1024)
            sprintf(TmpBuffer,"%lld GB",partition_size/(1024*1024));
        else
            sprintf(TmpBuffer,"%lld TB",partition_size/(1024*1024*1024));
        this->Size->setText(tr(TmpBuffer));
        if(this->Partition->type & PED_PARTITION_FREESPACE){
            this->FsName->setText(tr("Free Space"));
            this->Path->setText(tr("Free Space"));
        }else{
            this->FsName->setText(tr(this->Partition->fs_type->name));
            this->Path->setText(tr(ped_partition_get_path(Partition)));
        }
    }
    return;
}

void partition_item::mousePressEvent(QMouseEvent *){
    this->setStyleSheet("background-color:yellow");
    emit this->clicked(this);
}

void partition_item::unClicked(){
    this->setStyleSheet("background-color:white;");
}

PedPartition* partition_item::getPartition(){
    return this->Partition;
}

partition_item* partition_item::getPrev(){
    return this->prev;
}

partition_item* partition_item::getNext(){
    return this->next;
}

PedDevice* partition_item::getDevice(){
    return this->Device;
}

void partition_item::setMountPoint(int MountPointFlag){
    this->flagMountPoint = MountPointFlag;
    if(this->flagMountPoint == INSTALLER_MOUNT_POINT_BOOT)
        this->MountPoint->setText(tr("/boot"));
    else if(this->flagMountPoint == INSTALLER_MOUNT_POINT_HOME)
        this->MountPoint->setText(tr("/home"));
    else if(this->flagMountPoint == INSTALLER_MOUNT_POINT_NONE)
        this->MountPoint->setText(tr(" "));
    else if(this->flagMountPoint == INSTALLER_MOUNT_POINT_ROOT)
        this->MountPoint->setText(tr("/"));
    else if(this->flagMountPoint == INSTALLER_MOUNT_POINT_USR)
        this->MountPoint->setText(tr("/usr"));
    return;
}

int partition_item::getMountPoint(){
    return this->flagMountPoint;
}

void partition_item::setPrev(partition_item *s){
    this->prev = s;
}

void partition_item::setNext(partition_item *s){
    this->next = s;
}


//
// Disk Items
//
//
//

disk_item::disk_item(QWidget *parent, PedDisk *disk, PedDevice *dev):
    QWidget(parent){
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->Title     = new QLabel(this);
    this->shown     = true;
    PartitionsMap   = new p_map_t;

    this->Title->setGeometry(DISK_SPACING,0,PARTED_WIDGET_WIDTH,DISK_HEIGTH);

    TitleFont.setPixelSize(14);
    this->Title->setFont(TitleFont);
    this->Title->setText(tr("/dev/sda xxxx xxGB xxxxx"));
    this->setStyleSheet("background-color:white;");

    this->resize(PARTED_WIDGET_WIDTH,0);
    this->resize(PARTED_WIDGET_WIDTH,DISK_HEIGTH);

    this->set_disk(disk,dev);
}

disk_item::~disk_item(){
    if(this->Disk)
        delete  this->Disk;
    if(this->Device)
        delete  this->Device;
    if(this->PartitionsMap->isEmpty() == false){
        p_map_t::iterator i = this->PartitionsMap->begin();
        while(i != this->PartitionsMap->end()){
            delete i.value();
            i++;
        }
        this->PartitionsMap->clear();
    }
    delete  this->PartitionsMap;
    delete  this->Title;
}

void disk_item::set_disk(PedDisk *disk, PedDevice *dev){
    if(disk != NULL && dev != NULL){
        this->Disk = new PedDisk;
        this->Device = new PedDevice;
        memcpy(this->Disk,disk,sizeof(PedDisk));
        memcpy(this->Device,dev,sizeof(PedDevice));
        char TmpBuffer[64] = {0};
        long long disk_size = (this->Device->sector_size * this->Device->length)/(1024*1024*1024);
        if(disk_size < 1024)
            sprintf(TmpBuffer,"%s   %lld GB",this->Device->model,disk_size);
        else
            sprintf(TmpBuffer,"%s   %lld TB",this->Device->model,disk_size/1024);
        this->Title->setText(tr(TmpBuffer));
    }
}

void disk_item::clearClickedStatus(){
    if(this->PartitionsMap->isEmpty() == false){
        p_map_t::iterator i = this->PartitionsMap->begin();
        while(i != this->PartitionsMap->end()){
            i.value()->unClicked();
            i++;
        }
    }
}

int  disk_item::getSize(){
    return this->PartitionsMap->size();
}

void disk_item::mousePressEvent(QMouseEvent *){
    if (this->shown){
        this->resize(PARTED_WIDGET_WIDTH,DISK_HEIGTH);
        if(this->PartitionsMap->isEmpty() == false){
            p_map_t::iterator i = this->PartitionsMap->begin();
            while(i != this->PartitionsMap->end()){
                i.value()->setVisible(false);
                i.value()->unClicked();
                i++;
            }
        }
        this->shown = false;
    }else{
        this->resize(PARTED_WIDGET_WIDTH,DISK_HEIGTH+(this->PartitionsMap->size())*PARTITION_HEIGTH);
        if(this->PartitionsMap->isEmpty() == false){
            p_map_t::iterator i = this->PartitionsMap->begin();
            while(i != this->PartitionsMap->end()){
                i.value()->setVisible(true);
                i.value()->unClicked();
                i++;
            }
        }
        this->shown = true;
    }
    emit diskClicked(this,this->shown);
}


/* If the 'order' is -1, then insert this item to the bottom */
void disk_item::InsertPartitions(partition_item *Item,int order){
    if(Item == NULL)    return;
    if (order == -1)
        order = this->PartitionsMap->size();
    Item->setParent(this);
    if(this->PartitionsMap->size() == 0){
        Item->setPrev(NULL);
        Item->setNext(NULL);
        this->PartitionsMap->insert(order,Item);
        Item->setGeometry(PARTITION_SPACING,DISK_HEIGTH+(PartitionsMap->size()-1)*PARTITION_HEIGTH,PARTED_WIDGET_WIDTH,PARTITION_HEIGTH);
        this->resize(this->width(),this->height()+PARTITION_HEIGTH);
        this->connect(Item,SIGNAL(clicked(partition_item*)),this,SLOT(onItemClicked(partition_item*)));
        return;
    }
    Item->setNext(NULL);
    this->PartitionsMap->insert(order,Item);
    p_map_t::iterator i = this->PartitionsMap->end();
    i -= 2;
    i.value()->setNext(Item);
    Item->setPrev(i.value());
    Item->setGeometry(PARTITION_SPACING,DISK_HEIGTH+(PartitionsMap->size()-1)*PARTITION_HEIGTH,PARTED_WIDGET_WIDTH,PARTITION_HEIGTH);
    this->resize(this->width(),this->height()+PARTITION_HEIGTH);
    this->connect(Item,SIGNAL(clicked(partition_item*)),this,SLOT(onItemClicked(partition_item*)));
    return;
}

void disk_item::onItemClicked(partition_item *item){
    if(this->PartitionsMap->isEmpty() == false){
        p_map_t::iterator i = this->PartitionsMap->begin();
        while(i != this->PartitionsMap->end()){
            if(i.value() != item)
                i.value()->unClicked();
            i++;
        }
    }
    emit partitionClicked(this,item);
}

void disk_item::removeItem(partition_item *Item){
    bool Found = false;
    p_map_t::iterator i;
    if(this->PartitionsMap->isEmpty() == false){
        i = this->PartitionsMap->begin();
        while(i != this->PartitionsMap->end()){
            if(i.value() == Item){
                Found = true;
                break;
            }
            i++;
        }
    }
    int key = i.key();
    i++;
    this->PartitionsMap->remove(key);
    if(Found == false)
        return;
    if(Item->getPrev() != NULL)
        Item->getPrev()->setNext(Item->getNext());
    if(Item->getNext() != NULL)
        Item->getNext()->setPrev(Item->getPrev());
    QRect location;
    while(i != this->PartitionsMap->end()){
        location = i.value()->geometry();
        location.setY(location.y() - PARTITION_HEIGTH);
        i.value()->setGeometry(location);
        i++;
    }
    Item->hide();
    return;
}

partition_item* disk_item::getNextPartition(partition_item *Item){
    if(this->PartitionsMap->isEmpty())
        return NULL;
    if(Item == this->PartitionsMap->end().value())
        return NULL;
    if(Item == NULL)
        return this->PartitionsMap->begin().value();
    p_map_t::iterator i;
    i = this->PartitionsMap->begin();
    while(i != this->PartitionsMap->end()){
        if(i.value() == Item){
            i++;
            if(i != this->PartitionsMap->end())
                return i.value();
            else
                return NULL;
        }
        i++;
    }
    return NULL;
}

partition_item* disk_item::getPrevPartition(partition_item *Item){
    if(this->PartitionsMap->isEmpty())
        return NULL;
    if(Item == NULL)
        return NULL;
    if(Item == this->PartitionsMap->begin().value())
        return NULL;
    p_map_t::iterator i;
    i = this->PartitionsMap->begin();
    while(i != this->PartitionsMap->end()){
        if(i.value() == Item){
            i--;
            return i.value();
        }
        i++;
    }
    return NULL;
}

bool disk_item::spreaded(){
    return shown;
}

PedDisk* disk_item::getDisk(){
    return this->Disk;
}

PedDevice* disk_item::getDevice(){
    return this->Device;
}


//
// Partition Select
//
//
//

partition_select::partition_select(QWidget *parent):
    QWidget(parent){
    this->resize(PARTED_WIDGET_WIDTH,0);
    this->setWindowFlags(Qt::FramelessWindowHint);
    DiskMap     = new d_map_t;
    this->setStyleSheet("background-color:white;");
    this->refreshSelect();

}

partition_select::~partition_select(){
    if(this->DiskMap->isEmpty() == false){
        d_map_t::iterator i = this->DiskMap->begin();
        while(i != this->DiskMap->end()){
            delete i.value();
            i++;
        }
        this->DiskMap->clear();
    }
    delete DiskMap;
}

void partition_select::onDiskClicked(disk_item *Item, bool spreaded){
    d_map_t::iterator i;
    bool found = false;
    QRect locale;
    int h_change    = 0;
    int h_temp      = 0;
    if(spreaded)
        h_change = Item->getSize()*PARTITION_HEIGTH;
    else
        h_change = -1*(Item->getSize()*PARTITION_HEIGTH);
    this->resize(this->width(),this->height()+h_change);
    if(this->DiskMap->isEmpty() == false){
        i = this->DiskMap->begin();
        while(i != this->DiskMap->end()){
            if(i.value() == Item){
                found = true;
                break;
            }
            i++;
        }
        if(found == true){
            i++;
            while(i != this->DiskMap->end()){
                locale = i.value()->geometry();
                locale.setY(locale.y()+h_change);
                h_temp = DISK_HEIGTH;
                if(i.value()->spreaded())
                    h_temp += i.value()->getSize()*PARTITION_HEIGTH;
                locale.setHeight(h_temp);
                locale.setWidth(PARTED_WIDGET_WIDTH);
                i.value()->setGeometry(locale);
                i++;
            }
        }
    }
    if(found)
        emit this->diskClicked(Item);
}


void partition_select::onPartitionClicked(disk_item *disk, partition_item *item){
    d_map_t::iterator i;
    if(this->DiskMap->isEmpty() == false){
        i = this->DiskMap->begin();
        while(i != this->DiskMap->end()){
            if(i.value() != disk)
                i.value()->clearClickedStatus();
            i++;
        }
    }
    emit this->partitionClicked(disk,item);
}
void partition_select::insertDisk(disk_item *Item, int order){
    if (order == -1)
        order = this->DiskMap->size();
    int h = 0;
    if(this->DiskMap->isEmpty() == false){
        d_map_t::iterator i = this->DiskMap->begin();
        while(i != this->DiskMap->end()){
            h += DISK_HEIGTH;
            if(i.value()->spreaded())
                h += i.value()->getSize()*PARTITION_HEIGTH;
            i++;
        }
    }
    Item->setGeometry(0,h,PARTED_WIDGET_WIDTH,DISK_HEIGTH+PARTITION_HEIGTH*Item->getSize());
    this->DiskMap->insert(order,Item);
    h += Item->getSize()*PARTITION_HEIGTH + DISK_HEIGTH;

    this->resize(this->width(),h);
    this->connect(Item,SIGNAL(diskClicked(disk_item*,bool)),this,SLOT(onDiskClicked(disk_item*,bool)));
    this->connect(Item,SIGNAL(partitionClicked(disk_item*,partition_item*)),this,SLOT(onPartitionClicked(disk_item*,partition_item*)));
    Item->show();
    return;
}

void partition_select::removePartitionFromDisk(partition_item *item, disk_item *disk){
    disk->removeItem(item);
    d_map_t::iterator i;
    if(this->DiskMap->isEmpty() == false){
        i = this->DiskMap->begin();
        while(i != this->DiskMap->end()){
            if(i.value() == disk)
                break;
            i++;
        }
    }
    QRect location;
    i++;
    while(i != this->DiskMap->end()){
        location = i.value()->geometry();
        location.setY(location.y()-PARTITION_HEIGTH);
        i.value()->setGeometry(location);
        i++;
    }
    return;
}

void partition_select::clearSelect(){
    if(this->DiskMap->isEmpty() == false){
        d_map_t::iterator i = this->DiskMap->begin();
        while(i != this->DiskMap->end()){
            delete i.value();
            i++;
        }
        this->DiskMap->clear();
    }
}

void partition_select::refreshSelect(){
    this->clearSelect();
    PedDevice       *dev = 0;
    disk_item       *d_item;
    partition_item  *p_item;
    PedDisk         *disk;
    PedPartition    *part;
    while((dev = ped_device_get_next(dev))){
        disk = ped_disk_new(dev);
        part = 0;
        d_item = new disk_item(this,disk,dev);
        while((part = ped_disk_next_partition(disk, part))){
            if ((part->type & PED_PARTITION_METADATA) || (part->type & PED_PARTITION_EXTENDED))
                continue;
            p_item = NULL;
            p_item = new partition_item;
            p_item->set_partition(part,dev);
            d_item->InsertPartitions(p_item);
        }
        this->insertDisk(d_item);
    }
}

//
// Partition Modification Dialog
//
//
//

partition_modification_dialog::partition_modification_dialog(QWidget *parent):
    QWidget(parent){

    this->setMinimumSize(300,200);
    this->setMaximumSize(300,200);
    this->setWindowTitle(tr("Change partition"));
    ApplyButton     = new QPushButton(this);
    CancelButton    = new QPushButton(this);
    ApplyButton->setText(tr("Apply"));
    CancelButton->setText(tr("Cancel"));
    ApplyButton->setGeometry(160,160,60,30);
    CancelButton->setGeometry(230,160,60,30);

    FileSystemSelect = new QComboBox(this);
    MountPointSelect = new QComboBox(this);
    MountPointLabel  = new QLabel(this);
    PartitionPath    = new QLabel(this);
    FileSystemLabel  = new QLabel(this);
    DoFormatLabel    = new QLabel(this);
    PartitionSizeLabel=new QLabel(this);
    DoFormatCheckBox = new QCheckBox(this);
    PartitionSize    = new QSpinBox(this);

    PartitionPath->setGeometry(35,10,200,20);
    FileSystemLabel->setGeometry(35,40,95,20);
    FileSystemSelect->setGeometry(130,40,75,20);
    MountPointLabel->setGeometry(35,60,95,20);
    MountPointSelect->setGeometry(130,60,75,20);
    DoFormatLabel->setGeometry(35,80,75,20);
    DoFormatCheckBox->setGeometry(130,80,110,20);
    PartitionSizeLabel->setGeometry(35,100,75,20);
    PartitionSize->setGeometry(130,100,75,20);


    PartitionPath->setText(tr("Partition path: /dev/sda1"));
    FileSystemLabel->setText(tr("Filesystem"));
    MountPointLabel->setText(tr("Mount point"));
    DoFormatLabel->setText(tr("Format"));

    FileSystemSelect->addItem(tr("ext2"));
    FileSystemSelect->addItem(tr("ext3"));
    FileSystemSelect->addItem(tr("ext4"));
    FileSystemSelect->addItem(tr("ntfs"));
    FileSystemSelect->addItem(tr("fat32"));

    MountPointSelect->addItem(tr("---"));
    MountPointSelect->addItem(tr("/"));
    MountPointSelect->addItem(tr("/home"));
    MountPointSelect->addItem(tr("/usr"));
    MountPointSelect->addItem(tr("/boot"));

    PartitionSizeLabel->setText(tr("Size(MB)"));



    this->connect(ApplyButton,SIGNAL(clicked()),this,SLOT(ApplyButtonClicked()));
    this->connect(CancelButton,SIGNAL(clicked()),this,SLOT(CancelButtonClicked()));
    this->connect(FileSystemSelect,SIGNAL(currentIndexChanged(int)),this,SLOT(FileSystemSelectChanged(int)));
}

partition_modification_dialog::~partition_modification_dialog(){
    delete  ApplyButton;
    delete  CancelButton;
    delete  PartitionPath;
    delete  FileSystemLabel;
    delete  MountPointLabel;
    delete  DoFormatLabel;
    delete  FileSystemSelect;
    delete  MountPointSelect;
    delete  DoFormatCheckBox;
    delete  PartitionSize;
    delete  PartitionSizeLabel;
}

void partition_modification_dialog::SetCurrentPartition(PedPartition Partition, PedDisk Disk, PedDevice Device,int MountPoint, int _WorkType){
    memcpy((void*)&CurrentPartition,(void*)&Partition,sizeof(PedPartition));
    memcpy((void*)&CurrentDisk,(void*)&Disk,sizeof(PedDisk));
    memcpy((void*)&CurrentDevice,(void*)&Device,sizeof(PedDevice));
    WorkType = _WorkType;
    OriginMountPoint =MountPoint;
    char Name[64];
    sprintf(Name,"%s :%s",tr("Partition path").toUtf8().data(),ped_partition_get_path(&Partition));
    PartitionPath->setText(Name);
    if(MountPoint > 0){
        if(MountPoint == INSTALLER_MOUNT_POINT_ROOT){
            MountPointSelect->insertItem(MountPoint,tr("/"));
        }else if(MountPoint == INSTALLER_MOUNT_POINT_HOME){
            MountPointSelect->insertItem(MountPoint,tr("/home"));
        }else if(MountPoint == INSTALLER_MOUNT_POINT_USR){
                    MountPointSelect->insertItem(MountPoint,tr("/usr"));
        }else if(MountPoint == INSTALLER_MOUNT_POINT_BOOT){
            MountPointSelect->insertItem(MountPoint,tr("/boot"));
        }
    }
    MountPointSelect->setCurrentIndex(MountPoint);
    PartitionSize->setRange(0,(CurrentPartition.geom.length * CurrentDevice.sector_size)/(1024*1024));
    PartitionSize->setValue((CurrentPartition.geom.length * CurrentDevice.sector_size)/(1024*1024));
    if(WorkType == INSTALLER_WORKTYPE_ADD){
        OriginFileSystem = INSTALLER_FILESYSTEM_FREESPACE;
        FileSystemSelect->setCurrentIndex(INSTALLER_FILESYSTEM_EXT4);
        return;
    }else if(Partition.fs_type != NULL){
        if(strcmp(Partition.fs_type->name,"ext2") == 0)          OriginFileSystem = INSTALLER_FILESYSTEM_EXT2;
        else if(strcmp(Partition.fs_type->name,"ext3") == 0)     OriginFileSystem = INSTALLER_FILESYSTEM_EXT3;
        else if(strcmp(Partition.fs_type->name,"ext4") == 0)     OriginFileSystem = INSTALLER_FILESYSTEM_EXT4;
        else if(strcmp(Partition.fs_type->name,"ntfs") == 0)     OriginFileSystem = INSTALLER_FILESYSTEM_NTFS;
        else if(strcmp(Partition.fs_type->name,"fat32")== 0)     OriginFileSystem = INSTALLER_FILESYSTEM_FAT32;
        FileSystemSelect->setCurrentIndex(OriginFileSystem);
    }else{
        OriginFileSystem = INSTALLER_FILESYSTEM_NONE;
        FileSystemSelect->setCurrentIndex(INSTALLER_FILESYSTEM_EXT4);
    }
}


void partition_modification_dialog::FormatDone(int Status){
    if(Status != 0){
        QMessageBox::warning(this,tr("Warning"),tr("Format failure!"),QMessageBox::Yes);
    }else{
        QMessageBox::information(this,tr("Information"),tr("Format success!"),QMessageBox::Yes);
    }
    this->close();
}

void partition_modification_dialog::ApplyButtonClicked(){
    QString MountPoint = MountPointSelect->currentText();
    int MountPointFlag = INSTALLER_MOUNT_POINT_NONE;
    if(MountPoint == "---")
        MountPointFlag = INSTALLER_MOUNT_POINT_NONE;
    else if(MountPoint == "/home")
        MountPointFlag = INSTALLER_MOUNT_POINT_HOME;
    else if(MountPoint == "/boot")
        MountPointFlag = INSTALLER_MOUNT_POINT_BOOT;
    else if(MountPoint == "/")
        MountPointFlag = INSTALLER_MOUNT_POINT_ROOT;
    else if(MountPoint == "/usr")
        MountPointFlag = INSTALLER_MOUNT_POINT_USR;
    emit MountPointChangeApplied(MountPointFlag);
    if(MountPointSelect->currentIndex()>0){
        MountPointSelect->removeItem(MountPointSelect->currentIndex());
    }
    if(DoFormatCheckBox->isChecked() == false){
        this->close();
    }else{
        if(WorkType == INSTALLER_WORKTYPE_ADD){
            PedPartition *NewPartition;
            PedFileSystemType *fstype = ped_file_system_type_get("msdos");
            int ToEnd = (((CurrentPartition.geom.length * CurrentDevice.sector_size)/(1024*1024) - PartitionSize->value()))
                                                                *(1024*1024)/CurrentDevice.sector_size;
            NewPartition = ped_partition_new(&CurrentDisk,PED_PARTITION_NORMAL,fstype,CurrentPartition.geom.start,CurrentPartition.geom.end - ToEnd);
            if(NewPartition)
                ped_disk_add_partition(&CurrentDisk,NewPartition,ped_constraint_exact(&NewPartition->geom));
            return;
        }
    }
}

void partition_modification_dialog::CancelButtonClicked(){
    if(OriginMountPoint > 0){
        MountPointSelect->removeItem(OriginMountPoint);
    }
    this->close();
}

void partition_modification_dialog::FileSystemSelectChanged(int Now){
    if(Now != OriginFileSystem){
        DoFormatCheckBox->setDisabled(true);
        DoFormatCheckBox->setChecked(true);
    }else{
        DoFormatCheckBox->setDisabled(false);
    }
}



//
// Partition Controllor
//
//
//


partition_controllor::partition_controllor(QWidget *parent):
    QWidget(parent){
    MainArea    = new QScrollArea(this);
    Select      = new partition_select;
    AddButton   = new QPushButton(this);
    DelButton   = new QPushButton(this);
    ChangeButton= new QPushButton(this);
    modification_dialog =   new partition_modification_dialog;
    modification_dialog->hide();
    MainArea->setWidget(Select);
    MainArea->setGeometry(0,0,PARTED_WIDGET_WIDTH+16,PARTED_WIDGET_HEIGTH);
    this->setMaximumSize(PARTED_WIDGET_WIDTH+15,PARTED_WIDGET_HEIGTH+70);
    this->setMinimumSize(PARTED_WIDGET_WIDTH+15,PARTED_WIDGET_HEIGTH+70);
    this->connect(Select,SIGNAL(diskClicked(disk_item*)),this,SLOT(onDiskClicked(disk_item*)));
    this->connect(Select,SIGNAL(partitionClicked(disk_item*,partition_item*)),this,SLOT(onPartitionClicked(disk_item*,partition_item*)));
    this->connect(ChangeButton,SIGNAL(clicked()),this,SLOT(onModificationButtonClicked()));
    this->connect(modification_dialog,SIGNAL(MountPointChangeApplied(int)),this,SLOT(onMountPointChanged(int)));
    this->connect(DelButton,SIGNAL(clicked()),this,SLOT(onDeleteButtonClicked()));

    DefaultFont.setPointSize(8);
    this->AddButton->setFont(DefaultFont);
    this->DelButton->setFont(DefaultFont);
    this->ChangeButton->setFont(DefaultFont);

    this->AddButton->setText(tr("New"));
    this->DelButton->setText(tr("Del"));
    this->ChangeButton->setText(tr("Change"));
    this->setStyleSheet("background-color:white;");

    this->AddButton->setGeometry(15,PARTED_WIDGET_HEIGTH+5,30,20);
    this->ChangeButton->setGeometry(45,PARTED_WIDGET_HEIGTH+5,50,20);
    this->DelButton->setGeometry(95,PARTED_WIDGET_HEIGTH+5,30,20);
}

partition_controllor::~partition_controllor(){
    delete Select;
    delete AddButton;
    delete DelButton;
    delete ChangeButton;
    delete MainArea;
    delete modification_dialog;
}


void partition_controllor::onDiskClicked(disk_item *){

}

void partition_controllor::onPartitionClicked(disk_item *disk, partition_item *Item){
    this->currenlytSelectedPartition    = Item;
    this->currentlySelectedDisk         = disk;
    PedPartition *currentPartition = Item->getPartition();
    if(currentPartition->type & PED_PARTITION_FREESPACE){
        this->AddButton->setDisabled(false);
        this->DelButton->setDisabled(true);
        this->ChangeButton->setDisabled(true);
        return;
    }
    if(!currentPartition->fs_type->name){
        this->AddButton->setDisabled(true);
        this->DelButton->setDisabled(true);
        this->ChangeButton->setDisabled(true);
    }else{
        this->AddButton->setDisabled(true);
        this->DelButton->setDisabled(false);
        if( strncmp(currentPartition->fs_type->name,"ext",   3)  == 0    ||
            strncmp(currentPartition->fs_type->name,"ntfs",  4)  == 0    ||
            strncmp(currentPartition->fs_type->name,"fat",   3)  == 0    ||
            strncmp(currentPartition->fs_type->name,"btrfs", 5)  == 0    ||
            strncmp(currentPartition->fs_type->name,"xfs",   3)  == 0    ||
            strncmp(currentPartition->fs_type->name,"f2fs",  4)  == 0)
            this->ChangeButton->setDisabled(false);
        else
            this->ChangeButton->setDisabled(true);
    }
}

void partition_controllor::onModificationButtonClicked(){
    PedPartition *currentPartition = this->currenlytSelectedPartition->getPartition();
    modification_dialog->SetCurrentPartition(*currentPartition,*currentPartition->disk,*this->currenlytSelectedPartition->getDevice(),this->currenlytSelectedPartition->getMountPoint(),INSTALLER_WORKTYPE_CHANGE);
    modification_dialog->show();
}

void partition_controllor::onDeleteButtonClicked(){
    PedPartition *currentPartition  = new PedPartition;
    PedPartition *deletedPartition  = new PedPartition;
    memcpy(currentPartition,this->currenlytSelectedPartition->getPartition(),sizeof(PedPartition));
    currentPartition->type = PED_PARTITION_FREESPACE;
    if(this->currenlytSelectedPartition->getNext() != NULL){
        if(this->currenlytSelectedPartition->getNext()->getPartition()->type == PED_PARTITION_FREESPACE){
            this->currenlytSelectedPartition->getPartition()->geom.end = this->currenlytSelectedPartition->getNext()->getPartition()->geom.end;
            currentPartition->geom.length   = this->currenlytSelectedPartition->getPartition()->geom.length + this->currenlytSelectedPartition->getNext()->getPartition()->geom.length;
            currentPartition->geom.end      = this->currenlytSelectedPartition->getNext()->getPartition()->geom.end;
            this->Select->removePartitionFromDisk(this->currenlytSelectedPartition->getNext(),this->currentlySelectedDisk);
        }
    }
    if(this->currenlytSelectedPartition->getPrev() != NULL){
        if(this->currenlytSelectedPartition->getPrev()->getPartition()->type ==  PED_PARTITION_FREESPACE){
            currentPartition->geom.length   = this->currenlytSelectedPartition->getPartition()->geom.length + this->currenlytSelectedPartition->getPrev()->getPartition()->geom.length;
            currentPartition->geom.start    = this->currenlytSelectedPartition->getPrev()->getPartition()->geom.start;
            this->Select->removePartitionFromDisk(this->currenlytSelectedPartition->getPrev(),this->currentlySelectedDisk);
        }
    }
    this->currenlytSelectedPartition->set_partition(currentPartition);
    delete currentPartition;
    delete deletedPartition;
    currentPartition = NULL;
    deletedPartition = NULL;
}

void partition_controllor::onMountPointChanged(int MountPoint){
    this->currenlytSelectedPartition->setMountPoint(MountPoint);
}
