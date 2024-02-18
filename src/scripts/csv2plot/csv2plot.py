import argparse
from os.path import basename, join
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy.interpolate import griddata
from datetime import datetime


def calc_magnitude(row):
    timestamp = row[0]
    data = row[1:]
    columns = data.index.to_list()

    # Build the Series object that will be returned
    ret_val_data_columns = list()
    for idx in range(0, len(data), 2):
        ret_val_data_columns.append(float(columns[idx].split('_')[0]))
    ret_val_columns = ['timestamp'] + ret_val_data_columns
    ret_val = pd.Series(index=ret_val_columns)

    # Iterate over data again, in order to calculate the magnitude.
    for idx in range(0, len(data), 2):
        magnitude = np.sqrt(data[idx] * data[idx] + data[idx + 1] + data[idx + 1])
        ret_val[ret_val_data_columns[int(idx / 2)]] = magnitude
        ret_val['timestamp'] = timestamp

    return ret_val


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Takes a CSV file and plots it to a spectrogram.")
    parser.add_argument('input_file', help="Path to the CSV file that shall be plotted.")
    parser.add_argument('-o', metavar="output_folder", default='.', help="Path to the output folder")
    parser.add_argument('-s', "--show", help="If given, shows the sepctrogram after it has been created.",
                        action='store_true')

    args = parser.parse_args()

    print("Reading from " + args.input_file + " ...")
    df = pd.read_csv(args.input_file)
    print("Read " + str(len(df)) + " lines.")

    print("Converting " + str(len(df)) + " lines ...")
    df_magnitude = df.apply(calc_magnitude, axis='columns')
    df_melted = df_magnitude.melt(id_vars=['timestamp'], value_vars=df_magnitude.columns)
    extent = (
        df_melted['variable'].min(),
        df_melted['variable'].max(), df_melted['timestamp'].max(), df_melted['timestamp'].min())
    frequencies_resampled, timestamps_resampled = np.mgrid[extent[0]:extent[1]:100j, extent[2]:extent[3]:100j]
    resampled = griddata((df_melted['timestamp'], df_melted['variable']), df_melted['value'], (timestamps_resampled,
                                                                                               frequencies_resampled),
                         method='linear')

    fig, ax = plt.subplots()
    begin = datetime.fromtimestamp(extent[2])
    end = datetime.fromtimestamp(extent[3])
    ax.set_title(basename(args.input_file) + "\n" + begin.strftime("%c"))
    im = ax.imshow(resampled.T, origin='lower', extent=(extent[0], extent[1], begin, end), aspect='auto')
    ax.set_xlabel("Frequency (Hz)")
    ax.set_ylabel("Time")

    fig.colorbar(im, label="|Z| (Ohm)")
    if args.show:
        plt.show()

    file_name = basename(args.input_file).split(".")[0] + ".png"
    file_path = join(args.o, file_name)
    fig.savefig(file_path)

    print("Done")
