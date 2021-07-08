import sys
import os
import pathlib
import docker
import argparse
import platform

MYSELF = pathlib.Path(__file__)
CONTEXT = MYSELF.parent.parent
CLIENT = docker.from_env()


def get_linux_args(py_ver):
    return {"PY_VER": py_ver, "USER_ID": str(os.getuid()), "GROUP_ID": str(os.getgid())}


IMAGE_DESCRIPTORS = [
    {"kind": "wheels", "os": "centos7", "tag": "centos7-py36-gcc83", "args": get_linux_args("36")},
    {"kind": "wheels", "os": "centos7", "tag": "centos7-py38-gcc83", "args": get_linux_args("38")},
    {"kind": "conda", "os": "centos7", "tag": "centos7-py36-conda-gcc83", "args": get_linux_args("3.6")},
    {"kind": "conda", "os": "centos7", "tag": "centos7-py37-conda-gcc83", "args": get_linux_args("3.7")},
    {"kind": "conda", "os": "centos7", "tag": "centos7-py38-conda-gcc83", "args": get_linux_args("3.8")},
]


def build_image(desc):
    docker_file = CONTEXT.joinpath(f"envs/{desc['os']}/{desc['kind']}/Dockerfile")
    print(f">>> Building image for {docker_file} with args {desc['args']} ...")
    return CLIENT.images.build(path=str(CONTEXT), dockerfile=str(docker_file), quiet=False, rm=True,
                               tag=f"pyprt:{desc['tag']}", buildargs=desc['args'])


def print_logs(logs):
    for item in logs:
        if 'stream' in item:
            sys.stdout.write(item['stream'])


def can_run(desc):
    if platform.system() == "Windows" and desc['os'] == "windows":
        return True
    elif platform.system() == "Linux" and desc['os'] == "centos7":
        return True
    else:
        return False


def main():
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('-v', action='store_true', default=False, help='Verbose output')
    args = arg_parser.parse_args()

    for desc in IMAGE_DESCRIPTORS:
        if can_run(desc):
            (image, logs) = build_image(desc)
            print(f"   ... built {image}")
            if args.v:
                print_logs(logs)
        else:
            print(f"Skipping desc: {desc}")


if __name__ == "__main__":
    main()
