from flask import Flask, request, session
import subprocess
import os
import signal
from time import sleep

class Process:
    def __init__(self, pid, name, father):
        self.pid = pid
        self.name = name
        self.father = father        

app = Flask(__name__)
app_processes  = []

thermometer = [             
    ['odsupercomponent','--cid=112'],
    ['./../odv/sensornode/collector/build/DataCollectorApp', '--cid=112'],    
    ['./../odv/sensornode/filter/build/Filter',              '--cid=112'],
    ['./../odv/sensornode/sender/build/SenderApp', '--cid=112', '--id=8080']
]

oximeter = [             
    ['odsupercomponent','--cid=113'],
    ['./../odv/sensornode/collector/build/DataCollectorApp', '--cid=113'],    
    ['./../odv/sensornode/filter/build/Filter',              '--cid=113'],
    ['./../odv/sensornode/sender/build/SenderApp', '--cid=113', '--id=8081']
]

sphygmomanometer = [
    ['odsupercomponent','--cid=114'],
    ['./../odv/sensornode/collector/build/DataCollectorApp', '--cid=114', '--id=0'],    
    ['./../odv/sensornode/collector/build/DataCollectorApp', '--cid=114', '--id=1'],
    ['./../odv/sensornode/filter/build/Filter',              '--cid=114'],
    ['./../odv/sensornode/sender/build/SenderApp', '--cid=114', '--id=8082']
]

ecg = [
    ['odsupercomponent','--cid=115'],
    ['./../odv/sensornode/collector/build/DataCollectorApp', '--cid=115'],    
    ['./../odv/sensornode/filter/build/Filter',              '--cid=115'],
    ['./../odv/sensornode/sender/build/SenderApp', '--cid=115', '--id=8083']
]

centralhub = [
    ['odsupercomponent','--cid=116'],
    ['./../odv/centralhub/listener/build/tcp_listenerApp', '--cid=116', '--id=8080'],
    ['./../odv/centralhub/listener/build/tcp_listenerApp', '--cid=116', '--id=8081'],
    ['./../odv/centralhub/listener/build/tcp_listenerApp', '--cid=116', '--id=8082'],
    ['./../odv/centralhub/listener/build/tcp_listenerApp', '--cid=116', '--id=8083'],
    ['./../odv/centralhub/processor/build/ProcessorApp', '--cid=116']
]

def id_status(pid):
    p = os.popen(' '.join(['ps','-p', str(pid), '-o', 'cmd'])).read()
    if ( p == "CMD\n" ):
        return('inexistent')

    else:        
        if 'defunct' in p:            
            return('zombie')
        else:
            return('active')

def execute_process(commands, father_name):
    global app_processes
    for command in commands:
        # Abre em uma thread os processos requisitados
        if(command[0] == 'odsupercomponent' and father_name != 'centralhub'):
            # Para os supercomponents do sensornode
            path = os.getcwd().rsplit('/', 1)[0] + '/odv/sensornode/configs/' + father_name
            p = subprocess.Popen(command, stdout=subprocess.PIPE, cwd=path)

        elif(command[0] == 'odsupercomponent' and father_name == 'centralhub'):
            # Para os supercomponents do centralhub
            path = os.getcwd().rsplit('/', 1)[0] + '/odv/centralhub/configs/'
            p = subprocess.Popen(command, stdout=subprocess.PIPE, cwd=path)

        else:            
            p = subprocess.Popen(command, stdout=subprocess.PIPE)
        # Coloca nos processos ativos os ids dos mesmos
        if(command[0] != 'odsupercomponent'):
            name = command[0].rsplit('/',1)[1]
        else: 
            name = command[0]            
            
        app_processes.append(Process(p.pid,name, father_name))


@app.route('/status')
def get_status():
    ret = ""
    for process in app_processes:
        pid = process.pid
        string = process.father + ' => ' + process.name + ' ' + id_status(pid) + '<br>'
        ret += string

    return ret, 200

@app.route('/bsn')
def bsn():
    global thermometer, app_processes

    order = request.args.get('order','')
    if order == 'start':
        app_processes = []
        execute_process(centralhub, 'centralhub')
        execute_process(sphygmomanometer, 'sphygmomanometer')
        execute_process(thermometer, 'thermometer')
        execute_process(oximeter, 'oximeter')
        execute_process(ecg, 'ecg')
        current_processes = ' '.join(str(x.pid) for x in app_processes)
        return 'bsn started the processes ' + current_processes

    elif order == 'stop':
        # Não mata nada caso não existam processos ativos
        if(app_processes == []):
            return 'App has no processes running'

        # Mata todos os processos ativos do app        
        for process in app_processes:    
            try :
                os.kill(process.pid, signal.SIGKILL)
            except:
                continue

        current_processes = ' '.join(str(x.pid) for x in app_processes)

        return 'bsn stopped the processes ' + current_processes

@app.route('/')
def hello_world():
    return '<h1>Welcome to bsn</h1>'
    
app.run(debug=True)