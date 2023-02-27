# This is a sample Python script.
import os
from datetime import datetime

PROGRAM_FILE_PATH = "../main"
GRAPHS = [
    {
        "path": "graf_10_3.txt",
        "runtime": 0.0,
        "rec_calls": "1K",
        "max_weight": 1300,
    },
    {
        "path": "graf_10_5.txt",
        "runtime": 0.0,
        "rec_calls": "12K",
        "max_weight": 1885,
    },
    {
        "path": "graf_10_6.txt",
        "runtime": 0.02,
        "rec_calls": "374K",
        "max_weight": 2000,
    },
    {
        "path": "graf_12_3.txt",
        "runtime": 0.0,
        "rec_calls": "1K",
        "max_weight": 1422,
    },
    {
        "path": "graf_12_5.txt",
        "runtime": 0.0,
        "rec_calls": "171K",
        "max_weight": 2219,
    },
    {
        "path": "graf_12_6.txt",
        "runtime": 0.0,
        "rec_calls": "401K",
        "max_weight": 2533,
    },
    {
        "path": "graf_12_9.txt",
        "runtime": 0.47,
        "rec_calls": "49M",
        "max_weight": 3437,
    },
    {
        "path": "graf_13_9.txt",
        "runtime": 0.8,
        "rec_calls": "116M",
        "max_weight": 3700,
    },
    {
        "path": "graf_15_4.txt",
        "runtime": 0.0,
        "rec_calls": "502K",
        "max_weight": 2547,
    },
    {
        "path": "graf_15_5.txt",
        "runtime": 0.03,
        "rec_calls": "1M",
        "max_weight": 2892,
    },
    {
        "path": "graf_15_6.txt",
        "runtime": 0.05,
        "rec_calls": "4M",
        "max_weight": 3353,
    },
    {
        "path": "graf_15_8.txt",
        "runtime": 1.8,
        "rec_calls": "160M",
        "max_weight": 3984,
    },
    {
        "path": "graf_15_12.txt",
        "runtime": 103,
        "rec_calls": "21G",
        "max_weight": 5380,
    },
    {
        "path": "graf_15_14.txt",
        "runtime": 5700,
        "rec_calls": "1.3T",
        "max_weight": 5578,
    },
    {
        "path": "graf_17_10.txt",
        "runtime": 78,
        "rec_calls": "18G",
        "max_weight": 5415,
    },
]


def main():
    for testcase in GRAPHS:
        print(50*"=")
        print(f"RUNNING {testcase['path']},")
        print(testcase)
        graph_path = f"../graphs/{testcase['path']}"

        start = datetime.now()
        stream_project = os.popen(f"{PROGRAM_FILE_PATH} {graph_path} 0")# {sdaf}")
        output = stream_project.read()
        print(output)
        end = datetime.now()
        print(f"Runtime: {end - start}")



# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()

# See PyCharm help at https://www.jetbrains.com/help/pycharm/
