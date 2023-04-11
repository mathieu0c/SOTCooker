#pragma once

#include <QTimer>
#include <QTime>

#include <QDebug>

namespace sot{

inline
QString GetAudioTimeStr(const QTime& time){
    const QString kMinutes{((time.minute() > 1)?"minutes":"minute")};
    const QString kSeconds{((time.second() > 1)?"seconds":"second")};
    QString out{};
    if(time.minute()){
        out += QString{"%0 %1"}.arg(time.minute()).arg(kMinutes);
    }
    if(time.second()){
        out += QString{" %0 %1 "}.arg(time.second()).arg(kSeconds);
    }

    if(!time.minute() && !time.second()){
        out += "0 seconds";
    }

    return out;
}

inline
auto GetAudioTimeStr(double secs){
    return GetAudioTimeStr(QTime::fromMSecsSinceStartOfDay(secs*1000));
}

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
        UpdateProgress();
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

    int32_t GetRemainingTimeMs() const{
        return m_total_timer.remainingTime();
    }
    int32_t GetElapsedTimeMs() const{
        return m_total_timer.interval() - m_total_timer.remainingTime();
    }
    QTime GetRemainingTime() const{
        return QTime::fromMSecsSinceStartOfDay(GetRemainingTimeMs());
    }

public slots:
    void UpdateProgress(){
        if(!m_total_timer.isActive()){
            return;
        }
        //due to some computation errors that I'm too lazy to fix
        //the progress may mark the timer as "completed"
        //before the timeout.
        //So let's just call the onTimeout function if it happens
        if(static_cast<int>(GetProgress()) == 100){
            OnTotalTimeout();
        }
        emit Progress(GetProgress());
        emit ProgressRemainingMs(GetRemainingTimeMs());
    }

signals:
    void ProgressRemainingMs(int32_t ellapsed_time);
    void Progress(double percentage);//in [0,100]
    void Timeout();

private:
    void OnTotalTimeout(){
        m_progress_timer.stop();
        m_total_timer.stop();
        UpdateProgress();
        emit Timeout();
    }

private:
    QTimer m_total_timer{};
    QTimer m_progress_timer{};
};

}//namespace sot
