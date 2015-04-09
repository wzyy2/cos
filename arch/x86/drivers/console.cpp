#include "console.h"

#include <arch/arch.h>
#include <cos/cos.h>
#include <cos/cosHw.h>
#include <string.h>


#define C(x) (x-'@')

/**
 * @addtogroup x86
 */

/*@{*/

#define NO          0

#define SHIFT       (1 << 0)
#define CTL         (1 << 1)
#define ALT         (1 << 2)

#define CAPSLOCK    (1<<3)
#define NUMLOCK     (1<<4)
#define SCROLLOCK   (1<<5)


Console *Console::self = NULL;

int Console::shiftcode[256];

int Console::togglecode[256];

const char Console::normalmap[256] =
{
    NO,    033,  '1',  '2',  '3',  '4',  '5',  '6',
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
    'b',  'n',  'm',  ',',  '.',  '/',  NO,   '*',
    NO,   ' ',   NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',
};

const char Console::shiftmap[256] =
{
    NO,   033,  '!',  '@',  '#',  '$',  '%',  '^',
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
    'O',  'P',  '{',  '}',  '\n', NO,   'A',  'S',
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',
    '"',  '~',  NO,   '|',  'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  '<',  '>',  '?',  NO,   '*',
    NO,   ' ',   NO,   NO,   NO,   NO,   NO,   NO,
    NO,   NO,   NO,   NO,   NO,   NO,   NO,   '7',
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
    '2',  '3',  '0',  '.',
};


const char Console::ctlmap[256] =
{
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    NO,      NO,      NO,      NO,      NO,      NO,      NO,      NO,
    C('Q'),  C('W'),  C('E'),  C('R'),  C('T'),  C('Y'),  C('U'),  C('I'),
    C('O'),  C('P'),  NO,      NO,      '\r',    NO,      C('A'),  C('S'),
    C('D'),  C('F'),  C('G'),  C('H'),  C('J'),  C('K'),  C('L'),  NO,
    NO,      NO,      NO,      C('\\'), C('Z'),  C('X'),  C('C'),  C('V'),
    C('B'),  C('N'),  C('M'),  NO,      NO,      C('/'),  NO,      NO,
};

const char *Console::charcode[4] =
{
    normalmap,
    shiftmap,
    ctlmap,
    ctlmap,
};

Console *Console::Instance()
{


    if (self == NULL) {
        self = new Console;
        if(self != NULL && get_errno() == ERR_OK){
            /* install  keyboard isr */
            arch_interrupt_install(INTKEYBOARD, (isr_handler_t) Console::isr, NULL);
            arch_interrupt_umask(INTKEYBOARD);
        }else{

        }
    }
    return self;
}

bool Console::Destroy()
{
    delete self;
    self = NULL;
    arch_interrupt_mask(INTKEYBOARD);
}

Console::Console():Device(Device::Device_Class_Char,
                          "console", DEVICE_FLAG_RDWR | DEVICE_FLAG_INT_RX | DEVICE_FLAG_STREAM)
{
    memset(shiftcode, CTL, 29);
    memset(shiftcode + 29, SHIFT, 96);
    memset(shiftcode + 29 + 96, ALT, 56);

    memset(togglecode, CAPSLOCK, 58);
    memset(togglecode + 58, NUMLOCK, 69);
    memset(togglecode + 58 + 69, SCROLLOCK, 70);


}

Console::~Console()
{

}

/**
 * This function get a char from the keyboard
 */
