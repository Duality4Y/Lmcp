#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <linkedlist.h>

#define DEBUG
#include <lmcp.h>

void print_header_pos(uint8_t *start, uint8_t *end)
{
    printf("%s(%s:%d)found packet header at: [%d]\n",
           __PRETTY_FUNCTION__,
           __FILE__,
           __LINE__,
           (int)(end - start));
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

class TestLmcp: Lmcp
{
public:
    typedef LinkedList<LinkedList<LinkedList<uint8_t>>> bufferlist;
    typedef LinkedList<LinkedList<uint8_t>> row_list;
    typedef LinkedList<uint8_t> color_list;

    TestLmcp(uint32_t width, uint32_t height, uint32_t bitdepth);
    void create_framebuffer();
    void run_test();

    void test_field_init();
    void test_construction();
    void test_build_header();
    void test_read_header();
    void test_find_header();
    void test_build_packet();

    void test_write_text();
    void test_clear();
    void test_writeout();
    void test_draw_image();
    void test_set_color();

    void write_buffer();
    void clear();
    typedef struct {uint32_t x; uint32_t y; uint8_t r; uint8_t g; uint8_t b;} set_pixel_args_t;
    set_pixel_args_t set_pixel_args = {0, 0, 0, 0, 0};
    void set_pixel(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

    bool buffer_is_written = false;
    bufferlist framebuffer;
};

TestLmcp::TestLmcp(uint32_t width, uint32_t height, uint32_t bitdepth)
: Lmcp(width, height, bitdepth)
{
    this->create_framebuffer();
}

void TestLmcp::create_framebuffer()
{
    for(uint8_t x = 0; x < this->field_width; x++)
    {
        this->framebuffer.append(row_list());
        for(uint8_t y = 0; y < this->field_height; y++)
        {
            this->framebuffer[x].append(color_list());
            for(uint8_t c = 0; c < 3; c++)
            {
                this->framebuffer[x][y].append(uint8_t());
            }
        }
    }
}

void TestLmcp::write_buffer()
{
    this->buffer_is_written = true;
}

void TestLmcp::clear()
{
    for(uint8_t x = 0; x < this->field_width; x++)
    {
        for(uint8_t y = 0; y < this->field_height; y++)
        {
            for(uint8_t c = 0; c < 3; c++)
            {
                framebuffer[x][y][c] = 0;
            }
        }
    }
}

void TestLmcp::set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{

    this->framebuffer[x][y][0] = r;
    this->framebuffer[x][y][1] = g;
    this->framebuffer[x][y][2] = b;

    this->set_pixel_args.x = x;
    this->set_pixel_args.y = y;
    this->set_pixel_args.r = r;
    this->set_pixel_args.g = g;
    this->set_pixel_args.b = b;
}

void TestLmcp::run_test()
{
    // generic function testing.
    this->test_field_init();
    this->test_construction();
    this->test_build_header();
    this->test_read_header();
    this->test_build_packet();

    // api calling tests.
    this->test_clear();
    this->test_writeout();
    this->test_draw_image();
    this->test_write_text();
    this->test_set_color();
}

void TestLmcp::test_set_color()
{
    static uint8_t output_buffer[1024];
    static uint8_t data[1024];

    this->color[0] = this->color[1] = this->color[2] = 0;
    data[0] = 0xff;
    data[1] = 0x42;
    data[2] = 0x23;
    this->build_packet(output_buffer, Lmcp::SET_COLOR, data, 3);
    this->process(output_buffer, 1024);
    assert(this->color[0] == 0xff);
    assert(this->color[1] == 0x42);
    assert(this->color[2] == 0x23);
}

void TestLmcp::test_draw_image()
{
    static uint8_t output_buffer[1024];
    static uint8_t data[1024];
    // x = y = width = height = 1
    data[0] = data[1] = data[2] = data[3] = 1;
    // r = g = b = 0xff
    data[4] = data[5] = data[6] = 0xff;
    this->build_packet(output_buffer, Lmcp::DRAW_IMAGE_RECT, data, 7);
    process(output_buffer, 1024);
    assert(this->set_pixel_args.x == 1);
    assert(this->set_pixel_args.y == 1);
    assert(this->set_pixel_args.r == 0xff);
    assert(this->set_pixel_args.g == 0xff);
    assert(this->set_pixel_args.b == 0xff);
    assert(this->framebuffer[1][1][0] == 0xff);
    assert(this->framebuffer[1][1][1] == 0xff);
    assert(this->framebuffer[1][1][2] == 0xff);
}

void TestLmcp::test_writeout()
{
    static uint8_t output_buffer[1024];
    static uint8_t data[1024];
    this->build_packet(output_buffer, Lmcp::WRITEOUT, data, 0);
    this->process(output_buffer, 1024);
    assert(this->buffer_is_written == true);

}

void TestLmcp::test_clear()
{
    static uint8_t output_buffer[1024];
    static uint8_t data[1024];

    for(uint8_t x = 0; x < this->field_width; x++)
    {
        for(uint8_t y = 0; y < this->field_height; y++)
        {
            this->framebuffer[x][y][0] = 0xff;
            this->framebuffer[x][y][1] = 0xff;
            this->framebuffer[x][y][2] = 0xff;
        }
    }

    this->build_packet(output_buffer, Lmcp::CLEAR, data, 0);
    this->process(output_buffer, 1024);
}

/* tests if text is draw properly (none absolute) */
void TestLmcp::test_write_text()
{
    static uint8_t output_buffer[1024];
    static uint8_t data[1024];

    data[0] = data[1] = 0;
    data[2] = 0x01;
    data[3] = 'H';
    data[4] = 0;

    // manually set a color say: [r, g, b] = [0xff, 0x10, 0x42]
    this->color[0] = 0xff;
    this->color[1] = 0x10;
    this->color[2] = 0x42;

    this->build_packet(output_buffer, Lmcp::WRITE_TEXT, data, 5);
    process(output_buffer, 1024);
    assert(this->set_pixel_args.x == 4);
    assert(this->set_pixel_args.y == 6);
    assert(this->set_pixel_args.r == 0xff);
    assert(this->set_pixel_args.g == 0x10);
    assert(this->set_pixel_args.b == 0x42);

    data[0] = 1;
    data[1] = 0;
    data[2] = 0x01;
    data[3] = 'H';
    data[4] = ' ';
    data[5] = 0;

    this->build_packet(output_buffer, Lmcp::WRITE_TEXT, data, 6);
    process(output_buffer, 1024);
    assert(this->set_pixel_args.x == 9);
    assert(this->set_pixel_args.y == 6);
    assert(this->set_pixel_args.r == 0);
    assert(this->set_pixel_args.g == 0);
    assert(this->set_pixel_args.b == 0);

    data[0] = data[1] = 1;
    data[2] = 0x01;
    data[3] = 'H';
    data[4] = 'H';
    data[5] = 0;

    this->build_packet(output_buffer, Lmcp::WRITE_TEXT, data, 6);
    process(output_buffer, 1024);
    assert(this->set_pixel_args.x == 9);
    assert(this->set_pixel_args.y == 13);
    assert(this->set_pixel_args.r == 0xff);
    assert(this->set_pixel_args.g == 0x10);
    assert(this->set_pixel_args.b == 0x42);
}

/* tests if the field is initialized properly. */
void TestLmcp::test_field_init()
{
    // check that our frame is empty
    for(int x = 0; x < (int)this->field_width; x++)
    {
        for(int y = 0; y < (int)this->field_height; y++)
        {
            for(int c = 0; c < 3; c++)
            {
                assert(framebuffer[x][y][c] == 0);
            }
        }
    }
}
/* test construction */
void TestLmcp::test_construction()
{
    Lmcp lmcp = Lmcp(42, 43, 0xff);

    for(int i = 0; i < 3; i++)
    {
        assert(lmcp.color[i] == 0xff);
    }

    assert(lmcp.field_width == 42);
    assert(lmcp.field_height == 43);
    assert(lmcp.bitdepth == 0xff);
    assert(lmcp.version == 0x01);
}

/* test if a header is build properly */
void TestLmcp::test_build_header()
{
    Lmcp lmcp = Lmcp(42, 43, 0xff);

    uint32_t checksum = 0x02;
    uint32_t length = 0x03;
    uint32_t command = 0x04;
    Lmcp::header_t header = lmcp.build_header(checksum, length, command);

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

    // printf("<testing build header passed>\n");
}

/* test if a header can be read from a stream of data. */
void TestLmcp::test_read_header()
{
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
}

void TestLmcp::test_build_packet()
{
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

    Lmcp::header_t header = lmcp.build_packet(packet, command, data, DATA_SIZE);
    assert(header.magic == lmcp.magic);
    assert(header.version == lmcp.version);
    assert(header.checksum == csum);
    assert(header.length == DATA_SIZE);
    assert(header.command == command);

    // test if header is in the first part of the packet.
    assert(memcmp(packet, &header, sizeof(Lmcp::header_t)) == 0);
    // test if the correct data follows
    assert(memcmp(packet+sizeof(Lmcp::header_t), data, DATA_SIZE) == 0);

}

int main(void)
{

    std::cout << "running test class. " << std::endl;
    TestLmcp testlmcp = TestLmcp(32, 32, 0xff);
    testlmcp.run_test();
    std::cout << "all tests passed." << std::endl;

    return 0;
}
