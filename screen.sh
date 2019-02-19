#!/bin/bash
export SERVIDOR="localhost"
export PUERTO=30000

cd $1
screen -c .screenrc
