import os
import json
from time import sleep


#folder = "all_heuristic_equal_graphs"
#dataset = "layer_class_1_equal_graphs"
#os.system(f"rm -rf scripts/edf_misf_layer_research")
#os.system(f"mkdir scripts/{folder}")

for based_on in [5, 10, 20, 30, 40, 50]:
    dataset = f"layer_class_1_based_on_{based_on}"
    folder = f"based_on_{based_on}_*"
    cr_bound = 40
    for flag in ["greedy", "edfbase", "edffollow", "edfb_misf", "edff_misf", "est", "eft"]:
        for i in [5, 10, 20, 30, 40, 50]:
            os.system(f"mkdir scripts/{folder}")
            os.system(f"mkdir scripts/{folder}/{cr_bound}")
            #os.system(f"rm -rf scripts/edf_misf_layer_research/{i}")
            os.system(f"mkdir scripts/{folder}/{cr_bound}/{i}")
            for j in [500, 1000, 2000, 5000, 10000]:
                #os.system(f"rm -rf scripts/edf_misf_layer_research/{i}/{j}")
                os.system(f"mkdir scripts/{folder}/{cr_bound}/{i}/{j}")
                os.system(f"build/opts --input ../{dataset}/{i}_{j}.in --output scripts/{folder}/{cr_bound}/{i}/{j}/{flag}.json --conf config.toml --command {flag} --random {i * 10 + j * 100}")
                with open(f'scripts/{folder}/{cr_bound}/{i}/{j}/{flag}.json') as f:  
                    data = json.loads(f.read())["time"]
                with open(f"scripts/{folder}/{cr_bound}/{i}/{j}/result_{flag}.txt", "w") as f:
                    f.write(str(data))
                os.system(f"rm -rf scripts/{folder}/{cr_bound}/{i}/{j}/{flag}.json")
    """for i in [5, 10, 25, 50, 75, 100, 125, 150]:
    #for i in [25, 50, 75, 100, 125, 150]:
        #os.system(f"rm -rf scripts/edf_misf_layer_research/{i}")
        os.system(f"mkdir scripts/edf_misf_ideal_research/{i}")
        for j in [2000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000]:
        #for j in [2000, 5000]:
            #os.system(f"rm -rf scripts/edf_misf_layer_research/{i}/{j}")
            os.system(f"mkdir scripts/edf_misf_ideal_research/{i}/{j}")
            times = []
            for round_ in range(1, 21):
                os.system(f"build/opts --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/{i}_{j}.in --output scripts/edf_misf_ideal_research/{i}/{j}/{flag}_{round_}.json --conf config.toml --command {flag} --random {i * 10 + j * 100 + round_}")
                with open(f'scripts/edf_misf_ideal_research/{i}/{j}/{flag}_{round_}.json') as f:  
                    data = json.loads(f.read())
                    times.append(data["time"])
            with open(f"scripts/edf_misf_ideal_research/{i}/{j}/result_{flag}.txt", "w") as f:
                for q in range(len(times)):
                    f.write(str(times[q]))
                    if q < len(times) - 1:
                        f.write(" ")
            for round_ in range(1, 21):
                os.system(f"rm -rf scripts/edf_misf_ideal_research/{i}/{j}/{flag}_{round_}.json")"""
""" 
flag = "edf_gc1"
for i in [5, 10, 20, 30, 40, 50]:
    for j in [500, 1000, 2000, 5000, 10000]:
        #os.system(f"mkdir scripts/edf_gc1_layer_research/{i}/{j}")
        times = []
        for round_ in range(1, 21):
            os.system(f"build/opts --input ../layer_class_1-input/{i}_{j}.in --output scripts/edf_gc1_layer_research/{i}/{j}/{flag}_{round_}.json --conf config.toml --command {flag} --random {i * 10 + j * 100 + round_}")
            with open(f'scripts/edf_gc1_layer_research/{i}/{j}/{flag}_{round_}.json') as f:  
                data = json.loads(f.read())
                times.append(data["time"])
        with open(f"scripts/edf_gc1_layer_research/{i}/{j}/result_{flag}.txt", "w") as f:
            for q in range(len(times)):
                f.write(str(times[q]))
                if q < len(times) - 1:
                    f.write(" ")
"""