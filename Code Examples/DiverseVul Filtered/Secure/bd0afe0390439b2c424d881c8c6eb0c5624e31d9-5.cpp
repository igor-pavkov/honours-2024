    size_t MemIo::write(const byte* data, size_t wcount)
    {
        p_->reserve(wcount);
        assert(p_->isMalloced_);
        if (data != nullptr) {
            std::memcpy(&p_->data_[p_->idx_], data, wcount);
        }
        p_->idx_ += wcount;
        return wcount;
    }