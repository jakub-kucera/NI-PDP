import json
import os
from datetime import datetime

PROGRAM_FILE_PATH = "../main"
GRAPHS = [
    {
        "path": "graf_10_3.txt",
        "runtime": 0.0,
        "rec_calls": "1K",
        "rec_calls_num": 1000,
        "max_weight": 1300,
    },
    {
        "path": "graf_10_5.txt",
        "runtime": 0.0,
        "rec_calls": "12K",
        "rec_calls_num": 1200,
        "max_weight": 1885,
    },
    {
        "path": "graf_10_6.txt",
        "runtime": 0.02,
        "rec_calls": "374K",
        "rec_calls_num": 374000,
        "max_weight": 2000,
    },
    {
        "path": "graf_12_3.txt",
        "runtime": 0.0,
        "rec_calls": "1K",
        "rec_calls_num": 1000,
        "max_weight": 1422,
    },
    {
        "path": "graf_12_5.txt",
        "runtime": 0.0,
        "rec_calls": "171K",
        "rec_calls_num": 171000,
        "max_weight": 2219,
    },
    {
        "path": "graf_12_6.txt",
        "runtime": 0.0,
        "rec_calls": "401K",
        "rec_calls_num": 401000,
        "max_weight": 2533,
    },
    {
        "path": "graf_12_9.txt",
        "runtime": 0.47,
        "rec_calls": "49M",
        "rec_calls_num": 49000000,
        "max_weight": 3437,
    },
    {
        "path": "graf_13_9.txt",
        "runtime": 0.8,
        "rec_calls": "116M",
        "rec_calls_num": 116000000,
        "max_weight": 3700,
    },
    {
        "path": "graf_15_4.txt",
        "runtime": 0.0,
        "rec_calls": "502K",
        "rec_calls_num": 502000,
        "max_weight": 2547,
    },
    {
        "path": "graf_15_5.txt",
        "runtime": 0.03,
        "rec_calls": "1M",
        "rec_calls_num": 1000000,
        "max_weight": 2892,
    },
    {
        "path": "graf_15_6.txt",
        "runtime": 0.05,
        "rec_calls": "4M",
        "rec_calls_num": 4000000,
        "max_weight": 3353,
    },
    {
        "path": "graf_15_8.txt",
        "runtime": 1.8,
        "rec_calls": "160M",
        "rec_calls_num": 160000000,
        "max_weight": 3984,
    },
    {
        "path": "graf_15_12.txt",
        "runtime": 103,
        "rec_calls": "21G",
        "rec_calls_num": 21000000000,
        "max_weight": 5380,
    },
    {
        "path": "graf_15_14.txt",
        "runtime": 5700,
        "rec_calls": "1.3T",
        "rec_calls_num": 1300000000000000,
        "max_weight": 5578,
    },
    {
        "path": "graf_17_10.txt",
        "runtime": 78,
        "rec_calls": "18G",
        "rec_calls_num": 18000000000000000,
        "max_weight": 5415,
    },
]


def main():
    for testcase in GRAPHS:
        print(50*"=")
        print(f"RUNNING {testcase['path']},")
        # print(testcase)
        graph_path = f"../graphs/{testcase['path']}"

        start = datetime.now()
        stream_project = os.popen(f"{PROGRAM_FILE_PATH} {graph_path} 0")# {sdaf}")
        output = stream_project.read()
        # print(output)
        output_dict = json.loads(output)
        end = datetime.now()
        weight_match_str = "MATCH" if output_dict['MAX_WEIGHT'] == testcase['max_weight'] else "NOT MATCH"
        rec_calls_less_str = "LESS" if output_dict['REC_CALLS'] < testcase['rec_calls_num'] else "MORE (or =)"
        print(f"max weight {weight_match_str}, ref: {testcase['max_weight']}, result: {output_dict['MAX_WEIGHT']}")
        print(f"rec calls {rec_calls_less_str}, ref: {testcase['rec_calls_num']}, result: {output_dict['REC_CALLS']}")
        # print(f"REC CALL, REF: {testcase['max_weight']}, RESULT: {output_dict['REC_CALLS']}")
        print(f"Runtime: {end - start}")


if __name__ == '__main__':
    main()
