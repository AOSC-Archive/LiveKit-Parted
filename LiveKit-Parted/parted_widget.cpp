#include "parted_widget.h"


partition_item::partition_item(QWidget *parent, PedPartition *Part, PedDevice *Dev):
    QWidget(parent){
    this->Path      = new QLabel(this);
    this->FsName    = new QLabel(this);
    this->Size      = new QLabel(this);
    this->MountPoint= new QLabel(this);
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
    this->setStyleSheet("background-color:white;");
    this->resize(PARTED_WIDGET_WIDTH,PARTITION_HEIGTH);

    this->set_partition(Part,Dev);
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

PedDevice* partition_item::getDevice(){
    return this->Device;
}


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
    this->PartitionsMap->insert(order,Item);
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

bool disk_item::spreaded(){
    return shown;
}

PedDisk* disk_item::getDisk(){
    return this->Disk;
}

PedDevice* disk_item::getDevice(){
    return this->Device;
}


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
    PedDevice *dev = 0;
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


partition_controllor::partition_controllor(QWidget *parent):
    QWidget(parent){
    MainArea    = new QScrollArea(this);
    Select      = new partition_select;
    AddButton   = new QPushButton(this);
    DelButton   = new QPushButton(this);
    ChangeButton= new QPushButton(this);
    MainArea->setWidget(Select);
    MainArea->setGeometry(0,0,PARTED_WIDGET_WIDTH+16,PARTED_WIDGET_HEIGTH);
    this->setMaximumSize(PARTED_WIDGET_WIDTH+15,PARTED_WIDGET_HEIGTH+70);
    this->setMinimumSize(PARTED_WIDGET_WIDTH+15,PARTED_WIDGET_HEIGTH+70);
    this->connect(Select,SIGNAL(diskClicked(disk_item*)),this,SLOT(onDiskClicked(disk_item*)));
    this->connect(Select,SIGNAL(partitionClicked(disk_item*,partition_item*)),this,SLOT(onPartitionClicked(disk_item*,partition_item*)));

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
}


void partition_controllor::onDiskClicked(disk_item *){

}

void partition_controllor::onPartitionClicked(disk_item *, partition_item *Item){
    this->currenlytSelected = Item->getPartition();
    if(this->currenlytSelected->type & PED_PARTITION_FREESPACE){
        this->AddButton->setDisabled(false);
        this->DelButton->setDisabled(true);
        this->ChangeButton->setDisabled(true);
        return;
    }
    if(!this->currenlytSelected->fs_type->name){
        this->AddButton->setDisabled(true);
        this->DelButton->setDisabled(true);
        this->ChangeButton->setDisabled(true);
    }else{
        this->AddButton->setDisabled(true);
        this->DelButton->setDisabled(false);
        if( strncmp(this->currenlytSelected->fs_type->name,"ext",   3)  == 0    ||
            strncmp(this->currenlytSelected->fs_type->name,"ntfs",  4)  == 0    ||
            strncmp(this->currenlytSelected->fs_type->name,"fat",   3)  == 0    ||
            strncmp(this->currenlytSelected->fs_type->name,"btrfs", 5)  == 0    ||
            strncmp(this->currenlytSelected->fs_type->name,"xfs",   3)  == 0    ||
            strncmp(this->currenlytSelected->fs_type->name,"f2fs",  4)  == 0)
            this->ChangeButton->setDisabled(false);
        else
            this->ChangeButton->setDisabled(true);
    }
}
