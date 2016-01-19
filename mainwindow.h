#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QComboBox>

#include "telemetry.h"
#include "serialthread.h"

#define PWM_OUT_PITCH           0x00
#define PWM_OUT_ROLL            0x01

#define PWM_OUT_FLAG_REVERSE    0x01
#define PWM_OUT_FLAG_USE_THI    0x02
#define PWM_OUT_FLAG_DISABLED   0x04

typedef struct tagOutputSettings
{
    quint8 power;
    quint8 flags;
} __attribute__((packed)) OutputSettings, *POutputSettings;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void serialPortConnect();
    void serialPortError(const QString &s);
    void serialPortTimeout(const QString &s);
    void processTelemetryMessage(const TelemetryMessage &msg);
    void getSettings();
    void setSettings();
    void storeSettings();
    void setPitchSpeed();
    void setRollSpeed();

private:
    void fillSerialPortInfo();
    void sendTelemetryMessage(const TelemetryMessage &msg);
    void setOutputSettings();
    void getOutputSettings();

private:
    Ui::MainWindow *ui;
    QComboBox *m_serialPortList;
    SerialThread m_serialThread;
    bool m_serialConnected;
    TelemetryMessage m_msg;
    OutputSettings m_outSettings[2];
};

#endif // MAINWINDOW_H
