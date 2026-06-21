import os
import glob
from SCons.Script import Import

Import("env")

current_env = env["PIOENV"]
source_dir = env["PROJECT_DIR"]

# 1. Inizializziamo le liste per CMake
src_files = []
include_dirs = [os.path.join(source_dir, ".")]

def add_subdirectories_to_sources_and_includes(base_path):
    """Cerca ricorsivamente sorgenti e cartelle di include tipo la funzione CMake"""
    global include_dirs, src_files
    # Trova ricorsivamente tutti i file con estensioni desiderate
    for ext in ("/**/*.cpp", "/**/*.c", "/**/*.S", "/**/*.h", "/**/*.hpp"):
        for path in glob.glob(base_path + ext, recursive=True):
            if path.endswith(('.cpp', '.c', '.S')):
                src_files.append(path.replace("\\", "/")) # Normalizza i path per CMake
            elif path.endswith(('.h', '.hpp')):
                header_dir = os.path.dirname(path).replace("\\", "/")
                if header_dir not in include_dirs:
                    include_dirs.append(header_dir)

# 2. Logica condizionale (ex CMake) spostata in Python
if current_env != "UdpAp":
    add_subdirectories_to_sources_and_includes(os.path.join(source_dir, "src", "node", "CarduinoNode"))
    add_subdirectories_to_sources_and_includes(os.path.join(source_dir, "src", "shared"))
else:
    include_dirs.append(os.path.join(source_dir, "src", "shared").replace("\\", "/"))

# Aggiungi l'ambiente specifico
add_subdirectories_to_sources_and_includes(os.path.join(source_dir, "src", "node", current_env))
src_files.append(os.path.join(source_dir, "src", "main", f"{current_env}.cpp").replace("\\", "/"))

# 3. Trasformiamo le liste in stringhe separate da punto e virgola (formato liste CMake)
cmake_srcs = ";".join(src_files)
cmake_includes = ";".join(include_dirs)

# Iniettiamo le stringhe direttamente nelle variabili d'ambiente del processo
os.environ["PLATFORMIO_APP_SOURCES"] = cmake_srcs
os.environ["PLATFORMIO_APP_INCLUDE_DIRS"] = cmake_includes

# 5. Definizione macro C++
env.Append(
    CPPDEFINES=[
        ("NODE_NAME", f'\\"{current_env}\\"'),
    ]
)

print(f"\n>>> Script Pre-Build [{current_env}]: Configurate {len(src_files)} sorgenti e {len(include_dirs)} directory di include <<<\n")