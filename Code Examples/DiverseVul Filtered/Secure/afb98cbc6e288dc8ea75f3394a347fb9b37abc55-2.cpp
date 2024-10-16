    void TiffImage::writeMetadata()
    {
#ifdef DEBUG
        std::cerr << "Writing TIFF file " << io_->path() << "\n";
#endif
        ByteOrder bo = byteOrder();
        byte* pData = 0;
        long size = 0;
        IoCloser closer(*io_);
        if (io_->open() == 0) {
            // Ensure that this is the correct image type
            if (isTiffType(*io_, false)) {
                pData = io_->mmap(true);
                size = (long) io_->size();
                TiffHeader tiffHeader;
                if (0 == tiffHeader.read(pData, 8)) {
                    bo = tiffHeader.byteOrder();
                }
            }
        }
        if (bo == invalidByteOrder) {
            bo = littleEndian;
        }
        setByteOrder(bo);

        // fixup ICC profile
        Exiv2::ExifKey            key("Exif.Image.InterColorProfile");
        Exiv2::ExifData::iterator pos   = exifData_.findKey(key);
        bool                      found = pos != exifData_.end();
        if ( iccProfileDefined() ) {
            Exiv2::DataValue value(iccProfile_.pData_,iccProfile_.size_);
            if ( found ) pos->setValue(&value);
            else     exifData_.add(key,&value);
        } else {
            if ( found ) exifData_.erase(pos);
        }

        // set usePacket to influence TiffEncoder::encodeXmp() called by TiffVisitor.encode()
        xmpData().usePacket(writeXmpFromPacket());

        TiffParser::encode(*io_, pData, size, bo, exifData_, iptcData_, xmpData_); // may throw
    } // TiffImage::writeMetadata