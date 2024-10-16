    uint32_t PsdImage::writeXmpData(const XmpData& xmpData, BasicIo& out) const
    {
        std::string xmpPacket;
        uint32_t resLength = 0;
        byte buf[8];

#ifdef DEBUG
        std::cerr << "writeXmpFromPacket(): " << writeXmpFromPacket() << "\n";
#endif
//        writeXmpFromPacket(true);
        if (writeXmpFromPacket() == false) {
            if (XmpParser::encode(xmpPacket, xmpData) > 1) {
#ifndef SUPPRESS_WARNINGS
                EXV_ERROR << "Failed to encode XMP metadata.\n";
#endif
            }
        }

        if (xmpPacket.size() > 0) {
#ifdef DEBUG
            std::cerr << std::hex << "write: resourceId: " << kPhotoshopResourceID_XMPPacket << "\n";
            std::cerr << std::dec << "Writing XMPPacket: size: " << xmpPacket.size() << "\n";
#endif
            if (out.write(reinterpret_cast<const byte*>(Photoshop::irbId_[0]), 4) != 4) throw Error(kerImageWriteFailed);
            us2Data(buf, kPhotoshopResourceID_XMPPacket, bigEndian);
            if (out.write(buf, 2) != 2) throw Error(kerImageWriteFailed);
            us2Data(buf, 0, bigEndian);                      // NULL resource name
            if (out.write(buf, 2) != 2) throw Error(kerImageWriteFailed);
            ul2Data(buf, static_cast<uint32_t>(xmpPacket.size()), bigEndian);
            if (out.write(buf, 4) != 4) throw Error(kerImageWriteFailed);
            // Write XMPPacket
            if (out.write(reinterpret_cast<const byte*>(xmpPacket.data()), static_cast<long>(xmpPacket.size()))
                != static_cast<long>(xmpPacket.size())) throw Error(kerImageWriteFailed);
            if (out.error()) throw Error(kerImageWriteFailed);
            resLength += static_cast<uint32_t>(xmpPacket.size()) + 12;
            if (xmpPacket.size() & 1)    // even padding
            {
                buf[0] = 0;
                if (out.write(buf, 1) != 1) throw Error(kerImageWriteFailed);
                resLength++;
            }
        }
        return resLength;
    } // PsdImage::writeXmpData