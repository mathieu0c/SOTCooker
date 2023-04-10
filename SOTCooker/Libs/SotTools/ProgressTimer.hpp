#pragma once

#include <QTimer>
#include <QTime>

#include <QDebug>

namespace sot{

class ProgressTimer : public QObject{
    Q_OBJECT
public:
    ProgressTimer(QObject* parent) : QObject{parent}{
        connect(&m_total_timer,&QTimer::timeout,this,&ProgressTimer::OnTotalTimeout);
        m_total_timer.setSingleShot(true);
        connect(&m_progress_timer,&QTimer::timeout,this,&ProgressTimer::UpdateProgress);
    }

    const auto& Tim()const{
        return m_total_timer;
    }

    void Start(int msec = -1){
        if(msec == -1){
            m_total_timer.start();
        } else {
            m_total_timer.start(msec);
        }
        m_progress_timer.start(32);
        emit Progress(0);
    }
    void Stop(){
        m_total_timer.stop();
        m_progress_timer.stop();
    }

    void SetInterval(int msec){
        m_total_timer.setInterval(msec);
    }

    auto IsActive()const{
        return m_total_timer.isActive();
    }

    double GetProgress()const{
        const double kRemainingPercentage{static_cast<double>(m_total_timer.remainingTime()) / static_cast<double>(m_total_timer.interval()) * 100};
        return 100. - kRemainingPercentage;
    }

    int GetRemainingTimeMs() const{
        return m_total_timer.remainingTime();
    }
    QTime GetRemainingTime() const{
        return QTime::fromMSecsSinceStartOfDay(GetRemainingTimeMs());
    }

public slots:
    void UpdateProgress(){
        if(!m_total_timer.isActive()){
            return;
        }
        emit Progress(GetProgress());
    }

signals:
    void Progress(double percentage);//in [0,100]
    void Timeout();

private:
    void OnTotalTimeout(){
        m_progress_timer.stop();
        emit Progress(100.);
        emit Timeout();
    }

private:
    QTimer m_total_timer{};
    QTimer m_progress_timer{};
};

}//namespace sot
