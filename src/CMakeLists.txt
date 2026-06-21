# --- DEFINIZIONE DELLE FUNZIONI UTILITÀ ---

# Funzione per i file sorgente
function(add_subdirectories_to_sources TARGET_LIST_VAR START_DIR)
    file(GLOB_RECURSE found_sources "${START_DIR}/*.cpp" "${START_DIR}/*.c" "${START_DIR}/*.S")
    set(${TARGET_LIST_VAR} ${${TARGET_LIST_VAR}} ${found_sources} PARENT_SCOPE)
endfunction()

# Funzione per le cartelle di include
function(add_subdirectories_to_include TARGET_LIST_VAR START_DIR)
    file(GLOB_RECURSE found_headers "${START_DIR}/*.h" "${START_DIR}/*.hpp")
    set(local_include_dirs "${START_DIR}")
    foreach(header_file ${found_headers})
        get_filename_component(header_dir ${header_file} DIRECTORY)
        list(APPEND local_include_dirs ${header_dir})
    endforeach()
    list(REMOVE_DUPLICATES local_include_dirs)
    set(${TARGET_LIST_VAR} ${${TARGET_LIST_VAR}} ${local_include_dirs} PARENT_SCOPE)
endfunction()


# --- LOGICA DEL PROGETTO ---

# Inizializziamo le directory di include con il path base del progetto
set(include_dirs ".")
set(app_sources "") # Inizializziamo la lista dei sorgenti vuota

# message(STATUS "${CMAKE_SOURCE_DIR}/cmake_config.cmake")
# Includi il file generato da Python
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake_config.cmake")
    # message(STATUS "--- IL FILE ESISTE ---")
    include("${CMAKE_SOURCE_DIR}/cmake_config.cmake")
# else()
#     message(STATUS "--- IL FILE NON ESISTE ---")
endif()

# message(STATUS "--- LA VARIABILE IN CMAKE E': ${CARDUINO_NODE_FILE_NAME} ---")

if(NOT CARDUINO_NODE_FILE_NAME STREQUAL "UdpAp")
    add_subdirectories_to_sources(app_sources "${CMAKE_SOURCE_DIR}/src/node/CarduinoNode")
    add_subdirectories_to_include(include_dirs "${CMAKE_SOURCE_DIR}/src/node/CarduinoNode")

    add_subdirectories_to_sources(app_sources "${CMAKE_SOURCE_DIR}/src/shared")
    add_subdirectories_to_include(include_dirs "${CMAKE_SOURCE_DIR}/src/shared")
else()
    list(APPEND include_dirs "${CMAKE_SOURCE_DIR}/src/shared")
    list(APPEND app_sources "${CMAKE_SOURCE_DIR}/src/main/UdpServer.cpp")
endif()

# Usa le funzioni per KlineNode (prende file e sottocartelle in automatico)
add_subdirectories_to_sources(app_sources "${CMAKE_SOURCE_DIR}/src/node/${CARDUINO_NODE_FILE_NAME}")
add_subdirectories_to_include(include_dirs "${CMAKE_SOURCE_DIR}/src/node/${CARDUINO_NODE_FILE_NAME}")

# Aggiungiamo il Main specifico (essendo un file singolo, usiamo il list APPEND classico)
list(APPEND app_sources "${CMAKE_SOURCE_DIR}/src/main/${CARDUINO_NODE_FILE_NAME}.cpp")

# 3. Registrazione del componente ESP-IDF
idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS ${include_dirs}
    # REQUIRES esp_wifi esp_event esp_netif lwip
)