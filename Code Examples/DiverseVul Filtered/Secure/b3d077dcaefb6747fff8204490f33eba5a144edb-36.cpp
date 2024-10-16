    void CiffHeader::print(std::ostream& os, const std::string& prefix) const
    {
        std::ios::fmtflags f( os.flags() );
        os << prefix
           << _("Header, offset") << " = 0x" << std::setw(8) << std::setfill('0')
           << std::hex << std::right << offset_ << "\n";
        if (pRootDir_) pRootDir_->print(os, byteOrder_, prefix);
        os.flags(f);
    } // CiffHeader::print