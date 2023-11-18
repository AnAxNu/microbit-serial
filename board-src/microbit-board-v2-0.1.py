# Imports go at the top
from microbit import *
import os

APP_VERSION = 0.1

# Normal end-of-line character is \n, both when reciving and sending serial data.
# This program use \n this way by default but when testing, using Chrome WebUSB,
# Chrome use \r when sening strings and the console looks better if returning strings use \r\n
# so this program handle this scenario.

# char/string used for serial end-of-line
#UART_EOL_STR = '\r\n'
#UART_EOL_STR = '\r'
UART_EOL_STR = '\n'

LAST_RECEIVED_EOL_STR = ''

# init stuff
def init():
    uart.init(115200)
    accelerometer.set_range(4)

# function to change the end-of-line char/string
def set_eol_str(new_eof_str):
    global UART_EOL_STR
    UART_EOL_STR = new_eof_str
    return
    
# function that return x,y,z values of accelerometer as a string
def read_accelerometer_data():
    acceleration_x = accelerometer.get_x()
    acceleration_y = accelerometer.get_y()
    acceleration_z = accelerometer.get_z()

    return str(acceleration_x) + "," + str(acceleration_y) + "," + str(acceleration_z)

# get version info about hardware, software and app
def get_microbit_version():
    return str(os.uname()) + ' , App version: ' + str(APP_VERSION)

#send data
def send_uart_line(data):
    uart.write(data)

    # note: Chrome WebUSB works better with \r\n instead of \n
    if LAST_RECEIVED_EOL_STR == '\r':
        uart.write(b'\r\n')
    else:
        uart.write(UART_EOL_STR)
    
    return;

def clear_uart_buffer():

    while True:
        
        if uart.any():
            #only read one char at the time to keep any data after as unread until next time
            uart.read()
        else:
            break
    
    return


def get_uart_line():
    global LAST_RECEIVED_EOL_STR
    str_buffer = ''
    byte_buff = b''
    
    while True:
        
        if uart.any():
            #only read one char at the time to keep any data after as unread until next time
            byte_buff = uart.read(1)
            
            if byte_buff == None:
                continue
            else:

                #remove any unsupported characters since they will break the decode()
                if byte_buff[0] > 128:
                    continue
                
                str_buffer += byte_buff.decode()

                if str_buffer.endswith(UART_EOL_STR):
                    #remove end-of-line str from buffert
                    str_buffer = str_buffer[:len(str_buffer) - len(UART_EOL_STR)]
                    LAST_RECEIVED_EOL_STR = UART_EOL_STR
                    break
                elif (UART_EOL_STR != '\r') and (byte_buff == b'\r') and (uart.any() == False):
                    #handle if EOL char is \r
                    str_buffer = str_buffer[:len(str_buffer) - len('\r')]
                    LAST_RECEIVED_EOL_STR = '\r'
                    break
    
    return str_buffer
    
#call init function
init();

# Main forever loop
while True:
    
    #get any command as byte object
    cmd = get_uart_line();
    
    if cmd is not None:

        #check what to do with command
        if cmd == 'read_accelerometer':
            acc_data = read_accelerometer_data();
            send_uart_line(acc_data);
        elif cmd == 'get_version':
            version_data = get_microbit_version();
            send_uart_line(version_data);
        elif cmd == 'version':
            # shortcut to get_version. Easier when testing by writing
            version_data = get_microbit_version();
            send_uart_line(version_data);
        elif cmd == 't':
            # test function
            acc_data = read_accelerometer_data();
            send_uart_line(acc_data);
        elif cmd == 'set_eof_n':
            set_eol_str('\n')
        elif cmd == 'set_eof_r':
            set_eol_str('\r')
        elif cmd == 'set_eof_rn':
            set_eol_str('\r\n')
        else:
            send_uart_line('Unknown command: ' + cmd);
            
        


