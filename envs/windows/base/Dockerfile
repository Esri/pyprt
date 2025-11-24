# escape=`

ARG BASE_IMAGE="mcr.microsoft.com/windows/servercore:1809"
# or with python base image: python:3.11-windowsservercore-1809

FROM ${BASE_IMAGE}
SHELL ["powershell", "-Command"]
WORKDIR c:/tmp

COPY envs/windows/base/install_vs_toolchain.ps1 ./
RUN ./install_vs_toolchain.ps1; Remove-Item 'C:/tmp/install_vs_toolchain.ps1'

COPY envs/windows/base/install_git.ps1 ./
RUN ./install_git.ps1; Remove-Item 'C:/tmp/install_git.ps1'
