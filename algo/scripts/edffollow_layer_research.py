import os
import json
from time import sleep

flag = "edfbase"
for i in [5, 10, 20, 30, 40, 50]:
    for j in [500, 1000, 2000, 5000, 10000]:
        #os.system(f"rm -rf scripts/edffollow_layer_research/{i}/{j}")
        #os.system(f"mkdir scripts/edffollow_layer_research/{i}/{j}")
        times = []
        for round_ in range(1, 21):
            os.system(f"build/sched --input ../layer_class_1-input/{i}_{j}.in --output scripts/edffollow_layer_research/{i}/{j}/{flag}_{round_}.json --conf config.toml --command {flag} --random {i * 10 + j * 100 + round_}")
            with open(f'scripts/edffollow_layer_research/{i}/{j}/{flag}_{round_}.json') as f:  
                data = json.loads(f.read())
                times.append(data["time"])
        with open(f"scripts/edffollow_layer_research/{i}/{j}/result_{flag}.txt", "w") as f:
            for q in range(len(times)):
                f.write(str(times[q]))
                if q < len(times) - 1:
                    f.write(" ")