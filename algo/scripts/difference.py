import json
import matplotlib.pyplot as plt

was = []
my = []
bottom = []
for i in [5, 10, 20, 30, 40, 50]:
    for j in [500, 1000, 2000, 5000, 10000]:
        bottom.append(f'{i}_{j}')
        with open(f'../../GC1_CR_layers-results_and_logs/{i}_{j}.json') as f:
            s = json.loads(f.read())
            was.append(s['time'] / 1000)
        with open(f'output-layer-class-1/{i}_{j}.json') as f:
            s = json.loads(f.read())
            my.append(s['time'] / 1000)

plt.figure(figsize=(40, 10))

plt.plot(bottom, was, label='was', color='red')
plt.plot(bottom, my, label='my', color='blue')
plt.xlabel('procs&tasks')
plt.ylabel('Time')
plt.legend()
plt.grid(True)

plt.show()