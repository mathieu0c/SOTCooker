#pragma once

#include <queue>

namespace utils {

template<typename T>
class MovingAverage{
 public:
  MovingAverage(int window_size) : m_values{}, m_window_size{window_size}, m_sum{} {}
  void AddValue(T value){
    const auto kDValue{static_cast<double>(value)};
    m_sum += kDValue;
    m_values.emplace(value);
    if(m_values.size() > m_window_size){
      m_sum -= m_values.front();
      m_values.pop();
    }
    m_avg = m_sum / static_cast<double>(m_values.size());
  }
  double GetAverage() const{
    return m_avg;
  }
  T GetAverageTyped()const{
    return static_cast<T>(m_avg);
  }
  void Reset(){
    m_values = {};
    m_sum = 0;
    m_avg = 0;
  }

 private:
  std::queue<T> m_values;
  int m_window_size{};
  double m_sum{};
  double m_avg{};
};

}
