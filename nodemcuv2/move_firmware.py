import os, json, shutil
from SCons.Script import DefaultEnvironment

# env::<board type?>
# example : esp32doit-devkit-v1, nodemcuv2, uno
extension_firmware   = ".bin"  # .bin / .hex
extension_filesys    = ".bin"
board                = "nodemcuv2"
child                = "esp8266"
firmware             = "firmware" + extension_firmware
filesys              = "littlefs" + extension_filesys  # or spiffs.bin
dest_path            = "firmware"

# list board no filesystem
listBoard = ["uno", "megaatmega2560"]

class Move:
    def __init__(self) -> None:
        self.build_dir = f".pio/build/{board}"
        self.output_firmware = firmware
        self.output_fs = filesys
        self.dest_path = dest_path
        self.child_dir = child

    def __move_file__(self, src: str, dest: str):
        try:
            if os.path.exists(dest):
                os.remove(dest)
                print(f"Remove existing file: {dest}")
            shutil.copy(src, dest)
            print(f"Copied {src} to {dest}")
        except Exception as e:
            print(f"Error moving file: {e}")

    def process_file(self, new_firmware_name: str, new_fs_name: str=None):
        firmware_src = os.path.join(self.build_dir, self.output_firmware)
        fs_src = os.path.join(self.build_dir, self.output_fs)
        dest_dir = os.path.join(self.dest_path, self.child_dir)

        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
            print(f"Create directory: {dest_dir}")
        else:
            print(f"Direcotry already exists: {dest_dir}")

        if os.path.exists(firmware_src):
            firmware_dest = os.path.join(dest_dir, new_firmware_name)
            self.__move_file__(firmware_src, firmware_dest)
        else:
            print(f"Firmware file not found: {firmware_src}")

        if new_fs_name and os.path.exists(fs_src):
            fs_dest = os.path.join(dest_dir, new_fs_name)
            self.__move_file__(fs_src, fs_dest)
        elif new_fs_name:
            print(f"FS file not found: {fs_src}")

def main(target, source, env):
    # Open and read the configuration file
    with open('build.config.json', 'r') as f:
        config = json.load(f)

    data_name = config["name"]
    data_version = config["version"]
    data_revision = config["revision"]
    typefile_1 = config["typefile-1"]
    typefile_2 = config["typefile-2"]

    if (data_revision != 0):
        out_file_firmware = f"{data_name}-v{data_version}-{data_revision}-{typefile_1}{extension_firmware}"
        out_file_fs = f"{data_name}-v{data_version}-{data_revision}-{typefile_2}{extension_filesys}"
    else:
        out_file_firmware = f"{data_name}-v{data_version}-{typefile_1}{extension_firmware}"
        out_file_fs = f"{data_name}-v{data_version}-{typefile_2}{extension_filesys}"

    # Process files
    move_file = Move();
    if (board in listBoard):
        move_file.process_file(out_file_firmware)
    else:
        move_file.process_file(out_file_firmware, out_file_fs)

env = DefaultEnvironment()
env.AddPostAction("$BUILD_DIR/${PROGNAME}"+extension_firmware, main)
env.AddPostAction(f"$BUILD_DIR/{filesys}", main)
