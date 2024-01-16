#pragma once

#include <functional>

template <typename T>
struct SendHeartbeatCallback;

template <typename Ret, typename... Params>
struct SendHeartbeatCallback<Ret(Params...)> {
   template <typename... Args> 
   static Ret callback(Args... args) {                    
      func(args...);  
   }
   static std::function<Ret(Params...)> func; 
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> SendHeartbeatCallback<Ret(Params...)>::func;
