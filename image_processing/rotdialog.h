#ifndef ROTDIALOG_H
#define ROTDIALOG_H

#include <QDialog>

namespace Ui {
class RotDialog;
}

class RotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RotDialog(QWidget *parent = 0);
    ~RotDialog();

private slots:
    void setClockwise(bool);
    void setAnticlockwise(bool);
    void setAngle();

private:
    Ui::RotDialog *ui;
    int direction;
    double angle;

public:
    int getDirection();
    double getAngle();
};

#endif // ROTDIALOG_H
