import os

# toolchains options
ARCH='x86'
CPU=''

BUILD = 'debug'

#need for windows
EXEC_PATH="c:/gcc/"

# toolchains
PREFIX = 'i386-elf-'
CC = PREFIX + 'gcc'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

DEVICE = ' -mtune=generic'
CFLAGS = DEVICE + ' -Wall'
AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp'
LFLAGS = DEVICE + ' -nostartfiles -Wl,--gc-sections,-Map=target/x86/x86.map,-cref,-u,_start -T target/x86/x86.lds'

CPATH = ''
LPATH = ''

if BUILD == 'debug':
    CFLAGS += ' -O0 -gdwarf-2'
    AFLAGS += ' -gdwarf-2'
else:
    CFLAGS += ' -O2'

POST_ACTION = OBJCPY + ' -O binary $TARGET cos.bin\n' + SIZE + ' $TARGET \n'


