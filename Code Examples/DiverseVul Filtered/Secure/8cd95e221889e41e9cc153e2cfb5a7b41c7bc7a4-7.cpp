    void readChunk(DataBuf& buffer, BasicIo& io)
    {
#ifdef DEBUG
        std::cout << "Exiv2::PngImage::readMetadata: Position: " << io.tell() << std::endl;
#endif
        long bufRead = io.read(buffer.pData_, buffer.size_);
        if (io.error()) {
            throw Error(kerFailedToReadImageData);
        }
        if (bufRead != buffer.size_) {
            throw Error(kerInputDataReadFailed);
        }
    }