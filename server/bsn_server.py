from flask import Flask, request, session
import subprocess
import os
import signal
from time import sleep

app = Flask(__name__)
app.secret_key = b'**alksjflak1224812798##%%$$$$'
app_processes  = []

thermometer = [         
    ['sudo', 'odsupercomponent', '--cid=111'],
    ['./DataCollectorApp', '--cid=111'],    
    ['./Filter',                 '--cid=111'],
    ['./SenderApp', '--cid=111', '--id=8080']
]

def execute_process(processes):
    global app_processes
    for process in processes:        
        # Abre em uma thread os processos requisitados
        p = subprocess.Popen(process, stdout=subprocess.PIPE)
        # Coloca nos processos ativos os ids dos mesmos
        app_processes.append(p)        


@app.route('/bsn')
def bsn():
    global thermometer, app_processes

    order = request.args.get('order','')
    if order == 'start':
        execute_process(thermometer)
        current_processes = ' '.join(str(x.pid) for x in app_processes)
        return 'bsn started the processes ' + current_processes

    elif order == 'stop':
        # Não mata nada caso não existam processos ativos
        if(app_processes == []):
            return 'App has no processes running'

        # Mata todos os processos ativos do app        
        for process in app_processes:
            # process.kill()
            os.kill(process.pid, signal.SIGKILL)
            # os.killpg(os.getpgid(pid), signal.SIGTERM)

        current_processes = ' '.join(str(x.pid) for x in app_processes)           
        # Limpa a lista dos processos ativos
        app_processes = []
        return 'bsn stopped the processes ' + current_processes

@app.route('/')
def hello_world():
    return '<h1>Welcome to bsn</h1>'
    
app.run(debug=True)