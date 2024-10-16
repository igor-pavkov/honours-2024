    bool isBigTiffType(BasicIo& io, bool advance)
    {
        const long pos = io.tell();
        const Header header = readHeader(io);
        const bool valid = header.isValid();

        if (valid == false || advance == false)
            io.seek(pos, BasicIo::beg);

        return valid;
    }