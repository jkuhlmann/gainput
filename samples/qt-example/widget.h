#ifndef WIDGET_H
#define WIDGET_H


#include "ui_widget.h"

#include <QTimer>
#include <QPalette>

#include <gainput/gainput.h>


class Widget : public QWidget, private Ui::Widget
{
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = 0);

private slots:
    void onTimerTimeout();

private:
    QTimer _timer;
    QPalette _labelDefault;
    QPalette _labelActive;

    gainput::InputManager _manager;
    gainput::InputDevicePad* _gamepad;
};


#endif // WIDGET_H
