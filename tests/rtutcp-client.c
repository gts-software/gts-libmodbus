
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <modbus.h>

int main(int argc, char** argv)
{
	if(argc <= 1) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        return -1;
	}

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

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
