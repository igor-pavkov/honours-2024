    static bool tEXtToDataBuf(const byte* bytes, long length, DataBuf& result)
    {
        static const char* hexdigits = "0123456789ABCDEF";
        static int value[256];
        static bool bFirst = true;
        if (bFirst) {
            for (int i = 0; i < 256; i++)
                value[i] = 0;
            for (int i = 0; i < 16; i++) {
                value[tolower(hexdigits[i])] = i + 1;
                value[toupper(hexdigits[i])] = i + 1;
            }
            bFirst = false;
        }

        // calculate length and allocate result;
        // count: number of \n in the header
        long count = 0;
        // p points to the current position in the array bytes
        const byte* p = bytes;

        // header is '\nsomething\n number\n hex'
        // => increment p until it points to the byte after the last \n
        //    p must stay within bounds of the bytes array!
        while ((count < 3) && (p - bytes < length)) {
            // length is later used for range checks of p => decrement it for each increment of p
            --length;
            if (*p++ == '\n') {
                count++;
            }
        }
        for (long i = 0; i < length; i++)
            if (value[p[i]])
                ++count;
        result.alloc((count + 1) / 2);

        // hex to binary
        count = 0;
        byte* r = result.pData_;
        int n = 0;  // nibble
        for (long i = 0; i < length; i++) {
            if (value[p[i]]) {
                int v = value[p[i]] - 1;
                if (++count % 2)
                    n = v * 16;  // leading digit
                else
                    *r++ = n + v;  // trailing
            }
        }
        return true;
    }