from ast import Assert
import os
import re
import csv
import sys

if __name__ == "__main__":
    folder = "./results/"
    files = os.listdir(folder)

    Datasets = ["CAIDA"]
    Algs = ["SpaceSaving", "Elastic", "CountHeap", "CMHeap", "UnivMon"]

    dataset = Datasets[0]
    algorithm = sys.argv[1]
    if algorithm not in Algs:
        print("Parameter Error!")
    else:
        for key_type in range(1, 7):
            HH_pattern = "HH-" + dataset + "-" + algorithm + "-.*-" + str(key_type) + ".csv"
            HH_files = []

            for file in files:
                if re.match(HH_pattern, file):
                    HH_files.append(file)
            
            if len(HH_files) != 1:
                print("Multiple HH Files")
                print(HH_files)
            
            HH_file = HH_files[0]
            cin = open(folder + HH_file, "r")
            csv_reader = csv.reader(cin)
            
            metrics = {}
            temp_key = -1
            
            for row in csv_reader:
                if len(row) < 2:
                    continue
                if row[0] == 'key-type':
                    temp_key = int(row[1])
                    if metrics.get(temp_key) != None:
                        print("Key Error")
                    metrics[temp_key] = [0 for i in range(5)]
                    
                if temp_key < 0:
                    print("temp_key error")
                if row[0] == 'realHH':
                    metrics[temp_key][0] = int(row[1])
                if row[0] == 'estHH':
                    metrics[temp_key][1] = int(row[1])
                if row[0] == 'bothHH':
                    metrics[temp_key][2] = int(row[1])
                if row[0] == 'aae':
                    metrics[temp_key][3] = float(row[1])
                if row[0] == 'are':
                    metrics[temp_key][4] = float(row[1])
            
            realHH, estHH, bothHH, aae, are = 0, 0, 0, 0, 0
            for key in metrics.keys():
                realHH += metrics[key][0]
                estHH += metrics[key][1]
                bothHH += metrics[key][2]
                aae += metrics[key][3]
                are += metrics[key][4]
            
            print("Result for Key = " + str(key_type))
            print("CR: " + str(bothHH / realHH))
            print("PR: " + str(bothHH / estHH))
            print("AAE: " + str(aae / bothHH))
            print("ARE: " + str(are / bothHH))