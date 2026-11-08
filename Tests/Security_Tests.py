"""Native and Appeal-script encryption tests; optional WSL cross-platform round trips."""
from pathlib import Path
import os,re,shutil,subprocess,sys,tempfile,uuid
root=Path(__file__).resolve().parent.parent
base=root/'Resources/Modules/Appeal_Module_Security'
script=root/'Resources/Appeal/Test'/('Security_'+uuid.uuid4().hex+'.Appeal')
def run(command,**kwargs):
    p=subprocess.run(command,cwd=root,capture_output=True,text=True,timeout=90,**kwargs)
    assert p.returncode==0 and not p.stderr,(p.stdout,p.stderr)
    return p.stdout.strip()
def wsl_path(path):return '/mnt/'+path.drive[0].lower()+path.as_posix()[2:]
try:
 diagnostic=run([str(root/'Appeal.exe'),'Test/Security'])
 exports=set(re.findall(r'extern "C" \S+ (\w+)\(', (base/'Test/src/main.cpp').read_text()))
 called={line.split()[0] for line in diagnostic.splitlines() if line}
 assert exports<=called,exports-called
 assert all(secret not in diagnostic for secret in ['diagnostic-payload','diagnostic-key','diagnostic-password'])
 with tempfile.TemporaryDirectory(prefix='Security_',dir=root/'Tests') as temp:
    work=Path(temp);exe=work/'Check.exe'
    run(['g++','-std=c++17','-O2','Tests/Security_Plugin_Tests.cpp',str(base/'src/Data_Source.cpp'),'-o',str(exe)])
    plugin=base/'Appeal_Sodium_Security/Appeal_Sodium_Security.dll';module=base/'Appeal_Module_Security.dll'
    assert run([str(exe),str(plugin),str(module),str(work/'native')])=='SECURITY_NATIVE_PASSED'
    script.write_text('Add_Dynamic_Module Security Appeal_Sodium_Security\nSet_Var Key (Security Generate_Key)\nSet_Var Cipher (Security Encrypt "Hello secure world" Var Key)\nPrint (Security Decrypt Var Cipher Var Key)\nPrint_Global Security.Success\nSet_Var P (Security Encrypt_With_Password "password test" "test password")\nPrint (Security Decrypt_With_Password Var P "test password")\nDone\n')
    assert run([str(root/'Appeal.exe'),'Test/'+script.stem]).splitlines()==['Hello secure world','1','password test']
    if '--wsl' in sys.argv:
        linux=work/'Check_Linux'
        run(['wsl','-d','Ubuntu','--','g++','-std=c++17','-O2',wsl_path(root/'Tests/Security_Plugin_Tests.cpp'),wsl_path(base/'src/Data_Source.cpp'),'-ldl','-o',wsl_path(linux)])
        lp=base/'Appeal_Sodium_Security/Appeal_Sodium_Security.so';lm=base/'Appeal_Module_Security.so'
        prefix=['wsl','-d','Ubuntu','--',wsl_path(linux),wsl_path(lp),wsl_path(lm)]
        assert run(prefix+[wsl_path(work/'linux-native')])=='SECURITY_NATIVE_PASSED'
        assert run([str(exe),str(plugin),str(module),str(work/'from-windows'),'produce'])=='SECURITY_FIXTURES_CREATED'
        assert run(prefix+[wsl_path(work/'from-windows'),'consume'])=='SECURITY_INTEROP_PASSED'
        assert run(prefix+[wsl_path(work/'from-linux'),'produce'])=='SECURITY_FIXTURES_CREATED'
        assert run([str(exe),str(plugin),str(module),str(work/'from-linux'),'consume'])=='SECURITY_INTEROP_PASSED'
        print('SECURITY_WINDOWS_LINUX_INTEROP_PASSED')
    print('SECURITY_TESTS_PASSED')
finally:script.unlink(missing_ok=True)
