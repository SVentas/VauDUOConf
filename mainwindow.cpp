#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serialPortList(new QComboBox),
    m_serialConnected(false)
{
    ui->setupUi(this);

    m_serialPortList->setMinimumWidth(250);
    ui->mainToolBar->insertWidget(ui->actionConnect, m_serialPortList);

    fillSerialPortInfo();

    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(serialPortConnect()));
    connect(ui->actionGet, SIGNAL(triggered()), this, SLOT(getSettings()));
    connect(ui->actionSet, SIGNAL(triggered()), this, SLOT(setSettings()));
    connect(ui->actionStore, SIGNAL(triggered()), this, SLOT(storeSettings()));
    //connect(ui->actionBootloader, SIGNAL(triggered()), this, SLOT(ToBootloader()));
    connect(&m_serialThread, SIGNAL(serialError(QString)), this, SLOT(serialPortError(QString)));
    connect(&m_serialThread, SIGNAL(serialTimeout(QString)), this, SLOT(serialPortTimeout(QString)));
    connect(&m_serialThread, SIGNAL(serialDataReady(TelemetryMessage)), this, SLOT(processTelemetryMessage(TelemetryMessage)));
    connect(ui->sliderSpeedPitch, SIGNAL(valueChanged(int)), this, SLOT(setPitchSpeed()));
    connect(ui->sliderSpeedRoll, SIGNAL(valueChanged(int)), this, SLOT(setRollSpeed()));

    m_outSettings[PWM_OUT_PITCH].power = 1;
    m_outSettings[PWM_OUT_PITCH].flags = 0;
    m_outSettings[PWM_OUT_ROLL].power  = 1;
    m_outSettings[PWM_OUT_ROLL].flags  = 0;
    setOutputSettings();
}

MainWindow::~MainWindow()
{
    if (m_serialConnected)
    {
        /* Disconnect from serial port. */
        serialPortConnect();
    }
    delete ui;
}


/**
 * @brief MainWindow::serialPortInfoFill
 */
void MainWindow::fillSerialPortInfo()
{
    m_serialPortList->clear();
    if (QSerialPortInfo::availablePorts().count() == 0)
    {
        m_serialPortList->addItem(tr("Serial port not detected"), "None");
        m_serialPortList->setEnabled(false);
        ui->actionConnect->setEnabled(false);
    }
    else
    {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        {
            m_serialPortList->addItem(info.description() + ' ' + '(' + info.portName() + ')', info.portName());
        }
        m_serialPortList->setEnabled(true);
        ui->actionConnect->setEnabled(true);
    }
}

/**
 * @brief MainWindow::serialPortConnect
 */
void MainWindow::serialPortConnect()
{
    if (m_serialConnected)
    {
        m_serialThread.disconnect();
        ui->actionConnect->setText(tr("Connect"));
        ui->actionGet->setEnabled(false);
        ui->actionSet->setEnabled(false);
        ui->actionStore->setEnabled(false);
        m_serialPortList->setEnabled(true);
        ui->sliderSpeedPitch->setEnabled(false);
        ui->sliderSpeedRoll->setEnabled(false);
        ui->statusBar->showMessage(tr("Disconnected from: %1").arg(m_serialPortList->currentText()));
        m_serialConnected = false;
    }
    else
    {
        m_serialThread.connect(m_serialPortList->currentData().toString());
        ui->actionConnect->setText(tr("Disconnect"));
        ui->actionGet->setEnabled(true);
        ui->actionSet->setEnabled(true);
        ui->actionStore->setEnabled(true);
        m_serialPortList->setEnabled(false);
        ui->sliderSpeedPitch->setEnabled(true);
        ui->sliderSpeedRoll->setEnabled(true);
        ui->statusBar->showMessage(tr("Connected to: %1").arg(m_serialPortList->currentText()));
        m_serialConnected = true;
    }
}

/**
 * @brief MainWindow::serialPortError
 * @param s - error string;
 */
void MainWindow::serialPortError(const QString &s)
{
    if (m_serialConnected)
    {
        ui->actionConnect->setText(tr("Connect"));
        ui->actionGet->setEnabled(false);
        ui->actionSet->setEnabled(false);
        ui->actionStore->setEnabled(false);
        m_serialPortList->setEnabled(true);
        ui->sliderSpeedPitch->setEnabled(false);
        ui->sliderSpeedRoll->setEnabled(false);
        m_serialConnected = false;
        ui->statusBar->showMessage(s);
    }
}

/**
 * @brief MainWindow::serialPortTimeout
 * @param s - error string;
 */
