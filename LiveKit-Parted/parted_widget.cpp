#include "parted_widget.h"


partition_item::partition_item(QWidget *parent):
    QWidget(parent){
    this->Title     = new QLabel(this);
    this->TitleFont.setPointSize(16);
    this->Title->setFont(this->TitleFont);
    this->Title->setText(tr("Title desu"));
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



parted_widget::parted_widget(QWidget *parent):
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

parted_widget::~parted_widget(){
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

void parted_widget::mousePressEvent(QMouseEvent *){
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
}


/* If the 'order' is -1, then insert this item to the bottom */
void parted_widget::InsertPartitions(partition_item *Item,int order){
    if (order == -1)
        order = this->PartitionsMap->size();
    this->PartitionsMap->insert(order,Item);
    this->MainLayout->insertWidget(order,Item);
    this->resize(this->width(),this->height()+PARTITION_HEIGTH);
    this->connect(Item,SIGNAL(clicked(partition_item*)),this,SLOT(onItemClicked(partition_item*)));
    return;
}

void parted_widget::onItemClicked(partition_item *item){
    if(this->PartitionsMap->isEmpty() == false){
        p_map_t::iterator i = this->PartitionsMap->begin();
        while(i != this->PartitionsMap->end()){
            if(i.value() != item)
                i.value()->unClicked();
            i++;
        }
    }
}
