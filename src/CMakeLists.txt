# Leggiamo le variabili passate da Python tramite l'ambiente di sistema
set(APP_SOURCES "$ENV{PLATFORMIO_APP_SOURCES}")
set(APP_INCLUDE_DIRS "$ENV{PLATFORMIO_APP_INCLUDE_DIRS}")

# Uniformiamo i separatori per Windows/Linux
string(REPLACE "\\" "/" APP_SOURCES "${APP_SOURCES}")
string(REPLACE "\\" "/" APP_INCLUDE_DIRS "${APP_INCLUDE_DIRS}")

# Registrazione del componente
idf_component_register(
    SRCS ${APP_SOURCES}
    INCLUDE_DIRS ${APP_INCLUDE_DIRS}
)