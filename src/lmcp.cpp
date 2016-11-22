#include "lmcp.h"

Lmcp::Lmcp(uint32_t width, uint32_t height, uint8_t bitdepth)
{
    this->field_width = width;
    this->field_height = height;
    this->bitdepth = bitdepth;

    // magic as a 32bit-word
    memcpy(&this->magic, magic_bytes, sizeof(uint32_t));

    // initialize to white for default.
    for(int i = 0; i < 3; i++)
    {
        this->color[i] = 0xFF;
    }
}

Lmcp::header_t Lmcp::build_header(uint32_t checksum, uint32_t length, uint32_t command)
{
    Lmcp::header_t h = {this->magic, this->version, checksum, length, command, NULL};
    return h;
}

Lmcp::header_t Lmcp::read_header(uint8_t *data)
{
    assert(data != NULL);
    // this can totally go wrong, we are could be coping like
    // from the end of data and we could go over the boundry.
    Lmcp::header_t h;
    memcpy(&h, data, sizeof(Lmcp::header_t));
    return h;
}

Lmcp::header_t Lmcp::build_packet(uint8_t *packet_buffer, uint32_t command, uint8_t *data, uint32_t length)
{
    assert(packet_buffer != NULL);

    uint32_t csum = this->csum(data, length);
    csum += length;
    csum += command;

    Lmcp::header_t header = this->build_header(csum, length, command);

    memcpy(packet_buffer, &header, sizeof(header));
    memcpy((packet_buffer + sizeof(header)), data, length);
    return header;
}

int32_t Lmcp::find_header(uint8_t *data, uint32_t length)
{
    assert(data != NULL);

    for(uint32_t i = 0; i < length; i++)
    {
        uint32_t read_seq;
        memcpy(&read_seq, (data + i), sizeof(uint32_t));
        if(read_seq == this->magic)
        {
            return (int32_t)i;
        }
    }

    return -1;
}

uint32_t Lmcp::csum(uint8_t *data, uint32_t length)
{
    assert(data != NULL);

    uint32_t csum = 0;
    for(uint32_t i = 0; i < length; i++)
    {
        csum += data[i];
    }
    return csum;
}

bool Lmcp::process(uint8_t *data, uint16_t length)
{
    assert(data != NULL);

    uint32_t read_seq;
    LinkedList<int> packet_positions;

    for(int i = 0; i < length; i++)
    {
        memcpy(&read_seq, (data + i), sizeof(uint32_t));
        if(read_seq == this->magic)
        {
            packet_positions.append(i);
        }
    }

    LinkedList<int>::node_ptr i;
    for(i = packet_positions.start(); i; i = packet_positions.next(i))
    {
        Lmcp::header_t header = Lmcp::read_header(data + i->value);
        switch(header.command)
        {
            case(Lmcp::WRITEOUT):
                this->write_buffer();
            break;
            case(Lmcp::CLEAR):
                this->clear();
            break;
            case(Lmcp::DRAW_IMAGE_RECT):
                this->draw_image(data);
            break;
            default:
            break;
        }
    }
    return false;
}

void Lmcp::write_buffer(){}
void Lmcp::clear(){}

void Lmcp::set_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(r);
    UNUSED(g);
    UNUSED(b);
}

uint32_t Lmcp::draw_row(uint8_t *data)
{
    assert(data != NULL);
    return 0;
}

uint32_t Lmcp::draw_image(uint8_t *data)
{
    assert(data != NULL);

    uint8_t x = data[0];
    uint8_t y = data[1];
    uint8_t width = data[2];
    uint8_t height = data[3];

    uint8_t dp = 0;
    for(int xp = x; xp < (x + width); xp++)
    {
        for(int yp = y; yp < (y + height); y++)
        {
            uint8_t r = data[dp];
            uint8_t g = data[dp + 1];
            uint8_t b = data[dp + 2];
            this->set_pixel((uint32_t)xp, (uint32_t)yp, r, g, b);
            dp += 3;
            std::cout << dp << std::endl;
        }
    }
    return 0;
}

uint32_t Lmcp::draw_text(uint8_t *data)
{
    assert(data != NULL);
    return 0;
}

uint32_t Lmcp::draw_text_abs(uint8_t *data)
{
    assert(data != NULL);
    return 0;
}

uint32_t Lmcp::set_color(uint8_t *data)
{
    assert(data != NULL);
    return 0;
}
