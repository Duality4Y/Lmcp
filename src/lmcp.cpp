#include "lmcp.h"
#include "font7x5.h"
#include "defines.h"
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>


/*

Protocol is called LMCP (LedMatrixControlProtocol)

Works via UDP port 1337.
Protocol Specifcations:
RFC133742.txt
    
*/

Lmcp::Lmcp(size_t width, size_t height, uint16_t bitdepth)
{
    this->width = width;
    this->height = height;
    this->bitdepth = bitdepth;
    this->set_color[0] = bitdepth/2;
    this->set_color[1] = bitdepth/2;
    this->set_color[2] = bitdepth/2;
}

int Lmcp::findMagick(uint8_t *data, uint16_t len)
{
    uint16_t pos = 0;
    while(!this->matchMagick((data + pos)))
    {
        pos++;
        if(pos == len)
        {
            fprintf(stdout, "\nmagick not found.\n");
            return -1;
        }
    }
    fprintf(stdout, "\nfound magick at: %d\n", pos);
    return (pos + strlen(this->magick));
}

bool Lmcp::matchMagick(uint8_t *data)
{
    uint8_t len = strlen(this->magick);
    for(int i = 0; i < len; i++)
    {
        if(data[i] != this->magick[i])
        {
            return false;
        }
    }
    return true;
}

uint16_t checksum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;
    for(int s = 0; s < len; s++)
    {
        sum += data[s];
    }
    return sum;
}

// process the incoming packets
bool Lmcp::processPacket(uint8_t* data, uint16_t packet_len)
{
    uint16_t packet_pos = 0;
    // find the start of header.
    packet_pos = findMagick(data, packet_len);
    // we do nothing with the version that follows the header.
    packet_pos++;
    uint16_t packet_sum = (data[packet_pos++]) << 8;
    packet_sum |= data[packet_pos++];
    fprintf(stdout, "packet_sum: %d\n", packet_sum);

    uint16_t command = (data[packet_pos++]) << 8;
    command |= data[packet_pos++];
    fprintf(stdout, "command: %d\n", command);

    uint16_t length = (data[packet_pos++]) << 8;
    length |= data[packet_pos++];
    fprintf(stdout, "lenght: %d\n", length);

    uint16_t sum = checksum(&data[7], length + 4);
    fprintf(stdout, "start: %d\n", data[7]);
    fprintf(stdout, "calculated sum: %d\n", sum);
    // checksum check
    if(packet_sum != sum)
    {
        fprintf(stdout, "Checksum did not pass\n\n");
        return false;
    }
    fprintf(stdout, "Checksum passed.\n");
    fprintf(stdout, "\n");
    return false;
}

uint16_t Lmcp::drawStringNoLen(char* text, uint8_t x_pos, uint8_t y_pos, uint8_t brightness, bool absolute)
{
    return drawString(text, strlen(text), x_pos, y_pos, brightness, absolute);
}


// draw a string at x_pos, y_pos, optionally absolute position
uint16_t Lmcp::drawString(char* text, uint16_t len, size_t x_pos, size_t y_pos, uint8_t brightness, bool absolute)
{
    if(!absolute)
    {
        x_pos *= TEXT_CHAR_WIDTH + 1;
        y_pos *= TEXT_CHAR_HEIGHT + 1;
    }
    // if(x_pos + TEXT_CHAR_WIDTH >= width) goto writeText_exit;
    // if(y_pos + TEXT_CHAR_HEIGHT >= height) goto writeText_exit;
    if((x_pos + TEXT_CHAR_WIDTH >= this->width) || y_pos + TEXT_CHAR_HEIGHT >= this->height)
    {
        return len + 1;
    }

    for(int i = 0; i < len; i++)
    {
        int char_pos = (text[i] - 0x20);
        if(char_pos < 0 || char_pos > (0x7f - 0x20)) char_pos = 0;
        char_pos *= TEXT_CHAR_WIDTH; // 5 byte per char

        // draw 1 extra for the space, it's ok if it overflows there because setPixel will catch that
        for(size_t char_x = 0; char_x < TEXT_CHAR_WIDTH + 1; char_x++)
        {
            // this draws a 1 pixel empty column after each character
            uint8_t c = char_x == TEXT_CHAR_WIDTH ? 0 : Font5x7[char_pos + char_x];

            // draw 1 extra to make sure the pixels under each char are cleared too
            for(size_t k = 0; k < TEXT_CHAR_HEIGHT + 1; k++)
            {
                bool on = (c & (1 << k)) != 0;
                setPixel(
                    on ? brightness : 0x00,
                    x_pos + char_x + ((TEXT_CHAR_WIDTH + 1) * i), 
                    y_pos + k
                );
            }
        }
    }
    return len + 1;
}


// draws an image in the specified region
uint16_t Lmcp::drawImage(uint8_t x, uint8_t y, uint16_t width, uint16_t height, uint8_t* data)
{
    for(int y_pos = 0; y_pos < height; y_pos++)
    {
        for(int x_pos = 0; x_pos < width; x_pos++)
        {
            this->setPixel(*data++, x + x_pos, y + y_pos);
        }
    }
    return width * height;
}

uint16_t Lmcp::drawImageRgb(uint8_t x, uint8_t y, uint16_t width, uint16_t height, uint8_t *data)
{
    for(int y_pos = 0; y_pos < height; y_pos++)
    {
        for(int x_pos = 0; x_pos < width; x_pos++)
        {
            uint8_t r = *data++;
            uint8_t g = *data++;
            uint8_t b = *data++;
            this->setPixelRgb(r, g, b, x + x_pos , y + y_pos);
        }
    }
    return width * height;
}

// draw the curent framebuffer on the screen
void Lmcp::writeScreen()
{
}

// set pixel by x/y position
void Lmcp::setPixel(uint8_t val, uint8_t x, uint8_t y)
{
}

void Lmcp::setPixelRgb(uint8_t r, uint8_t g, uint8_t b, uint8_t x, uint8_t y)
{    
}

// clear the whole board
void Lmcp::clear()
{
}
