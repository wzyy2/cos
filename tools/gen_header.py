#!/usr/bin/python
import os
import sys


def GenerateHeader(root_directory, BuildOptions):
    gen_dir = root_directory + "/include/cos/"

    HeaderFilename = gen_dir + 'cosConf.h'

    HeaderFile = open(HeaderFilename, 'w')
    def writeheader(s): HeaderFile.write(s+"\n");

    writeheader("// auto-generated file")
    writeheader("#ifndef _COSCONF_H")
    writeheader("#define _COSCONF_H\n")

    for key,value in BuildOptions.items():
        writeheader("#define " + key + ' ' + value)

    writeheader("\n#endif /* _COSCONF_H  */\n")
    HeaderFile.close()

