    long WebPImage::getHeaderOffset(byte *data, size_t data_size,
                                    byte *header, size_t header_size) {
        long pos = -1;
        for (size_t i=0; i < data_size - header_size; i++) {
            if (memcmp(header, &data[i], header_size) == 0) {
                pos = (long)i;
                break;
            }
        }
        return pos;
    }