#!/bin/bash

# 日志文件路径
LOG_FILE="/var/log/git_check.log"

# 检查 git 是否安装
if command -v git > /dev/null 2>&1; then
    echo "$(date) - 欢迎使用 Linux！" >> "$LOG_FILE"
else
    echo "$(date) - 请先安装 git" >> "$LOG_FILE"
fi
