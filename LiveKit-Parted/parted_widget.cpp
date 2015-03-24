#include "parted_widget.h"


partition_item::partition_item(QWidget *parent):
    QWidget(parent){
    this->Title     = new QLabel(this);
    this->setMouseTracking(true);

    this->Title->setText(tr("Title desu"));
    this->setStyleSheet("background-color:green;");
}

partition_item::~partition_item(){
    delete  this->Title;
}

void partition_item::mouseMoveEvent(QMouseEvent *Event){
        this->setStyleSheet("background-color:white");
}

void partition_item::mousePressEvent(QMouseEvent *){
    //this->setStyleSheet("background-color:blue");
}



parted_widget::parted_widget(QWidget *parent):
    QWidget(parent){
    MainLayout      = new QVBoxLayout;
    PartitionsMap   = new p_map_t;

    this->resize(300,100);
    this->setLayout(MainLayout);

    this->MainLayout->setSpacing(0);
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
}


/* If the 'order' is -1, then insert this item to the bottom */
void parted_widget::InsertPartitions(partition_item *Item,int order){
    if (order == -1)
        order = this->PartitionsMap->size();
    this->PartitionsMap->insert(order,Item);
    this->MainLayout->insertWidget(order,Item);
    this->resize(this->width(),this->height()+PARTITION_HEIGTH);
    return;
}
