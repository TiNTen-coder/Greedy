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
    1. `nlohmann/json` - bundeled
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

## How to use it
```bash
cd algo
```
THAN

```bash
python3 scripts/filename.py
```

or

```bash
build/opts 

General options:
  --input                    Input dataset path
  --output                   Output result path
  --conf                     Configuration file path
  --command                  Heuristic name
  --random                   Random METIS flag

Example:
  build/opts --input ../{dataset}/{i}_{j}.in --output scripts/{folder}/{cr_bound}/{i}/{j}/{flag}.json --conf config.toml --command {flag} --random {i * 10 + j * 100}
```
