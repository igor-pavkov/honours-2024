    uint32_t CiffComponent::write(Blob&     blob,
                                  ByteOrder byteOrder,
                                  uint32_t  offset)
    {
        return doWrite(blob, byteOrder, offset);
    }