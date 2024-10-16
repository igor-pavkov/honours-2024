    void PsdImage::doWriteMetadata(BasicIo& outIo)
    {
        if (!io_->isopen()) throw Error(kerInputDataReadFailed);
        if (!outIo.isopen()) throw Error(kerImageWriteFailed);

#ifdef DEBUG
        std::cout << "Exiv2::PsdImage::doWriteMetadata: Writing PSD file " << io_->path() << "\n";
        std::cout << "Exiv2::PsdImage::doWriteMetadata: tmp file created " << outIo.path() << "\n";
#endif

        // Ensure that this is the correct image type
        if (!isPsdType(*io_, true)) {
            if (io_->error() || io_->eof()) throw Error(kerInputDataReadFailed);
            throw Error(kerNoImageInInputData);
        }

        io_->seek(0, BasicIo::beg);    // rewind

        DataBuf lbuf(4096);
        byte buf[8];

        // Get Photoshop header from original file
        byte psd_head[26];
        if (io_->read(psd_head, 26) != 26) throw Error(kerNotAnImage, "Photoshop");

        // Write Photoshop header data out to new PSD file
        if (outIo.write(psd_head, 26) != 26) throw Error(kerImageWriteFailed);

        // Read colorDataLength from original PSD
        if (io_->read(buf, 4) != 4) throw Error(kerNotAnImage, "Photoshop");

        uint32_t colorDataLength = getULong(buf, bigEndian);

        // Write colorDataLength
        ul2Data(buf, colorDataLength, bigEndian);
        if (outIo.write(buf, 4) != 4) throw Error(kerImageWriteFailed);
#ifdef DEBUG
        std::cerr << std::dec << "colorDataLength: " << colorDataLength << "\n";
#endif
        // Copy colorData
        uint32_t readTotal = 0;
        long toRead = 0;
        while (readTotal < colorDataLength) {
            toRead =   static_cast<long>(colorDataLength - readTotal) < lbuf.size_
                     ? static_cast<long>(colorDataLength - readTotal) : lbuf.size_;
            if (io_->read(lbuf.pData_, toRead) != toRead) throw Error(kerNotAnImage, "Photoshop");
            readTotal += toRead;
            if (outIo.write(lbuf.pData_, toRead) != toRead) throw Error(kerImageWriteFailed);
        }
        if (outIo.error()) throw Error(kerImageWriteFailed);

        uint32_t resLenOffset = io_->tell();  // remember for later update

        // Read length of all resource blocks from original PSD
        if (io_->read(buf, 4) != 4) throw Error(kerNotAnImage, "Photoshop");

        uint32_t oldResLength = getULong(buf, bigEndian);
        uint32_t newResLength = 0;

        // Write oldResLength (will be updated later)
        ul2Data(buf, oldResLength, bigEndian);
        if (outIo.write(buf, 4) != 4) throw Error(kerImageWriteFailed);

#ifdef DEBUG
        std::cerr << std::dec << "oldResLength: " << oldResLength << "\n";
#endif

        // Iterate over original resource blocks.
        // Replace or insert IPTC, EXIF and XMP
        // Original resource blocks assumed to be sorted ASC

        bool iptcDone = false;
        bool exifDone = false;
        bool xmpDone = false;
        while (oldResLength > 0) {
            if (io_->read(buf, 8) != 8) throw Error(kerNotAnImage, "Photoshop");

            // read resource type and ID
            uint32_t resourceType = getULong(buf, bigEndian);

            if (!Photoshop::isIrb(buf, 4))
            {
                throw Error(kerNotAnImage, "Photoshop"); // bad resource type
            }
            uint16_t resourceId = getUShort(buf + 4, bigEndian);
            uint32_t resourceNameLength = buf[6];
            uint32_t adjResourceNameLen = resourceNameLength & ~1;
            unsigned char resourceNameFirstChar = buf[7];

            // read rest of resource name, plus any padding
            DataBuf resName(256);
            if (   io_->read(resName.pData_, adjResourceNameLen)
                != static_cast<long>(adjResourceNameLen)) throw Error(kerNotAnImage, "Photoshop");

            // read resource size (actual length w/o padding!)
            if (io_->read(buf, 4) != 4) throw Error(kerNotAnImage, "Photoshop");

            uint32_t resourceSize = getULong(buf, bigEndian);
            uint32_t pResourceSize = (resourceSize + 1) & ~1;    // padded resource size
            uint32_t curOffset = io_->tell();

            // Write IPTC_NAA resource block
            if ((resourceId == kPhotoshopResourceID_IPTC_NAA  ||
                 resourceId >  kPhotoshopResourceID_IPTC_NAA) && iptcDone == false) {
                newResLength += writeIptcData(iptcData_, outIo);
                iptcDone = true;
            }

            // Write ExifInfo resource block
            else if ((resourceId == kPhotoshopResourceID_ExifInfo  ||
                      resourceId >  kPhotoshopResourceID_ExifInfo) && exifDone == false) {
                newResLength += writeExifData(exifData_, outIo);
                exifDone = true;
            }

            // Write XMPpacket resource block
            else if ((resourceId == kPhotoshopResourceID_XMPPacket  ||
                      resourceId >  kPhotoshopResourceID_XMPPacket) && xmpDone == false) {
                newResLength += writeXmpData(xmpData_, outIo);
                xmpDone = true;
            }

            // Copy all other resource blocks
            if (   resourceId != kPhotoshopResourceID_IPTC_NAA
                && resourceId != kPhotoshopResourceID_ExifInfo
                && resourceId != kPhotoshopResourceID_XMPPacket) {
#ifdef DEBUG
                std::cerr << std::hex << "copy : resourceType: " << resourceType << "\n";
                std::cerr << std::hex << "copy : resourceId: " << resourceId << "\n";
                std::cerr << std::dec;
#endif
                // Copy resource block to new PSD file
                ul2Data(buf, resourceType, bigEndian);
                if (outIo.write(buf, 4) != 4) throw Error(kerImageWriteFailed);
                us2Data(buf, resourceId, bigEndian);
                if (outIo.write(buf, 2) != 2) throw Error(kerImageWriteFailed);
                // Write resource name as Pascal string
                buf[0] = resourceNameLength & 0x00ff;
                if (outIo.write(buf, 1) != 1) throw Error(kerImageWriteFailed);
                buf[0] = resourceNameFirstChar;
                if (outIo.write(buf, 1) != 1) throw Error(kerImageWriteFailed);
                if (   outIo.write(resName.pData_, adjResourceNameLen)
                    != static_cast<long>(adjResourceNameLen)) throw Error(kerImageWriteFailed);
                ul2Data(buf, resourceSize, bigEndian);
                if (outIo.write(buf, 4) != 4) throw Error(kerImageWriteFailed);

                readTotal = 0;
                toRead = 0;
                while (readTotal < pResourceSize) {
                    toRead =   static_cast<long>(pResourceSize - readTotal) < lbuf.size_
                             ? static_cast<long>(pResourceSize - readTotal) : lbuf.size_;
                    if (io_->read(lbuf.pData_, toRead) != toRead) {
                        throw Error(kerNotAnImage, "Photoshop");
                    }
                    readTotal += toRead;
                    if (outIo.write(lbuf.pData_, toRead) != toRead) throw Error(kerImageWriteFailed);
                }
                if (outIo.error()) throw Error(kerImageWriteFailed);
                newResLength += pResourceSize + adjResourceNameLen + 12;
            }

            io_->seek(curOffset + pResourceSize, BasicIo::beg);
            oldResLength -= (12 + adjResourceNameLen + pResourceSize);
        }

        // Append IPTC_NAA resource block, if not yet written
        if (iptcDone == false) {
            newResLength += writeIptcData(iptcData_, outIo);
            iptcDone = true;
        }

        // Append ExifInfo resource block, if not yet written
        if (exifDone == false) {
            newResLength += writeExifData(exifData_, outIo);
            exifDone = true;
        }

        // Append XmpPacket resource block, if not yet written
        if (xmpDone == false) {
            newResLength += writeXmpData(xmpData_, outIo);
            xmpDone = true;
        }

        // Populate the fake data, only make sense for remoteio, httpio and sshio.
        // it avoids allocating memory for parts of the file that contain image-date.
        io_->populateFakeData();

        // Copy remaining data
        long readSize = 0;
        while ((readSize=io_->read(lbuf.pData_, lbuf.size_))) {
            if (outIo.write(lbuf.pData_, readSize) != readSize) throw Error(kerImageWriteFailed);
        }
        if (outIo.error()) throw Error(kerImageWriteFailed);

        // Update length of resources
#ifdef DEBUG
        std::cerr << "newResLength: " << newResLength << "\n";
#endif
        outIo.seek(resLenOffset, BasicIo::beg);
        ul2Data(buf, newResLength, bigEndian);
        if (outIo.write(buf, 4) != 4) throw Error(kerImageWriteFailed);

    } // PsdImage::doWriteMetadata