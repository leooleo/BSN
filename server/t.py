import subprocess
import os


path = os.getcwd().rsplit('/', 1)[0] + '/odv/sensornode/configs/thermometer'
subprocess.Popen(['odsupercomponent', '--cid=112'], cwd=path)