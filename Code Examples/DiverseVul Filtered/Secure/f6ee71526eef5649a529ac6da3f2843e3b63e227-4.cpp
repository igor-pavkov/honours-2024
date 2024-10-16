    static std::string toAscii(long n)
    {
        const char* p = (const char*) &n;
        std::string result;
        bool bBigEndian = isBigEndian();
        for ( int i = 0 ; i < 4 ; i++) {
            result += p[ bBigEndian ? i : (3-i) ];
        }
        return result;
    }