char Console::keyboard_getc(void)
{
    int c;
    uint8_t data;
    static uint32_t shift;

    if ((inb(KBSTATP) & KBS_DIB) == 0)
        return -1;

    data = inb(KBDATAP);

    if (data & 0x80)
    {
        /* key up */
        shift &= ~shiftcode[data&~0x80];
        return 0;
    }

    /* key down */
    shift |= shiftcode[data];
    shift ^= togglecode[data];
    c = charcode[shift&(CTL|SHIFT)][data];

    if (shift&CAPSLOCK)
    {
        if ('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if ('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }
    
    return c;
}

/**
 * This function initializes cga
 *
 */
void Console::cga_init(void)
{
    uint16_t volatile *cp;
    uint16_t was;
    uint32_t pos;

    cp = (uint16_t *) (CGA_BUF);
    was = *cp;
    *cp = (uint16_t) 0xA55A;
    if (*cp != 0xA55A)
    {
        cp = (uint16_t *) (MONO_BUF);
        addr_6845_ = MONO_BASE;
    }
    else
    {
        *cp = was;
        addr_6845_ = CGA_BASE;
    }

    /* Extract cursor location */
    outb(addr_6845_, 14);
    pos = inb(addr_6845_+1) << 8;
    outb(addr_6845_, 15);
    pos |= inb(addr_6845_+1);

    crt_buf_ = (uint16_t *)cp;
    crt_pos_ = pos;
}

/**
 * This function will write a character to cga
 *
 * @param c the char to write
 */
void Console::cga_putc(int c)
{
    /* if no attribute given, then use black on white */
    if (!(c & ~0xff)) c |= 0x0700;

    switch (c & 0xff)
    {
    case '\b':
        if (crt_pos_ > 0)
        {
            crt_pos_--;
            crt_buf_[crt_pos_] = (c&~0xff) | ' ';
        }
        break;
    case '\n':
        crt_pos_ += CRT_COLS;
        /* cascade  */
    case '\r':
        crt_pos_ -= (crt_pos_ % CRT_COLS);
        break;
    case '\t':
        printc(' ');
        printc(' ');
        printc(' ');
        printc(' ');
        printc(' ');
        break;
    default:
        crt_buf_[crt_pos_++] = c;     /* write the character */
        break;
    }

    if (crt_pos_ >= CRT_SIZE)
    {
        int32_t i;
        memcpy(crt_buf_, crt_buf_ + CRT_COLS, (CRT_SIZE - CRT_COLS) << 1);
        for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i++)
            crt_buf_[i] = 0x0700 | ' ';
        crt_pos_ -= CRT_COLS;
    }

    outb(addr_6845_, 14);
    outb(addr_6845_+1, crt_pos_ >> 8);
    outb(addr_6845_, 15);
    outb(addr_6845_+1, crt_pos_);
}

err_t Console::driver_init()
{
    cga_init();
    return ERR_OK;
}

/**
 * This function will write a character to serial an cga
 *
 * @param c the char to write
 */
void Console::printc(int c)
{
    Console::cga_putc(c);
}

size_t Console::driver_write(off_t pos, const void * buffer, size_t size)
{
    size_t i = size;
    const char* str = static_cast<const char*> (buffer);

    while(i--)
    {
        Console::printc(*str++);
    }

    return size;
}

size_t Console::driver_read(off_t pos, void* buffer, size_t size)
{
    uint8_t* ptr = static_cast<uint8_t*> (buffer);
    err_t err_code = ERR_OK;

    /* interrupt mode Rx */
    while (size)
    {
        base_t level;

        /* disable interrupt */
        level = arch_interrupt_disable();

        if (read_index_ != save_index_)
        {
            /* read a character */
            *ptr++ = rx_buffer_[read_index_];
            size--;

            /* move to next position */
            read_index_ ++;
            if (read_index_ >= CONSOLE_RX_BUFFER_SIZE)
                read_index_ = 0;
        }
        else
        {
            /* set error code */
            err_code = -ERR_EMPTY;

            /* enable interrupt */
            arch_interrupt_enable(level);
            break;
        }

        /* enable interrupt */
        arch_interrupt_enable(level);
    }

    /* set error code */
    set_errno(err_code);
    return (uint32_t)ptr - (uint32_t)buffer;
}

void Console::isr(int vector)
{
    char c;
    base_t level;

    Console *con = Console::Instance();

    while(1)
    {
        c = con->keyboard_getc();

        if(c == 0)
        {
            break;
        }
        else if(c > 0)
        {
            /* disable interrupt */
            level = arch_interrupt_disable();

            /* save character */
            con->rx_buffer_[con->save_index_] = c;
            con->save_index_ ++;
            if (con->save_index_ >= CONSOLE_RX_BUFFER_SIZE)
                con->save_index_ = 0;

            /* if the next position is read index, discard this 'read char' */
            if (con->save_index_ == con->read_index_)
            {
                con->read_index_ ++;
                if (con->read_index_ >= CONSOLE_RX_BUFFER_SIZE)
                    con->read_index_ = 0;
            }

            /* enable interrupt */
            arch_interrupt_enable(level);
        }
    }

    /* invoke callback */
    if (con->rx_indicate_ != NULL)
    {
        size_t rx_length;

        /* get rx length */
        rx_length = con->read_index_ > con->save_index_ ?
                    CONSOLE_RX_BUFFER_SIZE - con->read_index_ + con->save_index_ :
                    con->save_index_ - con->read_index_;

        if(rx_length > 0)
        {
            con->rx_indicate_(con, rx_length);
        }
    }
}

/**
 * This function initializes console
 *
 */
void Console_board_init(void)
{
    Console *con = Console::Instance();


}

/*@}*/
