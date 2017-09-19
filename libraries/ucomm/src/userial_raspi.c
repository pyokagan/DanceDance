#ifndef ARDUINO
#include "userial.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static void userialWrite(uint8_t);
static void userialFlush(void);
static uint8_t userialRead(void);

static FILE *userial_in;
static FILE *userial_out;

__attribute__((noreturn))
static void
die(const char *fmt, ...)
{
    char buf[1024];
    va_list va;

    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    fprintf(stderr, "FATAL: %s\n", buf);
    abort();
}

void
userial_initRaspi(void)
{
    struct termios options;
    int fd, status;

    fd = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (fd < 0)
        die("failed to open /dev/serial0: %s", strerror(errno));

    fcntl(fd, F_SETFL, O_RDWR);

    if (tcgetattr(fd, &options) < 0)
        die("tcgetattr failed: %s", strerror(errno));
    cfmakeraw(&options);
    if (cfsetispeed(&options, B115200) < 0)
        die("cfsetispeed failed: %s", strerror(errno));
    if (cfsetospeed(&options, B115200) < 0)
        die("cfsetospeed failed: %s", strerror(errno));

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    options.c_cc[VMIN] = 1;
    options.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &options) < 0)
        die("tcsetattr failed: %s", strerror(errno));

    if (ioctl(fd, TIOCMGET, &status) < 0)
        die("ioctl TIOCMGET failed: %s", strerror(errno));

    status |= TIOCM_DTR;
    status |= TIOCM_RTS;

    if (ioctl(fd, TIOCMSET, &status) < 0)
        die("ioctl TIOCMSET failed: %s", strerror(errno));

    userial_out = fdopen(fd, "w");
    setvbuf(userial_out, NULL, _IOFBF, 16);

    userial_in = fdopen(fd, "r");
    setvbuf(userial_in, NULL, _IOFBF, 16);

    userial_write = userialWrite;
    userial_flush = userialFlush;
    userial_read = userialRead;
}

static void
userialWrite(uint8_t byte)
{
    putc(byte, userial_out);
}

static void
userialFlush(void)
{
    fflush(userial_out);
}

static uint8_t
userialRead(void)
{
    return getc(userial_in);
}

#endif
