import json
import os
from datetime import datetime

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


SEQ_PROGRAM_FILE_PATH = "../main_seq"
OPEN_MP_PROGRAM_FILE_PATH = "../main_openmp"

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
    print(300 * "#")
    for testcase in GRAPHS:
        print(50*"=")
        print(f"RUNNING {testcase['path']},")
        # print(testcase)
        graph_path = f"../graphs/{testcase['path']}"

        # start = datetime.now()
        # stream_project = os.popen(f"{SEQ_PROGRAM_FILE_PATH} {graph_path} 0")  # {sdaf}")
        # output = stream_project.read()
        # # print(output)
        # output_dict = json.loads(output)
        # end = datetime.now()
        # # weight_match_str = "MATCH" if output_dict['MAX_WEIGHT'] == testcase['max_weight'] else "NOT MATCH"
        # # rec_calls_less_str = "LESS" if output_dict['REC_CALLS'] < testcase['rec_calls_num'] else "MORE (or =)"
        # # print(f"max weight {weight_match_str}, ref: {testcase['max_weight']}, result: {output_dict['MAX_WEIGHT']}")
        # # print(f"rec calls {rec_calls_less_str}, ref: {testcase['rec_calls_num']}, result: {output_dict['REC_CALLS']}")
        # weight_match_str = f"{bcolors.OKGREEN}MATCH{bcolors.ENDC}     " if output_dict['MAX_WEIGHT'] == testcase['max_weight'] else f"{bcolors.FAIL}NOT MATCH{bcolors.ENDC} "
        # print(f"max weight {weight_match_str}, REF: {testcase['max_weight']}, SEQ: {output_dict['MAX_WEIGHT']}")
        # rec_calls_less_str = f"{bcolors.OKGREEN}LESS{bcolors.ENDC}       " if output_dict['REC_CALLS'] < testcase['rec_calls_num'] else f"{bcolors.FAIL}MORE (or =){bcolors.ENDC}"
        # print(f"rec calls {rec_calls_less_str}, REF: {testcase['rec_calls_num']}, SEQ: {output_dict['REC_CALLS']}")
        # print(f"Runtime {end-start}")



        # sequential
        seq_start = datetime.now()
        seq_stream_project = os.popen(f"{SEQ_PROGRAM_FILE_PATH} {graph_path} 1")
        seq_output = seq_stream_project.read()
        seq_output_dict = json.loads(seq_output)
        seq_end = datetime.now()
        seq_runtime = (seq_end - seq_start)

        # openMP
        open_mp_start = datetime.now()
        open_mp_stream_project = os.popen(f"{OPEN_MP_PROGRAM_FILE_PATH} {graph_path} 4")
        open_mp_output = open_mp_stream_project.read()
        open_mp_output_dict = json.loads(open_mp_output)
        open_mp_end = datetime.now()
        open_mp_runtime = (open_mp_end - open_mp_start) # .total_seconds()

        weight_match_str = f"{bcolors.OKGREEN}MATCH{bcolors.ENDC}     " if open_mp_output_dict['MAX_WEIGHT'] == seq_output_dict['MAX_WEIGHT'] == testcase['max_weight'] else f"{bcolors.FAIL}NOT MATCH{bcolors.ENDC} "
        print(f"max weight {weight_match_str}, REF: {testcase['max_weight']}, SEQ: {seq_output_dict['MAX_WEIGHT']}, OMP: {open_mp_output_dict['MAX_WEIGHT']}")

        rec_calls_less_str = f"{bcolors.OKGREEN}LESS{bcolors.ENDC}       " if seq_output_dict['REC_CALLS'] < testcase['rec_calls_num'] else f"{bcolors.FAIL}MORE (or =){bcolors.ENDC}"
        print(f"rec calls {rec_calls_less_str}, REF: {testcase['rec_calls_num']}, SEQ: {seq_output_dict['REC_CALLS']}, OMP: {open_mp_output_dict['REC_CALLS']}")
        # print(f"REC CALL, REF: {testcase['max_weight']}, RESULT: {output_dict['REC_CALLS']}")
        runtime_str = f"{bcolors.FAIL}SEQ LESS{bcolors.ENDC}     " if seq_runtime < open_mp_runtime else f"{bcolors.OKGREEN}SEQ MORE{bcolors.ENDC}     "
        print(f"Runtime {runtime_str}, SEQ: {seq_runtime}, OMP: {open_mp_runtime}")


if __name__ == '__main__':
    main()
