#pragma once

#include <functional>

template <typename T>
struct TemperatureCallback;

template <typename Ret, typename... Params>
struct TemperatureCallback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> TemperatureCallback<Ret(Params...)>::func;
