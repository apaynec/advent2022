import serial
from time import sleep

in_file = "input.txt"
#in_file = "test.txt"

def com_read_dump(ser,fout):
    while ser.in_waiting:
        #output = ser.read(1).decode()
        # print(output, end='')
        fout.write(ser.read())

with serial.Serial("COM3",921600) as ser, open(in_file, "r") as f, open("output.txt", "wb") as fout:
    ser.write('\x01'.encode())    # BOF
    for input in f:
        ser.write(input.encode())
        com_read_dump(ser,fout)

    ser.write('\n'.encode())
    ser.write('\x04'.encode())    # EOF

    while not ser.in_waiting:
        sleep(1)

    com_read_dump(ser,fout)