void MainWindow::serialPortTimeout(const QString &s)
{
    if (m_serialConnected)
    {
        ui->actionConnect->setText(tr("Connect"));
        ui->actionGet->setEnabled(false);
        ui->actionSet->setEnabled(false);
        ui->actionStore->setEnabled(false);
        m_serialPortList->setEnabled(true);
        ui->sliderSpeedPitch->setEnabled(false);
        ui->sliderSpeedRoll->setEnabled(false);
        m_serialConnected = false;
        ui->statusBar->showMessage(s);
    }
}

/**
 * @brief MainWindow::writeTelemetryMessage
 * @param msg - telemetry message to be send.
 */
void MainWindow::sendTelemetryMessage(const TelemetryMessage &msg)
{
    QByteArray data;
    data.append((char *)&msg, msg.data_size + TELEMETRY_MSG_HDR_SIZE);
    m_serialThread.write(data);
}

/**
 * @brief MainWindow::setOutputSettings
 */
void MainWindow::setOutputSettings()
{
    /* Pitch: */
    if (m_outSettings[PWM_OUT_PITCH].flags & PWM_OUT_FLAG_REVERSE) {
        ui->checkRevPitch->setChecked(true);
    } else {
        ui->checkRevPitch->setChecked(false);
    }
    if (m_outSettings[PWM_OUT_PITCH].flags & PWM_OUT_FLAG_USE_THI) {
        ui->checkTHIPitch->setChecked(true);
    } else {
        ui->checkTHIPitch->setChecked(false);
    }
    if (m_outSettings[PWM_OUT_PITCH].flags & PWM_OUT_FLAG_DISABLED) {
        ui->checkDisablePitch->setChecked(true);
    } else {
        ui->checkDisablePitch->setChecked(false);
    }
    ui->spinPowerPitch->setValue(m_outSettings[PWM_OUT_PITCH].power);

    /* Roll : */
    if (m_outSettings[PWM_OUT_ROLL].flags & PWM_OUT_FLAG_REVERSE) {
        ui->checkRevRoll->setChecked(true);
    } else {
        ui->checkRevRoll->setChecked(false);
    }
    if (m_outSettings[PWM_OUT_ROLL].flags & PWM_OUT_FLAG_USE_THI) {
        ui->checkTHIRoll->setChecked(true);
    } else {
        ui->checkTHIRoll->setChecked(false);
    }
    if (m_outSettings[PWM_OUT_ROLL].flags & PWM_OUT_FLAG_DISABLED) {
        ui->checkDisableRoll->setChecked(true);
    } else {
        ui->checkDisableRoll->setChecked(false);
    }
    ui->spinPowerRoll->setValue(m_outSettings[PWM_OUT_ROLL].power);
}

/**
 * @brief MainWindow::setOutputSettings
 */
void MainWindow::getOutputSettings()
{
    /* Pitch: */
    if (ui->checkRevPitch->isChecked()) {
        m_outSettings[PWM_OUT_PITCH].flags |= PWM_OUT_FLAG_REVERSE;
    } else {
        m_outSettings[PWM_OUT_PITCH].flags &= ~PWM_OUT_FLAG_REVERSE;
    }

    if (ui->checkTHIPitch->isChecked()) {
        m_outSettings[PWM_OUT_PITCH].flags |= PWM_OUT_FLAG_USE_THI;
    } else {
        m_outSettings[PWM_OUT_PITCH].flags &= ~PWM_OUT_FLAG_USE_THI;
    }

    if (ui->checkDisablePitch->isChecked()) {
        m_outSettings[PWM_OUT_PITCH].flags |= PWM_OUT_FLAG_DISABLED;
    } else {
        m_outSettings[PWM_OUT_PITCH].flags &= ~PWM_OUT_FLAG_DISABLED;
    }
    m_outSettings[PWM_OUT_PITCH].power = (quint8)ui->spinPowerPitch->value();

    /* Roll: */
    if (ui->checkRevRoll->isChecked()) {
        m_outSettings[PWM_OUT_ROLL].flags |= PWM_OUT_FLAG_REVERSE;
    } else {
        m_outSettings[PWM_OUT_ROLL].flags &= ~PWM_OUT_FLAG_REVERSE;
    }

    if (ui->checkTHIRoll->isChecked()) {
        m_outSettings[PWM_OUT_ROLL].flags |= PWM_OUT_FLAG_USE_THI;
    } else {
        m_outSettings[PWM_OUT_ROLL].flags &= ~PWM_OUT_FLAG_USE_THI;
    }

    if (ui->checkDisableRoll->isChecked()) {
        m_outSettings[PWM_OUT_ROLL].flags |= PWM_OUT_FLAG_DISABLED;
    } else {
        m_outSettings[PWM_OUT_ROLL].flags &= ~PWM_OUT_FLAG_DISABLED;
    }
    m_outSettings[PWM_OUT_ROLL].power = (quint8)ui->spinPowerRoll->value();
    qDebug() << "PITCH power:" << m_outSettings[PWM_OUT_PITCH].power \
             << "flags:" << m_outSettings[PWM_OUT_PITCH].flags;
    qDebug() << "ROLL power:" << m_outSettings[PWM_OUT_ROLL].power \
             << "flags:" << m_outSettings[PWM_OUT_ROLL].flags;
}

