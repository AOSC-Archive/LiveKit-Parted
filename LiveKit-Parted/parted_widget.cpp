#include "parted_widget.h"


partition_item::partition_item(QWidget *parent, PedPartition *Part, PedDevice *Dev):
    QWidget(parent){
    this->Path      = new QLabel(this);
    this->FsName    = new QLabel(this);
    this->Size      = new QLabel(this);
    this->Font.setPointSize(10);
    this->Path->setFont(this->Font);
    this->FsName->setFont(this->Font);
    this->Size->setFont(this->Font);
    this->Path->setText(tr("/dev/sda12"));
    this->FsName->setText(tr("EXT3"));
    this->Size->setText(tr("100G"));
    this->Path->setGeometry(0,0,PARTITION_PATH_LENGTH,PARTITION_HEIGTH);
    this->FsName->setGeometry(PARTITION_PATH_LENGTH,0,PARTITION_FS_NAME_LENGTH,PARTITION_HEIGTH);
    this->Size->setGeometry(PARTITION_PATH_LENGTH+PARTITION_FS_NAME_LENGTH,0,PARTITION_SIZE_LENGTH,PARTITION_HEIGTH);
    this->setStyleSheet("background-color:white;");
    this->resize(PARTED_WIDGET_WIDTH,PARTITION_HEIGTH);

    if(Part != NULL && Dev != NULL){
        this->Partition = new PedPartition;
        this->Device    = new PedDevice;
        memcpy(this->Partition,Part,sizeof(PedPartition));
        memcpy(Device,Dev,sizeof(PedDevice));
        char TmpBuffer[64] = {0};
        sprintf(TmpBuffer,"%lld MB",(Partition->geom.length * Device->sector_size)/(1024*1024));
        this->Size->setText(tr(TmpBuffer));
        if(this->Partition->type & PED_PARTITION_FREESPACE){
            this->FsName->setText(tr("Free Space"));
            this->Path->setText(tr("Free Space"));
        }else{
            this->FsName->setText(tr(this->Partition->fs_type->name));
            this->Path->setText(tr(ped_partition_get_path(Partition)));
        }
    }
}

partition_item::~partition_item(){
    if(this->Partition != NULL)
        delete this->Partition;
    if(this->Device != NULL)
        delete this->Device;
    delete  this->Path;
    delete  this->FsName;
    delete  this->Size;
}

void partition_item::mousePressEvent(QMouseEvent *){
    this->setStyleSheet("background-color:yellow");
    emit this->clicked(this);
}

void partition_item::unClicked(){
    this->setStyleSheet("background-color:white;");
}


disk_item::disk_item(QWidget *parent, PedDisk *disk, PedDevice *dev):
    QWidget(parent){
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->Title     = new QLabel(this);
    this->shown     = true;
    PartitionsMap   = new p_map_t;

    this->Title->setGeometry(0,0,PARTED_WIDGET_WIDTH,DISK_HEIGTH);

    TitleFont.setPixelSize(16);
    this->Title->setFont(TitleFont);
    this->Title->setText(tr("/dev/sda xxxx xxGB xxxxx"));

    this->resize(PARTED_WIDGET_WIDTH,0);
    this->resize(PARTED_WIDGET_WIDTH,DISK_HEIGTH);

    /*this->InsertPartitions(new partition_item(this));
    this->InsertPartitions(new partition_item(this));
    this->InsertPartitions(new partition_item(this));
    this->InsertPartitions(new partition_item(this));*/

    if(disk != NULL && dev != NULL){
        this->Disk = new PedDisk;
        this->Device = new PedDevice;
        memcpy(this->Disk,disk,sizeof(PedDisk));
        memcpy(this->Device,dev,sizeof(PedDevice));
        char TmpBuffer[64] = {0};
        sprintf(TmpBuffer,"%s -- %lld G",this->Device->model,(this->Device->sector_size * this->Device->length)/(1024*1024*1024));
        this->Title->setText(tr(TmpBuffer));
    }
}

disk_item::~disk_item(){
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
    this->PartitionsMap->insert(order,Item);
    //this->MainLayout->insertWidget(order,Item);
    Item->setGeometry(0,DISK_HEIGTH+(PartitionsMap->size()-1)*PARTITION_HEIGTH,PARTED_WIDGET_WIDTH,PARTITION_HEIGTH);
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


partition_select::partition_select(QWidget *parent):
    QWidget(parent){
    this->resize(PARTED_WIDGET_WIDTH,0);
    this->setWindowFlags(Qt::FramelessWindowHint);
    DiskMap     = new d_map_t;
    this->refreshSelect();
    //this->insertDisk(new disk_item(this));
    //this->insertDisk(new disk_item(this));
    //this->insertDisk(new disk_item(this));
    //this->insertDisk(new disk_item(this));

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

void partition_select::onPartitionClicked(disk_item *disk, partition_item *){
    d_map_t::iterator i;
    if(this->DiskMap->isEmpty() == false){
        i = this->DiskMap->begin();
        while(i != this->DiskMap->end()){
            if(i.value() != disk)
                i.value()->clearClickedStatus();
            i++;
        }
    }
}

void partition_select::clearSelect(){

}

void partition_select::refreshSelect(){
    this->clearSelect();
    PedPartition Part;
    PedDevice *dev = 0;
    disk_item       *d_item = NULL;
    partition_item  *p_item = NULL;
    while((dev = ped_device_get_next(dev))){
        printf("\n ==============================================\n");
        printf("device model: %s\n", dev->model);
        printf("path: %s\n",dev->path);
        long long size = (dev->sector_size * dev->length)/(1024*1024*1024);
        printf("size: %lld G\n", size);
        PedDisk* disk = ped_disk_new(dev);
        PedPartition* part = 0;
        d_item = new disk_item(this,disk,dev);
        while((part = ped_disk_next_partition(disk, part))){
            //略过不是分区的空间
          if ((part->type & PED_PARTITION_METADATA) ||
                (part->type & PED_PARTITION_EXTENDED))
                    continue;
           printf("++++++++++++++++++++++++++++++++++++\n");
            printf("partition: %s\n", ped_partition_get_path(part));
            if(part->fs_type)
                printf("fs_type: %s\n", part->fs_type->name);
            else
                printf("fs_type: (null)\n");
            printf("partition start:%lld/n", part->geom.start);
            printf("partition end: %lld/n", part->geom.end);
            printf("partition length:%lld M\n", (part->geom.length * dev->sector_size)/(1024*1024));
            memcpy((void*)&Part,(void*)part,sizeof(Part));
            p_item = new partition_item(this,part,dev);
            d_item->InsertPartitions(p_item);
            //AddPartition(part,dev,disk);
        }
        this->insertDisk(d_item);
    }
}
