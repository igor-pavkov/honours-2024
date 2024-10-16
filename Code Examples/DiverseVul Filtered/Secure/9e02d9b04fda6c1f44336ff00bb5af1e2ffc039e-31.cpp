    static inline void _swap16Buffer(short *pData, int numWords)
    {
        int i;

        for (i = 0; i < numWords; i ++)
        {
            pData[i] = _swap16(pData[i]);
        }
    }