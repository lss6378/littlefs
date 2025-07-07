; 指明目标格式：exe, lib, dll 三选一
mode: exe

; 指定输出文件名
out: main

; 编译选项
flag: -Wall, -O2, -g

; 手动指定临时文件夹，emake 会将生成的 .o 文件放在这个文件夹中
int: objs

; 加入源文件
# src: foo.c
# src: bar.c
src: main.c