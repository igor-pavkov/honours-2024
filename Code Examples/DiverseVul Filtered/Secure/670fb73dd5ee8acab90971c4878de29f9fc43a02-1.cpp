    void TiffImage::readMetadata()
    {
#ifdef DEBUG
        std::cerr << "Reading TIFF file " << io_->path() << "\n";
#endif
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }

        IoCloser closer(*io_);
        // Ensure that this is the correct image type
        if (!isTiffType(*io_, false)) {
            if (io_->error() || io_->eof())
                throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "TIFF");
        }
        clearMetadata();

        ByteOrder bo = TiffParser::decode(exifData_,
                                          iptcData_,
                                          xmpData_,
                                          io_->mmap(),
                                          (uint32_t) io_->size());
        setByteOrder(bo);

        // read profile from the metadata
        Exiv2::ExifKey            key("Exif.Image.InterColorProfile");
        Exiv2::ExifData::iterator pos   = exifData_.findKey(key);
        if ( pos != exifData_.end() ) {
            long size = pos->count() * pos->typeSize();
            if (size == 0) {
                throw Error(kerFailedToReadImageData);
            }
            iccProfile_.alloc(size);
            pos->copy(iccProfile_.pData_,bo);
        }

    }