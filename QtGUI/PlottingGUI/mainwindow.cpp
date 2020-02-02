#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Integrators/borisPush.h"
#include "Integrators/yee1dVacuum.h"
#include "QCustomPlot/qcustomplot.h"
#include <algorithm>
#include <tuple>
#include <string>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    VacuumComputeNeeded=true;
    BorisComputeNeeded=true;
    //ui->fieldPlot->yAxis->setScaleRatio(ui->fieldPlot->xAxis,1.0);
    ui->fieldPlot->addGraph();
    ui->fieldPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->fieldPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->fieldPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->fieldPlot->graph(0)->setName("E[V/M]");
    ui->fieldPlot->addGraph();
    ui->fieldPlot->graph(1)->setPen(QPen(Qt::red));
    ui->fieldPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->fieldPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->fieldPlot->graph(1)->setName("c*B[V/M]");
    ui->fieldPlot->setInteraction(QCP::iRangeDrag, true);
    ui->fieldPlot->setInteraction(QCP::iRangeZoom, true);
    ui->fieldPlot->xAxis->setLabel("Position[m]");
    ui->fieldPlot->yAxis->setLabel("Field strength");
    ui->fieldPlot->legend->setVisible(true);
    ui->fieldPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    ui->fieldPlot->legend->setFont(legendFont);
    ui->fieldPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    ui->fieldPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    ui->particlePlot->addGraph();
    ui->particlePlot->graph(0)->setPen(QPen(Qt::blue));
    ui->particlePlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->particlePlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->particlePlot->setInteraction(QCP::iRangeDrag, true);
    ui->particlePlot->setInteraction(QCP::iRangeZoom, true);

    x=BorisAxis::Time;
    y=BorisAxis::X;

    VacuumSlice=0;
    ui->sliceSlider->setMaximum(ui->lengthKSpinBox->value());
    ui->sliceSlider->setValue(0);

    connect(ui->deltaSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->ratioSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->timeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(recomputeNeeded()));
    connect(ui->xSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->ySpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->zSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->vxSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->vySpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->vzSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->exSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->eySpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->ezSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->bxSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->bySpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->bzSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));

    connect(ui->timeSpinBox_2,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    connect(ui->wavenumberSpinBox,SIGNAL(valueChanged(int)),this,SLOT(recomputeNeeded()));
    connect(ui->lengthSpinBox,SIGNAL(valueChanged(double)),this,SLOT(recomputeNeeded()));
    //connect(ui->timeNSpinBox,SIGNAL(editingFinished()),this,SLOT(recomputeNeeded()));
    connect(ui->lengthKSpinBox,SIGNAL(valueChanged(int)),this,SLOT(recomputeNeeded()));
    connect(ui->sliceSlider,SIGNAL(valueChanged(int)),this,SLOT(recomputeNeeded()));

    plotParticle();
    plotField();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setEnabledInput(bool enable){
    QList<QDoubleSpinBox *> list1 = this->findChildren<QDoubleSpinBox *>();
    for (int i = 0; i < list1.count(); i++){
        list1.at(i)->setEnabled(enable);
    }

    QList<QSpinBox *> list2 = this->findChildren<QSpinBox *>();
    for (int i = 0; i < list2.count(); i++){
        list2.at(i)->setEnabled(enable);
    }
    QList<QComboBox *> list3 = this->findChildren<QComboBox *>();
    for (int i = 0; i < list3.count(); i++){
        list3.at(i)->setEnabled(enable);
    }
}

void MainWindow::computeBoris(){
        BorisData=Boris::Compute(ui->timeSpinBox->value(),ui->deltaSpinBox->value(),ui->ratioSpinBox->value(),
                  Boris::StateVector{Boris::Vector{ui->xSpinBox->value(),ui->ySpinBox->value(),ui->zSpinBox->value()},
                                     Boris::Vector{ui->vxSpinBox->value(),ui->vySpinBox->value(),ui->vzSpinBox->value()}},
                  Boris::StateVector{Boris::Vector{ui->exSpinBox->value(),ui->eySpinBox->value(),ui->ezSpinBox->value()},
                                     Boris::Vector{ui->bxSpinBox->value(),ui->bySpinBox->value(),ui->bzSpinBox->value()}});
    BorisComputeNeeded=false;
    BorisTime.clear();
    BorisTime.assign(ui->timeSpinBox->value(),0.0);
    std::generate(BorisTime.begin(),BorisTime.end(),[i=0,d=ui->deltaSpinBox->value()]() mutable {return(d*(i++));});
}

void MainWindow::computeVacuum(){
    VacuumData=Vacuum::Compute(ui->timeNSpinBox->value(),ui->lengthKSpinBox->value(),
                              (ui->timeSpinBox_2->value())/c/2.0,ui->lengthSpinBox->value(),
                               (ui->wavenumberSpinBox->value())*2.0*pi/(ui->lengthKSpinBox->value())/ui->lengthSpinBox->value());
    VacuumComputeNeeded=false;
    VacuumTime.clear();
    VacuumTime.assign(ui->timeNSpinBox->value(),0.0);
    VacuumPosition.clear();
    VacuumPosition.assign(ui->lengthKSpinBox->value(),0.0);
    std::generate(VacuumTime.begin(),VacuumTime.end(),[i=0,d=ui->timeSpinBox_2->value()]() mutable {return(d*(i++));});
    std::generate(VacuumPosition.begin(),VacuumPosition.end(),[i=0,d=ui->lengthSpinBox->value()]() mutable {return(d*(i++));});
}

void MainWindow::fillBorisAxes(){
    BorisX.clear();
    BorisY.clear();
//    VacuumY1.clear();
    if(x==BorisAxis::Time){
        for(size_t i=0;i<BorisTime.size();++i){
            BorisX.append(BorisTime[i]);
        }
    }
    else{
        for(size_t i=0;i<BorisData.size();++i){
            BorisX.append(BorisData[i][((size_t)x)/3][((size_t)x)%3]);
        }
    }
    if(y==BorisAxis::Time){
        for(size_t i=0;i<BorisTime.size();++i){
            BorisY.append(BorisTime[i]);
        }
    }
    else{
        for(size_t i=0;i<BorisData.size();++i){
            BorisY.append(BorisData[i][((size_t)y)/3][((size_t)y)%3]);
        }
    }
//    for(size_t i=0;i<BorisTime.size();++i){
//        VacuumY1.append(-BorisTime[i]*8);
//    }
}

void MainWindow::fillVacuumAxes(){
    VacuumX1.clear();
    VacuumX2.clear();
    VacuumY1.clear();
    VacuumY2.clear();
    for(size_t i=0;i<VacuumPosition.size();++i){
        VacuumX1.append(VacuumPosition[i]);
        VacuumX2.append(VacuumPosition[i]+(ui->lengthSpinBox->value())/2.0);
        VacuumY1.append(VacuumData[ui->sliceSlider->value()][i][ELECTRIC]);
        VacuumY2.append(VacuumData[ui->sliceSlider->value()][i][MAGNETIC]*c);
    }
}

void MainWindow::plotParticle()
{
    if(BorisComputeNeeded){
        computeBoris();
    }
    fillBorisAxes();
    ui->particlePlot->xAxis->setLabel(axes.at((size_t)x));
    ui->particlePlot->yAxis->setLabel(axes.at((size_t)y));
    ui->particlePlot->graph(0)->setData(BorisX,BorisY);
//    ui->particlePlot->addGraph();
//    ui->particlePlot->graph(1)->setData(BorisX,VacuumY1);
//    ui->particlePlot->graph(1)->setPen(QPen(Qt::red));
//    QPen redDotPen;
//    redDotPen.setStyle(Qt::DashLine);
//    redDotPen.setColor(Qt::red);
//    ui->particlePlot->graph(1)->setPen(redDotPen);
    auto range=std::minmax_element(BorisY.begin(),BorisY.end());
    ui->particlePlot->yAxis->setRange(*range.first-0.1,*range.second+0.1);
    range=std::minmax_element(BorisX.begin(),BorisX.end());
    ui->particlePlot->xAxis->setRange(*range.first,*range.second);
    ui->particlePlot->xAxis->setScaleRatio(ui->particlePlot->yAxis,1.0);
    ui->particlePlot->replot();
    ui->particlePlot->update();
}

void MainWindow::plotField()
{
    if(VacuumComputeNeeded){
        computeVacuum();
    }
    fillVacuumAxes();
    ui->fieldPlot->graph(0)->setData(VacuumX1,VacuumY1);
    ui->fieldPlot->graph(1)->setData(VacuumX2,VacuumY2);
    auto range1=std::minmax_element(VacuumY1.begin(),VacuumY1.end());
    auto range2=std::minmax_element(VacuumY2.begin(),VacuumY2.end());
    ui->fieldPlot->yAxis->setRange(std::min(*range1.first,*range2.first)-0.1,std::max(*range1.second,*range2.second)+0.1);
    ui->fieldPlot->xAxis->setRange(0.0,VacuumPosition.back()+ui->lengthSpinBox->value());
    ui->fieldPlot->replot();
    ui->fieldPlot->update();
}

void MainWindow::recomputeNeeded()
{
    on_actionRecompute_triggered();
}

void MainWindow::on_comboBoxXAxis_currentIndexChanged(int index)
{
    x=(BorisAxis)index;
    plotParticle();
}

void MainWindow::on_comboBoxYAxis_currentIndexChanged(int index)
{
    y=(BorisAxis)index;
    plotParticle();
}

//void MainWindow::on_sliceSlider_valueChanged(int value)
//{
//    VacuumSlice=value;
//    plotField();
//}

void MainWindow::on_sliceSlider_rangeChanged(int min, int max)
{
    VacuumSlice=std::min(VacuumSlice,(size_t)max);
    ui->sliceSlider->setValue(VacuumSlice);
}

void MainWindow::on_timeNSpinBox_valueChanged(int arg1)
{
    VacuumComputeNeeded=true;
    ui->sliceSlider->setMaximum(arg1);
    plotField();
}

void MainWindow::on_actionRecompute_triggered()
{
    if(recomputeActivated){
        do{
            recomputeNeededBool=false;
            if(ui->tabWidget->currentIndex()==0){
                BorisComputeNeeded=true;
                plotParticle();
            }
            else{
                VacuumComputeNeeded=true;
                plotField();
            }
        }while(recomputeNeededBool);
    }else{
        recomputeNeededBool=true;
    }
}

void MainWindow::on_actionSave_triggered()
{
    static int i=0;
    QString filename=QStringList{"figure",QString::fromStdString(std::to_string(i++)),".pdf"}.join("");
    if(ui->tabWidget->currentIndex()==0){
        ui->particlePlot->savePdf(filename,0,0,QCP::epAllowCosmetic,"Idan Niv","Particle Plot");
    }
    else{
        ui->fieldPlot->savePdf(filename,0,0,QCP::epAllowCosmetic,"Idan Niv","Field Plot");
    }
}
