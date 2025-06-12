#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# BinFile2HeaderFile.py
# 2023-12-29 10:58:49

import sys
import os

class BinFile2HeaderFile:
    def __init__(self, input_file, output_file, array_name):
        self.input_file = input_file
        self.output_file = output_file
        self.array_name = array_name

    def read_file(self):
        try:
            with open(self.input_file, 'rb') as file:
                data = bytearray(file.read())
            return data, True
        except Exception as e:
            print(f"Error reading the file: {e}")
            return None, False

    def write_c_array_header(self, data):
        flag_newline = True

        image_name = self.array_name
        image_len = len(data)

        # const int ramrun_image_len = image_len;
        array_len_str = "const int {_an}_len = {_al};\n".format(_an=image_name, _al=image_len)
        # const unsigned char ramrun_image[] = {
        array_name_str = f"const unsigned char {image_name}[{image_len}] = " + "{"

        if os.path.exists(self.output_file):
            os.remove(self.output_file)

        try:
            with open(self.output_file, 'w') as c_file:
                c_file.write(array_len_str)
                c_file.write("\n")
                c_file.write(array_name_str)
                for i, byte in enumerate(data):
                    if i % 16 == 0:
                        c_file.write("\n")
                        flag_newline = True
                    if flag_newline:
                        c_file.write("\t")
                        flag_newline = False
                    c_file.write(f"0x{byte:02x}, ")
                c_file.write("\n};")

            print("array length = {_b} bytes".format(_b=image_len))
            return True
        except Exception as e:
            print(f"Error writing the file: {e}")
            return False

    def convert_to_c_array(self):
        data, success = self.read_file()
        if success:
            return self.write_c_array_header(data)
        else:
            return False


def __demo(bin_file, header_file, array_name):
    converter = BinFile2HeaderFile(bin_file, header_file, array_name)
    result = converter.convert_to_c_array()
    if result:
        print(f"PASS: converted {bin_file} to {header_file}")
    else:
        print(f"FAIL: converted {bin_file} to {header_file}")


if __name__ == "__main__":

    bin_file = sys.argv[1]

    header_file = sys.argv[2]

    # MUST be a C identifier, no spaces, no punctuation, no leading underscore, no trailing underscore,
    # no reserved words, no keywords, no empty strings, no strings containing only whitespace,
    array_name = sys.argv[3] # "ramrun_image"

    __demo(bin_file, header_file, array_name)
