import subprocess
import argparse
import re

# This script can blur/produce inaccurate results on a weak computer because
# it also uses compute power, which trades off w/ the DB instances.
# This may be especially true for two nodes at once.

def extract_throughput(text):
    return float(re.search("Throughput:\s*(\d*.\d)", text)[1])

if __name__ == "__main__":
     parser = argparse.ArgumentParser(description="Run DB tests quickly")
     parser.add_argument("-t", "--two", action="store_true", help="Test with two instances of rundb")
     args = parser.parse_args()

     # config variables
     cmd = "./rundb"
     trials = 5
     config_file = "ifconfig.txt"
     hostname = "DESKTOP-MN0EL94"

     def write_hostname(config_file):
         config_file.write(hostname + '\n')

     with open(config_file, "w+") as f:
         f.write("=s\n")
         f.write("# a list of servers; each on a new line\n")
     if args.two:
         with open(config_file, "a") as f:
             write_hostname(f)
             write_hostname(f)
         from multiprocessing import Pool
         def start_node(num):
             # -Dn option manually sets node number. It is used for running multiple nodes on the same machine.
             # It only works if compiled with Ved's patch.
             text = subprocess.run([cmd, "-Dn" + str(num)], stdout=subprocess.PIPE).stdout.decode('utf-8')
             return text
         pool = Pool()
         sum_of_avgs = 0
         for _ in range(trials):
            outputs = pool.map(start_node, range(2))
            node0_throughput = extract_throughput(outputs[0])
            node1_throughput = extract_throughput(outputs[1])
            avg = (node0_throughput + node1_throughput) / 2
            sum_of_avgs += avg
            print("Node 0: " + str(node0_throughput))
            print("Node 1: " + str(node1_throughput))
            print("Average: " + str(avg))
            print("---")
         print("Total average of both nodes: " + str(sum_of_avgs / trials))
     else:
         with open(config_file, "a") as f:
             write_hostname(f)
         total_throughput = 0
         for _ in range(trials):
             text = subprocess.run([cmd], stdout=subprocess.PIPE).stdout.decode('utf-8')
             throughput = extract_throughput(text)
             stats = re.findall("\sved_.*", text)
             print("Throughput: " + str(throughput))
             for stat in stats:
                 print(str.strip(stat))
             print("---")
             total_throughput += throughput
         print("Average Throughput:")
         print(total_throughput / trials)
