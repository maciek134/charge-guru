#include <iostream>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"

std::vector<std::pair<QString, b6::BATTERY_TYPE>> MainWindow::m_batteryTypes = {
    { "Li-Po", b6::BATTERY_TYPE::LIPO },
    { "Li-Ion", b6::BATTERY_TYPE::LIIO },
    { "Li-Fe", b6::BATTERY_TYPE::LIFE },
    { "Li-Hv", b6::BATTERY_TYPE::LIHV },
    { "Ni-Mh", b6::BATTERY_TYPE::NIMH },
    { "Ni-Cd", b6::BATTERY_TYPE::NICD },
    { "Pb", b6::BATTERY_TYPE::PB },
};
std::vector<std::pair<QString, b6::CHARGING_MODE_LI>> MainWindow::m_chargingModesLi = {
    { "Standard", b6::CHARGING_MODE_LI::STANDARD },
    { "Discharge", b6::CHARGING_MODE_LI::DISCHARGE },
    { "Storage", b6::CHARGING_MODE_LI::STORAGE },
    { "Fast", b6::CHARGING_MODE_LI::FAST },
    { "Balance", b6::CHARGING_MODE_LI::BALANCE },
};
std::vector<std::pair<QString, b6::CHARGING_MODE_NI>> MainWindow::m_chargingModesNi = {
    { "Standard", b6::CHARGING_MODE_NI::STANDARD },
    { "Auto", b6::CHARGING_MODE_NI::AUTO },
    { "Discharge", b6::CHARGING_MODE_NI::DISCHARGE },
    { "Re-peak", b6::CHARGING_MODE_NI::REPEAK },
    { "Cycle", b6::CHARGING_MODE_NI::CYCLE },
};
std::vector<std::pair<QString, b6::CHARGING_MODE_PB>> MainWindow::m_chargingModesPb = {
    { "Charge", b6::CHARGING_MODE_PB::CHARGE },
    { "Discharge", b6::CHARGING_MODE_PB::DISCHARGE },
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(std::chrono::milliseconds(400));

    lblCore = new QLabel(this);
    lblCore->setText("NOT CONNECTED!");

    lblHW = new QLabel(this);
    lblHW->setText("");

    lblSW = new QLabel(this);
    lblSW->setText("");

    lblCells = new QLabel(this);
    lblCells->setText("");

    lblStatus = new QLabel(this);
    lblStatus->setText("STATUS: IDLE");

    ui->statusBar->addWidget(lblCore);
    ui->statusBar->addWidget(lblHW);
    ui->statusBar->addWidget(lblSW);
    ui->statusBar->addWidget(lblCells);
    ui->statusBar->addPermanentWidget(lblStatus);

    ui->cbBatteryType->clear();
    for (const auto &it : m_batteryTypes) {
        ui->cbBatteryType->addItem(QString(it.first), QVariant::fromValue(it.second));
    }

    ui->tbCells->setFocusPolicy(Qt::NoFocus);

    m_seriesCurrent = new QLineSeries();
    m_seriesCurrent->setName("Current (mA)");
    m_seriesCurrent->setColor(QColor(0xff, 0x00, 0x00));

    m_seriesVoltage = new QLineSeries();
    m_seriesVoltage->setName("Voltage (mV)");
    m_seriesVoltage->setColor(QColor(0x00, 0x00, 0xff));

    m_seriesCapacity = new QLineSeries();
    m_seriesCapacity->setName("Capacity (mAh)");
    m_seriesCapacity->setColor(QColor(0x00, 0xff, 0x00));

    m_seriesTempExt = new QLineSeries();
    m_seriesTempExt->setName("Temperature external");

    m_seriesTempInt = new QLineSeries();
    m_seriesTempInt->setName("Temperature internal");

    m_chartCurrent = new QChart();
    m_chartCurrent->addSeries(m_seriesCurrent);

    m_chartVoltage = new QChart();
    m_chartVoltage->addSeries(m_seriesVoltage);

    m_chartCapacity = new QChart();
    m_chartCapacity->addSeries(m_seriesCapacity);

    m_chartTempExt = new QChart();
    m_chartTempExt->addSeries(m_seriesTempExt);

    m_chartTempInt = new QChart();
    m_chartTempInt->addSeries(m_seriesTempInt);

    m_chartCurrent->createDefaultAxes();
    m_chartCurrent->axisY()->setRange(0.0, 6.0);
    m_chartCurrent->setTitle("Current (A)");
    m_chartCurrent->legend()->hide();

    m_chartVoltage->createDefaultAxes();
    m_chartVoltage->axisY()->setRange(0.0, 4.5);
    m_chartVoltage->setTitle("Voltage (V)");
    m_chartVoltage->legend()->hide();

    m_chartCapacity->createDefaultAxes();
    m_chartCapacity->axisY()->setRange(0, 6000);
    m_chartCapacity->setTitle("Capacity (mAh)");
    m_chartCapacity->legend()->hide();

    m_chartTempExt->createDefaultAxes();
    m_chartTempExt->axisY()->setRange(20, 80);
    m_chartTempExt->setTitle("Temperature External");
    m_chartTempExt->legend()->hide();

    m_chartTempInt->createDefaultAxes();
    m_chartTempInt->axisY()->setRange(20, 80);
    m_chartTempInt->setTitle("Temperature Internal");
    m_chartTempInt->legend()->hide();

    ui->ctCurrent->setChart(m_chartCurrent);
    ui->ctCurrent->setRenderHint(QPainter::Antialiasing);

    ui->ctVoltage->setChart(m_chartVoltage);
    ui->ctVoltage->setRenderHint(QPainter::Antialiasing);

    ui->ctCapacity->setChart(m_chartCapacity);
    ui->ctCapacity->setRenderHint(QPainter::Antialiasing);

    ui->ctTempExt->setChart(m_chartTempExt);
    ui->ctTempExt->setRenderHint(QPainter::Antialiasing);

    ui->ctTempInt->setChart(m_chartTempInt);
    ui->ctTempInt->setRenderHint(QPainter::Antialiasing);

    connect(ui->ckChartCurrent, SIGNAL(toggled(bool)), this, SLOT(onCkChartToggled(bool)));
    connect(ui->ckChartVoltage, SIGNAL(toggled(bool)), this, SLOT(onCkChartToggled(bool)));
    connect(ui->ckChartCapacity, SIGNAL(toggled(bool)), this, SLOT(onCkChartToggled(bool)));
    connect(ui->ckChartTempExt, SIGNAL(toggled(bool)), this, SLOT(onCkChartToggled(bool)));
    connect(ui->ckChartTempInt, SIGNAL(toggled(bool)), this, SLOT(onCkChartToggled(bool)));

    for (int i = 0; i < 8; i++) {
        m_cells[i] = ui->tbCells->item(0, i);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onTimer() {
    if (m_dev == nullptr) {
        m_createDevice();
    } else if (m_charging) {
        m_loadChargeInfo();
    }
}

void MainWindow::on_btLoad_clicked() {
    m_loadSysInfo();
}

void MainWindow::on_btSave_clicked() {
    m_saveSysInfo();
}

void MainWindow::on_btStartCharging_clicked() {
    m_startCharging();
}

void MainWindow::on_btStopCharging_clicked() {
    m_stopCharging();
}

void MainWindow::on_cbBatteryType_currentIndexChanged(int index) {
    b6::BATTERY_TYPE battType = ui->cbBatteryType->itemData(index).value<b6::BATTERY_TYPE>();
    ui->cbChargingMode->clear();

    if (b6::Device::isBatteryLi(battType)) {
        for (const auto &it : m_chargingModesLi) {
            ui->cbChargingMode->addItem(QString(it.first), QVariant::fromValue(it.second));
        }
    } else if (b6::Device::isBatteryNi(battType)) {
        for (const auto &it : m_chargingModesNi) {
            ui->cbChargingMode->addItem(QString(it.first), QVariant::fromValue(it.second));
        }
    } else {
        for (const auto &it : m_chargingModesPb) {
            ui->cbChargingMode->addItem(QString(it.first), QVariant::fromValue(it.second));
        }
    }

    m_updateUI();
}

void MainWindow::on_cbChargingMode_currentIndexChanged(int) {
    m_updateUI();
}

void MainWindow::on_sbCellCount_valueChanged(int value) {
    ui->lbCellCount->setText(QString("%1").arg(value));
}

void MainWindow::onCkChartToggled(bool value) {
    QString oName = ((QCheckBox *) sender())->objectName();
    if (oName == "ckChartCurrent") {
        ui->ctCurrent->setVisible(value);
    } else if (oName == "ckChartVoltage") {
        ui->ctVoltage->setVisible(value);
    } else if (oName == "ckChartCapacity") {
        ui->ctCapacity->setVisible(value);
    } else if (oName == "ckChartTempExt") {
        ui->ctTempExt->setVisible(value);
    } else if (oName == "ckChartTempInt") {
        ui->ctTempInt->setVisible(value);
    }
}


void MainWindow::m_createDevice() {
    try {
        m_dev = new b6::Device();
        lblCore->setText(QString("Core: %1").arg(QString::fromStdString(m_dev->getCoreType())));
        lblHW->setText(QString("HW: %1").arg(m_dev->getHWVersion(), 0, 'f', 2));
        lblSW->setText(QString("SW: %1").arg(m_dev->getSWVersion(), 0, 'f', 2));
        lblCells->setText(QString("Cells: %1").arg(m_dev->getCellCount()));

        ui->sbCellCount->setMaximum(m_dev->getCellCount());

        ui->gbChargingParameters->setEnabled(true);
        ui->gbSystemSettings->setEnabled(true);

        m_resizeCellTable();

        m_loadSysInfo();
        m_loadChargeInfo();
    } catch (std::runtime_error& e) {
        m_dev = nullptr;
    }
}

void MainWindow::m_loadSysInfo() {
    try {
        b6::SysInfo info = m_dev->getSysInfo();
        ui->ckTimeLimit->setCheckState(info.timeLimitOn ? Qt::Checked : Qt::Unchecked);
        ui->sbTimeLimit->setValue(info.timeLimit);

        ui->ckCapacityLimit->setCheckState(info.capLimitOn ? Qt::Checked : Qt::Unchecked);
        ui->sbCapacityLimit->setValue(info.capLimit);

        m_chartCapacity->axisY()->setRange(0, info.capLimitOn ? info.capLimit : 10000);

        ui->sbTemperatureLimit->setValue(info.tempLimit);
        ui->sbCycleTime->setValue(info.cycleTime);

        ui->ckKeyBuzzer->setCheckState(info.keyBuzzer ? Qt::Checked : Qt::Unchecked);
        ui->ckSystemBuzzer->setCheckState(info.systemBuzzer ? Qt::Checked : Qt::Unchecked);
    } catch (std::exception& e) {

    }
}

void MainWindow::m_loadChargeInfo() {
    try {
        b6::ChargeInfo info = m_dev->getChargeInfo();
        lblStatus->setText(QString("STATUS: %1").arg(info.state));

        QTime cTime(0, 0, 0);
        cTime = cTime.addSecs(info.time);

        if (info.state == static_cast<uint8_t>(b6::STATE::CHARGING)) {
            m_charging = true;
            m_updateUI();
        } else if (info.state == 0x03) {
            m_charging = false;
            QMessageBox::information(this, "Charging complete",
                                     QString("Charging completed in %1, capacity: %2 mAh.")
                                        .arg(cTime.toString("hh:mm:ss"))
                                        .arg(info.capacity));
        }

        if (m_charging) {
            double cCurrent = (double)(info.current) / 1000.0;
            double cVoltage = (double)(info.voltage) / 1000.0;
            m_seriesCurrent->append(info.time, cCurrent);
            m_seriesVoltage->append(info.time, cVoltage);
            m_seriesCapacity->append(info.time, info.capacity);
            m_seriesTempExt->append(info.time, info.tempExt);
            m_seriesTempInt->append(info.time, info.tempInt);

            if (cCurrent < m_minCurrent) m_minCurrent = cCurrent;
            if (cCurrent > m_maxCurrent) m_maxCurrent = cCurrent;
            if (cVoltage < m_minVoltage) m_minVoltage = cVoltage;
            if (cVoltage > m_maxVoltage) m_maxVoltage = cVoltage;
            if (info.capacity < m_minCapacity) m_minCapacity = info.capacity;
            if (info.capacity > m_maxCapacity) m_maxCapacity = info.capacity;
            if (info.tempExt < m_minTempExt) m_minTempExt = info.tempExt;
            if (info.tempExt > m_maxTempExt) m_maxTempExt = info.tempExt;
            if (info.tempInt < m_minTempInt) m_minTempInt = info.tempInt;
            if (info.tempInt > m_maxTempInt) m_maxTempInt = info.tempInt;

            m_chartCurrent->axisX()->setRange(0, info.time);
            m_chartCurrent->axisY()->setRange(std::max(0.0, m_minCurrent - 0.5), m_maxCurrent + 0.5);

            m_chartVoltage->axisX()->setRange(0, info.time);
            m_chartVoltage->axisY()->setRange(std::max(0.0, m_minVoltage - 0.5), m_maxVoltage + 0.5);

            m_chartCapacity->axisX()->setRange(0, info.time);
            m_chartCapacity->axisY()->setRange(std::max(0.0, m_minCapacity - 0.5), m_maxCapacity + 0.5);

            m_chartTempExt->axisX()->setRange(0, info.time);
            m_chartTempExt->axisY()->setRange(std::max(0.0, m_minTempExt - 0.5), m_maxTempExt + 0.5);

            m_chartTempInt->axisX()->setRange(0, info.time);
            m_chartTempInt->axisY()->setRange(std::max(0.0, m_minTempInt - 0.5), m_maxTempInt + 0.5);

            for (int i = 0; i < m_dev->getCellCount(); i++) {
                double cellV = (double)(info.cells[i]) / 1000.0;
                m_cells[i]->setText(QString("%1V").arg(cellV > 0.3 ? cellV : 0.0, 0, 'f', 3));
            }

            ui->lbChargeTime->setText(cTime.toString("hh:mm:ss"));
            ui->lbChargeCurrent->setText(QString("%1 A").arg((double)(info.current) / 1000.0, 0, 'f', 3));
            ui->lbChargeVoltage->setText(QString("%1 V").arg((double)(info.voltage) / 1000.0, 0, 'f', 3));
            ui->lbChargeCapacity->setText(QString("%1 mAh").arg(info.capacity));
            ui->lbChargeTempExt->setText(QString("%1°C").arg(info.tempExt));
            ui->lbChargeTempInt->setText(QString("%1°C").arg(info.tempInt));
        }
        return;
    } catch (b6::ChargingError& e) {
        QMessageBox::critical(this, "Error!", QString::fromStdString(e.message()));
    } catch (std::exception& e) {

    }
    m_stopCharging();
}

void MainWindow::m_updateUI() {
    if (m_charging) {
        ui->cbBatteryType->setEnabled(false);
        ui->cbChargingMode->setEnabled(false);
        ui->sbCellCount->setEnabled(false);
        ui->sbChargeCurrent->setEnabled(false);
        ui->sbDischargeCurrent->setEnabled(false);
        ui->sbCellDischargeVoltage->setEnabled(false);
        ui->sbEndVoltage->setEnabled(false);
        ui->sbRepeakCount->setEnabled(false);
        ui->sbCycleCount->setEnabled(false);

        m_chartCurrent->axisY()->setRange(0, (double)(ui->sbChargeCurrent->value()) / 1000.0 + 1.0);
        m_chartVoltage->axisY()->setRange(0, (double)(ui->sbEndVoltage->value()) *
                                             (double)(ui->sbCellCount->value()) / 1000.0 + 1.0);
    } else {
        ui->cbBatteryType->setEnabled(true);
        ui->cbChargingMode->setEnabled(true);
        b6::BATTERY_TYPE battType = ui->cbBatteryType->currentData().value<b6::BATTERY_TYPE>();
        if (b6::Device::isBatteryLi(battType)) {
            b6::CHARGING_MODE_LI mode = ui->cbChargingMode->currentData().value<b6::CHARGING_MODE_LI>();
            ui->sbChargeCurrent->setEnabled(mode != b6::CHARGING_MODE_LI::DISCHARGE);
            ui->sbDischargeCurrent->setEnabled(
                        mode == b6::CHARGING_MODE_LI::DISCHARGE ||
                        mode == b6::CHARGING_MODE_LI::STORAGE);
            ui->sbCellDischargeVoltage->setEnabled(mode == b6::CHARGING_MODE_LI::DISCHARGE);
            ui->sbEndVoltage->setEnabled(mode != b6::CHARGING_MODE_LI::DISCHARGE);
            ui->sbRepeakCount->setEnabled(false);
            ui->sbCycleCount->setEnabled(false);
        } else if (b6::Device::isBatteryNi(battType)) {
            b6::CHARGING_MODE_NI mode = ui->cbChargingMode->currentData().value<b6::CHARGING_MODE_NI>();
            ui->sbChargeCurrent->setEnabled(mode != b6::CHARGING_MODE_NI::DISCHARGE);
            ui->sbDischargeCurrent->setEnabled(
                        mode == b6::CHARGING_MODE_NI::DISCHARGE ||
                        mode == b6::CHARGING_MODE_NI::CYCLE);
            ui->sbCellDischargeVoltage->setEnabled(
                        mode == b6::CHARGING_MODE_NI::DISCHARGE ||
                        mode == b6::CHARGING_MODE_NI::CYCLE);
            ui->sbEndVoltage->setEnabled(mode != b6::CHARGING_MODE_NI::DISCHARGE);
            ui->sbRepeakCount->setEnabled(mode == b6::CHARGING_MODE_NI::REPEAK);
            ui->sbCycleCount->setEnabled(mode == b6::CHARGING_MODE_NI::CYCLE);
        } else {
            b6::CHARGING_MODE_PB mode = ui->cbChargingMode->currentData().value<b6::CHARGING_MODE_PB>();
            ui->sbChargeCurrent->setEnabled(mode == b6::CHARGING_MODE_PB::CHARGE);
            ui->sbDischargeCurrent->setEnabled(mode == b6::CHARGING_MODE_PB::DISCHARGE);
            ui->sbCellDischargeVoltage->setEnabled(mode == b6::CHARGING_MODE_PB::DISCHARGE);
            ui->sbEndVoltage->setEnabled(mode == b6::CHARGING_MODE_PB::CHARGE);
            ui->sbRepeakCount->setEnabled(false);
            ui->sbCycleCount->setEnabled(false);
        }
    }
    ui->btStartCharging->setEnabled(!m_charging);
    ui->btStopCharging->setEnabled(m_charging);
    ui->gbChargingInfo->setEnabled(m_charging);
}

void MainWindow::m_saveSysInfo() {
    try {
        m_dev->setTimeLimit(
                    ui->ckTimeLimit->checkState() == Qt::Checked,
                    ui->sbTimeLimit->value());
        m_dev->setCapacityLimit(
                    ui->ckCapacityLimit->checkState() == Qt::Checked,
                    ui->sbCapacityLimit->value());
        m_dev->setTempLimit(ui->sbTemperatureLimit->value());
        m_dev->setCycleTime(ui->sbCycleTime->value());
        m_dev->setBuzzers(
                    ui->ckSystemBuzzer->checkState() == Qt::Checked,
                    ui->ckKeyBuzzer->checkState() == Qt::Checked);



        m_chartCapacity->axisY()->setRange(0, ui->ckCapacityLimit->checkState() == Qt::Checked ?
                                               ui->sbCapacityLimit->value() : 10000);
    } catch (std::exception& e) {

    }
}

void MainWindow::m_startCharging() {
    try {
        b6::BATTERY_TYPE battType = ui->cbBatteryType->currentData().value<b6::BATTERY_TYPE>();
        b6::ChargeProfile profile = m_dev->getDefaultChargeProfile(battType);
        if (b6::Device::isBatteryLi(battType)) {
            profile.mode.li = ui->cbChargingMode->currentData().value<b6::CHARGING_MODE_LI>();
        } else if (b6::Device::isBatteryNi(battType)) {
            profile.mode.ni = ui->cbChargingMode->currentData().value<b6::CHARGING_MODE_NI>();
        } else {
            profile.mode.pb = ui->cbChargingMode->currentData().value<b6::CHARGING_MODE_PB>();
        }
        profile.cellCount = ui->sbCellCount->value();
        profile.chargeCurrent = ui->sbChargeCurrent->value();
        profile.dischargeCurrent = ui->sbDischargeCurrent->value();
        profile.cellDischargeVoltage = ui->sbCellDischargeVoltage->value();
        profile.endVoltage = ui->sbEndVoltage->value();
        profile.rPeakCount = ui->sbRepeakCount->value();
        profile.cycleCount = ui->sbCycleCount->value();

        m_seriesCurrent->clear();
        m_seriesVoltage->clear();
        m_seriesCapacity->clear();
        m_seriesTempExt->clear();
        m_seriesTempInt->clear();

        m_dev->startCharging(profile);
        m_charging = true;
        m_updateUI();
    } catch (std::exception& e) {

    }
}

void MainWindow::m_stopCharging() {
    try {
        m_dev->stopCharging();
        m_charging = false;
        m_updateUI();
    } catch (std::exception& e) {

    }
}

void MainWindow::m_resizeCellTable() {
    if (m_dev == nullptr) {
        return;
    }

    int cellWidth = ui->tbCells->width() / m_dev->getCellCount();
    for (int i = 0; i < m_dev->getCellCount(); i++) {
        ui->tbCells->horizontalHeader()->resizeSection(i, cellWidth);
    }
}

void MainWindow::resizeEvent(QResizeEvent*) {
    m_resizeCellTable();
}
