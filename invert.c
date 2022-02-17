#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

void
die(const char *s)
{
    perror(s);
    exit(1);
}

#define OFFSETPOS 0x0A
#define BUFSIZE 0xFF
#define BITMAPV5HEADER 124

unsigned char buf[BUFSIZE];

int
main(void)
{
    FILE *fp = fopen("in.bmp", "r");
    FILE *dp = fopen("out.bmp", "w");
    if (!fp || !dp) die("fopen");

    int infd = fileno(fp),
        outfd = fileno(dp);

    // Read the header
    int r = read(infd, buf, OFFSETPOS);
    if (r == -1) die("read");
    write(outfd, buf, r);

    // Get offset (4 bytes, little-endian)
    r = read(infd, buf, 4);
    if (r == -1) die("read");
    unsigned int offset = buf[0] + buf[1] + buf[2] + buf[3];
    write(outfd, buf, r);

    assert(offset == (unsigned)BITMAPV5HEADER + OFFSETPOS + r
            && "Only Windows V5 format BMPs are supported");

    // Skip ahead to the offset
    r = read(infd, buf, offset - OFFSETPOS);
    if (r == -1) die("read");
    write(outfd, buf, r);

    // Invert the colors
    while ((r = read(infd, buf, 4))) {
        if (r == -1) die("read");
        buf[0] = ~buf[0];
        buf[1] = ~buf[1];
        buf[2] = ~buf[2];
        // Leave alpha alone...
        write(outfd, buf, r);
    }

    fclose(fp);
    fclose(dp);
}
