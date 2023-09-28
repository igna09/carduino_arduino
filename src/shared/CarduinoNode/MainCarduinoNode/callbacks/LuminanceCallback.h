#pragma once

#include <functional>

template <typename T>
struct LuminanceCallback;

template <typename Ret, typename... Params>
struct LuminanceCallback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> LuminanceCallback<Ret(Params...)>::func;
