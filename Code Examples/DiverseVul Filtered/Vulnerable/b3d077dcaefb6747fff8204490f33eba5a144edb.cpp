    void CiffDirectory::readDirectory(const byte* pData,
                                      uint32_t    size,
                                      ByteOrder   byteOrder)
    {
        uint32_t o = getULong(pData + size - 4, byteOrder);
        if (size < 2 || o > size-2) throw Error(kerNotACrwImage);
        uint16_t count = getUShort(pData + o, byteOrder);
#ifdef DEBUG
        std::cout << "Directory at offset " << std::dec << o
                  <<", " << count << " entries \n";
#endif
        o += 2;
        for (uint16_t i = 0; i < count; ++i) {
            if (o + 10 > size) throw Error(kerNotACrwImage);
            uint16_t tag = getUShort(pData + o, byteOrder);
            CiffComponent::AutoPtr m;
            switch (CiffComponent::typeId(tag)) {
            case directory: m = CiffComponent::AutoPtr(new CiffDirectory); break;
            default: m = CiffComponent::AutoPtr(new CiffEntry); break;
            }
            m->setDir(this->tag());
            m->read(pData, size, o, byteOrder);
            add(m);
            o += 10;
        }
    }  // CiffDirectory::readDirectory