    uint32_t CiffEntry::doWrite(Blob&     blob,
                                ByteOrder /*byteOrder*/,
                                uint32_t  offset)
    {
        return writeValueData(blob, offset);
    } // CiffEntry::doWrite