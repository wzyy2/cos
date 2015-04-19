import os
import sys

COS_ROOT = os.path.normpath(os.getcwd())

try:
    TAR = ARGUMENTS['target']
    BSP_PATH = os.path.join(COS_ROOT, 'target')

    sys.path = sys.path + [os.path.join(BSP_PATH, TAR)]

    import settings
except:
    print("No target specified!")
    sys.exit(-1)

sys.path = sys.path + [os.path.join(COS_ROOT, 'tools')]
from building import *


TARGET = 'cos.' + settings.TARGET_EXT

env = Environment(ENV = os.environ,
    AS = settings.AS, ASFLAGS = settings.AFLAGS,
    CC = settings.CC, CCFLAGS = settings.CFLAGS,
    CXX = settings.CXX, CXXFLAGS = settings.CXXFLAGS,
    AR = settings.AR, ARFLAGS = '-rc',
    LINK = settings.LINK, LINKFLAGS = settings.LFLAGS)


Export('COS_ROOT')
Export('settings')

# prepare building environment
objs = PrepareBuilding(env, COS_ROOT)

if TAR == 'test':
    UnitTest(objs)
else:
    # make a building
    DoBuilding(TARGET, objs)

#Env.Command(None, Target + '.elf', SIZE + " $SOURCE")