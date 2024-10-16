    MemIo::Impl::Impl(const byte* data, size_t size)
        : data_(const_cast<byte*>(data)),
          idx_(0),
          size_(size),
          sizeAlloced_(0),
          isMalloced_(false),
          eof_(false)
    {
    }