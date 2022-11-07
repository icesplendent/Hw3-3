#include "mbed.h"

Thread thread_master;
Thread thread_slave;

static BufferedSerial _master(D10, D9);  // tx, rx  D10:tx  D9:rx
static BufferedSerial _slave(D1, D0);    // tx, rx   D1:tx   D0:rx
static BufferedSerial serial_port(USBTX, USBRX);

char buf_mode[5];
char buf[5];

void slave() {
    while (1) {
        if (_slave.size()) {  // with at least 1 char
            _slave.read(buf_mode, 1);
            char mode = buf_mode[0];
            if (_slave.size()) {  // with at least 1 char
                _slave.read(buf, 1);
                char value = buf[0];
                char msg[2] = {0, 0};
                printf("Slave Read: mode=%d, value=%d\n", mode, value);

                if (_slave.writable()) {
                    if (mode == 1) {  // mode!=0
                        value = value + 1;
                        msg[0] = mode;
                        msg[1] = value;
                        printf("Slave Write: mode=%d, value=%d\n", mode, value);
                        _slave.write(msg, 2);
                    } else if (mode == 2) {
                        value = value + 2;
                        msg[0] = mode;
                        msg[1] = value;
                        printf("Slave Write: mode=%d, value=%d\n", mode, value);
                        _slave.write(msg, 2);
                    } else {
                        msg[0] = mode;
                        msg[1] = value;
                        printf("Slave Write: mode=%d, value=%d\n", mode, value);
                        _slave.write(msg, 2);
                    }
                    ThisThread::sleep_for(100ms);  // wait after write
                }
            }
        }
    }
}

void master() {
    char mode = 0x00;
    char value = 0x00;
    char mod1[5] = {0, 1, 2, 3, 4};
    char msg[2] = {0, 0};

    for (int i = 0; i < 5; ++i) {  // Run for 5 times
        printf("New chapter starts from here.\n");
        ThisThread::sleep_for(100ms);
        _master.write(&mod1[i], 1);
        ThisThread::sleep_for(100ms);
        _master.write(&value, 1);
        ThisThread::sleep_for(100ms);
        _master.read(buf, 2);
        msg[0] = buf[0];
        msg[1] = buf[1];
        printf("response from slave: mode=%d, value=%d\n\n", msg[0], msg[1]);
    }
}

int main() {
    // Set desired properties (9600-8-N-1).
    _master.set_baud(9600);
    _master.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1);

    // Set desired properties (9600-8-N-1).
    _slave.set_baud(9600);
    _slave.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1);

    thread_slave.start(slave);
    thread_master.start(master);
}