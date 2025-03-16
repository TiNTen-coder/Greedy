import numpy as np
import matplotlib.pyplot as plt
import json
from mpl_toolkits.mplot3d import Axes3D

# Данные
works = [2000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000]  # работы (ось X)
processors = [5, 10, 25, 50, 75, 100, 125, 150]  # процессоры (ось Y)
processors_grid, works_grid = np.meshgrid(works, processors)

# Времена выполнения для двух методов (две плоскости)
old = []
new = []
for i in processors:
    old.append([])
    new.append([])
    for j in works:
        with open(f'scripts/edfbase_old/{i}_{j}.json') as f:
            s = json.loads(f.read())
            old[-1].append(s['time'])
        with open(f'scripts/edfbase_new/{i}_{j}.json') as f:
            s = json.loads(f.read())
            new[-1].append(s['time'])
        print(old[-1][-1], new[-1][-1], old[-1][-1] >= new[-1][-1])

fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')

old = np.array(old)
new = np.array(new)

# Построение плоскостей
surf1 = ax.plot_surface(works_grid, processors_grid, old, cmap='viridis', alpha=0.7)
surf2 = ax.plot_surface(works_grid, processors_grid, new, cmap='plasma', alpha=0.7)

# Настройка осей
ax.set_xlabel('Работы (X)')
ax.set_ylabel('Процессоры (Y)')
ax.set_zlabel('Время выполнения (Z)')
ax.set_title('Сравнение двух методов построения расписаний')

# Добавление кастомной легенды
legend_labels = ['Метод 1', 'Метод 2']
legend_colors = [plt.cm.viridis(0.5), plt.cm.plasma(0.5)]
for label, color in zip(legend_labels, legend_colors):
    ax.scatter([], [], [], color=color, label=label)  # фиктивные точки для легенды
ax.legend(loc='upper left')

# Показ графика
plt.show()
