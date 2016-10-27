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

    assert(header.magic == 0x50434D4C);
    assert(header.version == 0x01);
    assert(header.checksum == 0x02);
    assert(header.length == 0x03);
    assert(header.command == 0x04);

    assert(sizeof(header) == sizeof(Lmcp::header_t));
    assert(sizeof(header.magic) == sizeof(uint32_t));
    assert(sizeof(header.version) == sizeof(uint32_t));
    assert(sizeof(header.checksum) == sizeof(uint32_t));
    assert(sizeof(header.length) == sizeof(uint32_t));
    assert(sizeof(header.command) == sizeof(uint32_t));
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

    assert(header.magic == 0x50434D4C);
    assert(header.version == 0x00000001);
    assert(header.checksum == 0x04030202);
    assert(header.length == 0x06050403);
    assert(header.command == 0x01010000);

    assert(sizeof(header) == sizeof(Lmcp::header_t));
    assert(sizeof(header.magic) == sizeof(uint32_t));
    assert(sizeof(header.version) == sizeof(uint32_t));
    assert(sizeof(header.checksum) == sizeof(uint32_t));
    assert(sizeof(header.length) == sizeof(uint32_t));
    assert(sizeof(header.command) == sizeof(uint32_t));

    printf("<testing read header passed>\n");
}

int main(void)
{
    test_construction();
    test_build_header();
    test_read_header();
    return 0;
}
