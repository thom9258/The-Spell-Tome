import platform
import os.path

Target = "yal"
Main = "main.c"
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

default_flags = ['-m64', '-g']
env.Append( CCFLAGS=[default_flags])

if DebugMode:
    if isLinux():
        flags = ['-Wall', '-Wextra', '-Werror', '-ggdb']
        env.Append( CCFLAGS=[flags])
        
libs = ['-lreadline']
env.Append( LIBS=[libs])

env.Program(target=Target, source=Main)
