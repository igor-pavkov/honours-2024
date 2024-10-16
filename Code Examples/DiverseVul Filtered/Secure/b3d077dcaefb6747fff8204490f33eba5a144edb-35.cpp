    void CiffHeader::read(const byte* pData, uint32_t size)
    {
        if (size < 14) throw Error(kerNotACrwImage);

        if (pData[0] == 'I' && pData[0] == pData[1]) {
            byteOrder_ = littleEndian;
        }
        else if (pData[0] == 'M' && pData[0] == pData[1]) {
            byteOrder_ = bigEndian;
        }
        else {
            throw Error(kerNotACrwImage);
        }
        offset_ = getULong(pData + 2, byteOrder_);
        if (offset_ < 14 || offset_ > size) throw Error(kerNotACrwImage);
        if (std::memcmp(pData + 6, signature(), 8) != 0) {
            throw Error(kerNotACrwImage);
        }

        delete pPadding_;
        pPadding_ = new byte[offset_ - 14];
        padded_ = offset_ - 14;
        std::memcpy(pPadding_, pData + 14, padded_);

        pRootDir_ = new CiffDirectory;
        pRootDir_->readDirectory(pData + offset_, size - offset_, byteOrder_);
    } // CiffHeader::read