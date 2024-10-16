    void WebPImage::decodeChunks(long filesize)
    {
        DataBuf   chunkId(5);
        byte      size_buff[WEBP_TAG_SIZE];
        bool      has_canvas_data = false;

#ifdef DEBUG
        std::cout << "Reading metadata" << std::endl;
#endif

        chunkId.pData_[4] = '\0' ;
        while (!io_->eof() && io_->tell() < filesize) {
            io_->readOrThrow(chunkId.pData_, WEBP_TAG_SIZE);
            io_->readOrThrow(size_buff, WEBP_TAG_SIZE);

            const uint32_t size_u32 = Exiv2::getULong(size_buff, littleEndian);

            // Check that `size_u32` is safe to cast to `long`. The `#if`
            // is needed because this check is redundant on platforms where
            // `long` is 64 bits.  On those platforms, it causes the build
            // to fail due to a compiler warning in clang.
#if LONG_MAX < UINT_MAX
            enforce(size_u32 <= static_cast<size_t>(std::numeric_limits<long>::max()),
                    Exiv2::kerCorruptedMetadata);
#endif
            const long size = static_cast<long>(size_u32);

            // Check that `size` is within bounds.
            enforce(io_->tell() <= filesize, Exiv2::kerCorruptedMetadata);
            enforce(size <= (filesize - io_->tell()), Exiv2::kerCorruptedMetadata);

            DataBuf payload(size);

            if (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_VP8X) && !has_canvas_data) {
                enforce(size >= 10, Exiv2::kerCorruptedMetadata);

                has_canvas_data = true;
                byte size_buf[WEBP_TAG_SIZE];

                io_->readOrThrow(payload.pData_, payload.size_);

                // Fetch width
                memcpy(&size_buf, &payload.pData_[4], 3);
                size_buf[3] = 0;
                pixelWidth_ = Exiv2::getULong(size_buf, littleEndian) + 1;

                // Fetch height
                memcpy(&size_buf, &payload.pData_[7], 3);
                size_buf[3] = 0;
                pixelHeight_ = Exiv2::getULong(size_buf, littleEndian) + 1;
            } else if (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_VP8) && !has_canvas_data) {
                enforce(size >= 10, Exiv2::kerCorruptedMetadata);

                has_canvas_data = true;
                io_->readOrThrow(payload.pData_, payload.size_);
                byte size_buf[WEBP_TAG_SIZE];

                // Fetch width""
                memcpy(&size_buf, &payload.pData_[6], 2);
                size_buf[2] = 0;
                size_buf[3] = 0;
                pixelWidth_ = Exiv2::getULong(size_buf, littleEndian) & 0x3fff;

                // Fetch height
                memcpy(&size_buf, &payload.pData_[8], 2);
                size_buf[2] = 0;
                size_buf[3] = 0;
                pixelHeight_ = Exiv2::getULong(size_buf, littleEndian) & 0x3fff;
            } else if (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_VP8L) && !has_canvas_data) {
                enforce(size >= 5, Exiv2::kerCorruptedMetadata);

                has_canvas_data = true;
                byte size_buf_w[2];
                byte size_buf_h[3];

                io_->readOrThrow(payload.pData_, payload.size_);

                // Fetch width
                memcpy(&size_buf_w, &payload.pData_[1], 2);
                size_buf_w[1] &= 0x3F;
                pixelWidth_ = Exiv2::getUShort(size_buf_w, littleEndian) + 1;

                // Fetch height
                memcpy(&size_buf_h, &payload.pData_[2], 3);
                size_buf_h[0] = ((size_buf_h[0] >> 6) & 0x3) | ((size_buf_h[1]  & 0x3F) << 0x2);
                size_buf_h[1] = ((size_buf_h[1] >> 6) & 0x3) | ((size_buf_h[2] & 0xF) << 0x2);
                pixelHeight_ = Exiv2::getUShort(size_buf_h, littleEndian) + 1;
            } else if (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_ANMF) && !has_canvas_data) {
                enforce(size >= 12, Exiv2::kerCorruptedMetadata);

                has_canvas_data = true;
                byte size_buf[WEBP_TAG_SIZE];

                io_->readOrThrow(payload.pData_, payload.size_);

                // Fetch width
                memcpy(&size_buf, &payload.pData_[6], 3);
                size_buf[3] = 0;
                pixelWidth_ = Exiv2::getULong(size_buf, littleEndian) + 1;

                // Fetch height
                memcpy(&size_buf, &payload.pData_[9], 3);
                size_buf[3] = 0;
                pixelHeight_ = Exiv2::getULong(size_buf, littleEndian) + 1;
            } else if (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_ICCP)) {
                io_->readOrThrow(payload.pData_, payload.size_);
                this->setIccProfile(payload);
            } else if (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_EXIF)) {
                io_->readOrThrow(payload.pData_, payload.size_);

                // 4 meaningful bytes + 2 padding bytes
                byte  exifLongHeader[]   = { 0xFF, 0x01, 0xFF, 0xE1, 0x00, 0x00 };
                byte  exifShortHeader[]  = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
                byte  exifTiffLEHeader[] = { 0x49, 0x49, 0x2A };       // "MM*"
                byte  exifTiffBEHeader[] = { 0x4D, 0x4D, 0x00, 0x2A }; // "II\0*"
                byte* rawExifData = nullptr;
                long  offset = 0;
                bool  s_header = false;
                bool  le_header = false;
                bool  be_header = false;
                long  pos = getHeaderOffset (payload.pData_, (long)payload.size_, (byte*)&exifLongHeader, 4);

                if (pos == -1) {
                    pos = getHeaderOffset (payload.pData_, (long)payload.size_, (byte*)&exifLongHeader, 6);
                    if (pos != -1) {
                        s_header = true;
                    }
                }
                if (pos == -1) {
                    pos = getHeaderOffset (payload.pData_, (long)payload.size_, (byte*)&exifTiffLEHeader, 3);
                    if (pos != -1) {
                        le_header = true;
                    }
                }
                if (pos == -1) {
                    pos = getHeaderOffset (payload.pData_, (long)payload.size_, (byte*)&exifTiffBEHeader, 4);
                    if (pos != -1) {
                        be_header = true;
                    }
                }

                if (s_header) {
                    offset += 6;
                }
                if (be_header || le_header) {
                    offset += 12;
                }

                const uint16_t sizePayload = (uint16_t)(payload.size_ + offset);
                rawExifData = (byte*)malloc(sizePayload);

                byte  sizeBuff[2];
                if (s_header) {
                    us2Data(sizeBuff, (sizePayload - 6), bigEndian);
                    memcpy(rawExifData, (char*)&exifLongHeader, 4);
                    memcpy(rawExifData + 4, (char*)&sizeBuff, 2);
                }

                if (be_header || le_header) {
                    us2Data(sizeBuff, (sizePayload - 6), bigEndian);
                    memcpy(rawExifData, (char*)&exifLongHeader, 4);
                    memcpy(rawExifData + 4, (char*)&sizeBuff, 2);
                    memcpy(rawExifData + 6, (char*)&exifShortHeader, 6);
                }

                memcpy(rawExifData + offset, payload.pData_, payload.size_);

#ifdef DEBUG
                std::cout << "Display Hex Dump [size:" << (unsigned long)sizePayload << "]" << std::endl;
                std::cout << Internal::binaryToHex(rawExifData, sizePayload);
#endif

                if (pos != -1) {
                    XmpData  xmpData;
                    ByteOrder bo = ExifParser::decode(exifData_,
                                                      payload.pData_ + pos,
                                                      static_cast<uint32_t>(payload.size_ - (size_t)pos));
                    setByteOrder(bo);
                }
                else
                {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode Exif metadata." << std::endl;
#endif
                    exifData_.clear();
                }

                if (rawExifData)
                    free(rawExifData);
            } else if (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_XMP)) {
                io_->readOrThrow(payload.pData_, payload.size_);
                xmpPacket_.assign(reinterpret_cast<char*>(payload.pData_), payload.size_);
                if (xmpPacket_.size() > 0 && XmpParser::decode(xmpData_, xmpPacket_)) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode XMP metadata." << std::endl;
#endif
                } else {
#ifdef DEBUG
                    std::cout << "Display Hex Dump [size:" << (unsigned long)payload.size_ << "]" << std::endl;
                    std::cout << Internal::binaryToHex(payload.pData_, payload.size_);
#endif
                }
            } else {
                io_->seek(size, BasicIo::cur);
            }

            if ( io_->tell() % 2 ) io_->seek(+1, BasicIo::cur);
        }
    }