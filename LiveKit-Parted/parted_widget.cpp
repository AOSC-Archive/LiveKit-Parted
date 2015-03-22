#include "parted_widget.h"


partition_item::partition_item(QWidget *parent):
    QWidget(parent){
    this->Title     = new QLabel(this);

    this->Title->setText(tr("Title desu"));
    this->setStyleSheet("background-color:green;");
}

partition_item::~partition_item(){
    delete  this->Title;
}



parted_widget::parted_widget(QWidget *parent):
    QWidget(parent){
    MainLayout      = new QVBoxLayout;
    ItemsMap        = new partitions_map;

    this->resize(300,100);
    this->setLayout(MainLayout);


    this->MainLayout->setSpacing(0);
    this->MainLayout->addWidget(new partition_item());
    this->MainLayout->addWidget(new partition_item());
    this->MainLayout->addWidget(new partition_item());
    this->MainLayout->addWidget(new partition_item());
    this->MainLayout->addWidget(new partition_item());
}

parted_widget::~parted_widget(){
    delete  this->MainLayout;
}
