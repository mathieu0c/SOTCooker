#include "SOTCooker.hpp"

#include "SOTCookerUtils.hpp"

namespace sot {

Cooker::Cooker(QObject* parent) : QObject{parent},
    m_cooking_timer{this},
    m_notifier{this}
{
    connect(&m_cooking_timer,&ProgressTimer::Timeout,this,&Cooker::OnCookingFinished);

    connect(&m_cooking_timer,&ProgressTimer::Progress,this,&Cooker::OnCookingProgress);

    connect(&m_cooking_timer,&ProgressTimer::ProgressRemainingMs,&m_notifier,&CookerAudioProgressNotifier::Update);
}

void Cooker::StartCooking(sot::CookingType type){
    //this value must not be sot::CookingType::kCycle or sot::CookingType::kRestart
    const auto kPreviousCookingType{m_cooking_type};

    m_cooking_type = type;

    //handling special cooking types
    if (type == sot::CookingType::kCycle){
        if(IsCooking()){
            m_cooking_type = sot::CycleType(kPreviousCookingType);
        } else {
            m_cooking_type = sot::CookingType::kRestart;
        }
    }
    if(m_cooking_type == sot::CookingType::kRestart){
        if(kPreviousCookingType == sot::CookingType::kNone){
            m_cooking_type = sot::CookingType::kFish;
        } else {
            m_cooking_type = kPreviousCookingType;
        }
    }

    assert(m_cooking_type != sot::CookingType::kCycle && m_cooking_type != sot::CookingType::kRestart);

    if(IsCooking()){
        qWarning() << "Was already cooking a" << GetTypeHumanStr(kPreviousCookingType);
        emit RestartedRunningCooking(kPreviousCookingType,m_cooking_type);
    }

    const auto kMsInterval{GetCookingMs(m_cooking_type)};
    m_cooking_timer.SetInterval(kMsInterval);
    m_cooking_timer.Start();
    qInfo() << "Starting cooking for:" << kMsInterval << "ms (" << GetTypeHumanStr(m_cooking_type) << ")";
    emit StartedCooking(m_cooking_type);
    m_notifier.SetCooking(m_cooking_type);
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
