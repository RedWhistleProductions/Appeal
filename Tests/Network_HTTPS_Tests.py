"""Certificate-verified HTTPS, HTTP compatibility, bad peers, and shutdown."""
from pathlib import Path
import datetime,http.client,os,socket,ssl,subprocess,sys,tempfile,time,uuid,warnings
root=Path(__file__).resolve().parent.parent
sys.path.insert(0,str(root/'Resources/Modules/Appeal_Module_Security/Appeal_Sodium_Security/.deps/verifier'))
from cryptography import x509
from cryptography.hazmat.primitives import hashes,serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.x509.oid import NameOID
from ipaddress import ip_address
script=root/'Resources/Appeal/Test'/('HTTPS_'+uuid.uuid4().hex+'.Appeal')
def port():
    with socket.socket() as s:s.bind(('127.0.0.1',0));return s.getsockname()[1]
def save_key(path,key):path.write_bytes(key.private_bytes(serialization.Encoding.PEM,serialization.PrivateFormat.PKCS8,serialization.NoEncryption()))
try:
 with tempfile.TemporaryDirectory(prefix='HTTPS_',dir=root/'Tests') as temp:
    folder=Path(temp);now=datetime.datetime.now(datetime.timezone.utc)
    ca_key=ec.generate_private_key(ec.SECP256R1());name=x509.Name([x509.NameAttribute(NameOID.COMMON_NAME,'Appeal test CA')])
    ca=(x509.CertificateBuilder().subject_name(name).issuer_name(name).public_key(ca_key.public_key()).serial_number(x509.random_serial_number()).not_valid_before(now-datetime.timedelta(days=1)).not_valid_after(now+datetime.timedelta(days=2)).add_extension(x509.BasicConstraints(ca=True,path_length=0),critical=True).sign(ca_key,hashes.SHA256()))
    key=ec.generate_private_key(ec.SECP256R1());leaf_name=x509.Name([x509.NameAttribute(NameOID.COMMON_NAME,'localhost')])
    leaf=(x509.CertificateBuilder().subject_name(leaf_name).issuer_name(name).public_key(key.public_key()).serial_number(x509.random_serial_number()).not_valid_before(now-datetime.timedelta(hours=1)).not_valid_after(now+datetime.timedelta(days=1)).add_extension(x509.SubjectAlternativeName([x509.DNSName('localhost'),x509.IPAddress(ip_address('127.0.0.1'))]),critical=False).sign(ca_key,hashes.SHA256()))
    (folder/'ca.pem').write_bytes(ca.public_bytes(serialization.Encoding.PEM));(folder/'cert.pem').write_bytes(leaf.public_bytes(serialization.Encoding.PEM)+ca.public_bytes(serialization.Encoding.PEM));save_key(folder/'key.pem',key)
    save_key(folder/'wrong-key.pem',ec.generate_private_key(ec.SECP256R1()))
    page=b'<html>Appeal HTTPS verified</html>';(folder/'index.html').write_bytes(page)
    exe=folder/'HTTPS.exe';subprocess.run(['g++','-std=c++17','Tests/Network_HTTPS_Driver.cpp','-o',str(exe)],cwd=root,check=True)
    secure,plain=port(),port();plugin=root/'Resources/Modules/Appeal_Module_Network/Appeal_SDL2_Network/Appeal_SDL2_Network.dll'
    process=subprocess.Popen([str(exe),str(plugin),str(folder/'cert.pem'),str(folder/'key.pem'),str(folder),str(secure),str(plain)],cwd=root,env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH','')),stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
    try:
        assert process.stdout.readline().strip()=='READY'
        context=ssl.create_default_context(cafile=str(folder/'ca.pem'))
        def fetch(ctx=context):
            connection=http.client.HTTPSConnection('localhost',secure,context=ctx,timeout=3)
            try:
                connection.request('GET','/');response=connection.getresponse();assert response.status==200;assert response.read()==page
            finally:connection.close()
        fetch()
        plain_client=http.client.HTTPConnection('127.0.0.1',plain,timeout=3);plain_client.request('GET','/');response=plain_client.getresponse();assert response.status==200 and response.read()==page;plain_client.close()
        try:fetch(ssl.create_default_context());raise AssertionError('Untrusted CA accepted')
        except ssl.SSLCertVerificationError:pass
        with socket.create_connection(('127.0.0.1',secure),timeout=3) as raw:
            try:context.wrap_socket(raw,server_hostname='wrong.example');raise AssertionError('Wrong hostname accepted')
            except ssl.SSLCertVerificationError:pass
        with socket.create_connection(('127.0.0.1',secure),timeout=3) as raw:
            raw.sendall(b'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n')
            try:assert b'200 OK' not in raw.recv(4096)
            except ConnectionResetError:pass
        with warnings.catch_warnings():
            warnings.simplefilter('ignore',DeprecationWarning)
            legacy=ssl.create_default_context(cafile=str(folder/'ca.pem'));legacy.minimum_version=ssl.TLSVersion.TLSv1_1;legacy.maximum_version=ssl.TLSVersion.TLSv1_1;legacy.set_ciphers('ALL:@SECLEVEL=0')
            try:fetch(legacy);raise AssertionError('TLS 1.1 accepted')
            except ssl.SSLError:pass
        peers=[socket.create_connection(('127.0.0.1',secure),timeout=3) for _ in range(4)]
        fetch() # Incomplete handshakes must not stall healthy clients.
        start=time.monotonic();process.stdin.write('stop\n');process.stdin.flush();assert process.stdout.readline().strip()=='STOPPED';assert time.monotonic()-start<2
        for peer in peers:peer.close()
        process.stdin.write('restart\n');process.stdin.flush();assert process.stdout.readline().strip()=='RESTARTED';fetch()
        process.stdin.write('quit\n');process.stdin.flush();out,err=process.communicate(timeout=5);assert process.returncode==0 and not err,(out,err)
    finally:
        if process.poll() is None:process.kill();process.communicate()
    # HTTPS also works with no SDL/plaintext listeners in the socket set.
    development=folder/'development'
    helper=root/'Resources/Modules/Appeal_Module_Network/Appeal_SDL2_Network/Packaging/Create_Development_Certificate.py'
    generated=subprocess.run([sys.executable,str(helper),str(development)],cwd=root,capture_output=True,text=True,timeout=15)
    assert generated.returncode==0,(generated.stdout,generated.stderr)
    dev_context=ssl.create_default_context(cafile=str(development/'server.crt'))
    secure=port()
    process=subprocess.Popen([str(exe),str(plugin),str(development/'server.crt'),str(development/'server.key'),str(folder),str(secure),'0'],cwd=root,env=dict(os.environ,PATH=str(root)+os.pathsep+os.environ.get('PATH','')),stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
    try:
        assert process.stdout.readline().strip()=='READY';fetch(dev_context)
        process.stdin.write('quit\n');process.stdin.flush();out,err=process.communicate(timeout=5);assert process.returncode==0 and not err,(out,err)
    finally:
        if process.poll() is None:process.kill();process.communicate()
    # Wrong credentials fail through the real Appeal module before any listener opens.
    script.write_text('Add_Dynamic_Module Network Appeal_SDL2_Network\n'+f'Print (Network Configure_TLS "{(folder/"cert.pem").as_posix()}" "{(folder/"wrong-key.pem").as_posix()}")\nNetwork Configure HTTPS 127.0.0.1 {port()}\nNetwork Server\nPrint (Network Is_Server_Running)\nNetwork Stop_Server\nDone\n')
    result=subprocess.run([str(root/'Appeal.exe'),'Test/'+script.stem],cwd=root,capture_output=True,text=True,timeout=10)
    assert result.returncode==0 and not result.stderr,(result.stdout,result.stderr)
    assert result.stdout.strip().splitlines()==['0','0'],result.stdout
    print('NETWORK_HTTPS_PASSED')
finally:script.unlink(missing_ok=True)
