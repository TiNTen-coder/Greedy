"""
import os
import numpy as np
import matplotlib as plt
import matplotlib.pyplot as plt2
import seaborn as sns


fig, axes = plt2.subplots(3, 2, figsize=(6, 8))  # 5x2 grid for 10 samples
for i in [5, 10, 20, 30, 40, 50]:
    data = []
    for j in [500, 1000, 2000, 5000, 10000]:
        first, second = [], []
        with open(f"scripts/edffollow_layer_research/{i}/{j}/result_edffollow.txt", "r") as f:
            first = list(map(int, f.readline().split()))
        with open(f"scripts/edffollow_layer_research/{i}/{j}/result_edfbase.txt", "r") as g:
            second = list(map(int, g.readline().split()))
        data.append(first[0]/second[0])
    for j, ax in enumerate(axes.flatten()):
        sns.boxplot(data=data, ax=ax)
        ax.set_title(f"Boxplot график {j}")
        ax.set_xlabel("Выборки")
        ax.set_ylabel("Значения")

plt2.tight_layout()
plt2.show()
"""
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np


procs = [5, 10, 20, 30, 40, 50]
tasks = [500, 1000, 2000, 5000, 10000]
data_sets = []
for i in procs:
    same_value_procs = []
    for j in tasks:
        first, second = [], []
        with open(f"scripts/edffollow_layer_research/{i}/{j}/result_edffollow.txt", "r") as f:
            first = list(map(int, f.readline().split()))
        with open(f"scripts/edffollow_layer_research/{i}/{j}/result_edfbase.txt", "r") as g:
            second = list(map(int, g.readline().split()))
        data = []
        for k in range(20):
            data.append(first[k] / second[k])
        same_value_procs.append(data)
    data_sets.append(same_value_procs)

fig, axes = plt.subplots(3, 2, figsize=(12, 9))

for i, ax in enumerate(axes.flatten()):
    sns.boxplot(data=data_sets[i], ax=ax)
    ax.set_title(f"Количество процессоров: {procs[i]}")
    ax.set_xlabel("Количество задач")
    ax.set_ylabel("Отношение follow к base")
    ax.set_xticklabels(tasks)
    ax.set_yticks([0.9, 0.95, 1.00, 1.05, 1.1])
    ax.axhline(y=1, color="black", linestyle='--', linewidth=1.5)

plt.tight_layout()
plt.show()
