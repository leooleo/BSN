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
app.secret_key = b'**alksjflak1224812798##%%$$$$'
app_processes  = []

oximeter = [         
    ['odsupercomponent', '--cid=112'],
    ['./DataCollectorApp.out', '--cid=112'],    
    ['./Filter.out',                 '--cid=112'],
    ['./SenderApp.out', '--cid=112', '--id=8080']
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

def execute_process(commands):
    global app_processes
    for command in commands:
        # Abre em uma thread os processos requisitados
        p = subprocess.Popen(command, stdout=subprocess.PIPE)
        # Coloca nos processos ativos os ids dos mesmos            
        app_processes.append(Process(p.pid,command[0] + ' ' + command[1], 'oximeter'))


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
    global oximeter, app_processes

    order = request.args.get('order','')
    if order == 'start':
        app_processes = []
        execute_process(oximeter)
        current_processes = ' '.join(str(x.pid) for x in app_processes)
        return 'bsn started the processes ' + current_processes

    elif order == 'stop':
        # Não mata nada caso não existam processos ativos
        if(app_processes == []):
            return 'App has no processes running'

        # Mata todos os processos ativos do app        
        for process in app_processes:
            # process.kill()            
            try :
                os.kill(process.pid, signal.SIGKILL)            
            except:
                continue

        current_processes = ' '.join(str(x.pid) for x in app_processes)           
        # # Limpa a lista dos processos ativos
        # app_processes = []
        return 'bsn stopped the processes ' + current_processes

@app.route('/')
def hello_world():
    return '<h1>Welcome to bsn</h1>'
    
app.run(debug=True)