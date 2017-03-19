#!/usr/bin/env python

import copy
import datetime
import argparse
import getpass
import json
import logging
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import os
import pandas as pd
import re
import time
import sys
import uuid

#matplotlib.style.use('ggplot')

def main():
    # Debug logging
    logging.basicConfig(level=logging.DEBUG)

    # Argument Parsing
    parser = argparse.ArgumentParser()
    parser.add_argument('--file', help='file to run')
    args = parser.parse_args()

    df = pd.read_csv(args.file + '.csv',
                     names=['0','1','2','3','4','5'],
                     header=None,
                     nrows=1000000)

    df[(df <= 2).all(axis=1)]
    df_p = df[(df != 0).all(axis=1)]
    df_n = df[(df != 0).all(axis=1)]
    df_p = np.log2(df_p)
    df_n = np.log2(-df_n)

    bits = 24

    figname ='positive values - %s' % (args.file)
    ax = df_p.plot.hist(alpha=0.5,
                        bins=range(bits+1),
                        figsize=(10, 8),
                        xlim=(0,bits),
                        title=figname)
    fig = ax.get_figure()
    fig.savefig(args.file + '_p.png')


    figname ='negative values - %s' % (args.file)
    ax = df_n.plot.hist(alpha=0.5,
                        bins=range(bits+1),
                        figsize=(10, 8),
                        xlim=(0,bits),
                        title=figname)
    fig = ax.get_figure()
    fig.savefig(args.file + '_n.png')

if __name__ == "__main__":
    main()


