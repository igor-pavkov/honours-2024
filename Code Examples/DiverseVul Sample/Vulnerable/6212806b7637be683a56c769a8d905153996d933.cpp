    DataLocId CiffComponent::dataLocation(uint16_t tag)
    {
        DataLocId di = invalidDataLocId;
        switch (tag & 0xc000) {
        case 0x0000: di = valueData; break;
        case 0x4000: di = directoryData; break;
        }
        return di;
    } // CiffComponent::dataLocation