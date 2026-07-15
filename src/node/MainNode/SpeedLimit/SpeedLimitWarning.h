#pragma once
#include <cstdint>
#include <functional>
#include "esp_timer.h"

class SpeedLimitWarning {
public:
    using WarnCallback = std::function<void()>;

    void init(WarnCallback cb) { _onWarn = std::move(cb); }

    void setLimit(uint8_t kmh) { _limitKmh = kmh; }
    uint8_t getLimit() const { return _limitKmh; }

    void setEnabled(bool en) { _enabled = en; }
    bool isEnabled() const { return _enabled; }

    void onSpeedUpdate(uint8_t speedKmh) {
        if (!_enabled || _limitKmh == 0) { _wasOver = false; return; }

        bool isOver = speedKmh > _limitKmh;

        if (isOver && !_wasOver) {
            triggerWarning();
            _lastWarnMs = nowMs();
            _wasOver = true;
        } else if (isOver && _wasOver) {
            uint32_t t = nowMs();
            if (t - _lastWarnMs >= _repeatMs) {
                triggerWarning();
                _lastWarnMs = t;
            }
        }

        if (speedKmh <= (int32_t)_limitKmh - _hysteresisKmh) {
            _wasOver = false;
        }
    }

    void setRepeatIntervalMs(uint32_t ms) { _repeatMs = ms; }
    void setHysteresisKmh(uint8_t kmh) { _hysteresisKmh = kmh; }

private:
    void triggerWarning() { if (_onWarn) _onWarn(); }
    static uint32_t nowMs() { return (uint32_t)(esp_timer_get_time() / 1000); }

    WarnCallback _onWarn;
    uint8_t _limitKmh = 0;
    bool _enabled = true;
    bool _wasOver = false;
    uint32_t _lastWarnMs = 0;
    uint32_t _repeatMs = 3000;
    uint8_t _hysteresisKmh = 3;
};