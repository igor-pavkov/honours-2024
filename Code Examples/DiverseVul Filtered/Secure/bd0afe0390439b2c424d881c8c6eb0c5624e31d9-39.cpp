    MemIo::MemIo(const byte* data, size_t size)
        : p_(new Impl(data, size))
    {
    }