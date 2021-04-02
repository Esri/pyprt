# TODO: these python images are based on debian buster
#       we should probably build our own base image with centos:7 and add the corresponding python
FROM python:3.7

RUN apt-get update && apt-get install -y cmake ninja-build

# TODO: start with clean git checkout
WORKDIR /tmp/pyprt-build
COPY envs ./envs
COPY pyprt ./pyprt
COPY src ./src
COPY tests ./tests
COPY *.py ./

RUN pip install --no-cache-dir --upgrade pip
RUN pip install --no-cache-dir --upgrade wheel
RUN pip install --no-cache-dir -r envs/py37/requirements.txt

# TODO: add conda support

CMD [ "python", "setup.py", "bdist_wheel" ]
