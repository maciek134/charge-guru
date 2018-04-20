#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QtCharts>
#include <QTableWidgetItem>
#include <b6/Device.hh>

using namespace QtCharts;

Q_DECLARE_METATYPE(b6::BATTERY_TYPE)
Q_DECLARE_METATYPE(b6::CHARGING_MODE_LI)
Q_DECLARE_METATYPE(b6::CHARGING_MODE_NI)
Q_DECLARE_METATYPE(b6::CHARGING_MODE_PB)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onTimer();

private slots:
    void on_btLoad_clicked();
    void on_btSave_clicked();
    void on_btStartCharging_clicked();
    void on_btStopCharging_clicked();
    void on_cbBatteryType_currentIndexChanged(int index);
    void on_cbChargingMode_currentIndexChanged(int);
    void on_sbCellCount_valueChanged(int value);
    void onCkChartToggled(bool value);

private:
    static std::vector<std::pair<QString, b6::BATTERY_TYPE>> m_batteryTypes;
    static std::vector<std::pair<QString, b6::CHARGING_MODE_LI>> m_chargingModesLi;
    static std::vector<std::pair<QString, b6::CHARGING_MODE_NI>> m_chargingModesNi;
    static std::vector<std::pair<QString, b6::CHARGING_MODE_PB>> m_chargingModesPb;

    QTimer *timer;
    Ui::MainWindow *ui;
    QLabel *lblCore, *lblSW, *lblHW, *lblCells;
    QLabel *lblStatus;
    QTableWidgetItem *m_cells[8];
    QChart *m_chartCurrent, *m_chartVoltage, *m_chartCapacity, *m_chartTempExt, *m_chartTempInt;
    QLineSeries *m_seriesCurrent, *m_seriesVoltage, *m_seriesCapacity,
                *m_seriesTempExt, *m_seriesTempInt;

    double m_minCurrent = 100.0, m_maxCurrent = 0.0,
           m_minVoltage = 100.0, m_maxVoltage = 0.0;
    int m_minCapacity = 10000, m_maxCapacity = 0,
        m_minTempExt = 80, m_maxTempExt = 0,
        m_minTempInt = 80, m_maxTempInt = 0;

    b6::Device *m_dev = nullptr;
    bool m_charging = false;

    void m_createDevice();
    void m_loadSysInfo();
    void m_loadChargeInfo();

    void m_updateUI();

    void m_saveSysInfo();
    void m_startCharging();
    void m_stopCharging();

    void m_resizeCellTable();
protected:
    void resizeEvent(QResizeEvent*);
};

#endif // MAINWINDOW_H
