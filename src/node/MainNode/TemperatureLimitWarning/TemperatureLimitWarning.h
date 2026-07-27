#pragma once
#include <cstdint>
#include <functional>
#include "esp_timer.h"

class TemperatureLimitWarning {
public:
    using WarnCallback = std::function<void()>;

    void init(WarnCallback cb) { _onWarn = std::move(cb); }

    void setLimit(float celsius) { _limitCelsius = celsius; }
    float getLimit() const { return _limitCelsius; }

    void setEnabled(bool en) { _enabled = en; }
    bool isEnabled() const { return _enabled; }

    void onTemperatureUpdate(float tempCelsius) {
        if (!_enabled || _limitCelsius == 0) { _wasOver = false; return; }

        bool isOver = tempCelsius > _limitCelsius;

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

        if (tempCelsius <= _limitCelsius - _hysteresisCelsius) {
            _wasOver = false;
        }
    }

    void setRepeatIntervalMs(uint32_t ms) { _repeatMs = ms; }
    void setHysteresisCelsius(float celsius) { _hysteresisCelsius = celsius; }

private:
    void triggerWarning() { if (_onWarn) _onWarn(); }
    static uint32_t nowMs() { return (uint32_t)(esp_timer_get_time() / 1000); }

    WarnCallback _onWarn;
    float _limitCelsius = 0;
    bool _enabled = true;
    bool _wasOver = false;
    uint32_t _lastWarnMs = 0;
    uint32_t _repeatMs = 3000;
    float _hysteresisCelsius = 3;
};