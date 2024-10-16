    void PngImage::doWriteMetadata(BasicIo& outIo)
    {
        if (!io_->isopen()) throw Error(kerInputDataReadFailed);
        if (!outIo.isopen()) throw Error(kerImageWriteFailed);

#ifdef DEBUG
        std::cout << "Exiv2::PngImage::doWriteMetadata: Writing PNG file " << io_->path() << "\n";
        std::cout << "Exiv2::PngImage::doWriteMetadata: tmp file created " << outIo.path() << "\n";
#endif

        if (!isPngType(*io_, true)) {
            throw Error(kerNoImageInInputData);
        }

        // Write PNG Signature.
        if (outIo.write(pngSignature, 8) != 8) throw Error(kerImageWriteFailed);

        DataBuf cheaderBuf(8);       // Chunk header : 4 bytes (data size) + 4 bytes (chunk type).

        while(!io_->eof())
        {
            // Read chunk header.

            std::memset(cheaderBuf.pData_, 0x00, cheaderBuf.size_);
            long bufRead = io_->read(cheaderBuf.pData_, cheaderBuf.size_);
            if (io_->error()) throw Error(kerFailedToReadImageData);
            if (bufRead != cheaderBuf.size_) throw Error(kerInputDataReadFailed);

            // Decode chunk data length.

            uint32_t dataOffset = getULong(cheaderBuf.pData_, bigEndian);
            if (dataOffset > 0x7FFFFFFF) throw Exiv2::Error(kerFailedToReadImageData);

            // Read whole chunk : Chunk header + Chunk data (not fixed size - can be null) + CRC (4 bytes).

            DataBuf chunkBuf(8 + dataOffset + 4);                     // Chunk header (8 bytes) + Chunk data + CRC (4 bytes).
            memcpy(chunkBuf.pData_, cheaderBuf.pData_, 8);            // Copy header.
            bufRead = io_->read(chunkBuf.pData_ + 8, dataOffset + 4); // Extract chunk data + CRC
            if (io_->error()) throw Error(kerFailedToReadImageData);
            if (bufRead != (long)(dataOffset + 4)) throw Error(kerInputDataReadFailed);

            char szChunk[5];
            memcpy(szChunk,cheaderBuf.pData_ + 4,4);
            szChunk[4]  = 0;

            if (!memcmp(cheaderBuf.pData_ + 4, "IEND", 4))
            {
                // Last chunk found: we write it and done.
#ifdef DEBUG
                std::cout << "Exiv2::PngImage::doWriteMetadata: Write IEND chunk (length: " << dataOffset << ")\n";
#endif
                if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(kerImageWriteFailed);
                return;
            }
            else if (!memcmp(cheaderBuf.pData_ + 4, "IHDR", 4))
            {
#ifdef DEBUG
                std::cout << "Exiv2::PngImage::doWriteMetadata: Write IHDR chunk (length: " << dataOffset << ")\n";
#endif
                if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(kerImageWriteFailed);

                // Write all updated metadata here, just after IHDR.
                if (!comment_.empty())
                {
                    // Update Comment data to a new PNG chunk
                    std::string chunk = PngChunk::makeMetadataChunk(comment_, mdComment);
                    if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size())
                    {
                        throw Error(kerImageWriteFailed);
                    }
                }

                if (exifData_.count() > 0)
                {
                    // Update Exif data to a new PNG chunk
                    Blob blob;
                    ExifParser::encode(blob, littleEndian, exifData_);
                    if (blob.size() > 0)
                    {
                        static const char exifHeader[] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
                        std::string rawExif =   std::string(exifHeader, 6)
                                              + std::string((const char*)&blob[0], blob.size());
                        std::string chunk = PngChunk::makeMetadataChunk(rawExif, mdExif);
                        if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size())
                        {
                            throw Error(kerImageWriteFailed);
                        }
                    }
                }

                if (iptcData_.count() > 0)
                {
                    // Update IPTC data to a new PNG chunk
                    DataBuf newPsData = Photoshop::setIptcIrb(0, 0, iptcData_);
                    if (newPsData.size_ > 0)
                    {
                        std::string rawIptc((const char*)newPsData.pData_, newPsData.size_);
                        std::string chunk = PngChunk::makeMetadataChunk(rawIptc, mdIptc);
                        if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size())
                        {
                            throw Error(kerImageWriteFailed);
                        }
                    }
                }

                if ( iccProfileDefined() ) {
                    DataBuf compressed;
                    if ( zlibToCompressed(iccProfile_.pData_,iccProfile_.size_,compressed) ) {

                        const byte* nullComp = (const byte*) "\0\0";
                        const byte*  type    = (const byte*) "iCCP";
                        const uint32_t nameLength  = static_cast<uint32_t>(profileName_.size());
                        const uint32_t chunkLength = nameLength + 2 + compressed.size_ ;
                        byte     length[4];
                        ul2Data (length,chunkLength,bigEndian);

                        // calculate CRC
                        uLong   tmp = crc32(0L, Z_NULL, 0);
                        tmp         = crc32(tmp, (const Bytef*)type, 4);
                        tmp         = crc32(tmp, (const Bytef*)profileName_.data(), nameLength);
                        tmp         = crc32(tmp, (const Bytef*)nullComp, 2);
                        tmp         = crc32(tmp, (const Bytef*)compressed.pData_,compressed.size_);
                        byte    crc[4];
                        ul2Data(crc, tmp, bigEndian);

                        if( outIo.write(length, 4) != 4
                        ||  outIo.write(type, 4) != 4
                        ||  outIo.write(reinterpret_cast<const byte*>(profileName_.data()), nameLength) != nameLength
                        ||  outIo.write(nullComp,2) != 2
                        ||  outIo.write (compressed.pData_,compressed.size_) != compressed.size_
                        ||  outIo.write(crc,4)            != 4
                        ){
                            throw Error(kerImageWriteFailed);
                        }
#ifdef DEBUG
                        std::cout << "Exiv2::PngImage::doWriteMetadata: build iCCP"
                        << " chunk (length: " << compressed.size_ + chunkLength << ")" << std::endl;
#endif
                    }
                }

                if (writeXmpFromPacket() == false) {
                    if (XmpParser::encode(xmpPacket_, xmpData_) > 1) {
#ifndef SUPPRESS_WARNINGS
                        EXV_ERROR << "Failed to encode XMP metadata.\n";
#endif
                    }
                }
                if (xmpPacket_.size() > 0) {
                    // Update XMP data to a new PNG chunk
                    std::string chunk = PngChunk::makeMetadataChunk(xmpPacket_, mdXmp);
                    if (outIo.write((const byte*)chunk.data(), static_cast<long>(chunk.size())) != (long)chunk.size()) {
                        throw Error(kerImageWriteFailed);
                    }
                }
            }
            else if (!memcmp(cheaderBuf.pData_ + 4, "tEXt", 4) ||
                     !memcmp(cheaderBuf.pData_ + 4, "zTXt", 4) ||
                     !memcmp(cheaderBuf.pData_ + 4, "iTXt", 4) ||
                     !memcmp(cheaderBuf.pData_ + 4, "iCCP", 4))
            {
                DataBuf key = PngChunk::keyTXTChunk(chunkBuf, true);
                if (compare("Raw profile type exif", key, 21) ||
                    compare("Raw profile type APP1", key, 21) ||
                    compare("Raw profile type iptc", key, 21) ||
                    compare("Raw profile type xmp",  key, 20) ||
                    compare("XML:com.adobe.xmp",     key, 17) ||
                    compare("icc",                   key,  3) || // see test/data/imagemagick.png
                    compare("ICC",                   key,  3) ||
                    compare("Description",           key, 11))
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::doWriteMetadata: strip " << szChunk
                              << " chunk (length: " << dataOffset << ")" << std::endl;
#endif
                }
                else
                {
#ifdef DEBUG
                    std::cout << "Exiv2::PngImage::doWriteMetadata: write " << szChunk
                              << " chunk (length: " << dataOffset << ")" << std::endl;
#endif
                    if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(kerImageWriteFailed);
                }
            }
            else
            {
                // Write all others chunk as well.
#ifdef DEBUG
                std::cout << "Exiv2::PngImage::doWriteMetadata:  copy " << szChunk
                          << " chunk (length: " << dataOffset << ")" << std::endl;
#endif
                if (outIo.write(chunkBuf.pData_, chunkBuf.size_) != chunkBuf.size_) throw Error(kerImageWriteFailed);

            }
        }

    } // PngImage::doWriteMetadata