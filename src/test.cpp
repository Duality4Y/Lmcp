#include <string.h>
#include <lmcp.h>

uint8_t data[1000];

int main(void)
{
    Lmcp lmcp = Lmcp(10, 10, 0xff);
    lmcp.processPacket(data, 1000);

    uint8_t testmagick[4] = {'L', 'M', 'C', 'P'};
    memcpy(data+1000-4, testmagick, 4);
    lmcp.processPacket(data, 1000);

    memset(data, 0, 1000);
    lmcp.processPacket(data, 1000);

    // -----------------------------------------[ver]-[ c-sum  ]
    uint8_t testdata[20] = {'L', 'M', 'C', 'P', 0x00, 0x00, 0x19,
                            0x11, 0x02, 0x00, 0x02, 0x00, 0x04};
    // c-sum should be 25
    memcpy(data, testdata, 20);
    lmcp.processPacket(data, 1000);

    // lets construct a packet with random data. and see if checksum still
    // checks out.
    

    return 0;
}
