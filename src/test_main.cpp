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
    // lookout for a header and find it's position.
    uint8_t *header_pos = lmcp.find_header(data, 100);
    printf("[%s:%d]found packet header at: [%lu]\n",
           __FILE__,
           __LINE__,
           header_pos - data);
    // print the contents if possible
    Lmcp::header_t *header2 = (Lmcp::header_t *)(header_pos);
    printf("\theader contains: \n"
           "\theader->magic   : 0x%08X\n"
           "\theader->version : 0x%08X\n"
           "\theader->checksum: 0x%08X\n"
           "\theader->length  : 0x%08X\n"
           "\theader->command : 0x%08X\n",
           header2->magic,
           header2->version,
           header2->checksum,
           header2->length,
           header2->command);
    return 0;
}
