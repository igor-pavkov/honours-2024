    DataBuf packIfdId(const ExifData& exifData,
                            IfdId     ifdId,
                            ByteOrder byteOrder)
    {
        const uint16_t size = 1024;
        DataBuf buf(size);
        std::memset(buf.pData_, 0x0, buf.size_);

        uint16_t len = 0;
        const ExifData::const_iterator b = exifData.begin();
        const ExifData::const_iterator e = exifData.end();
        for (ExifData::const_iterator i = b; i != e; ++i) {
            if (i->ifdId() != ifdId) continue;
            const uint16_t s = i->tag()*2 + static_cast<uint16_t>(i->size());
            assert(s <= size);
            if (len < s) len = s;
            i->copy(buf.pData_ + i->tag()*2, byteOrder);
        }
        // Round the size to make it even.
        buf.size_ = len + len%2;
        return buf;
    }