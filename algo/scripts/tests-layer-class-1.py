#RUN FROM algo directory

import os

for i in [5, 10, 20, 30, 40, 50]:
    for j in [500, 1000, 2000, 5000, 10000]:
        os.system(f"build/opts --input ../layer_class_1-input/{i}_{j}.in --output scripts/output-layer-class-1/{i}_{j}.json --conf config.toml --command edfbase")