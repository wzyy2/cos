import os,platform

PREFIX = ''

# toolchains options
ARCH='test'
CPU=''

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


CFLAGS = DEVICE + ' -Wall'
AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp'
LFLAGS = DEVICE + '  -gc-sections  -lgtest -ldl'

if platform.system() == 'Darwin':
    LFLAGS += ' -I/opt/local/include -L/opt/local/lib'

CXXFLAGS = " -std=c++11"


CPATH = ''
LPATH = ''

CFLAGS += ' -O0 -gdwarf-2'
AFLAGS += ' -gdwarf-2'


