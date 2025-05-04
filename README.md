# Code section

## Build

```bash
bash compile.sh
```

## Build docs

```bash
doxygen Doxyfile
```

## All dependencies

1. `graph_partitioning`
1. `json_dumper`
    1. `nlohmann/json` - bundled
    1. `boost/lexical_cast`
1. `logging`
    1. `boost/log`
1. `parser`
    1. `boost/numeric`
1. `schedule`
    1. `boost/numeric`
    1. `boost/graph`
    1. `boost/property_map`
1. `time_schedule`
    1. `boost/numeric`
    1. `boost/iterator`
    1. `boost/range`
1. `main.cpp`
    1. `boost/program_options`
    1. `boost/lexical_cast`

## Datasets description

Each dataset is described by the following parameters:

- **N** — number of tasks  
- **P** — number of processors  
- **M** — number of edges in the task dependency graph

### Data Format

1. **First line**:  
   ```
   N  P  M
   ```

2. **Next P lines** — each line contains N integers:  
   Task execution costs for each of the N tasks on each of the P processors:  
   ```
   a₁  a₂  a₃  ...  aₙ
   ```

3. **Adjacency matrix (P × P)** — transfer time between processors:  
   `tᵢⱼ` represents the time to transfer data from processor *i* to processor *j*.  
   Example:
   ```
   0 2 ...
   2 2 ...
   ```

4. **Next M lines** — edges of the task dependency graph, one per line.

---

## How to use it

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
