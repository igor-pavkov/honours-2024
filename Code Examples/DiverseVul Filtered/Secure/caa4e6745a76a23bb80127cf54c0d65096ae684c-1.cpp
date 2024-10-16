    void WebPImage::printStructure(std::ostream& out, PrintStructureOption option,int depth)
    {
        if (io_->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io_->path(), strError());
        }
        // Ensure this is the correct image type
        if (!isWebPType(*io_, true)) {
            if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
            throw Error(kerNotAnImage, "WEBP");
        }

        bool bPrint  = option==kpsBasic || option==kpsRecursive;
        if ( bPrint || option == kpsXMP || option == kpsIccProfile || option == kpsIptcErase ) {
            byte      data [WEBP_TAG_SIZE * 2];
            io_->read(data, WEBP_TAG_SIZE * 2);
            uint64_t filesize = Exiv2::getULong(data + WEBP_TAG_SIZE, littleEndian);
            DataBuf  chunkId(5)      ;
            chunkId.pData_[4] = '\0' ;

            if ( bPrint ) {
                out << Internal::indent(depth)
                << "STRUCTURE OF WEBP FILE: "
                << io().path()
                << std::endl;
                out << Internal::indent(depth)
                << Internal::stringFormat(" Chunk |   Length |   Offset | Payload")
                << std::endl;
            }

            io_->seek(0,BasicIo::beg); // rewind
            while ( !io_->eof() && (uint64_t) io_->tell() < filesize) {
                uint64_t offset = (uint64_t) io_->tell();
                byte     size_buff[WEBP_TAG_SIZE];
                io_->read(chunkId.pData_, WEBP_TAG_SIZE);
                io_->read(size_buff, WEBP_TAG_SIZE);
                long size = Exiv2::getULong(size_buff, littleEndian);
                DataBuf payload(offset?size:WEBP_TAG_SIZE); // header is different from chunks
                io_->read(payload.pData_, payload.size_);

                if ( bPrint ) {
                    out << Internal::indent(depth)
                    << Internal::stringFormat("  %s | %8u | %8u | ", (const char*)chunkId.pData_,(uint32_t)size,(uint32_t)offset)
                    << Internal::binaryToString(makeSlice(payload, 0, payload.size_ > 32 ? 32 : payload.size_))
                    << std::endl;
                }

                if ( equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_EXIF) && option==kpsRecursive ) {
                    // create memio object with the payload, then print the structure
                    BasicIo::AutoPtr p = BasicIo::AutoPtr(new MemIo(payload.pData_,payload.size_));
                    printTiffStructure(*p,out,option,depth);
                }

                bool bPrintPayload = (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_XMP) && option==kpsXMP)
                                  || (equalsWebPTag(chunkId, WEBP_CHUNK_HEADER_ICCP) && option==kpsIccProfile)
                                   ;
                if ( bPrintPayload ) {
                    out.write((const char*) payload.pData_,payload.size_);
                }

                if ( offset && io_->tell() % 2 ) io_->seek(+1, BasicIo::cur); // skip padding byte on sub-chunks
            }
        }
    }