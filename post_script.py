Import("env")
import os
import shutil

BUILD_DIRECTORY_PATH = os.path.join(os.getcwd(), '.pio', 'build')
FIRMWARES_DIRECTORY_PATH = os.path.join(BUILD_DIRECTORY_PATH, 'firmwares')

if not os.path.exists(FIRMWARES_DIRECTORY_PATH): 
    os.makedirs(FIRMWARES_DIRECTORY_PATH) 

def copy_bin_file(source, target, env):
    # print("copy file", source, source[0].path, target, target[0].path, env)
    shutil.copy(
        os.path.join(BUILD_DIRECTORY_PATH, f"{env['PIOENV']}", "firmware.bin"),
        os.path.join(FIRMWARES_DIRECTORY_PATH, f"{env['PIOENV']}.bin")
    )
    # shutil.copy(target[0].path, os.path.join(FIRMWARES_DIRECTORY_PATH, "${env['PIOENV']}.bin"))

# env.AddPostAction(
#     "$BUILD_DIR/${PROGNAME}.bin",
#     copy_bin_file
# )
# env.AddPostAction(
#     "checkprogsize",
#     copy_bin_file
# )
env.AddPostAction(
    "buildprog",
    # "checkprogsize",
    copy_bin_file
)