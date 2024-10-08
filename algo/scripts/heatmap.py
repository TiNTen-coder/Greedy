import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import json

process_values = [5, 10, 25, 50, 75, 100, 125, 150]
work_values = [2000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000]
data = []
for i in process_values:
    data.append([])
    for j in work_values:
        with open(f'output-ideal_1/{i}_{j}.json') as f:
            s = json.loads(f.read())
            data[-1].append(s['time'])
        with open(f'../../ideal_1-input_for_PROGRAMMIR_greedy_paper/{i}_{j}.in') as f:
            f.readline()
            s = f.readline()
            data[-1][-1] /= sum(list(map(int, s.split())))
            data[-1][-1] *= i


process_values_sorted = sorted(process_values, reverse=True)
data_sorted = data[::-1]  # Переворот строк матрицы

# Создание тепловой карты
plt.figure(figsize=(10, 6))
ax = sns.heatmap(data_sorted, annot=False, cmap='Greys', cbar_kws={'label': ''})

# Настройка осей
ax.set_xticklabels(work_values)
ax.set_yticklabels(process_values_sorted)

# Названия осей
plt.xlabel("Количество работ")
plt.ylabel("Количество процессоров")

# Показать цветовую шкалу
cbar = ax.collections[0].colorbar
cbar.set_label('Время (с)')
#cbar.set_ticks([1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4])

# Показать график
plt.show()
