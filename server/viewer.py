import subprocess
import os
p = os.popen(' '.join(['ps','-p','9842', '-o', 'cmd'])).read()

print(p)
if 'defunct' in p:
    print('zombie')
else:
    print('not zombie')