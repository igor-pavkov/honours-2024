    void CiffComponent::writeDirEntry(Blob& blob, ByteOrder byteOrder) const
    {
#ifdef DEBUG
        std::cout << "  Directory entry for tag 0x"
                  << std::hex << tagId() << " (0x" << tag()
                  << "), " << std::dec << size_
                  << " Bytes, Offset is " << offset_ << "\n";
#endif
        byte buf[4];

        DataLocId dl = dataLocation();
        assert(dl == directoryData || dl == valueData);

        if (dl == valueData) {
            us2Data(buf, tag_, byteOrder);
            append(blob, buf, 2);

            ul2Data(buf, size_, byteOrder);
            append(blob, buf, 4);

            ul2Data(buf, offset_, byteOrder);
            append(blob, buf, 4);
        }

        if (dl == directoryData) {
            // Only 8 bytes fit in the directory entry
            assert(size_ <= 8);

            us2Data(buf, tag_, byteOrder);
            append(blob, buf, 2);
            // Copy value instead of size and offset
            append(blob, pData_, size_);
            // Pad with 0s
            for (uint32_t i = size_; i < 8; ++i) {
                blob.push_back(0);
            }
        }
    } // CiffComponent::writeDirEntry