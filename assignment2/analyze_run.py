#!/usr/bin/env python

import datetime
import argparse
import getpass
import json
import logging
import os
import re
import time
import sys
import uuid


TIMESCALE = 1e-12;  # 1ps

def main():
    # Debug logging
    logging.basicConfig(level=logging.DEBUG)

    # Argument Parsing
    parser = argparse.ArgumentParser()
    parser.add_argument('--run', help='name for the run')
    parser.add_argument('--comment', help='comment for the run')
    args = parser.parse_args()

    if args.run == None:
        raise ValueError('Give the run a name (like --run=baseline)')

    data = dict()

    logging.debug('user = %s' % getpass.getuser())
    logging.debug('run_name = %s' % args.run)
    logging.debug('comment = %s' % args.comment)

    # Open sim log file and parse it
    f = open('simv.log', 'r')
    data['mtime_simv'] = os.stat('simv.log').st_mtime
    for l in f:
        # Get the time that the run happened
        if re.search('Runtime version F-2011.12_Full64', l):
            data['rundate'] = l.split(';')[2].strip()
            data['runtime'] = time.mktime(datetime.datetime.strptime(data['rundate'], "%b %d %H:%M %Y").timetuple())
            logging.debug('rundate = %s' % data['rundate'])
            logging.debug('runtime = %f' % data['runtime'])
            logging.debug('now = %f' % time.time())

        # Get the test vector for the run
        if re.search('Simulation will use test file', l):
            data['test_vector'] = l.split('test file')[1].strip()
            logging.debug('test_vector = %s' % data['test_vector'])

        # Get the total time for the run
        if re.search('FINISH', l):
            end_t = int(l.split('=')[1].split('*')[0])
            data['end_t_ns'] = end_t * TIMESCALE * 1e9
            logging.debug('end time = %dns' % data['end_t_ns'])

        # Get the cycles and upoly data
        if re.search('Cycles:', l):
            line_list = l.split(':')
            data['cycles'] = int(line_list[1].split('uPoly')[0])
            data['upoly'] = int(line_list[2].split('sampleTests')[0])
            data['sample_tests'] = int(line_list[3].split('sampleHits')[0])
            data['sample_hits'] = int(line_list[4])
            logging.debug('cycles = %d' % data['cycles'])
            logging.debug('upoly = %d' % data['upoly'])
            logging.debug('samples = %d' % data['sample_tests'])
            logging.debug('hits = %d' % data['sample_hits'])
    assert 'cycles' in data
    assert 'upoly' in data
    assert 'sample_tests' in data
    assert 'sample_hits' in data

    data['performance'] = float(data['end_t_ns']) / data['upoly']
    data['hit_rate'] = float(data['sample_hits']) / data['sample_tests']
    data['comment'] = args.comment
    data['run'] = args.run
    data['user'] = getpass.getuser()
    logging.debug('hit_rate = %f%%' % (data['hit_rate'] * 100.0))

    # Open area report and parse it
    f = open('synth/reports/area_report', 'r')
    data['mtime_area'] = os.stat('synth/reports/area_report').st_mtime
    for l in f:
        # Get the time that the run happened
        if re.search('Total cell area', l):
            data['total_area_um'] = float(l.split(':')[1])
            data['total_area_mm'] = data['total_area_um'] / 1.0e6
            logging.debug('hits = %d' % data['total_area_um'])
    assert 'total_area_um' in data

    # Open power report and parse it
    f = open('synth/reports/power_report', 'r')
    data['mtime_power'] = os.stat('synth/reports/power_report').st_mtime
    for l in f:
        # Get the time that the run happened
        if re.search('Total Dynamic Power', l):
            data['dynamic_power_mw'] = float(l.split('=')[1].split('mW')[0])
            logging.debug('dynamic_power = %f mW' % data['dynamic_power_mw'])
        if re.search('Cell Leakage Power', l):
            data['leakage_power_mw'] = float(l.split('=')[1].split('mW')[0])
            logging.debug('leakage_power = %f mW' % data['leakage_power_mw'])

    data['total_power_mw'] = data['leakage_power_mw'] + data['dynamic_power_mw']

    # Open timing report and parse it
    f = open('synth/reports/timing_report_maxsm', 'r')
    data['mtime_power'] = os.stat('synth/reports/power_report').st_mtime
    for l in f:
        if re.search("slack", l):
            data['slack'] = float(l.split(")")[1])
            if re.search("MET", l):
                data['timing_met'] = True
            else:
                data['timing_met'] = False
    assert 'timing_met' in data
    assert 'slack' in data

    # Open rast.vp and get the pipeline settings
    f = open('rtl/rast.vp', 'r')
    for l in f:
        if re.search("Name=>'PipesBox'", l):
            l = f.next()
            data['pipes_box'] = int(l.split('=>')[1].split(',')[0])
        if re.search("Name=>'PipesIter'", l):
            l = f.next()
            data['pipes_iter'] = int(l.split('=>')[1].split(',')[0])
        if re.search("Name=>'PipesHash'", l):
            l = f.next()
            data['pipes_hash'] = int(l.split('=>')[1].split(',')[0])
        if re.search("Name=>'PipesSamp'", l):
            l = f.next()
            data['pipes_samp'] = int(l.split('=>')[1].split(',')[0])
    assert 'pipes_box' in data
    assert 'pipes_iter' in data
    assert 'pipes_hash' in data
    assert 'pipes_samp' in data

    # Open Makefile and get the clock settings
    f = open('Makefile', 'r')
    for l in f:
        # Get the time that the run happened
        if re.search('CLK_PERIOD=', l):
            data['clk_period'] = float(l.split('=')[1].split(';')[0])
    assert 'clk_period' in data
    # Make sure the reports were run within 15 min of one another - as a double
    # check against stale data
    report_age = abs(data['mtime_simv'] - data['mtime_area'])
    if report_age > 30*60:
        raise AssertionError(
                'Simulation or Area Report is stale (>30min) - suggest make cleanall && make run_dc && make run (%d > %d)' % (report_age, 30*60))

    # Make a JSON object and save it to a file
    j = json.dumps(data, sort_keys=True,indent=4, separators=(',', ': '))

    # write JSON to a file and append a uuid pad so that we don't overrite runs
    fname = 'run_data/' + data['run'] + '_' + str(uuid.uuid4())[0:8] + '.json'
    f = open(fname, 'w')

    f.write(j)
    print j

if __name__ == "__main__":
    main()


