#pragma once

#include <QObject>
#include <ProgressTimer.hpp>

#include <CookerProto.hpp>

namespace sot {

inline
int GetCookingMs(sot::CookingType type){
    return static_cast<int>(type)*1000;
}

inline
QString GetTypeHumanStr(sot::CookingType type){
    return GetEnumValueName(type).mid(1);
}

class Cooker : public QObject{
    Q_OBJECT
public:
    Cooker(QObject* parent);

    void StartCooking(sot::CookingType type);

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

signals:
    void StartedCooking(sot::CookingType);
    void RestartedRunningCooking(sot::CookingType old_val,sot::CookingType new_val);
    void Progress(double);

private slots:
    void OnCookingFinished();
    void OnCookingProgress(const double kPercentage);

private:
    ProgressTimer m_cooking_timer;
    sot::CookingType m_cooking_type{sot::CookingType::kNone};
};

}//namesapce sot
