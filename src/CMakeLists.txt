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

if(NOT CARDUINO_NODE STREQUAL "UDP_AP")
    add_subdirectories_to_sources(app_sources "${CMAKE_SOURCE_DIR}/src/node/CarduinoNode")
    add_subdirectories_to_include(include_dirs "${CMAKE_SOURCE_DIR}/src/node/CarduinoNode")

    add_subdirectories_to_sources(app_sources "${CMAKE_SOURCE_DIR}/src/shared")
    add_subdirectories_to_include(include_dirs "${CMAKE_SOURCE_DIR}/src/shared")
else()
    list(APPEND include_dirs "${CMAKE_SOURCE_DIR}/src/shared")
    list(APPEND app_sources "${CMAKE_SOURCE_DIR}/src/main/UdpServer.cpp")
endif()


# 2. Controllo se il flag è impostato su KLINE
if(CARDUINO_NODE STREQUAL "KLINE")
    
    # Usa le funzioni per KlineNode (prende file e sottocartelle in automatico)
    add_subdirectories_to_sources(app_sources "${CMAKE_SOURCE_DIR}/src/node/KlineNode")
    add_subdirectories_to_include(include_dirs "${CMAKE_SOURCE_DIR}/src/node/KlineNode")
    
    # Aggiungiamo il Main specifico (essendo un file singolo, usiamo il list APPEND classico)
    list(APPEND app_sources "${CMAKE_SOURCE_DIR}/src/main/KlineNodeMain.cpp")
    
endif()


# 3. Registrazione del componente ESP-IDF
idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS ${include_dirs}
    # REQUIRES esp_wifi esp_event esp_netif lwip
)