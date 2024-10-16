    void CiffComponent::setValue(DataBuf buf)
    {
        if (isAllocated_) {
            delete pData_;
            pData_ = 0;
            size_ = 0;
        }
        isAllocated_ = true;
        std::pair<byte *, long> p = buf.release();
        pData_ = p.first;
        size_  = p.second;
        if (size_ > 8 && dataLocation() == directoryData) {
            tag_ &= 0x3fff;
        }
    } // CiffComponent::setValue