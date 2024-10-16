    void PsdImage::readResourceBlock(uint16_t resourceId, uint32_t resourceSize)
    {
        switch(resourceId)
        {
            case kPhotoshopResourceID_IPTC_NAA:
            {
                DataBuf rawIPTC(resourceSize);
                io_->read(rawIPTC.pData_, rawIPTC.size_);
                if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
                if (IptcParser::decode(iptcData_, rawIPTC.pData_, rawIPTC.size_)) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode IPTC metadata.\n";
#endif
                    iptcData_.clear();
                }
                break;
            }

            case kPhotoshopResourceID_ExifInfo:
            {
                DataBuf rawExif(resourceSize);
                io_->read(rawExif.pData_, rawExif.size_);
                if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
                ByteOrder bo = ExifParser::decode(exifData_, rawExif.pData_, rawExif.size_);
                setByteOrder(bo);
                if (rawExif.size_ > 0 && byteOrder() == invalidByteOrder) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode Exif metadata.\n";
#endif
                    exifData_.clear();
                }
                break;
            }

            case kPhotoshopResourceID_XMPPacket:
            {
                DataBuf xmpPacket(resourceSize);
                io_->read(xmpPacket.pData_, xmpPacket.size_);
                if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);
                xmpPacket_.assign(reinterpret_cast<char *>(xmpPacket.pData_), xmpPacket.size_);
                if (xmpPacket_.size() > 0 && XmpParser::decode(xmpData_, xmpPacket_)) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to decode XMP metadata.\n";
#endif
                }
                break;
            }

            // - PS 4.0 preview data is fetched from ThumbnailResource
            // - PS >= 5.0 preview data is fetched from ThumbnailResource2
            case kPhotoshopResourceID_ThumbnailResource:
            case kPhotoshopResourceID_ThumbnailResource2:
            {
                /*
                  Photoshop thumbnail resource header

                  offset  length    name            description
                  ======  ========  ====            ===========
                   0      4 bytes   format          = 1 (kJpegRGB). Also supports kRawRGB (0).
                   4      4 bytes   width           Width of thumbnail in pixels.
                   8      4 bytes   height          Height of thumbnail in pixels.
                  12      4 bytes   widthbytes      Padded row bytes as (width * bitspixel + 31) / 32 * 4.
                  16      4 bytes   size            Total size as widthbytes * height * planes
                  20      4 bytes   compressedsize  Size after compression. Used for consistentcy check.
                  24      2 bytes   bitspixel       = 24. Bits per pixel.
                  26      2 bytes   planes          = 1. Number of planes.
                  28      variable  data            JFIF data in RGB format.
                                                    Note: For resource ID 1033 the data is in BGR format.
                */
                byte buf[28];
                if (io_->read(buf, 28) != 28)
                {
                    throw Error(kerNotAnImage, "Photoshop");
                }
                NativePreview nativePreview;
                nativePreview.position_ = io_->tell();
                nativePreview.size_ = getLong(buf + 20, bigEndian);    // compressedsize
                nativePreview.width_ = getLong(buf + 4, bigEndian);
                nativePreview.height_ = getLong(buf + 8, bigEndian);
                const uint32_t format = getLong(buf + 0, bigEndian);

                if (nativePreview.size_ > 0 && nativePreview.position_ >= 0) {
                    io_->seek(static_cast<long>(nativePreview.size_), BasicIo::cur);
                    if (io_->error() || io_->eof()) throw Error(kerFailedToReadImageData);

                    if (format == 1) {
                        nativePreview.filter_ = "";
                        nativePreview.mimeType_ = "image/jpeg";
                        nativePreviews_.push_back(nativePreview);
                    } else {
                        // unsupported format of native preview
                    }
                }
                break;
            }

            default:
            {
                break;
            }
        }
    } // PsdImage::readResourceBlock