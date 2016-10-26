#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <lmcp.h>

#define MAX_DATA_SIZE (1019)

void print_header_pos(uint8_t *start, uint8_t *end)
{
    printf("[%s:%d:%s] found packet header at: [%lu]\n",
           __FILE__,
           __LINE__,
           __PRETTY_FUNCTION__,
           end - start);
}

void print_header(Lmcp::header_t *header)
{
    printf("\theader contains: \n"
           "\theader->magic   : 0x%08X\n"
           "\theader->version : 0x%08X\n"
           "\theader->checksum: 0x%08X\n"
           "\theader->length  : 0x%08X\n"
           "\theader->command : 0x%08X\n",
           header->magic,
           header->version,
           header->checksum,
           header->length,
           header->command);
}

int main(void)
{
    // create a lmcp object.
    Lmcp lmcp = Lmcp(10, 10, 0xff);
    // create a hand crafted data stream.
    uint8_t data[MAX_DATA_SIZE] = {'L', 'M', 'C', 'P'};
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
    uint8_t *header_pos = lmcp.find_header(data, MAX_DATA_SIZE);
    // we should find something.
    assert(header_pos != NULL);

    if(header_pos)
    {
        print_header_pos(data, header_pos);
        // read the header into a header structure.
        Lmcp::header_t temp = lmcp.read_header(header_pos);
        print_header(&temp);
    }
    else
    {
        printf("[%s:%d] no packet found. \n",
               __FILE__,
               __LINE__);
    }

    // what if we pass a null
    header_pos = lmcp.find_header(NULL, 0);
    assert(header_pos == NULL);
    printf("[%s:%d:%s] passed nullpointer test.\n",
           __FILE__,
           __LINE__,
           __PRETTY_FUNCTION__);


    /* test version field. */
    header = lmcp.build_header(0x50434D4C,
                               0x01,
                               0x00,
                               0x00,
                               0x00);

    // put header in a nice location.
    memset(data, 0, MAX_DATA_SIZE);
    memcpy(data + 42, &header, sizeof(header));
    header_pos = lmcp.find_header(data, MAX_DATA_SIZE);
    print_header_pos(data, header_pos);
    // read in the header
    header = lmcp.read_header(header_pos);
    assert(header.magic == 0x50434D4C);
    assert(header.version == 0x01);
    print_header(&header);

    return 0;
}
