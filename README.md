# Операционная система
Сборка проекта проводилась на операционной системе MacOS Sequoia 15.3

Проект успешно собирается на операционной системе Linux Ubuntu 20.04 LTS

# Секция кода

## Сборка

```bash
bash compile.sh
```

## Сборка документации

```bash
doxygen Doxyfile
```

## Все зависимости
Для сборки проекта необходима библиотека boost.

Установка на MacOS

```bash
brew install boost
```

Используемая в проекте версия:

boost: stable 1.87.0 (bottled)


## Описание входных данных

Каждый файл входных данных описывается следующими параметрами: 

- **N** — Количество работ
- **P** — Количество процессоров  
- **M** — Набор ребер в графе связей работ

### Формат входных данных

1. **Первая строка**:  
   ```
   N  P  M
   ```

2. **Следующие *P* строк** — каждая строка содержит *N* чисел:
3. Времена выполнения для каждой из *N* работ на каждом из *P* процессоров:  
   ```
   a₁  a₂  a₃  ...  aₙ
   ```

4. **Матрица смежности (P × P)** — время межпроцессорных передач данных:  
   `tᵢⱼ` представляет время передачи данных с процессора *i* на процессор *j*.  
   Пример:
   ```
   0 2 ...
   2 2 ...
   ```

5. **Следующие M строк** — вершины, связанные ребрами в графе, по одной в строке.

---

## Использование и запуск

Запуск одного эксперимента:

Сборка проекта:

```bash
bash compile.sh
```

Запуск бинарного файла

```bash
build/opts 
```


```bash
cd algo
```

Then:

```bash
python3 scripts/filename.py
```

or

```bash
build/opts 
```

### General options:
```
  --input                    Input dataset path
  --output                   Output result path
  --conf                     Configuration file path
  --command                  Heuristic name
  --random                   Random METIS flag
```

### Example:
```bash
build/opts --input ../{dataset}/{i}_{j}.in \
           --output scripts/{folder}/{cr_bound}/{i}/{j}/{flag}.json \
           --conf config.toml \
           --command {flag} \
           --random $((i * 10 + j * 100))
```
