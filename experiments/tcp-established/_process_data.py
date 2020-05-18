import json

def process_data():
    ## Client data
    log_file = open('./client.dat', 'w')
    #data_file = open('./iperf3-log/client.json')
    data_file = open('./log4.log')
    data = json.load(data_file)
    for interval in data['intervals']:
        _ts = interval['sum']['start']
        _bytes = interval['sum']['bytes'] / 1024.0 / 1024.0 #MB
        log_file.write('{} {:.2f}\n'.format(_ts, _bytes))
    log_file.close()
    data_file.close()

if __name__ == '__main__':
    process_data()
