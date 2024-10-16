    uint32_t PsdImage::writeExifData(const ExifData& exifData, BasicIo& out)
    {
        uint32_t resLength = 0;
        byte buf[8];

        if (exifData.count() > 0) {
            Blob blob;
            ByteOrder bo = byteOrder();
            if (bo == invalidByteOrder) {
                bo = littleEndian;
                setByteOrder(bo);
            }
            ExifParser::encode(blob, bo, exifData);

            if (blob.size() > 0) {
#ifdef DEBUG
                std::cerr << std::hex << "write: resourceId: " << kPhotoshopResourceID_ExifInfo << "\n";
                std::cerr << std::dec << "Writing ExifInfo: size: " << blob.size() << "\n";
#endif
                if (out.write(reinterpret_cast<const byte*>(Photoshop::irbId_[0]), 4) != 4) throw Error(kerImageWriteFailed);
                us2Data(buf, kPhotoshopResourceID_ExifInfo, bigEndian);
                if (out.write(buf, 2) != 2) throw Error(kerImageWriteFailed);
                us2Data(buf, 0, bigEndian);                      // NULL resource name
                if (out.write(buf, 2) != 2) throw Error(kerImageWriteFailed);
                ul2Data(buf, static_cast<uint32_t>(blob.size()), bigEndian);
                if (out.write(buf, 4) != 4) throw Error(kerImageWriteFailed);
                // Write encoded Exif data
                if (out.write(&blob[0], static_cast<long>(blob.size())) != static_cast<long>(blob.size())) throw Error(kerImageWriteFailed);
                resLength += static_cast<long>(blob.size()) + 12;
                if (blob.size() & 1)    // even padding
                {
                    buf[0] = 0;
                    if (out.write(buf, 1) != 1) throw Error(kerImageWriteFailed);
                    resLength++;
                }
            }
        }
        return resLength;
    } // PsdImage::writeExifData