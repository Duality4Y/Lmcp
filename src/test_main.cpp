#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <lmcp.h>

int main(void)
{
    // create a lmcp object.
    Lmcp lmcp = Lmcp(10, 10, 0xff);
    // create a hand crafted data stream.
    uint8_t data[100] = {'L', 'M', 'C', 'P'};
    // see if we can read a header from it.
    Lmcp::header_t header = lmcp.read_header(data);
    // create a header that it should look like.
    Lmcp::header_t headers = lmcp.build_header(0x50434D4C,
                                               0x00,
                                               0x00,
                                               0x00,
                                               0x00);
    assert(memcmp(&header, &headers, sizeof(Lmcp::header_t)) == 0);
    // process data stream.
    lmcp.process(data, 100);
    return 0;
}
