import os
import sys
import string

from SCons.Script import *

BuildOptions = {}
Projects = []
Cos_root = ''
Env = None

def GetCurrentDir():
    conscript = File('SConscript')
    fn = conscript.rfile()
    name = fn.name
    path = os.path.dirname(fn.abspath)
    return path

def SrcRemove(src, remove):
    if type(src[0]) == type('str'):
        for item in src:
            if os.path.basename(item) in remove:
                src.remove(item)
        return

    for item in src:
        if os.path.basename(item.rstr()) in remove:
            src.remove(item)

def GetDepend(depend):
    building = True
    if type(depend) == type('str'):
        if not BuildOptions.has_key(depend) or BuildOptions[depend] == 0:
            building = False
        elif BuildOptions[depend] != '':
            return BuildOptions[depend]
          
        return building

    # for list type depend
    for item in depend:
        if item != '':
            if not BuildOptions.has_key(item) or BuildOptions[item] == 0:
                building = False

    return building

def MergeGroup(src_group, group):
    src_group['src'] = src_group['src'] + group['src']
    if group.has_key('CCFLAGS'):
        if src_group.has_key('CCFLAGS'):
            src_group['CCFLAGS'] = src_group['CCFLAGS'] + group['CCFLAGS']
        else:
            src_group['CCFLAGS'] = group['CCFLAGS']
    if group.has_key('CPPPATH'):
        if src_group.has_key('CPPPATH'):
            src_group['CPPPATH'] = src_group['CPPPATH'] + group['CPPPATH']
        else:
            src_group['CPPPATH'] = group['CPPPATH']
    if group.has_key('CPPDEFINES'):
        if src_group.has_key('CPPDEFINES'):
            src_group['CPPDEFINES'] = src_group['CPPDEFINES'] + group['CPPDEFINES']
        else:
            src_group['CPPDEFINES'] = group['CPPDEFINES']
    if group.has_key('LINKFLAGS'):
        if src_group.has_key('LINKFLAGS'):
            src_group['LINKFLAGS'] = src_group['LINKFLAGS'] + group['LINKFLAGS']
        else:
            src_group['LINKFLAGSc'] = group['LINKFLAGS']
    if group.has_key('LIBS'):
        if src_group.has_key('LIBS'):
            src_group['LIBS'] = src_group['LIBS'] + group['LIBS']
        else:
            src_group['LIBS'] = group['LIBS']
    if group.has_key('LIBPATH'):
        if src_group.has_key('LIBPATH'):
            src_group['LIBPATH'] = src_group['LIBPATH'] + group['LIBPATH']
        else:
            src_group['LIBPATH'] = group['LIBPATH']

def DefineGroup(name, src, depend, **parameters):
    global Env
    if not GetDepend(depend):
        return []

    # find exist group and get path of group
    group_path = ''
    for g in Projects:
        if g['name'] == name:
            group_path = g['path']
    if group_path == '':
        group_path = GetCurrentDir()

    group = parameters
    group['name'] = name
    group['path'] = group_path
    if type(src) == type(['src1']):
        group['src'] = File(src)
    else:
        group['src'] = src

    if group.has_key('CCFLAGS'):
        Env.Append(CCFLAGS = group['CCFLAGS'])
    if group.has_key('CPPPATH'):
        Env.Append(CPPPATH = group['CPPPATH'])
    if group.has_key('CPPDEFINES'):
        Env.Append(CPPDEFINES = group['CPPDEFINES'])
    if group.has_key('LINKFLAGS'):
        Env.Append(LINKFLAGS = group['LINKFLAGS'])

    if group.has_key('LIBS'):
        Env.Append(LIBS = group['LIBS'])
    if group.has_key('LIBPATH'):
        Env.Append(LIBPATH = group['LIBPATH'])

    objs = Env.Object(group['src'])
    if group.has_key('LIBRARY'):
        objs = Env.Library(name, objs)

    # merge group 
    for g in Projects:
        if g['name'] == name:
            # merge to this group
            MergeGroup(g, group)
            return objs

    # add a new group 
    Projects.append(group)

    return objs


def PrepareBuilding(env, root_directory, has_libcpu=False, remove_components = []):
    import SCons.cpp
    import settings

    global BuildOptions
    global Projects
    global Env
    global Cos_root

    Env = env
    Cos_root = root_directory

    # add program path
    env.PrependENVPath('PATH', settings.EXEC_PATH)

    import config
    BuildOptions = config.BuildOptions

    import gen_header
    gen_header.GenerateHeader(root_directory, BuildOptions)

    # we need to seperate the variant_dir for BSPs and the kernels. BSPs could
    # have their own components etc. If they point to the same folder, SCons
    # would find the wrong source code to compile.
    build_dir = 'build/' + settings.ARCH
    # build script
    objs = SConscript('SConscript', variant_dir=build_dir, duplicate=0)

    return objs


def DoBuilding(target, objects):
    program = None

    # merge the repeated items in the Env
    if Env.has_key('CPPPATH')   : Env['CPPPATH'] = list(set(Env['CPPPATH']))
    if Env.has_key('CPPDEFINES'): Env['CPPDEFINES'] = list(set(Env['CPPDEFINES']))
    if Env.has_key('LIBPATH')   : Env['LIBPATH'] = list(set(Env['LIBPATH']))
    if Env.has_key('LIBS')      : Env['LIBS'] = list(set(Env['LIBS']))

    program = Env.Program(target, objects)

    EndBuilding(target, program)

def EndBuilding(target, program = None):
    import settings

    Env.AddPostAction(target, settings.POST_ACTION)



def runUnitTest(env,target,source):
    import subprocess
    app = str(source[0].abspath)
    subprocess.call(app)

def UnitTest(objects):
    program = None

    # merge the repeated items in the Env
    if Env.has_key('CPPPATH')   : Env['CPPPATH'] = list(set(Env['CPPPATH']))
    if Env.has_key('CPPDEFINES'): Env['CPPDEFINES'] = list(set(Env['CPPDEFINES']))
    if Env.has_key('LIBPATH')   : Env['LIBPATH'] = list(set(Env['LIBPATH']))
    if Env.has_key('LIBS')      : Env['LIBS'] = list(set(Env['LIBS']))

    global Cos_root
    target = 'build/test/out/'

    test_dir = os.path.join(Cos_root, 'unittest/coslib')
    List = os.listdir(test_dir)
    for item in List:
        if os.path.splitext(item)[1] == '.cpp':
            out = target + os.path.splitext(item)[0] + '.test'
            program = Env.Program(out, objects + [os.path.join(test_dir, item)])
            Command(item, out, runUnitTest)


