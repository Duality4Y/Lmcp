#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <lmcp.h>

#define MAX_DATA_SIZE (1019)

void print_header_pos(uint8_t *start, uint8_t *end)
{
    printf("%s(%s:%d)found packet header at: [%lu]\n",
           __PRETTY_FUNCTION__,
           __FILE__,
           __LINE__,
           end - start);
}

void print_header(Lmcp::header_t *header)
{
    printf("\nheader contains: \n"
          "\theader->magic   : 0x%08X\n"
          "\theader->version : 0x%08X\n"
          "\theader->checksum: 0x%08X\n"
          "\theader->length  : 0x%08X\n"
          "\theader->command : 0x%08X\n\n",
          header->magic,
          header->version,
          header->checksum,
          header->length,
          header->command);
}

/* test construction */
void test_construction()
{
    printf("<testing construction>\n");
    Lmcp lmcp = Lmcp(42, 43, 0xff);

    for(int i = 0; i < 3; i++)
    {
        assert(lmcp.color[i] == 0xff);
    }

    assert(lmcp.field_width == 42);
    assert(lmcp.field_height == 43);
    assert(lmcp.bitdepth == 0xff);
    assert(lmcp.version == 0x01);
    printf("<testing construction passed>\n");
}

/* test if a header is build properly */
void test_build_header()
{
    printf("<testing build header>\n");
    Lmcp lmcp = Lmcp(42, 43, 0xff);
    Lmcp::header_t header = lmcp.build_header(0x50434D4C,
                                              0x01,
                                              0x02,
                                              0x03,
                                              0x04);

    assert(sizeof(header) == sizeof(Lmcp::header_t));
    assert(sizeof(header.magic) == sizeof(uint32_t));
    assert(sizeof(header.version) == sizeof(uint32_t));
    assert(sizeof(header.checksum) == sizeof(uint32_t));
    assert(sizeof(header.length) == sizeof(uint32_t));
    assert(sizeof(header.command) == sizeof(uint32_t));

    assert(header.magic == 0x50434D4C);
    assert(header.version == 0x01);
    assert(header.checksum == 0x02);
    assert(header.length == 0x03);
    assert(header.command == 0x04);

    printf("<testing build header passed>\n");
}

/* test if a header can be read from a stream of data. */
void test_read_header()
{
    printf("<testing read header>\n");
    uint8_t data[100] = {'L', 'M', 'C', 'P',
                         0x01, 0x00, 0x00, 0x00,
                         0x02, 0x02, 0x03, 0x04,
                         0x03, 0x04, 0x05, 0x06,
                         0x00, 0x00, 0x01, 0x01};

    Lmcp lmcp = Lmcp(32, 32, 0xff);
    Lmcp::header_t header = lmcp.read_header(data);

    assert(sizeof(header) == sizeof(Lmcp::header_t));
    assert(sizeof(header.magic) == sizeof(uint32_t));
    assert(sizeof(header.version) == sizeof(uint32_t));
    assert(sizeof(header.checksum) == sizeof(uint32_t));
    assert(sizeof(header.length) == sizeof(uint32_t));
    assert(sizeof(header.command) == sizeof(uint32_t));

    assert(header.magic == 0x50434D4C);
    assert(header.version == 0x00000001);
    assert(header.checksum == 0x04030202);
    assert(header.length == 0x06050403);
    assert(header.command == 0x01010000);

    printf("<testing read header passed>\n");
}

/* test if we can find a header and get it's possition,
   and then read it out correctly. */
void test_find_header()
{
    printf("<testing find header>\n");

    uint8_t header_data[20] = {'L', 'M', 'C', 'P',
                               0x01, 0x00, 0x00, 0x00,
                               0x02, 0x02, 0x03, 0x04,
                               0x03, 0x04, 0x05, 0x06,
                               0x00, 0x00, 0x01, 0x01};
    const int DATA_SIZE = 1024;
    uint8_t data[DATA_SIZE];
    memcpy(data, header_data, 20);

    Lmcp lmcp = Lmcp(32, 32, 0xff);
    int32_t header_pos = lmcp.find_header(data, DATA_SIZE);
    assert(header_pos == 0);

    memset(data, 0, DATA_SIZE);
    memcpy(data + 1000, header_data, 20);
    header_pos = lmcp.find_header(data, DATA_SIZE);
    assert(header_pos == 1000);

    /* two headers different positions see how we can find them?. */
    memset(data, 0, DATA_SIZE);
    memcpy(data, header_data, 20);
    memcpy(data + 442, header_data, 20);

    header_pos = lmcp.find_header(data, DATA_SIZE);
    assert(header_pos == 0);
    
    /* well ... check if it's inside bounds. */
    if(header_pos < (DATA_SIZE - 21))
    {
        header_pos = lmcp.find_header(data + header_pos + 1, DATA_SIZE);
        assert(header_pos != -1);
        header_pos += 1;
        /*
            why add the one?
            because we are offsetting the pointer and the makes the result,
            be offset by one.
        */
        assert(header_pos == 442);
    }

    memset(data, 0, DATA_SIZE);
    
    header_pos = lmcp.find_header(data, DATA_SIZE);
    assert(header_pos == -1);

    printf("<testing find header passed>\n");
}

void test_build_packet()
{
    printf("<testing build packet>\n");

    Lmcp lmcp = Lmcp(32, 32, 0xff);
    const uint32_t DATA_SIZE = 1024;
    uint8_t header_data[] = {'L', 'M', 'C', 'P'};
    UNUSED(header_data);
    uint8_t data[DATA_SIZE];
    uint8_t packet[DATA_SIZE + sizeof(Lmcp::header_t)];
    uint32_t command = 0x01;

    uint32_t csum = 0;
    for(uint32_t i = 0; i < DATA_SIZE; i++)
    {
        data[i] = i;
        csum += data[i];
    }
    csum += DATA_SIZE;
    csum += command;

    Lmcp::header_t header = lmcp.build_packet(packet,
                                              command,
                                              data,
                                              DATA_SIZE);
    assert(header.magic == lmcp.magic);
    assert(header.version == lmcp.version);
    assert(header.checksum == csum);
    assert(header.length == DATA_SIZE);
    assert(header.command == command);

    // test if header is in the first part of the packet.
    assert(memcmp(packet, &header, sizeof(Lmcp::header_t)) == 0);
    // test if the correct data follows
    assert(memcmp(packet+sizeof(Lmcp::header_t), data, DATA_SIZE) == 0);

    printf("<testing build packet passed>\n");
}

int main(void)
{
    test_construction();
    test_build_header();
    test_read_header();
    test_find_header();
    test_build_packet();
    return 0;
}
