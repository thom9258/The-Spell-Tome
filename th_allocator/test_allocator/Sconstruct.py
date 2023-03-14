import platform
import os.path

Target = "a.out"
Main = "test.c"
DebugMode = True
IsC99 = True
#BinFolder = "bin/"

env = Environment()
env['SYSTEM'] = platform.system().lower()
env['ENV']['TERM'] = os.environ['TERM']

def isLinux():
    if env['SYSTEM'] == 'linux':
        return True;
    return False;

print("INFO: platform [", env['SYSTEM'], "]")
print("INFO: building target [", Target, "]", " from main [", Main, "]")

if IsC99:
    if isLinux():
        env.CC = 'gcc'
        env.Append( CCFLAGS=['-std=c99'])

if DebugMode:
    if isLinux():
        flags = ['-m64', '-g', '-Wall', '-Wextra', '-Werror', '-ggdb']
        env.Append( CCFLAGS=[flags])

env.Program(target=Target, source=Main)
