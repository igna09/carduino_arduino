#pragma once

#include <functional>

template <typename T>
struct KlineCallback;

template <typename Ret, typename... Params>
struct KlineCallback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> KlineCallback<Ret(Params...)>::func;
