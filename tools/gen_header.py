#!/usr/bin/python
import os
import sys


def GenerateHeader(root_directory, BuildOptions):
    gen_dir = root_directory + "/include/"

    HeaderFilename = gen_dir + 'cos_conf.h'

    HeaderFile = open(HeaderFilename, 'w')
    def writeheader(s): HeaderFile.write(s+"\n");

    writeheader("// auto-generated file")
    writeheader("#ifndef __COSCONF_H__")
    writeheader("#define __COSCONF_H__ \n")

    for key,value in BuildOptions.items():
        writeheader("#define " + key + ' ' + value)

    writeheader("\n#endif // __COSCONF_H__\n")
    HeaderFile.close()