/**
 * @brief MainWindow::processTelemetryMessage
 * @param msg - telemetry message to be processed.
 */
void MainWindow::processTelemetryMessage(const TelemetryMessage &msg)
{
    quint32 tmp = 0;
    switch (msg.msg_id)
    {
    case 'a':
        if (msg.data_size == sizeof(OutputSettings)) {
          memcpy((void *)&m_outSettings[PWM_OUT_PITCH], (void *)msg.data, msg.data_size);
          setOutputSettings();
        } else {
            qDebug() << "Data size mismatch! Expected" << sizeof(OutputSettings) << "bytes" \
                     << "received" << msg.data_size << "bytes.";
        }
        break;
    case 'c':
        if (msg.data_size == sizeof(OutputSettings)) {
          memcpy((void *)&m_outSettings[PWM_OUT_ROLL], (void *)msg.data, msg.data_size);
          setOutputSettings();
        } else {
            qDebug() << "Data size mismatch! Expected" << sizeof(OutputSettings) << "bytes" \
                     << "received" << msg.data_size << "bytes.";
        }
        break;
    case 'b':
    case 'd':
        if (msg.data_size == sizeof(quint32)) {
            memcpy((void *)&tmp, (void *)msg.data, sizeof(tmp));
            qDebug() << "Speed:" << tmp;
        } else {
            qDebug() << "Data size mismatch! Expected" << sizeof(quint32) << "bytes" \
                     << "received" << msg.data_size << "bytes.";
        }
        break;
    case 'i':
        memcpy((void *)&tmp, (void *)msg.data, sizeof(tmp));
        qDebug() << "Size" << msg.data_size << "data" << hex << tmp << "(hex)";
        break;
    default:
        qDebug() << "Unhandled message received!";
    }
}

/**
 * @brief MainWindow::getSettings
 */
void MainWindow::getSettings()
{
    /* Get MOT1 settings. */
    m_msg.msg_id    = 'a';
    m_msg.data_size = 0;
    sendTelemetryMessage(m_msg);
    /* Get MOT2 settings. */
    m_msg.msg_id    = 'c';
    m_msg.data_size = 0;
    sendTelemetryMessage(m_msg);
}

/**
 * @brief MainWindow::setSettings
 */
void MainWindow::setSettings()
{
    getOutputSettings();

    m_msg.msg_id    = 'A';
    m_msg.data_size = sizeof(OutputSettings);
    memcpy((void *)m_msg.data, (void *)&m_outSettings[PWM_OUT_PITCH], m_msg.data_size);
    sendTelemetryMessage(m_msg);

    m_msg.msg_id    = 'C';
    m_msg.data_size = sizeof(OutputSettings);
    memcpy((void *)m_msg.data, (void *)&m_outSettings[PWM_OUT_ROLL], m_msg.data_size);
    sendTelemetryMessage(m_msg);
}

/**
 * @brief MainWindow::storeSettings
 */
void MainWindow::storeSettings()
{
    setSettings();

    /* Write to flash: */
    m_msg.msg_id    = 'W';
    m_msg.data_size = 0;
    sendTelemetryMessage(m_msg);
}

/**
 * @brief MainWindow::setPitchSpeed
 */
void MainWindow::setPitchSpeed()
{
    quint32 value   = ui->sliderSpeedPitch->value() * 64;
    m_msg.msg_id    = 'B';
    m_msg.data_size = sizeof(value);
    memcpy((void *)m_msg.data, (void *)&value, sizeof(value));
    sendTelemetryMessage(m_msg);
}

/**
 * @brief MainWindow::setRollSpeed
 */
void MainWindow::setRollSpeed()
{
    quint32 value   = ui->sliderSpeedRoll->value() * 64;
    m_msg.msg_id    = 'C';
    m_msg.data_size = sizeof(value);
    memcpy((void *)m_msg.data, (void *)&value, sizeof(value));
    sendTelemetryMessage(m_msg);
}
