    bool isPsdType(BasicIo& iIo, bool advance)
    {
        const int32_t len = 6;
        const unsigned char PsdHeader[6] = { '8', 'B', 'P', 'S', 0, 1 };
        byte buf[len];
        iIo.read(buf, len);
        if (iIo.error() || iIo.eof())
        {
            return false;
        }
        bool matched = (memcmp(buf, PsdHeader, len) == 0);
        if (!advance || !matched)
        {
            iIo.seek(-len, BasicIo::cur);
        }

        return matched;
    }