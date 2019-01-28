#!/usr/bin/env python
#-*- encoding: utf-8 -*-
from __future__ import print_function
import argparse
import difflib
import subprocess
import os
import logging

POSTFIXES = ('txt', 'reference', 'opts')

def run_single_test(executable, options, input_file, expected_output_file):
    cmd = "{exec_path} {opts} --input={inp}".format(
            exec_path=executable,
            opts=options,
            inp=input_file
    )
    process_output = subprocess.check_output(cmd, shell=True)
    with open(expected_output_file, 'r') as ok_res:
        actual_output = ok_res.read()

    if process_output != actual_output:
        return ("FAIL", "GOT:\n{}EXPECTED:\n{}".format(process_output, actual_output))
    return ("OK", "")

def group_test_suites(suites_path):
    tmp_set = set([])
    for fname in os.listdir(suites_path):
        tmp_set.add(os.path.join(suites_path, os.path.splitext(fname)[0]))

    result = []
    for name in tmp_set:
        result.append({postfix: name + '.' + postfix for postfix in POSTFIXES})
    return sorted(result)


def prepare_test_suites(test_suites):
    for test_suite in test_suites:
        opts_file = test_suite['opts']
        with open(opts_file) as opts_f:
            test_suite['opts'] = opts_f.read().strip()
    return test_suites

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)

    parser = argparse.ArgumentParser(
        description="Run executables with required options and compares results")
    parser.add_argument('-b', '--binary',
                        help='Path to executable to test', default='../../programs/sentizer')
    parser.add_argument('-p', '--path', help='Path to tests folder', default='./sentizer')

    args = parser.parse_args()
    suites = group_test_suites(args.path)
    prepared_suites = prepare_test_suites(suites)

    bad_exit = False
    for prepared_suite in prepared_suites:
        test_name = os.path.basename(prepared_suite.values()[0])
        test_name = os.path.splitext(test_name)[0]
        logging.info("Running suite '%s'", test_name)
        status, desc = run_single_test(
            args.binary,
            prepared_suite['opts'],
            prepared_suite['txt'],
            prepared_suite['reference']
        )
        print(test_name, ':\t', status)
        if status != 'OK':
            bad_exit = True
            print(desc)
    if bad_exit:
        exit(1)

