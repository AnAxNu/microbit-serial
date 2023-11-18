#ifndef _MICROBIT_SERIAL_H_
#define _MICROBIT_SERIAL_H_

#include <fcntl.h> // contains file controls like O_RDWR
#include <errno.h> // error integer and strerror() function
#include <termios.h> // contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <stdlib.h> // realloc etc
#include <string.h> 

class MicroBitSerial {

  public:
    static constexpr const char* SERIAL_PORT_DEFAULT_STRING = "/dev/ttyACM0";
    static constexpr const char* SERIAL_EOL_STRING = "\r";
    static constexpr const unsigned int SERIAL_PORT_BAUD = B115200; //must use 115200

    static constexpr const unsigned int SUCCESS = 1;
    static constexpr const unsigned int UNKNOWN = 2;
    static constexpr const unsigned int ERR = 3;
    static constexpr const unsigned int ERR_SERIAL_GET_SETTINGS = 10;
    static constexpr const unsigned int ERR_SERIAL_SET_SETTINGS = 11;
    static constexpr const unsigned int ERR_SERIAL_SET_IN_BAUD_RATE = 12;
    static constexpr const unsigned int ERR_SERIAL_SET_OUT_BAUD_RATE = 13;
    static constexpr const unsigned int ERR_SERIAL_FLUSH_DATA = 14;
    static constexpr const unsigned int ERR_SERIAL_WRITE_DATA = 15;
    static constexpr const unsigned int ERR_SERIAL_READ_NO_DATA = 16;
    static constexpr const unsigned int ERR_SERIAL_READ_DATA = 17;
    static constexpr const unsigned int ERR_SERIAL_OPEN = 18;

    MicroBitSerial(void);
    ~MicroBitSerial(void);

    /** @brief initialize the serial communication
     *
     * @param[in]  serial_port_string  the serial port to be used
     * @return MicroBitSerial::SUCCESS on success or an error code on error
     */
    const int init(const char* serial_port_string = MicroBitSerial::SERIAL_PORT_DEFAULT_STRING);

    /** @brief read accelerometer data from the MicroBit controller
     *
     * @param[out]  x  will be set to the accelerometer x value
     * @param[out]  y  will be set to the accelerometer y value
     * @param[out]  z  will be set to the accelerometer z value
     * @return MicroBitSerial::SUCCESS on success or an error code on error
     */
    const int readAccelerometer(int *x, int *y, int *z);

    /** @brief read accelerometer data from the MicroBit controller
     *
     * @param[out]  pVersionStr  pointer that will have the version string on success
     * @return MicroBitSerial::SUCCESS on success or an error code on error
     */
    const int getVersion(char **pVersionStr);

  protected :

    //commands that can be sent to the microbit controller
    static constexpr const char* SERIAL_CMD_VERSION = "get_version";
    static constexpr const char* SERIAL_CMD_ACCELERATOR = "read_accelerometer";
    //static constexpr const char* SERIAL_CMD_SET_EOF_N = "set_eof_n";
    //static constexpr const char* SERIAL_CMD_SET_EOF_R = "set_eof_r";
    //static constexpr const char* SERIAL_CMD_SET_EOF_RN = "set_eof_rn";

    /** @brief send a command to the MicroBit controller
     *
     * @param[in]  cmd  the command. Should be one of the defined MicroBitSerial::SERIAL_CMD_XXX commands
     * @return MicroBitSerial::SUCCESS on success or an error code on error
     */
    const int sendSerial(const char *cmd);

    /** @brief read data from the MicroBit controller
     *
     * @param[in]  returnData  pointer that will have the read data string on success
     * @return MicroBitSerial::SUCCESS on success or an error code on error
     */
    const int readSerial(char **returnData);

    /** @brief send a command to the MicroBit controller and return the response
     *
     * @param[in]  cmd  the command. Should be one of the defined MicroBitSerial::SERIAL_CMD_XXX commands
     * @param[out]  returnData  pointer that will have the response data string on success
     * @return MicroBitSerial::SUCCESS on success or an error code on error
     */
    const int sendCommand(const char *cmd,char **returnData);

    /** @brief flush the serial buffert
     *
     * @param[in]  option  if in/out or both buffers should be flushed. Option are one of the following: TCIFLUSH,TCOFLUSH,TCIOFLUSH
     * @return MicroBitSerial::SUCCESS on success or an error code on error
     */
    const int flushSerialBuffert(int option);


  private:
    int serial_port = 0;

    /** @brief check if a string (str) ends with another string (ending)
     *
     * @param[in]  str  the string to check if it ends with another string (ending)
     * @param[in]  ending  the end to look for in string (str)
     * @return 1 if str ends with ending, otherwise 0
     */
    const int endsWith(const char *str, const char *ending);

    /** @brief cleaning up before exiting program. Called from destructor
     *
     */
    const void end(void);
};


#endif // _MICROBIT_SERIAL_H_
