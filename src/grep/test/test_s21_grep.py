#!/usr/bin/env python3

from itertools import combinations_with_replacement
from random import shuffle
from os import system

s21_grep_binary = "./s21_grep"
grep_binary = "grep"

test_files = [
    "test/file1.txt",
    "test/file2.txt",
    "test/file3.txt",
    "test/file4.txt",
    "test/file5.txt"
]

flag_set = {
    "-e",
    "-i",
    "-v",
    "-c",
    "-l",
    "-n"
}

all_flag_combinations = list(combinations_with_replacement(flag_set, len(flag_set)))

temp_file_prefix = "TEST-FILE"
temp_file_suffix = ".output"

def compare_files(file_a, file_b):
    with open(file_a) as file_a_content, open(file_b) as file_b_content:
        content_a = file_a_content.read()
        content_b = file_b_content.read()

        if content_a != content_b:
            print("\033[41m\033[1m NO \033[0m")
            print("=============== s21_grep ===============")
            print(content_a[:200] + "..." if len(content_a) > 200 else content_a)
            print("=============== grep ===============")
            print(content_b[:200] + "..." if len(content_b) > 200 else content_b)
            print("=================================")
        else:
            print("\033[42m\033[1m YES \033[0m")

for test_idx, current_flag_combination in enumerate(all_flag_combinations):
    for flag_combination in (current_flag_combination, set(current_flag_combination)):
        shuffle(test_files)
        print(f"Running TEST [{test_idx + 1} / {len(all_flag_combinations)}] - Flags: {flag_combination}")
        
        for version, binary in (('0', s21_grep_binary), ('1', grep_binary)):
            command = f'{binary} {" ".join(flag_combination)} {" ".join(test_files)} > {temp_file_prefix}-{version}{temp_file_suffix}'
            system(f"echo '{command}' >> commands.testing")
            system(command)

        compare_files(f"{temp_file_prefix}-0{temp_file_suffix}", f"{temp_file_prefix}-1{temp_file_suffix}")

system(f'rm -rf *{temp_file_suffix}')
