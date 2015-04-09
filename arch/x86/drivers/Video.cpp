#include "Video.h"

Video::Video(){
    pos = 0;
    off = 0;
    videomem = (unsigned short*)0xb8000;
}

Video::~Video(){}

void Video::clear(){
    unsigned int i;
    for(i=0;i<(80*25);i++){
        videomem[i] = (unsigned short)' '|0x0700;
    }
    pos = 0;
    off = 0;
}

void Video::write(const char *cp){
    char *ch;
    ch = (char *)cp;
    for(ch=(char *)cp;*ch;ch++){
        put(*ch);
    }
}

void Video::put(char c){
    if(pos>=80){
        pos = 0;
        off+=80;
    }
    if(off>=(80*25)){
        clear();
    }

    videomem[off+pos] = (unsigned short)c|0x0700;
    pos++;
}
