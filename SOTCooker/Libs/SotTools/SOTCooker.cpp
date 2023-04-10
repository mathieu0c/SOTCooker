#include "SOTCooker.hpp"

namespace sot {

Cooker::Cooker(QObject* parent) : QObject{parent},
    m_cooking_timer{this}
{
    connect(&m_cooking_timer,&ProgressTimer::Timeout,this,&Cooker::OnCookingFinished);

    connect(&m_cooking_timer,&ProgressTimer::Progress,this,&Cooker::OnCookingProgress);
}

void Cooker::StartCooking(sot::CookingType type){
    const auto kPreviousCookingType{m_cooking_type};
    m_cooking_type = type;

    if(IsCooking()){
        qWarning() << "Was already cooking a" << GetTypeHumanStr(kPreviousCookingType);
        emit RestartedRunningCooking(kPreviousCookingType,m_cooking_type);
    }

    const auto kMsInterval{GetCookingMs(m_cooking_type)};
    m_cooking_timer.SetInterval(kMsInterval);
    m_cooking_timer.Start();
    qInfo() << "Starting cooking for:" << kMsInterval << "ms (" << GetTypeHumanStr(m_cooking_type) << ")";
    emit StartedCooking(m_cooking_type);
}

void Cooker::Cancel(){
    if(!m_cooking_timer.IsActive()){
        return;
    }

    m_cooking_timer.Stop();
    emit CookingCancelled(m_cooking_type);
}

void Cooker::OnCookingFinished(){
    qInfo() << "Finished cooking" << GetTypeHumanStr(m_cooking_type);
    emit FinishedCooking(m_cooking_type);
}

void Cooker::OnCookingProgress(const double kPercentage){
    emit Progress(kPercentage);
}

}//namesapce sot