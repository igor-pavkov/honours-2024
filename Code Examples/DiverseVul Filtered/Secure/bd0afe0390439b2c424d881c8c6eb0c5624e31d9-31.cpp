    size_t CurlIo::write(BasicIo& src)
    {
        if (p_->protocol_ == pHttp || p_->protocol_ == pHttps) {
            return RemoteIo::write(src);
        } else {
            throw Error(kerErrorMessage, "doesnt support write for this protocol.");
        }
    }