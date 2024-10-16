    bool isPngType(BasicIo& iIo, bool advance)
    {
        if (iIo.error() || iIo.eof()) {
            throw Error(kerInputDataReadFailed);
        }
        const int32_t len = 8;
        byte buf[len];
        iIo.read(buf, len);
        if (iIo.error() || iIo.eof())
        {
            return false;
        }
        int rc = memcmp(buf, pngSignature, 8);
        if (!advance || rc != 0)
        {
            iIo.seek(-len, BasicIo::cur);
        }

        return rc == 0;
    }