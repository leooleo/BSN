import subprocess
import os

def id_status(pid):
    p = os.popen(' '.join(['ps','-p', str(pid), '-o', 'cmd'])).read()

    if ( p == "CMD\n" ):
        return('inexistent')

    else:
        name = p.split('CMD\n')[1]
        print(name)
        name = name.rstrip()
        
        if 'defunct' in p:            
            return('zombie')
        else:
            return('active')
