#include "parted_widget.h"


partition_item::partition_item(QWidget *parent):
    QWidget(parent){
    this->Title     = new QLabel(this);
    this->TitleFont.setPointSize(16);
    this->Title->setFont(this->TitleFont);
    this->Title->setText(tr("Title desu"));
    this->Title->setGeometry(0,0,300,25);
    this->Title->show();
    this->setStyleSheet("background-color:white;");
    this->resize(PARTED_WIDGET_WIDTH,PARTITION_HEIGTH);
}

partition_item::~partition_item(){
    delete  this->Title;
}

void partition_item::mousePressEvent(QMouseEvent *){
    this->setStyleSheet("background-color:yellow");
    emit this->clicked(this);
}

void partition_item::unClicked(){
    this->setStyleSheet("background-color:white;");
}


disk_item::disk_item(QWidget *parent):
    QWidget(parent){
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->Title     = new QLabel(this);
    this->shown     = true;
    MainLayout      = new QVBoxLayout;
    PartitionsMap   = new p_map_t;

    this->Title->setGeometry(0,0,300,30);

    TitleFont.setPixelSize(20);
    this->Title->setFont(TitleFont);
    this->Title->setText(tr("/dev/sda xxxx xxGB"));

    this->resize(200,0);
    this->setLayout(MainLayout);

    this->MainLayout->setSpacing(0);
    this->MainLayout->insertSpacing(-1,20);
    this->PartitionsMap->insert(0,new partition_item);
    this->resize(300,30);

    this->InsertPartitions(new partition_item);
    this->InsertPartitions(new partition_item);

}

disk_item::~disk_item(){
    delete  this->MainLayout;
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
        i++;
        while(i != this->PartitionsMap->end()){
            i.value()->unClicked();
            i++;
        }
    }
}

int  disk_item::getSize(){
    return this->PartitionsMap->size()-1;
}

void disk_item::mousePressEvent(QMouseEvent *){
    if (this->shown){
        this->resize(PARTED_WIDGET_WIDTH,30);
        if(this->PartitionsMap->isEmpty() == false){
            p_map_t::iterator i = this->PartitionsMap->begin();
            i++;
            while(i != this->PartitionsMap->end()){
                i.value()->setVisible(false);
                i.value()->unClicked();
                i++;
            }
        }
        this->shown = false;
    }else{
        this->resize(PARTED_WIDGET_WIDTH,30+(this->PartitionsMap->size()-1)*PARTITION_HEIGTH);
        if(this->PartitionsMap->isEmpty() == false){
            p_map_t::iterator i = this->PartitionsMap->begin();
            i++;
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
    if (order == -1)
        order = this->PartitionsMap->size();
    this->PartitionsMap->insert(order,Item);
    this->MainLayout->insertWidget(order,Item);
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
    this->resize(300,0);
    this->setWindowFlags(Qt::FramelessWindowHint);
    DiskMap     = new d_map_t;

    this->insertDisk(new disk_item(this));
    this->insertDisk(new disk_item(this));
    this->insertDisk(new disk_item(this));
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
                h_temp = 30;
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
            h += 30;
            if(i.value()->spreaded())
                h += i.value()->getSize()*PARTITION_HEIGTH;
            i++;
        }
    }
    Item->setGeometry(0,h,300,30+PARTITION_HEIGTH*Item->getSize());
    this->DiskMap->insert(order,Item);
    h += Item->getSize()*PARTITION_HEIGTH + 30;

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
