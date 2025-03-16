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

for i in [5, 10, 25, 50, 75, 100, 125, 150]:
    for j in [2000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000]:
        os.system(f"build/opts --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/{i}_{j}.in --output scripts/gc1/{i}_{j}.json --conf config.toml --command greedy")
        os.system(f"build/opts --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/{i}_{j}.in --output scripts/edfbase/{i}_{j}.json --conf config.toml --command edfbase")
        os.system(f"build/opts --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/{i}_{j}.in --output scripts/edffollow/{i}_{j}.json --conf config.toml --command edffollow")
        with open(f'scripts/gc1/{i}_{j}.json') as f:
            s = json.loads(f.read())
            gc1.append(s['time'])
        with open(f'scripts/edfbase/{i}_{j}.json') as f:
            s = json.loads(f.read())
            edfbase.append(s['time'])
        with open(f'scripts/edffollow/{i}_{j}.json') as f:
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
