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

signals:
    void RestartedRunningCooking(sot::CookingType old_val,sot::CookingType new_val);

private slots:
    void OnCookingFinished();

private:
    ProgressTimer m_cooking_timer;
    sot::CookingType m_cooking_type{sot::CookingType::kNone};
};

}//namesapce sot
