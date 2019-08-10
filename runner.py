import subprocess
import re

if __name__ == "__main__":
    trials = 10
    cmd = "rundb"
    total_time = 0
    print("Times:")
    for _ in range(trials):
        text = subprocess.run(['./rundb'], stdout=subprocess.PIPE).stdout.decode('utf-8')
        time = float(re.search("Throughput:\s*(\d*.\d)", text)[1])
        stats = re.findall("\sved_.*", text)
        for stat in stats:
            print(stat)
        total_time += time
        print(time)
    print("Average:")
    print(total_time / trials)
