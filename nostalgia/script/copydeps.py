import sys
import getopt
import os
import ntpath
import shutil

# Will depend on platform
if os.name == "nt":    
    targetpaths = {
        "debug32"   : "../bin/Debug/Win32/",
        "release32" : "../bin/Release/Win32/",
        "debug64"   : "../bin/Debug/x64/",
        "release64" : "../bin/Release/x64/"
    }
    # SOURCE files
    files = { 
        "debug32" : [ 
            "../thirdparty/fmod/bin/fmodL.dll",
            "../thirdparty/vld/bin/Win32/dbghelp.dll",
            "../thirdparty/vld/bin/Win32/Microsoft.DTfW.DHL.manifest",
            "../thirdparty/vld/bin/Win32/vld_x86.dll",
            "../thirdparty/box2d/lib/Win32/Debug/box2d.pdb"
            ],
        "release32": [
            "../thirdparty/fmod/bin/fmod.dll"
            ],
        "debug64" : [
            "../thirdparty/fmod/bin/fmodL64.dll",
            "../thirdparty/vld/bin/Win64/dbghelp.dll",
            "../thirdparty/vld/bin/Win64/Microsoft.DTfW.DHL.manifes",
            "../thirdparty/vld/bin/Win64/vld_x64.dll"            
            ],
        "release64": [
            "../thirdparty/fmod/bin/fmod64.dll"
            ]
    }

curpath = os.path.dirname(os.path.realpath(__file__))    

def copydeps(config):
    if not config in targetpaths:
        print "%s not in target paths" % (config)
        return
    if not config in files:
        print "%s not in files" % (config)
    
    tgtpath = os.path.join(curpath, targetpaths[config])
    if not os.path.exists(tgtpath):
        os.makedirs(tgtpath)
    for f in files[config]:
        srcfile = os.path.normpath(os.path.join(curpath,f))
        basesrcfile = ntpath.basename(srcfile)
        tgtfile = os.path.normpath(os.path.join(tgtpath, basesrcfile))
        if not os.path.isfile(tgtfile) and os.path.isfile(srcfile):
            print basesrcfile
            shutil.copy(srcfile,tgtfile)
            

if __name__=="__main__":
    # check if windows
    
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hc:", ["help","config="])
    except getopt.GetoptError as err:
        print str(err)
        sys.exit(2)
        
    for o, a in opts:
        if o in ("-h","--help"):
            print "Usage: $copydeps --config=[debug32, release32, debug64, release64]"
        elif o in ("-c","--config"):
            print "==Copying dependencies (%s)==" % (os.name)
            copydeps(a)
            print "==Done=="
        else:
            assert False, "unhandled option"
            
        