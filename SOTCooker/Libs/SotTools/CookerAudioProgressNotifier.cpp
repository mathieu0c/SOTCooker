#include "CookerAudioProgressNotifier.hpp"

#include "SOTCookerUtils.hpp"
#include "ProgressTimer.hpp"

#include <TextToSpeech.hpp>

#include <concepts>

namespace sot {

namespace {

template<std::integral T>
T RoundUpToMultiple(T num, T factor)
{
    assert(num >= 0);
    return num + factor - 1 - (num + factor - 1) % factor;
}

void AddPercentage(CookerAudioProgressNotifier::CookingSteps* target,const int32_t kTotalCookingTimeMs,double percentage){
    int32_t time_value{static_cast<int32_t>(percentage/100000.*static_cast<double>(kTotalCookingTimeMs))};
    time_value = RoundUpToMultiple(time_value,5);//only allow multiple of 5
    //if the percentage time is smaller than the last value in the steps
    //ignore the percentage
    //means: only add a percentage if it's higher than the highest step value
    if(target->back() < time_value){
        target->emplace_back(time_value);
    }
}

}

CookerAudioProgressNotifier::CookerAudioProgressNotifier(QObject *parent) : QObject{parent}
{

}

void CookerAudioProgressNotifier::Update(int32_t ms){
    const auto kPreviousTime{m_previous_updt_time_ms};
    m_previous_updt_time_ms = ms;

    for(const auto& kStep : m_current_steps){
        //add 1s because we want to notify when
        //entering the second, not when leaving it
        const auto kStepMs{(kStep+1)*1000};
        if(!(kPreviousTime > kStepMs && ms < kStepMs)){
            continue;
        }

        cus::TextPlayer::Play(GetAudioTimeStr(kStep));
    }
}

void CookerAudioProgressNotifier::SetCooking(CookingType type){
    m_previous_updt_time_ms = {-1};
    m_current_steps = GetSteps(type);
}

CookerAudioProgressNotifier::CookingSteps CookerAudioProgressNotifier::GetSteps(CookingType type){
    const auto kCookingTime{GetCookingMs(type)};

    CookingSteps out{};
    if(type == CookingType::kCache){
        out.emplace_back(5);
        out.emplace_back(15);
        out.emplace_back(30);
        out.emplace_back(45);

        out.emplace_back(60);
        out.emplace_back(90);
        out.emplace_back(120);
        out.emplace_back(150);
    } else {
        out.emplace_back(5);
        out.emplace_back(15);

        AddPercentage(&out,kCookingTime,33);
        AddPercentage(&out,kCookingTime,50);
        AddPercentage(&out,kCookingTime,75);
    }

    std::remove_if(out.begin(),out.end(),[kCookingTime](const int32_t& t){
        return t >= kCookingTime || t <= 0;
    });

    qDebug() << GetTypeHumanStr(type) << "Notify on:";
    for(const auto& step : out){
        qDebug().nospace() << step << "s";
    }

    return out;
}

} // namespace sot
