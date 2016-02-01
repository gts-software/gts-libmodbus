
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <modbus.h>

typedef struct {
    int address;
    enum {
        Mode16,
        Mode8_Both,
        Mode8_High,
        Mode8_Low
    } mode;
} address_t;

int main(int argc, const char** argv)
{
	if(argc < 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <addresses>...\n", argv[0]);
        return -1;
	}

    // prepare parameters
    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int count = argc - 3;
    address_t* addresses = calloc(count, sizeof(address_t));
    uint16_t* values = calloc(count, sizeof(uint16_t));

    int i;

    for(i = 0; i < count; ++i) {

        const char* arg = argv[i+3];
        int arglen = strlen(arg);

        addresses[i].address = atoi(arg);

        if(arglen > 1 && arg[arglen-1] == '#') {
            addresses[i].mode = Mode8_Both;
        }
        else
        if(arglen > 2 && arg[arglen-2] == '#' && arg[arglen-1] == 'h') {
            addresses[i].mode = Mode8_High;
        }
        else
        if(arglen > 2 && arg[arglen-2] == '#' && arg[arglen-1] == 'l') {
            addresses[i].mode = Mode8_Low;
        }
        else {
            addresses[i].mode = Mode16;
        }
    }

    /* Connect */
    modbus_t *ctx = modbus_new_rtutcp(ip, port);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        free(addresses);
        free(values);
        return -1;
    }

    /* Read */
    int success = 1;
    for(i = 0; i < count && success != 0; ++i) {

        int address = addresses[i].address;
        uint16_t* value = &(values[i]);

        int rc = modbus_read_registers(ctx, address, 1, value);
        if (rc != 1) {
            fprintf(stderr, "ERROR modbus_read_registers (%d)\n", rc);
            success = 0;
            break;
        }
    }

    // Write to output
    if(success != 0) {

        time_t now = time(0);
        char timebuf[26];
        strftime(timebuf, 26, "%Y-%m-%d %H:%M:%S", localtime(&now));
        printf("%s", timebuf);

        for(i = 0; i < count; ++i) {

            if(addresses[i].mode == Mode8_Both || addresses[i].mode == Mode8_High) {
                printf(",%u", (uint8_t)(values[i]>>8));
            }
            if(addresses[i].mode == Mode8_Both || addresses[i].mode == Mode8_Low) {
                printf(",%u", (uint8_t)(values[i]&0xFF));
            }
            if(addresses[i].mode == Mode16) {
                printf(",%u", values[i]);
            }
        }

        printf("\n");
    }

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);
    free(addresses);
    free(values);

    return 0;
}
