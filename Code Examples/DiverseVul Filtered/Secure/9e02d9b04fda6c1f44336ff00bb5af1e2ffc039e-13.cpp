    static inline short _swap16(short &wData)
    {
        wData = ((wData >> 8) & 0x00FF) | 
                ((wData << 8) & 0xFF00);
        return wData;
    }