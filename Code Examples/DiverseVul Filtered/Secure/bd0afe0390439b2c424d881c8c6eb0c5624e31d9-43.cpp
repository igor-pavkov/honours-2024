    size_t FileIo::read(byte* buf, size_t rcount)
    {
        assert(p_->fp_ != 0);
        if (p_->switchMode(Impl::opRead) != 0) {
            return 0;
        }
        return (long)std::fread(buf, 1, rcount, p_->fp_);
    }