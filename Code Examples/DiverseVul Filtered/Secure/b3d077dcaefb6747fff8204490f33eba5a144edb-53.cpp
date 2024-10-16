    void CiffEntry::doDecode(Image& image, ByteOrder byteOrder) const
    {
        CrwMap::decode(*this, image, byteOrder);
    } // CiffEntry::doDecode