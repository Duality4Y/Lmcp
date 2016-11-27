#ifndef _LMCP_H_
#define _LMCP_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <iostream>

#include <linkedlist.h>

#define UNUSED(X) ((void)X)

class Lmcp
{
public:
    typedef struct __attribute__((packed, aligned(sizeof(uint32_t))))
    {
        uint32_t magic;
        uint32_t version;
        uint32_t checksum;
        uint32_t length;
        uint32_t command;
        uint8_t *data;
    } header_t;

    typedef struct 
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } color_t;

    enum
    {
        WRITEOUT = 0x01,
        CLEAR = 0x02,
        DRAW_IMAGE_RECT = 0x11,
        WRITE_TEXT = 0x20,
        WRITE_TEXT_ABS = 0x21,
        SET_COLOR = 0x32
    };

    LinkedList<header_t> send_queue;
    LinkedList<header_t> receive_queue;

    Lmcp(uint32_t, uint32_t, uint8_t);
    /* builds a packet with a valid header from a data stream. */
    header_t build_packet(uint8_t *, uint32_t, uint8_t *, uint32_t);
    /* build a header from the arguments. */
    header_t build_header(uint32_t, uint32_t, uint32_t);
    /* reads in a header corresponding to what is found in the data stream */
    header_t read_header(uint8_t *);
    /* returns a list of header positions. */
    LinkedList<size_t> find_headers(uint8_t *, uint32_t);
    /* pass data to this function for processing.*/
    bool process(uint8_t *, size_t);

    /* override to implement writing data out to the matrix. */
    virtual void write_buffer();
    /* override to clear a implementation specific surface. */
    virtual void clear();
    /* override to set a pixel on implementation specific drawing surface. */
    virtual void set_pixel(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

#ifndef DEBUG
protected:
#endif

    uint32_t field_width;
    uint32_t field_height;
    uint8_t bitdepth;
    uint8_t color[3];

    // header magic
    const uint8_t magic_bytes[4] = {'L', 'M', 'C', 'P'};
    // holds the magic number.
    uint32_t magic;
    // holds version of protocol.
    const static uint32_t version = 0x01;

    /* draw a image rect in the framebuffer. */
    uint32_t draw_image(uint8_t *);
    /* draw text line based. */
    uint32_t draw_text(uint8_t *);
    /* set a drawing color, for functions that don't have or use RGB data.*/
    uint32_t set_color(uint8_t *);

    /* calculate checksum*/
    uint32_t csum(uint8_t *, uint32_t);
};

#endif
