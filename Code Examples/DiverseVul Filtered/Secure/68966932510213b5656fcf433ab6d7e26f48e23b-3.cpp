    uint32_t PsdImage::writeIptcData(const IptcData& iptcData, BasicIo& out) const
    {
        uint32_t resLength = 0;
        byte buf[8];

        if (iptcData.count() > 0) {
            DataBuf rawIptc = IptcParser::encode(iptcData);
            if (rawIptc.size_ > 0) {
#ifdef DEBUG
                std::cerr << std::hex << "write: resourceId: " << kPhotoshopResourceID_IPTC_NAA << "\n";
                std::cerr << std::dec << "Writing IPTC_NAA: size: " << rawIptc.size_ << "\n";
#endif
                if (out.write(reinterpret_cast<const byte*>(Photoshop::irbId_[0]), 4) != 4) throw Error(kerImageWriteFailed);
                us2Data(buf, kPhotoshopResourceID_IPTC_NAA, bigEndian);
                if (out.write(buf, 2) != 2) throw Error(kerImageWriteFailed);
                us2Data(buf, 0, bigEndian);                      // NULL resource name
                if (out.write(buf, 2) != 2) throw Error(kerImageWriteFailed);
                ul2Data(buf, rawIptc.size_, bigEndian);
                if (out.write(buf, 4) != 4) throw Error(kerImageWriteFailed);
                // Write encoded Iptc data
                if (out.write(rawIptc.pData_, rawIptc.size_) != rawIptc.size_) throw Error(kerImageWriteFailed);
                resLength += rawIptc.size_ + 12;
                if (rawIptc.size_ & 1)    // even padding
                {
                    buf[0] = 0;
                    if (out.write(buf, 1) != 1) throw Error(kerImageWriteFailed);
                    resLength++;
                }
            }
        }
        return resLength;
    } // PsdImage::writeIptcData