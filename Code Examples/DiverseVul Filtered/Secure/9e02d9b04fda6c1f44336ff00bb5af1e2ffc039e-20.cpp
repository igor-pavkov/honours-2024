    static inline int _swap32(int &dwData)
    {
        dwData = ((dwData >> 24) & 0x000000FF) | 
               ((dwData >> 8)  & 0x0000FF00) | 
               ((dwData << 8)  & 0x00FF0000) | 
               ((dwData << 24) & 0xFF000000);
        return dwData;
    }   