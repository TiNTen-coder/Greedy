import json
import matplotlib.pyplot as plt
import seaborn as sns

import os

#os.system("rm -rf scripts/results_edf_misf_research")
#os.system("mkdir scripts/results_edf_misf_research")

procs = [5, 10, 20, 30, 40, 50]
tasks = [500, 1000, 2000, 5000, 10000]


for based_on in [5, 10]:
    folder = f"based_on_{based_on}_*"
    for i in procs:
        misf, edfb_misf, edff_misf, edfbase, edffollow, est, eft = [], [], [], [], [], [], []
        for j in tasks:
            with open(f"scripts/{folder}/{i}/{j}/result_greedy.txt", "r") as f:
                misf.append(int(f.readline()))
            with open(f"scripts/{folder}/{i}/{j}/result_edfb_misf.txt", "r") as f:
                edfb_misf.append(int(f.readline()) / misf[-1])
            with open(f"scripts/{folder}/{i}/{j}/result_edff_misf.txt", "r") as f:
                edff_misf.append(int(f.readline()) / misf[-1])
            with open(f"scripts/{folder}/{i}/{j}/result_edfbase.txt", "r") as f:
                edfbase.append(int(f.readline()) / misf[-1])
            with open(f"scripts/{folder}/{i}/{j}/result_edffollow.txt", "r") as f:
                edffollow.append(int(f.readline()) / misf[-1])
            with open(f"scripts/{folder}/{i}/{j}/result_est.txt", "r") as f:
                est.append(int(f.readline()) / misf[-1])
            with open(f"scripts/{folder}/{i}/{j}/result_eft.txt", "r") as f:
                eft.append(int(f.readline()) / misf[-1])
            misf[-1] /= misf[-1]
        
        plt.figure(figsize=(10, 5))
        plt.title(f"{i} processors (based on {based_on}_*)")
        plt.plot(tasks, misf, label='MISF', color='purple', marker='.')
        plt.plot(tasks, edfb_misf, label='EDFBase+MISF', color='orange', marker='.')
        plt.plot(tasks, edfbase, label='EDFBase', color='blue', marker='.')
        plt.plot(tasks, edffollow, label='EDFFollow', color='green', marker='.')
        plt.plot(tasks, edff_misf, label='EDFFollow+MISF', color='red', marker='.')
        plt.plot(tasks, est, label='EST', color='brown', marker='.')
        plt.plot(tasks, eft, label='EFT', color='yellow', marker='.')

        plt.xticks(tasks)
        plt.xlabel('Tasks')
        plt.ylabel('Time')
        plt.legend()
        plt.grid(True)

        #plt.show()
        os.system(f"mkdir scripts/{folder}/graphics")
        plt.savefig(f"scripts/{folder}/graphics/{i}.png")