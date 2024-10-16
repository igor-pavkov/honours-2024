    void WebPImage::inject_VP8X(BasicIo& iIo, bool has_xmp,
                                bool has_exif, bool has_alpha,
                                bool has_icc, int width, int height) {
        byte size[4]  = { 0x0A, 0x00, 0x00, 0x00 };
        byte data[10] = { 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00 };
        iIo.write((const byte*)WEBP_CHUNK_HEADER_VP8X, WEBP_TAG_SIZE);
        iIo.write(size, WEBP_TAG_SIZE);

        if (has_alpha) {
            data[0] |= WEBP_VP8X_ALPHA_BIT;
        }

        if (has_icc) {
            data[0] |= WEBP_VP8X_ICC_BIT;
        }

        if (has_xmp) {
            data[0] |= WEBP_VP8X_XMP_BIT;
        }

        if (has_exif) {
            data[0] |= WEBP_VP8X_EXIF_BIT;
        }

        /* set width - stored in 24bits*/
        int w = width - 1;
        data[4] = w & 0xFF;
        data[5] = (w >> 8) & 0xFF;
        data[6] = (w >> 16) & 0xFF;

        /* set height - stored in 24bits */
        int h = height - 1;
        data[7] = h & 0xFF;
        data[8] = (h >> 8) & 0xFF;
        data[9] = (h >> 16) & 0xFF;

        iIo.write(data, 10);

        /* Handle inject an icc profile right after VP8X chunk */
        if (has_icc) {
            byte size_buff[WEBP_TAG_SIZE];
            ul2Data(size_buff, (uint32_t)iccProfile_.size_, littleEndian);
            if (iIo.write((const byte*)WEBP_CHUNK_HEADER_VP8X, WEBP_TAG_SIZE) != WEBP_TAG_SIZE)
                throw Error(kerImageWriteFailed);
            if (iIo.write(size_buff, WEBP_TAG_SIZE) != WEBP_TAG_SIZE)
                throw Error(kerImageWriteFailed);
            if (iIo.write(iccProfile_.pData_, iccProfile_.size_) != iccProfile_.size_)
                throw Error(kerImageWriteFailed);
            if (iIo.tell() % 2) {
                if (iIo.write(&WEBP_PAD_ODD, 1) != 1) throw Error(kerImageWriteFailed);
            }

            has_icc = false;
        }
    }