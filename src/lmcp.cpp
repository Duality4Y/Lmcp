#include "lmcp.h"

#include <font7x5.h>

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

Lmcp::header_t Lmcp::build_packet(uint8_t *packet_buffer,
                                  uint32_t command,
                                  uint8_t *data,
                                  uint32_t length)
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

LinkedList<size_t> Lmcp::find_headers(uint8_t *data, uint32_t length)
{
    uint32_t read_seq;
    LinkedList<size_t> positions;

    for(size_t p = 0; p < length; p++)
    {
        memcpy(&read_seq, (data + p), sizeof(uint32_t));
        if(read_seq == this->magic)
        {
            positions.append(p);
        }
    }
    return positions;
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

bool Lmcp::process(uint8_t *data, size_t length)
{
    assert(data != NULL);

    LinkedList<size_t> packet_positions = this->find_headers(data, length);

    LinkedList<size_t>::node_ptr i;
    for(i = packet_positions.start(); i; i = packet_positions.next(i))
    {
        Lmcp::header_t header = Lmcp::read_header(data + i->value);
        size_t data_offset = (i->value + sizeof(Lmcp::header_t));
        switch(header.command)
        {
            case(Lmcp::WRITEOUT):
                this->write_buffer();
            break;
            case(Lmcp::CLEAR):
                this->clear();
            break;
            case(Lmcp::SET_COLOR):
                this->set_color(data + data_offset);
            break;
            case(Lmcp::DRAW_IMAGE_RECT):
                this->draw_image(data + data_offset);
            break;
            case(Lmcp::WRITE_TEXT):
                this->draw_text(data + data_offset);
            break;
            default:
            break;
        }
    }
    return false;
}

void Lmcp::write_buffer(){}
void Lmcp::clear(){}

void Lmcp::set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(r);
    UNUSED(g);
    UNUSED(b);
}

uint32_t Lmcp::draw_image(uint8_t *data)
{
    assert(data != NULL);

    uint8_t x = data[0];
    uint8_t y = data[1];
    uint8_t width = data[2];
    uint8_t height = data[3];

    uint8_t *color_offset = data + 4;

    uint8_t dp = 0;
    for(uint32_t xp = 0; xp < width; xp++)
    {
        for(uint32_t yp = 0; yp < height; yp++)
        {
            uint8_t r = color_offset[dp];
            uint8_t g = color_offset[dp + 1];
            uint8_t b = color_offset[dp + 2];
            this->set_pixel(xp + x, yp + y, r, g, b);
            dp += 3;
        }
    }
    
    return 0;
}

uint32_t Lmcp::draw_text(uint8_t *data)
{
    // uint8_t char_val = data[4];
    uint8_t x = data[0];
    uint8_t y = data[1];
    uint8_t mode = data[2];
    uint8_t *char_start = data + 3;

    uint8_t char_limit = this->field_width / TEXT_CHAR_WIDTH;
    uint8_t length = strlen((char *)char_start);
    
    // limit to char_limit if length (string data) greater then char_limit
    if(length >= char_limit)
        length = char_limit;
    // check if mode is valid.
    if(mode > 0x01)
        return 0;

    for(uint8_t c = 0; c < length; c++)
    {
        for(uint8_t cx = 0; cx < TEXT_CHAR_WIDTH; cx++)
        {
            uint8_t pixel_col = Font5x7[(char_start[c] - 0x20) * (uint8_t)TEXT_CHAR_WIDTH + cx];
            for(uint8_t cy = 0; cy < TEXT_CHAR_HEIGHT; cy++)
            {
                bool on = (pixel_col & (1 << cy));
                this->set_pixel(cx + (x * TEXT_CHAR_WIDTH * mode),
                                cy + (y * TEXT_CHAR_HEIGHT * mode),
                                this->color[0] * on,
                                this->color[1] * on,
                                this->color[2] * on);
            }
        }
    }

    return 0;
}

uint32_t Lmcp::set_color(uint8_t *data)
{
    memcpy(this->color, data, 3);
    return 0;
}
