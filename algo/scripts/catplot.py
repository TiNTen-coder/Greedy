import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Patch
import os

# Настройки
processors_count = [5, 10, 20, 30, 40, 50]
task_counts = [500, 1000, 2000, 5000, 10000]
cr_limits = [0.2, 0.4, 0.6, 0.8]
colors = ["purple", "gold", "brown", "blue", "orange", "green", "red"]

# Пример данных (замени на реальные при необходимости)
for based_on in processors_count:
    os.system(f"mkdir scripts/histplots/{based_on}")
    for i in processors_count:
        data = []
        for j in task_counts:
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
                group.append((misf, eft, est, edfbase, edfb_misf, edffollow, edff_misf))
            data.append(group)

        # Построение
        fig, ax = plt.subplots(figsize=(12, 5))
        x_labels = []
        x_ticks = []

        bar_width = 0.01
        bar_gap = 0.005      # Расстояние между столбиками внутри CR
        group_spacing = 0.5  # Расстояние между разными task_counts
        cr_spacing = 0.12    # Расстояние между CR внутри одной группы (уменьшено)

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
        xtick_positions = [i * group_spacing + ((len(cr_limits) - 1) * cr_spacing) / 2 for i in range(len(task_counts))]
        ax.set_xticks([])
        #ax.set_xticklabels([str(tc) for tc in task_counts])
        ax.set_ylim(0.75, 1.0)
        ax.set_ylabel("TIME RATIO")
        ax.set_title(f"{i} processors (based on {based_on}_*)")
        ax.grid(axis='y', linestyle='--', alpha=0.5)

        # Подписи CR под подблоками
        for group_idx in range(len(task_counts)):
            group_base_x = group_idx * group_spacing
            for j, cr in enumerate(cr_limits):
                cr_base_x = group_base_x + j * cr_spacing
                ax.text(cr_base_x + 3 * (bar_width + bar_gap), 0.735, f"{cr}", ha='center')

        for center, count in zip(xtick_positions, task_counts):
            ax.text(center + 0.05, 0.725, f"{count}", ha='center', va='top', fontsize=9)

        # Подпись оси X
        ax.text(len(task_counts) / 2 - 1.3, 0.71, "NUMBER OF TASKS; CR LIMITS", 
                ha='center', va='top')

        # Легенда (опционально)
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
                bbox_to_anchor=(0.5, -0.22),  # сдвиг вниз
                ncol=7,                        # количество колонок
                fontsize=9,
                frameon=False)                # без рамки

        plt.tight_layout()
        #plt.show()
        plt.savefig(f"scripts/histplots/{based_on}/{i}_processors.png")
