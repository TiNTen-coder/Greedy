"""import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import json
from pprint import pprint
import seaborn as sns

process_values = [5, 10, 25, 50, 75, 100, 125, 150]
work_values = [2000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000]
old = []
new = []
for i in process_values:
    old.append([])
    new.append([])
    for j in work_values:
        with open(f'scripts/edfbase_old/{i}_{j}.json') as f:
            s = json.loads(f.read())
            old[-1].append(s['time'])
        with open(f'scripts/edfbase_new/{i}_{j}.json') as f:
            s = json.loads(f.read())
            new[-1].append(s['time'])

plt.figure(figsize=(13, 8))
#pprint(data)
#formats = ['o-', 'X--', 's:', 'P-', '-.', 'D-', '2-.', '*-']
line_styles = ['-', '--', ':', '-.', '-.', '-.', '-.', '-.']
markers = ['o', 'X', 's', 'P', 'D', 'p', '^', 'H']
colors = ['black', 'black', 'black', 'black', 'red', 'blue', 'orange', 'green']

for i in range(len(process_values)):
    sns.lineplot(x=work_values, y=old[i], linestyle=line_styles[i], marker=markers[i], color=colors[i], 
                 label=str(process_values[i]), markersize=10)


#for i in range(len(process_values)):
#    plt.plot(work_values, data[i], linestyle=line_styles[i], marker=markers[i], color='black', label=str(process_values[i]), markersize=10)
#plt
plt.legend(fontsize=15, handlelength=6)
# Показать график

plt.grid(True, which='both', linestyle='--', linewidth=0.7)

plt.tight_layout()
plt.xticks(work_values)
plt.show()
"""

import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import json
from pprint import pprint
import seaborn as sns

process_values = [5, 10, 25, 50, 75, 100, 125, 150]
work_values = [2000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000]
data = []
for i in process_values:
    data.append([])
    for j in work_values:
        with open(f'scripts/edfbase_new/{i}_{j}.json') as f:
            s = json.loads(f.read())
            data[-1].append(s['time'])
        with open(f'../ideal_1-input_for_PROGRAMMIR_greedy_paper/{i}_{j}.in') as f:
            f.readline()
            s = f.readline()
            data[-1][-1] /= sum(list(map(int, s.split())))
            data[-1][-1] *= i

plt.figure(figsize=(13, 8))

line_styles = ['-', '--', ':', '-.', '-.', '-.', '-.', '-.']
markers = ['o', 'X', 's', 'P', 'D', 'p', '^', 'H']
colors = ['black', 'black', 'black', 'black', 'red', 'blue', 'orange', 'green']

for i in range(len(process_values)):
    sns.lineplot(x=work_values, y=data[i], linestyle=line_styles[i], marker=markers[i], color=colors[i], 
                 label=str(process_values[i]), markersize=10)


#for i in range(len(process_values)):
#    plt.plot(work_values, data[i], linestyle=line_styles[i], marker=markers[i], color='black', label=str(process_values[i]), markersize=10)
#plt
plt.legend(fontsize=15, handlelength=6)
# Показать график

plt.grid(True, which='both', linestyle='--', linewidth=0.7)
plt.title("EDF Base")
plt.tight_layout()
optimal = [1 + i / 20 for i in range(12)]
plt.xticks(work_values)
plt.yticks(optimal)
plt.show()
