import os

from serial_job_TEMPLATE import SERIAL_JOB_TEMPLATE

FILES = "graph-hard/graf-24-23.txt", "graph-hard/graf-25-16.txt", "graph-hard/graf-23-20.txt"

# MPI_THREADS = ("6" "8" "12" "16" "20")
# MPI_PROCESSES =

OPENMP_THREADS = (1, 2, 4, 6, 8, 12, 16, 20)

OMP_TYPES = ["main_task", "main_data"]

PDP_DIR = "~"
# PDP_DIR = "."

def main():
    for omp_task in OMP_TYPES:
        print(f"Compiling {omp_task}")
        print(os.popen(f"g++ -std=c++17 -fopenmp   -Wall -pedantic -Wno-long-long -O2 -o {PDP_DIR}/{omp_task} {PDP_DIR}/{omp_task}.cpp").read())
        print(f"Finished compiling {omp_task}")
        for graph in FILES:
            for thread_count in OPENMP_THREADS:
                print(f"Creating file for {PDP_DIR}/{graph} with {thread_count} threads")
                serial_task_config_file_str = SERIAL_JOB_TEMPLATE.format(program_path=f"{PDP_DIR}/{omp_task}", graph_path=f"{PDP_DIR}/{graph}", thread_count=thread_count)
                graph_safe_name = graph.replace("/", "_").replace(".", "_")
                serial_task_config_file = f"{PDP_DIR}/sh/{omp_task}_{graph_safe_name}_{thread_count}.sh"

                with(open(serial_task_config_file, "w")) as file:
                    file.write(serial_task_config_file_str)
                # task_register = os.popen(f'qrun 20c 1 pdp_serial {serial_task_config_file}')
                # print(f"registering {task_register.read()}")

                # print(serial_task_config_file)
    #
    #
    # print("Compiling OMP data")
    # print(os.popen(f"g++ -std=c++17 -fopenmp   -Wall -pedantic -Wno-long-long -O2 -o main_data main_data.cpp").read())
    #



if __name__ == '__main__':
    main()