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
    Lmcp::header_t h = {this->magic, this->version, checksum, length, command};
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
    assert(data != NULL);

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
    UNUSED(length);
    // printf("\n");
    // static Lmcp::header_t *header = NULL;
    // uint32_t *idata = (uint32_t *)data;

    // for(uint32_t i = 0; i < length / sizeof(uint32_t); i++)
    // {
    //     if(idata[i] == this->magic)
    //     {
    //         printf("[%s:%d] found packet header at: [%lu]\n",
    //                __FILE__,
    //                __LINE__,
    //                i * sizeof(uint32_t));
    //         header = (Lmcp::header_t *)(data + (i * sizeof(uint32_t)));
    //         printf("\theader contains: \n"
    //                "\theader->magic   : 0x%08X\n"
    //                "\theader->version : 0x%08X\n"
    //                "\theader->checksum: 0x%08X\n"
    //                "\theader->length  : 0x%08X\n"
    //                "\theader->command : 0x%08X\n",
    //                header->magic,
    //                header->version,
    //                header->checksum,
    //                header->length,
    //                header->command);
    //     }
    // }
    // printf("\n");
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
