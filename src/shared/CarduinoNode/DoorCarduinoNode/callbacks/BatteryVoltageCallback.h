#pragma once

#include <functional>

template <typename T>
struct BatteryVoltageCallback;

template <typename Ret, typename... Params>
struct BatteryVoltageCallback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> BatteryVoltageCallback<Ret(Params...)>::func;
