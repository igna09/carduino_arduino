#include "AfterReadExecutors.h"

AfterReadExecutors::AfterReadExecutors() {}

void AfterReadExecutors::addExecutor(std::shared_ptr<AfterReadExecutorInterface> executor) {
    this->executors.push_back(executor);
};

void AfterReadExecutors::execute(CarduinoNode *node) {
    if (node->isEnabled) {
        // Cicla su ogni smart pointer presente nel vettore
        for (const auto& executor : this->executors) {
            if (executor != nullptr) { // Controllo di sicurezza
                executor->execute(node);
            }
        }
    }
}
