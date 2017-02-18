#include "rotdialog.h"
#include "ui_rotdialog.h"
#include <iostream>

RotDialog::RotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotDialog)
{
    ui->setupUi(this);
    ui->clockDir->setChecked(true);
    ui->valLineEdit->setPlaceholderText("0");
    ui->valLineEdit->setValidator(new QDoubleValidator(0.000, 360.000, 3, this));
    direction = 0;
    angle = 0;
}

RotDialog::~RotDialog()
{
    delete ui;
}

void RotDialog::setClockwise(bool dir) {
    direction = dir;
}


void RotDialog::setAnticlockwise(bool dir) {
    direction = !dir;
}

void RotDialog::setAngle() {
    angle = ui->valLineEdit->text().toDouble();
}

int RotDialog::getDirection() {
    return (direction == 0) ? 1 : -1;
}

double RotDialog::getAngle() {
    return angle;
}
