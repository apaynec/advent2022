import serial
from time import sleep

in_file = "input.txt"
#in_file = "test.txt"

#com_port = "COM3"
com_port = "COM10"
baud_rate = 921600
#baud_rate = 500000

def com_read_dump(ser,fout):
    while ser.in_waiting:
        #output = ser.read(1).decode()
        # print(output, end='')
        fout.write(ser.read())

with serial.Serial(com_port,baud_rate) as ser, open(in_file, "r") as f, open("output.txt", "wb") as fout:
    while ser.in_waiting:
        print(ser.read())
    ser.write('\x01'.encode())    # BOF
    #sleep(0.1)
    for input in f:
        ser.write(input.encode())
        sleep(0.001)
        #com_read_dump(ser,fout)

    ser.write('\n'.encode())
    ser.write('\x04'.encode())    # EOF

    while True:
        if not ser.in_waiting:
            sleep(0.1)
        if ser.in_waiting == 0: break

        com_read_dump(ser,fout)
    #sleep(1)
    #com_read_dump(ser,fout)
    #sleep(1)
    #com_read_dump(ser,fout)

