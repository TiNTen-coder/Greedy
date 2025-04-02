import json
import matplotlib.pyplot as plt
import seaborn as sns

import os

os.system("rm -rf scripts/results_edf_misf_research")
os.system("mkdir scripts/results_edf_misf_research")

procs = [5, 10, 20, 30, 40, 50]
tasks = [500, 1000, 2000, 5000, 10000]


for i in procs:
    misf, edfb_misf, edff_misf, edfbase, edffollow = [], [], [], [], []
    for j in tasks:
        with open(f"scripts/edf_misf_layer_research/{i}/{j}/result_greedy.txt", "r") as f:
            misf.append(sum(list(map(int, f.readline().split()))) / 20)
        with open(f"scripts/edf_misf_layer_research/{i}/{j}/result_edfb_misf.txt", "r") as f:
            edfb_misf.append(sum(list(map(int, f.readline().split()))) / 20 / misf[-1])
        with open(f"scripts/edf_misf_layer_research/{i}/{j}/result_edff_misf.txt", "r") as f:
            edff_misf.append(sum(list(map(int, f.readline().split()))) / 20 / misf[-1])
        with open(f"scripts/edf_misf_layer_research/{i}/{j}/result_edfbase.txt", "r") as f:
            edfbase.append(sum(list(map(int, f.readline().split()))) / 20 / misf[-1])
        with open(f"scripts/edf_misf_layer_research/{i}/{j}/result_edffollow.txt", "r") as f:
            edffollow.append(sum(list(map(int, f.readline().split()))) / 20 / misf[-1])
        misf[-1] /= misf[-1]
    
    plt.figure(figsize=(10, 5))
    plt.title(f"{i} processors")
    plt.plot(tasks, misf, label='MISF', color='purple', marker='.')
    plt.plot(tasks, edfb_misf, label='EDFBase+MISF', color='orange', marker='.')
    plt.plot(tasks, edfbase, label='EDFBase', color='blue', marker='.')
    plt.plot(tasks, edffollow, label='EDFFollow', color='green', marker='.')
    plt.plot(tasks, edff_misf, label='EDFFollow+MISF', color='red', marker='.')

    plt.xticks(tasks)
    plt.xlabel('Tasks')
    plt.ylabel('Time')
    plt.legend()
    plt.grid(True)

    #plt.show()
    plt.savefig(f"scripts/results_edf_misf_research/{i}.png")