    uint32_t CiffComponent::writeValueData(Blob& blob, uint32_t offset)
    {
        if (dataLocation() == valueData) {
#ifdef DEBUG
            std::cout << "  Data for tag 0x" << std::hex << tagId()
                      << ", " << std::dec << size_ << " Bytes\n";
#endif
            offset_ = offset;
            append(blob, pData_, size_);
            offset += size_;
            // Pad the value to an even number of bytes
            if (size_ % 2 == 1) {
                blob.push_back(0);
                ++offset;
            }
        }
        return offset;
    } // CiffComponent::writeValueData