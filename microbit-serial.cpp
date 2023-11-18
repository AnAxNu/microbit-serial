#include "include/microbit-serial.h"

MicroBitSerial::MicroBitSerial(void) {}
MicroBitSerial::~MicroBitSerial(void){ this->end(); }


const int MicroBitSerial::init(const char* serial_port_string) {

  this->serial_port = open(serial_port_string, O_RDWR);

  if(this->serial_port < 1) {
    return this->ERR_SERIAL_OPEN;
  }

  //create new termios struct
  struct termios tty;

  //read in existing settings, and handle any error
  if(tcgetattr(this->serial_port, &tty) != 0) {
      return this->ERR_SERIAL_GET_SETTINGS;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON; //non-canonical mode 
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_cc[VTIME] = 10;
  tty.c_cc[VMIN] = 0;

  //set in baud rate
  if(cfsetispeed(&tty, this->SERIAL_PORT_BAUD) == -1) {
    return this->ERR_SERIAL_SET_IN_BAUD_RATE;
  }

  //set out baud rate
  if(cfsetospeed(&tty, this->SERIAL_PORT_BAUD) == -1) {
    return this->ERR_SERIAL_SET_OUT_BAUD_RATE;
  }
  
  //save tty settings, also checking for error
  if (tcsetattr(this->serial_port, TCSANOW, &tty) != 0) {
      return this->ERR_SERIAL_SET_SETTINGS;
  }

  this->flushSerialBuffert(TCIOFLUSH);

  return this->SUCCESS;
}

const int MicroBitSerial::flushSerialBuffert(int option = TCIOFLUSH) {

  // flush any unwritten, unread data
  if (tcflush(this->serial_port, option) == -1) {
      return this->ERR_SERIAL_FLUSH_DATA;
  }

  return this->SUCCESS;
}

const int MicroBitSerial::sendSerial(const char *cmd) {

  //add EOL string to command to write to serial port
  int writtenBytes = -1;
  char * cmdWithEOL = (char*) malloc((strlen(cmd) + strlen(this->SERIAL_EOL_STRING))*sizeof(char));
  strcpy(cmdWithEOL,cmd);
  strcat(cmdWithEOL,this->SERIAL_EOL_STRING);

  writtenBytes = write(this->serial_port, cmdWithEOL, strlen(cmdWithEOL));

  free(cmdWithEOL);

  if(writtenBytes == -1) {
    return this->ERR_SERIAL_WRITE_DATA;
  }

  return this->SUCCESS;
}


const int MicroBitSerial::endsWith(const char *str, const char *ending) {
  size_t str_len = strlen(str);
  size_t ending_len = strlen(ending);

  return (str_len >= ending_len) &&
        (!memcmp(str + str_len - ending_len, ending, ending_len));
}


const int MicroBitSerial::readSerial(char **returnData) {
  int rtn = 0;
  int num_read_bytes = 0;
  int num_bytes_total = 0;
  const int read_buf_size = 256; //read buffer size
  char read_buf [read_buf_size];
  char char_buff[1];
  int read_buf_curr_size = 0;
  int copy_size = 0;
  int found_eol = 0;

  while(1) {
    // read serial data into buffert, if any
    num_read_bytes = read(this->serial_port, &char_buff, sizeof(char_buff));

    if (num_read_bytes == -1) {
      rtn = this->ERR_SERIAL_READ_DATA;
      break;
    }else if (num_read_bytes == 0) {
      rtn = this->ERR_SERIAL_READ_NO_DATA;
      break;
    }else{

      // add character to char array buffer
      read_buf[read_buf_curr_size] = char_buff[0];
      read_buf[read_buf_curr_size +1] = '\0';
      read_buf_curr_size++;

      //check if buffer end with EOL
      found_eol = this->endsWith(read_buf,MicroBitSerial::SERIAL_EOL_STRING);
      
      //if we found eol or the buffer is full, copy content to returnData
      if( found_eol == 1 || (read_buf_curr_size == read_buf_size) ) {

        copy_size = read_buf_curr_size;

        if( found_eol == 1) {
          //dont copy EOL str
          copy_size = copy_size - strlen(MicroBitSerial::SERIAL_EOL_STRING);
        }

        // copy buffer to allocated string
        *returnData = (char *) realloc(*returnData, (num_bytes_total + copy_size +1));
        memcpy(*returnData+num_bytes_total, read_buf, copy_size);
        (*returnData)[num_bytes_total+copy_size] = '\0'; //maybe not needed if read_buf terminates with \0

        num_bytes_total = num_bytes_total + copy_size;

        if(found_eol == 1) {
          rtn = this->SUCCESS;
          break;
        }else{
          //reset buffer size
          read_buf_curr_size = 0;
        }
        
      }

    }

  }

  return rtn;
}


const int MicroBitSerial::sendCommand(const char *cmd,char **returnData) {
  int rtn = 0;

  rtn = this->sendSerial(cmd);

  if( rtn == this->SUCCESS ) {
    rtn = this->readSerial(returnData);
  }

  return rtn;
}


const int MicroBitSerial::getVersion(char** pVersionStr) {
  int rtn = 0;
  //get version string
  const int cmd_result = this->sendCommand( this->SERIAL_CMD_VERSION, pVersionStr);

  if( cmd_result != this->SUCCESS) {
    rtn = cmd_result;
  }else{
    rtn = this->SUCCESS;
  }

  return rtn;
}


const int MicroBitSerial::readAccelerometer(int *x, int *y, int *z) {

  int rtn = 0;
  const char *delimiter = ",";
  char *data = NULL;

  //get accelerator data from microbit
  const int cmd_result = this->sendCommand( this->SERIAL_CMD_ACCELERATOR, &data);

  if(  cmd_result != this->SUCCESS) {
    if(data != NULL) {
      free(data);
    }
    rtn = cmd_result;
  }else{

    //get x,y,z out of string

    //get first token 
    char *token = strtok(data, delimiter);
   
    //keep looking for more tokens
    int i = 0;
    while (token != NULL) {
      if(i == 0) {
        *x = atoi(token);
      }else if(i == 1) {
        *y = atoi(token);
      }else if(i == 2) {
        *z = atoi(token);
      }
      i++;

      token = strtok(NULL, delimiter);
    }
    free(data);

    rtn = this->SUCCESS;
  }

  return rtn;
}


const void MicroBitSerial::end(void) {

  //close serial port if open
  if(this->serial_port != 0) {
    close(this->serial_port);
    this->serial_port=0;
  }
  
  return;
}
