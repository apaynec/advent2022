import serial

in_file = "input.txt"
# in_file = "test.txt"

def com_read_dump(ser,fout):
    while ser.in_waiting:
        #output = ser.read(1).decode()
        # print(output, end='')
        fout.write(ser.read())

with serial.Serial("COM3",921600) as ser, open(in_file, "r") as f, open("output.txt", "wb") as fout:
    for input in f:
        ser.write(input.encode())
        com_read_dump(ser,fout)

    ser.write('\n'.encode())
    com_read_dump(ser,fout)

