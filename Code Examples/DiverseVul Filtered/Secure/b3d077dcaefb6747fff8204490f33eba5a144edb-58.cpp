    void CiffHeader::decode(Image& image) const
    {
        // Nothing to decode from the header itself, just add correct byte order
        if (pRootDir_) pRootDir_->decode(image, byteOrder_);
    } // CiffHeader::decode