#pragma once

#include <QObject>
#include <ProgressTimer.hpp>

#include <CookerProto.hpp>

#include "CookerAudioProgressNotifier.hpp"

namespace sot {

class Cooker : public QObject{
    Q_OBJECT
public:
    Cooker(QObject* parent);

    void StartCooking(sot::CookingType type);
    void Cancel();

    bool IsCooking()const{
        return m_cooking_timer.IsActive();
    }
    auto GetProgress(){
        if(!m_cooking_timer.IsActive()){
            return 100.;
        }
        return m_cooking_timer.GetProgress();
    }

    const auto& Timer() const{
        return m_cooking_timer;
    }

    auto CookingType()const{
        return m_cooking_type;
    }

signals:
    void StartedCooking(sot::CookingType);
    void RestartedRunningCooking(sot::CookingType old_val,sot::CookingType new_val);
    void Progress(double);
    void FinishedCooking(sot::CookingType);
    void CookingCancelled(sot::CookingType);

private slots:
    void OnCookingFinished();
    void OnCookingProgress(const double kPercentage);

private:
    ProgressTimer m_cooking_timer;
    sot::CookingType m_cooking_type{sot::CookingType::kNone};
    CookerAudioProgressNotifier m_notifier;
};

}//namesapce sot
