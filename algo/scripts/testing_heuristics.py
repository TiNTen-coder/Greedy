#RUN FROM algo directory

import json
import matplotlib.pyplot as plt
import seaborn as sns

import os

os.system("rm -rf scripts/gc1")
os.system("mkdir scripts/gc1")
os.system("rm -rf scripts/edffollow")
os.system("mkdir scripts/edffollow")
os.system("rm -rf scripts/edfbase")
os.system("mkdir scripts/edfbase")
gc1, edfbase, edffollow = [], [], []
bottom = [40000, 50000, 75000, 100000]
for j in bottom:
    os.system(f"build/sched --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/125_{j}.in --output scripts/gc1/125_{j}.json --conf config.toml --command greedy")
    os.system(f"build/sched --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/125_{j}.in --output scripts/edfbase/125_{j}.json --conf config.toml --command edfbase")
    os.system(f"build/sched --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/125_{j}.in --output scripts/edffollow/125_{j}.json --conf config.toml --command edffollow")
    with open(f'scripts/gc1/125_{j}.json') as f:
        s = json.loads(f.read())
        gc1.append(s['time'])
    with open(f'scripts/edfbase/125_{j}.json') as f:
        s = json.loads(f.read())
        edfbase.append(s['time'])
    with open(f'scripts/edffollow/125_{j}.json') as f:
        s = json.loads(f.read())
        edffollow.append(s['time'])

plt.figure(figsize=(40, 10))
plt.title("125 processors")
plt.plot(bottom, gc1, label='GC1', color='red')
plt.plot(bottom, edfbase, label='EDFBase', color='blue')
plt.plot(bottom, edffollow, label='EDFFollow', color='green')

plt.xticks(bottom)
plt.xlabel('Tasks')
plt.ylabel('Time')
plt.legend()
plt.grid(True)

plt.show()
