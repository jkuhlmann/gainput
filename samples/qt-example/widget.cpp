#include "widget.h"


Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , _gamepad(Q_NULLPTR)
{
    setupUi(this);

    _labelDefault = _labelA->palette();
    _labelActive = _labelDefault;
    _labelActive.setColor(QPalette::Window, Qt::green);

    connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

    _gamepad = _manager.CreateAndGetDevice<gainput::InputDevicePad>();

    if (_gamepad)
        _timer.start(1);
}

void Widget::onTimerTimeout()
{
    if (!_gamepad)
    {
        _timer.stop();
        return;
    }

    _manager.Update();

    if (!_gamepad->IsAvailable())
    {
        _labelL1->setPalette(_labelDefault);
        _labelR1->setPalette(_labelDefault);

        _progressL2->setValue(0);
        _progressR2->setValue(0);

        _labelA->setPalette(_labelDefault);
        _labelB->setPalette(_labelDefault);
        _labelX->setPalette(_labelDefault);
        _labelY->setPalette(_labelDefault);

        _labelTop->setPalette(_labelDefault);
        _labelBottom->setPalette(_labelDefault);
        _labelRight->setPalette(_labelDefault);
        _labelLeft->setPalette(_labelDefault);

        _labelL3->setPalette(_labelDefault);
        _progressL3T->setValue(0);
        _progressL3B->setValue(0);
        _progressL3R->setValue(0);
        _progressL3L->setValue(0);


        _labelR3->setPalette(_labelDefault);
        _progressR3T->setValue(0);
        _progressR3B->setValue(0);
        _progressR3R->setValue(0);
        _progressR3L->setValue(0);

        _labelSelect->setPalette(_labelDefault);
        _labelStart->setPalette(_labelDefault);
    }

    _labelL1->setPalette(_gamepad->GetBool(gainput::PadButtonL1) ? _labelActive : _labelDefault);
    _labelR1->setPalette(_gamepad->GetBool(gainput::PadButtonR1) ? _labelActive : _labelDefault);

    _progressL2->setValue(static_cast<int>(_gamepad->GetFloat(gainput::PadButtonAxis4) * 100.0f));
    _progressR2->setValue(static_cast<int>(_gamepad->GetFloat(gainput::PadButtonAxis5) * 100.0f));

    _labelA->setPalette(_gamepad->GetBool(gainput::PadButtonA) ? _labelActive : _labelDefault);
    _labelB->setPalette(_gamepad->GetBool(gainput::PadButtonB) ? _labelActive : _labelDefault);
    _labelX->setPalette(_gamepad->GetBool(gainput::PadButtonX) ? _labelActive : _labelDefault);
    _labelY->setPalette(_gamepad->GetBool(gainput::PadButtonY) ? _labelActive : _labelDefault);

    _labelTop->setPalette(_gamepad->GetBool(gainput::PadButtonUp) ? _labelActive : _labelDefault);
    _labelBottom->setPalette(_gamepad->GetBool(gainput::PadButtonDown) ? _labelActive : _labelDefault);
    _labelRight->setPalette(_gamepad->GetBool(gainput::PadButtonRight) ? _labelActive : _labelDefault);
    _labelLeft->setPalette(_gamepad->GetBool(gainput::PadButtonLeft) ? _labelActive : _labelDefault);

    // Left Stick
    _labelL3->setPalette(_gamepad->GetBool(gainput::PadButtonL3) ? _labelActive : _labelDefault);

    float l3Y = _gamepad->GetFloat(gainput::PadButtonLeftStickY);

    if (l3Y > 0.0f)
    {
        _progressL3T->setValue(static_cast<int>(l3Y * 100.0f));
        _progressL3B->setValue(0);
    }
    else
    {
        _progressL3T->setValue(0);
        _progressL3B->setValue(static_cast<int>(l3Y * -100.0f));
    }

    float l3X = _gamepad->GetFloat(gainput::PadButtonLeftStickX);

    if (l3X > 0.0f)
    {
        _progressL3R->setValue(static_cast<int>(l3X * 100.0f));
        _progressL3L->setValue(0);
    }
    else
    {
        _progressL3R->setValue(0);
        _progressL3L->setValue(static_cast<int>(l3X * -100.0f));
    }

    // Right Stick
    _labelR3->setPalette(_gamepad->GetBool(gainput::PadButtonR3) ? _labelActive : _labelDefault);

    float r3Y = _gamepad->GetFloat(gainput::PadButtonRightStickY);

    if (r3Y > 0.0f)
    {
        _progressR3T->setValue(static_cast<int>(r3Y * 100.0f));
        _progressR3B->setValue(0);
    }
    else
    {
        _progressR3T->setValue(0);
        _progressR3B->setValue(static_cast<int>(r3Y * -100.0f));
    }

    float r3X = _gamepad->GetFloat(gainput::PadButtonRightStickX);

    if (r3X > 0.0f)
    {
        _progressR3R->setValue(static_cast<int>(r3X * 100.0f));
        _progressR3L->setValue(0);
    }
    else
    {
        _progressR3R->setValue(0);
        _progressR3L->setValue(static_cast<int>(r3X * -100.0f));
    }

    _labelSelect->setPalette(_gamepad->GetBool(gainput::PadButtonSelect) ? _labelActive : _labelDefault);
    _labelStart->setPalette(_gamepad->GetBool(gainput::PadButtonStart) ? _labelActive : _labelDefault);
}
