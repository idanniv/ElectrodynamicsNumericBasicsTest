#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Integrators/borisPush.h"
#include "Integrators/yee1dVacuum.h"
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void computeBoris();
    void computeVacuum();
    void plotParticle();
    void plotField();
    void fillAxisVec(Boris::Array&,QVector<double>&,QComboBox&);

public slots:
    void recomputeNeeded(void);

    void setEnabledInput(bool enable);
    void fillBorisAxes();
    void fillVacuumAxes();
private slots:
    void on_comboBoxXAxis_currentIndexChanged(int index);

    void on_comboBoxYAxis_currentIndexChanged(int index);

//    void on_sliceSlider_valueChanged(int value);

    void on_sliceSlider_rangeChanged(int min, int max);

    void on_actionRecompute_triggered();

    void on_timeNSpinBox_valueChanged(int arg1);

    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;
    QVector<double> BorisX, BorisY;
    enum class BorisAxis{
        X=0,Y,Z,Vx,Vy,Vz,Time
    };
    BorisAxis x,y;
    bool BorisComputeNeeded;
    Boris::Array BorisData;
    std::vector<double> BorisTime;
    QVector<double> VacuumX1,VacuumX2, VacuumY1, VacuumY2;
    std::vector<double> VacuumTime,VacuumPosition;
    size_t VacuumSlice;
    bool VacuumComputeNeeded;
    Vacuum::Data VacuumData;
    bool recomputeActivated=true,recomputeNeededBool=false;
    QList<QString>axes{"X[m]","Y[m]","Z[m]","Vx[m/s]","Vy[m/s]","Vz[m/s]","Time[s]"};

};
#endif // MAINWINDOW_H
