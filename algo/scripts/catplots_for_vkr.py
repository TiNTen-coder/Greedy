import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Patch
import os

# Настройки глобальных шрифтов
plt.rc('font', size=14)          # базовый размер шрифта
plt.rc('axes', titlesize=14)     # заголовок осей
plt.rc('axes', labelsize=14)     # подписи осей
plt.rc('xtick', labelsize=14)    # подписи xtick
plt.rc('ytick', labelsize=14)    # подписи ytick
plt.rc('legend', fontsize=14)    # легенда
plt.rc('figure', titlesize=14)   # заголовок фигуры

# Настройки
processors_count = [5, 10, 20, 30, 40, 50]
task_counts = [500, 1000, 2000, 5000, 10000]
cr_limits = [0.2, 0.4, 0.6, 0.8]
colors = ["purple", "gold", "brown", "blue", "orange", "green", "red"]

# Пример данных (замени на реальные при необходимости)
for i in processors_count:
    for j in task_counts:
        data = []
        for based_on in processors_count:
            group = []
            folder = f"based_on_{based_on}_*"
            for cr_bound in cr_limits:
                misf, edfb_misf, edff_misf, edfbase, edffollow, est, eft = [], [], [], [], [], [], []
                with open(f"scripts/{folder}/{int(cr_bound * 100)}/{i}/{j}/result_greedy.txt", "r") as f:
                    misf.append(int(f.readline()))
                with open(f"scripts/{folder}/{int(cr_bound * 100)}/{i}/{j}/result_edfb_misf.txt", "r") as f:
                    edfb_misf.append(int(f.readline()) / misf[-1])
                with open(f"scripts/{folder}/{int(cr_bound * 100)}/{i}/{j}/result_edff_misf.txt", "r") as f:
                    edff_misf.append(int(f.readline()) / misf[-1])
                with open(f"scripts/{folder}/{int(cr_bound * 100)}/{i}/{j}/result_edfbase.txt", "r") as f:
                    edfbase.append(int(f.readline()) / misf[-1])
                with open(f"scripts/{folder}/{int(cr_bound * 100)}/{i}/{j}/result_edffollow.txt", "r") as f:
                    edffollow.append(int(f.readline()) / misf[-1])
                with open(f"scripts/{folder}/{int(cr_bound * 100)}/{i}/{j}/result_est.txt", "r") as f:
                    est.append(int(f.readline()) / misf[-1])
                with open(f"scripts/{folder}/{int(cr_bound * 100)}/{i}/{j}/result_eft.txt", "r") as f:
                    eft.append(int(f.readline()) / misf[-1])
                misf[-1] /= misf[-1]
                group.append([misf, eft, est, edfbase, edfb_misf, edffollow, edff_misf])
            data.append(group)

        average = {
            "20": {_: 0 for _ in ["misf", "eft", "est", "edfbase", "edfb_misf", "edffollow", "edff_misf"]},
            "40": {_: 0 for _ in ["misf", "eft", "est", "edfbase", "edfb_misf", "edffollow", "edff_misf"]},
            "60": {_: 0 for _ in ["misf", "eft", "est", "edfbase", "edfb_misf", "edffollow", "edff_misf"]},
            "80": {_: 0 for _ in ["misf", "eft", "est", "edfbase", "edfb_misf", "edffollow", "edff_misf"]}
        }
        cccc = ["20", "40", "60", "80"]
        heuristicsss = ["misf", "eft", "est", "edfbase", "edfb_misf", "edffollow", "edff_misf"]
        for xxx in data:
            for yyy in range(len(xxx)):
                for zzz in range(len(xxx[yyy])):
                    average[cccc[yyy]][heuristicsss[zzz]] += xxx[yyy][zzz][0]
        group = []
        for xxx in range(len(cccc)):
            average_data = []
            for yyy in range(len(heuristicsss)):
                average_data.append([average[cccc[xxx]][heuristicsss[yyy]] / len(data)])
            group.append(average_data)
        data.append(group)

        # Построение
        fig, ax = plt.subplots(figsize=(18, 7))  # увеличено

        x_labels = []
        x_ticks = []

        bar_width = 0.01
        bar_gap = 0.005
        group_spacing = 0.565
        cr_spacing = 0.13

        for group_idx, group in enumerate(data):
            group_base_x = group_idx * group_spacing
            for bar_idx, (misf, edfb_misf, edfbase, edffollow, edff_misf, est, eft) in enumerate(group):
                cr_base_x = group_base_x + bar_idx * cr_spacing
                values = [misf[0], edfb_misf[0], edfbase[0], edffollow[0], edff_misf[0], est[0], eft[0]]
                for method_idx, value in enumerate(values):
                    x = cr_base_x + method_idx * (bar_width + bar_gap)
                    ax.bar(x, value, color=colors[method_idx], width=bar_width)
                x_labels.append(f"{cr_limits[bar_idx]}")
                x_ticks.append(cr_base_x + 3 * (bar_width + bar_gap))

        # Настройка осей
        xtick_positions = [i * group_spacing + ((len(cr_limits) - 1) * cr_spacing) / 2 for i in range(len(processors_count) + 1)]
        ax.set_xticks([])
        ax.set_ylim(0.75, 1.0)
        ax.set_ylabel("НОРМИР. ДЛИТ. РАСПИСАНИЯ")
        ax.set_title(f"{j} РАБОТ {i} ПРОЦЕССОРОВ")
        ax.grid(axis='y', linestyle='--', alpha=0.5)

        # Подписи CR
        for group_idx in range(len(processors_count) + 1):
            group_base_x = group_idx * group_spacing
            for xxx, cr in enumerate(cr_limits):
                cr_base_x = group_base_x + xxx * cr_spacing
                ax.text(cr_base_x + 3 * (bar_width + bar_gap), 0.735, f"{cr}", ha='center')

        for center, count in zip(xtick_positions, list(range(1, len(processors_count) + 1)) + ["среднее"]):
            ax.text(center + 0.05, 0.725, f"{count}", ha='center', va='top', fontsize=14)


        # Подпись оси X
        ax.text((len(processors_count) + 1) / 2 - 1.58, 0.71, "НОМЕР ГРАФА; ОГРАНИЧЕНИЕ НА CR", 
                ha='center', va='top')

        # Легенда
        legend_elements = [
            Patch(facecolor='purple', label='MISF'),
            Patch(facecolor='gold', label='EFT'),
            Patch(facecolor='brown', label='EST'),
            Patch(facecolor='blue', label='EDFBase'),
            Patch(facecolor='orange', label='EDFBase+MISF'),
            Patch(facecolor='green', label='EDFFollow'),
            Patch(facecolor='red', label='EDFFollow+MISF'),
        ]

        ax.legend(handles=legend_elements,
                  loc='upper center',
                  bbox_to_anchor=(0.5, -0.25),  # сдвинуто ниже
                  ncol=7,
                  frameon=False)

        plt.tight_layout()
        plt.subplots_adjust(bottom=0.32)  # добавлен дополнительный отступ
        plt.savefig(f"scripts/new_histplots/{i}_{j}.png")
        #plt.show()
