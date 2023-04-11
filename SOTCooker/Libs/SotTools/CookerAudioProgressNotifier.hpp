#pragma once

#include <QObject>

#include <SOTCooker.pb.h>

namespace sot {

class CookerAudioProgressNotifier : public QObject
{
public:
    using CookingSteps = std::vector<int32_t>;
    CookerAudioProgressNotifier(QObject* parent = nullptr);

    void SetCooking(CookingType type);

public slots:
    void Update(int32_t ms);

public:
    static CookingSteps GetSteps(CookingType type);

private:
    CookingSteps m_current_steps{};//must be ordered from smaller to biggest
    int32_t m_previous_updt_time_ms{};
};

} // namespace sot
