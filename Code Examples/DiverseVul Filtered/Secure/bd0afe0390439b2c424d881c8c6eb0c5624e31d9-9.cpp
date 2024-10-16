    DataBuf RemoteIo::read(long rcount)
    {
        DataBuf buf(rcount);
        size_t readCount = read(buf.pData_, buf.size_);
        buf.size_ = readCount;
        return buf;
    }