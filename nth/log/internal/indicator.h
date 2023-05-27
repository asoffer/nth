#ifndef NTH_LOG_INTERNAL_INDICATOR_H
#define NTH_LOG_INTERNAL_INDICATOR_H

namespace nth::internal_log {

inline constinit std::atomic<struct Indicator*> log_indicators = nullptr;

struct Indicator {
  Indicator() { log_indicators.exchange(this, std::memory_order_relaxed); }

  Indicator(Indicator const&) = delete;
  Indicator(Indicator&&)      = delete;

  Indicator& operator=(Indicator const&) = delete;
  Indicator& operator=(Indicator&&)      = delete;

  bool off() const { return not on_.load(std::memory_order_relaxed); }

 private:
  std::atomic<bool> on_ = true;
};

}  // namespace nth::internal_log

#endif  // NTH_LOG_INTERNAL_INDICATOR_H
