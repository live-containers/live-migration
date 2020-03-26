import paramiko

HOST = "192.168.56.1"
VM1 = "192.168.56.101"
VM2 = "192.168.56.103"

class ConnHandler:

    def __init__(self, host):
        self.client = paramiko.client.SSHClient()
        # If we try to connect to a new host (whose key is not in the known
        # hosts file) automatically add it.
        self.client.set_missing_host_key_policy(paramiko.client.AutoAddPolicy)
        self.host = host
        try:
            self.client.load_system_host_keys()
        except Exception as e:
            print("Could not access system's host keys!")
        self.client.connect(host)

    def read_remote_file(self, path):
        self.sftp_cli = self.client.open_sftp()
        remote_file = self.sftp_cli.file(path, 'r')
        for line in remote_file.read().decode('utf-8').split('\n'):
            print(line)


def main():
    conn_vm1 = ConnHandler(VM2)
    _, out, __ = conn_vm1.client.exec_command('ls -l')
    for line in out:
        print(line.strip('\n'))
    conn_vm1.read_remote_file("setup.sh")

if __name__=="__main__":
    main()
