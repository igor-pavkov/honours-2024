    void CiffHeader::write(Blob& blob) const
    {
        assert(   byteOrder_ == littleEndian
               || byteOrder_ == bigEndian);
        if (byteOrder_ == littleEndian) {
            blob.push_back('I');
            blob.push_back('I');
        }
        else {
            blob.push_back('M');
            blob.push_back('M');
        }
        uint32_t o = 2;
        byte buf[4];
        ul2Data(buf, offset_, byteOrder_);
        append(blob, buf, 4);
        o += 4;
        append(blob, reinterpret_cast<const byte*>(signature_), 8);
        o += 8;
        // Pad as needed
        if (pPadding_) {
            assert(padded_ == offset_ - o);
            append(blob, pPadding_, padded_);
        }
        else {
            for (uint32_t i = o; i < offset_; ++i) {
                blob.push_back(0);
                ++o;
            }
        }
        if (pRootDir_) {
            pRootDir_->write(blob, byteOrder_, offset_);
        }
    }