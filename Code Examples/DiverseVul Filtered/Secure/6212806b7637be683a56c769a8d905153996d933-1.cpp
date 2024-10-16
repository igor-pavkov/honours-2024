    DataLocId CiffComponent::dataLocation(uint16_t tag)
    {
        switch (tag & 0xc000) {
        case 0x0000: return valueData;
        case 0x4000: return directoryData;
        default: throw Error(kerCorruptedMetadata);
        }
    } // CiffComponent::dataLocation