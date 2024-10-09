#RUN FROM algo directory

import os
import json

flag = True
for i in [5, 10, 20, 30, 40, 50]:
    for j in [500, 1000, 2000, 5000, 10000]:
        last_try = open(f'scripts/output-layer-class-1/{i}_{j}.json')
        base = open(f'scripts/output-layer-class-1-base/{i}_{j}.json')
        file1 = json.loads(last_try.read())
        file2 = json.loads(base.read())
        try:
            assert file1["procs"] == file2["procs"]
        except AssertionError:
            flag = False
            print(i, j)
        last_try.close()
        base.close()
        #os.system(f"diff scripts/output-layer-class-1/{i}_{j}.json scripts/output-layer-class-1-base/{i}_{j}.json")
if flag:
    print('Equal results')