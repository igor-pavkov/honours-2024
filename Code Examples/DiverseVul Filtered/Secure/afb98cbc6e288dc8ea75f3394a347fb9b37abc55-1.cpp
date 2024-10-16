    void TiffImage::printStructure(std::ostream& out, Exiv2::PrintStructureOption option,int depth)
    {
        if (io_->open() != 0) throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        // Ensure that this is the correct image type
        if ( imageType() == ImageType::none )
        if (!isTiffType(*io_, false)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAJpeg);
        }

        io_->seek(0,BasicIo::beg);

        printTiffStructure(io(),out,option,depth-1);
    }