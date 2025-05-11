#RUN FROM algo directory

import os

for i in [5, 10, 25, 50, 75, 100, 125, 150]:
    for j in [2000, 5000, 10000, 20000, 30000, 40000, 50000, 75000, 100000]:
        os.system(f"build/sched --input ../ideal_1-input_for_PROGRAMMIR_greedy_paper/{i}_{j}.in --output scripts/edffollow/{i}_{j}.json --conf config.toml --command edffollow")