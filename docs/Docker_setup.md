# Docker Setup Guide

## First time setup
For first time setup, follow the commands below to install all necessary libraries.
```bash
# Create Docker Image
(host) ./env.sh build # Take pretty long

# Create Docker Container
(host) ./env.sh run
```


## After Setup
```bash
# Default no arg will open the continer you created previously
(host) ./env.sh
```

## Structure
You can and *should checkout the script to accomodate to your need
```bash
Docker/
    ├── Dockerfile        # Define dependent libs usde by third-party libs
    └── requirements.txt  # Define Python libs & version
env.sh                    # Manage Docker container
```