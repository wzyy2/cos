#ifndef _VIDEO_H
#define _VIDEO_H

class Video{
    public:
        Video();
        ~Video();
        void clear();
        void write(const char *cp);
        void put(char c);
    private:
        unsigned short *videomem;
        unsigned int off;
        unsigned int pos;
};
#endif
