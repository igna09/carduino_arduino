#pragma once

#include <functional>

template <typename T>
struct StopMovingMirrorsCallback;

template <typename Ret, typename... Params>
struct StopMovingMirrorsCallback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> StopMovingMirrorsCallback<Ret(Params...)>::func;
