    size_t CurlIo::write(const byte* data, size_t wcount)
    {
        if (p_->protocol_ == pHttp || p_->protocol_ == pHttps) {
            return RemoteIo::write(data, wcount);
        } else {
            throw Error(kerErrorMessage, "doesnt support write for this protocol.");
        }
    }