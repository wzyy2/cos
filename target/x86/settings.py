import os,platform


PREFIX = ''
if platform.system() == 'Darwin':
    PREFIX = 'i386-elf-'

# toolchains options
ARCH='x86'
CPU=''

BUILD = 'debug'

#need for windows
EXEC_PATH="c:/gcc/"

# toolchains

CC = PREFIX + 'gcc'
CXX = PREFIX + 'g++'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
LINK = PREFIX + 'g++'
TARGET_EXT = 'elf'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

DEVICE = ' -mtune=generic'

# 64 bit gcc
if PREFIX != 'i386-elf-':
    DEVICE += ' -m32'

CFLAGS = DEVICE + ' -Wall'
AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp'
LFLAGS = DEVICE + '  -nodefaultlibs -nostartfiles -Wl,-Map=target/x86/x86.map,-cref,-u,_start -T target/x86/x86.lds'

CXXFLAGS = " -std=c++11 -ffreestanding -fno-builtin -nostdinc++ -fno-rtti -fno-exceptions -fno-threadsafe-statics"

# -nostdinc++

CPATH = ''
LPATH = ''

if BUILD == 'debug':
    CFLAGS += ' -O0 -gdwarf-2'
    AFLAGS += ' -gdwarf-2'
else:
    CFLAGS += ' -O2'

POST_ACTION = OBJCPY + ' -O binary $TARGET cos.bin\n' + SIZE + ' $TARGET \n'


