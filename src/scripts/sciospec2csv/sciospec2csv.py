import argparse
import pandas as pd
from os.path import isfile, join, basename
from os import listdir
from datetime import datetime


def get_timestamp(timestamp_str):
    # Append '000' to the milliseconds part, so it can be parsed by strptime.
    splitted = timestamp_str.strip().split(' ')
    splitted[1] += "000"
    splitted[2] = splitted[2].lower()
    timestamp_str_new = ' '.join(splitted)

    return datetime.strptime(timestamp_str_new, "%d-%b-%Y %I:%M:%S:%f %p")


def read_file(file_path, remove_negative):
    with open(file_path) as f:
        lines = f.readlines()
        timestamp_str = lines[4]
        timestamp = get_timestamp(timestamp_str)

        impedances = {}
        for line in lines[6:]:
            line = line.strip()
            splitted_line = line.split(',')
            frequency = float(splitted_line[0])
            real = float(splitted_line[1])
            if remove_negative and real < 0.0:
                real = float("nan")
            imag = float(splitted_line[2])
            if remove_negative and imag < 0.0:
                imag = float("nan")
            impedances[frequency] = complex(real, imag)

    return timestamp, impedances


def convert_files(input_folder, replace_negative):
    # Get all the files of the input folder.
    onlyfiles = [f for f in listdir(input_folder) if isfile(join(input_folder, f)) and f.endswith('.spec')]

    if (len(onlyfiles) == 0):
        print("No files found.")
        return

    print("Found " + str(len(onlyfiles)) + " files.")

    # Check the first file and construct the dataframe according to it.
    timestamp, impedances = read_file(join(input_folder, onlyfiles[0]), replace_negative)

    frequency_columns = [str(frequency) + postfix for frequency in impedances.keys() for postfix in ("_real", "_imag")]

    columns = ["timestamp"]
    columns.extend(frequency_columns)
    df = pd.DataFrame(columns=columns)

    for file in onlyfiles:
        timestamp, impedances = read_file(join(input_folder, file), replace_negative)

        data = [value for _, impedance in impedances.items() for value in (impedance.real, impedance.imag )]

        row = [timestamp.timestamp()]
        row.extend(data)

        df.loc[len(df.index)] = row

    df_sorted = df.sort_values(by='timestamp')
    return df_sorted



if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Converts multiple *.spec files into a single CSV that holds the impedance spectra.")
parser.add_argument('input_folder', help="Path to the folder that holds the *.spec files.")
parser.add_argument('-o', metavar="output_folder", default='.', help="Path to the output folder")
parser.add_argument('-rn', "--replace_negative", help="Whether to replace negative values with NaNs",
                    action='store_true')

args = parser.parse_args()

df = convert_files(args.input_folder, args.replace_negative)
file_name = basename(args.input_folder) + ".csv"
file_path = join(args.o, file_name)
print("Writing " + str(len(df)) + " data rows to " + file_path)

df.to_csv(file_path, index=False)

print("Done")
