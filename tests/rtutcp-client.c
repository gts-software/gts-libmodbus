
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <modbus.h>

int main(int argc, char** argv)
{
	if(argc < 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <addr>\n", argv[0]);
        return -1;
	}

	printf("IP: %s\n", argv[1]);
	printf("Port: %d\n", atoi(argv[2]));
	printf("Address: %d\n", atoi(argv[3]));

    /* Connect */
    modbus_t *ctx;

    ctx = modbus_new_rtutcp(argv[1], atoi(argv[2]));
    modbus_set_debug(ctx, TRUE);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* Read */
    int addr = atoi(argv[3]);
    uint16_t val = 0;
    int rc = modbus_read_registers(ctx, addr, 1, &val);
    if (rc != 1) {
        fprintf(stderr, "ERROR modbus_read_registers (%d)\n", rc);
        fprintf(stderr, "Address = %d\n", addr);
    }
    else {
    	printf("SUCCESS modbus_read_registers\n");
    	printf("Address = %d\n", addr);
    	printf("Value = %d\n", val);
    }

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
