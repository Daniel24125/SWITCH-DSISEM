savedcmd_/home/daniel/lkm/blinker/blinker.mod := printf '%s\n'   blinker.o | awk '!x[$$0]++ { print("/home/daniel/lkm/blinker/"$$0) }' > /home/daniel/lkm/blinker/blinker.mod
