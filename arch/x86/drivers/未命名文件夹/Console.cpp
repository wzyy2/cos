#include "Console.h"

#include <arch.h>

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
    Console *ret;
    return ret;
}

bool Console::Destroy()
{

}

Console::Console()
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
        cp = (rt_uint16_t *) (MONO_BUF);
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
        putc(' ');
        putc(' ');
        putc(' ');
        putc(' ');
        putc(' ');
        break;
    default:
        crt_buf_[crt_pos_++] = c;     /* write the character */
        break;
    }

    if (crt_pos_ >= CRT_SIZE)
    {
        int32_t i;
        rt_memcpy(crt_buf_, crt_buf_ + CRT_COLS, (CRT_SIZE - CRT_COLS) << 1);
        for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i++)
            crt_buf_[i] = 0x0700 | ' ';
        crt_pos_ -= CRT_COLS;
    }

    outb(addr_6845_, 14);
    outb(addr_6845_+1, crt_pos_ >> 8);
    outb(addr_6845_, 15);
    outb(addr_6845_+1, crt_pos_);
}

/**
 * This function will write a character to serial an cga
 *
 * @param c the char to write
 */
void Console::putc(int c)
{
    Console::cga_putc(c);
}


err_t Console::init (dev_t dev)
{
    return ERR_OK;
}

err_t Console::open(dev_t dev, uint16_t oflag)
{
    return ERR_OK;
}

err_t Console::close(dev_t dev)
{
    return ERR_OK;
}

err_t Console::control(dev_t dev, uint8_t cmd, void *args)
{
    return ERR_OK;
}

size_t Console::write(dev_t dev, off_t pos, const void * buffer, size_t size)
{
    size_t i = size;
    const char* str = buffer;

    while(i--)
    {
        Console::putc(*str++);
    }

    return size;
}

size_t Console::read(dev_t dev, off_t pos, void* buffer, size_t size)
{
    uint8_t* ptr = buffer;
    err_t err_code = ERR_OK;

    /* interrupt mode Rx */
    while (size)
    {
        rt_base_t level;

        /* disable interrupt */
        level = rt_hw_interrupt_disable();

        if (read_index != save_index)
        {
            /* read a character */
            *ptr++ = rx_buffer[read_index];
            size--;

            /* move to next position */
            read_index ++;
            if (read_index >= CONSOLE_RX_BUFFER_SIZE)
                read_index = 0;
        }
        else
        {
            /* set error code */
            err_code = -RT_EEMPTY;

            /* enable interrupt */
            rt_hw_interrupt_enable(level);
            break;
        }

        /* enable interrupt */
        rt_hw_interrupt_enable(level);
    }

    /* set error code */
    rt_set_errno(err_code);
    return (uint32_t)ptr - (uint32_t)buffer;
}

void Console::isr(int vector)
{

    char c;
    base_t level;

    while(1)
    {
        c = Console::keyboard_getc();

        if(c == 0)
        {
            break;
        }
        else if(c > 0)
        {
            /* disable interrupt */
            level = rt_hw_interrupt_disable();

            /* save character */
            rx_buffer[save_index] = c;
            save_index ++;
            if (save_index >= CONSOLE_RX_BUFFER_SIZE)
                save_index = 0;

            /* if the next position is read index, discard this 'read char' */
            if (save_index == read_index)
            {
                read_index ++;
                if (read_index >= CONSOLE_RX_BUFFER_SIZE)
                    read_index = 0;
            }

            /* enable interrupt */
            rt_hw_interrupt_enable(level);
        }
    }

    /* invoke callback */
    if (console_device.rx_indicate != RT_NULL)
    {
        size_t rx_length;

        /* get rx length */
        rx_length = read_index > save_index ?
                    CONSOLE_RX_BUFFER_SIZE - read_index + save_index :
                    save_index - read_index;

//        rt_kprintf("\r\nrx_length %d\r\n", rx_length);
        if(rx_length > 0)
        {
            console_device.rx_indicate(&console_device, rx_length);
        }
    }
    else
    {
//        rt_kprintf("\r\nconsole_device.rx_indicate == RT_NULL\r\n");
    }
}

/**
 * This function initializes console
 *
 */
void Console::hw_init(void)
{
    Console::cga_init();

    /* install  keyboard isr */
    rt_hw_interrupt_install(INTKEYBOARD, rt_console_isr, RT_NULL);
    rt_hw_interrupt_umask(INTKEYBOARD);

    console_device.type         = RT_Device_Class_Char;
    console_device.rx_indicate  = RT_NULL;
    console_device.tx_complete  = RT_NULL;
    console_device.init         = rt_console_init;
    console_device.open         = rt_console_open;
    console_device.close        = rt_console_close;
    console_device.read         = rt_console_read;
    console_device.write        = rt_console_write;
    console_device.control      = rt_console_control;
    console_device.user_data    = RT_NULL;

    /* register a character device */
    rt_device_register(&console_device,
                              "console",
                              RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM);
}

/**
 * This function is used to display a string on console, normally, it's
 * invoked by rt_kprintf
 *
 * @param str the displayed string
 *
 * Modified:
 *  caoxl 2009-10-14
 *  the name is change to rt_hw_console_output in the v0.3.0
 *
 */
void Console::hw_output(const char* str)
{
    while (*str)
    {
        Console::putc (*str++);
    }
}

/*@}*/
