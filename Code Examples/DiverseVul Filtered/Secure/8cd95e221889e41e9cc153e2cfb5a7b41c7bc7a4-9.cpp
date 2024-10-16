    inline bool compare(const char* str, const Exiv2::DataBuf& buf, size_t length)
    {
        // str & length should compile time constants => only running this in DEBUG mode is ok
        assert(strlen(str) <= length);
        return memcmp(str, buf.pData_, std::min(static_cast<long>(length), buf.size_)) == 0;
    }