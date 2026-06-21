# Import the current working construction
# environment to the `env` variable.
# alias of `env = DefaultEnvironment()`
Import("env")

# Prende il nome dell'ambiente corrente (es. "KlineNode")
current_env = env["PIOENV"]

# Lo inietta direttamente nei flag di CMake di ESP-IDF in modo sicuro
env.Append(CMAKE_EXTRA_ARGS=[f'-DCARDUINO_NODE_FILE_NAME=\"{current_env}\"'])

with open("cmake_config.cmake", "w") as f:
    f.write(f'set(CARDUINO_NODE_FILE_NAME "{current_env}")\n')

# Se vuoi che il nome dell'ambiente o altre variabili siano lette nel codice C++,
# aggiungile a CPPDEFINES. PlatformIO gestisce le stringhe racchiudendole tra macro.
env.Append(
    CPPDEFINES=[
        ("NODE_NAME", f'\\"{current_env}\\"'), # Diventa #define NOME_NODO "KlineNode"
        # ("DEBUG_MODE", 1),                     # Diventa #define DEBUG_MODE 1
        # "KLINE_ENABLE"                        # Diventa #define ABILITA_KLINE
    ]
)

# Dump construction environment (for debug purpose)
# print(env.Dump())

print(f"\n>>> Script Pre-Build: Iniettate macro per l'ambiente {current_env} <<<\n")