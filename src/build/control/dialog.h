#ifndef DIALOG_H
#define DIALOG_H

// Qt includes
#include <QDialog>

// Project inclues
#include <control_worker_wrapper.hpp>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

public slots:
    void onControlStateChanged(DeviceStatus oldState, DeviceStatus newState);
    void onControlSubStateChanged(ControlWorkerSubState oldState,
                             ControlWorkerSubState newState);

private:
    Ui::Dialog *ui;
    ControlWorkerWrapper controlWorkerWrapper;
};

#endif // DIALOG_H
