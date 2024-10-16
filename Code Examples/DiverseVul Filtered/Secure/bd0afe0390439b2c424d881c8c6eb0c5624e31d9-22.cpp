    DataBuf readFile(const std::string& path)
    {
        FileIo file(path);
        if (file.open("rb") != 0) {
            throw Error(kerFileOpenFailed, path, "rb", strError());
        }
        struct stat st;
        if (0 != ::stat(path.c_str(), &st)) {
            throw Error(kerCallFailed, path, strError(), "::stat");
        }
        DataBuf buf(st.st_size);
        size_t len = file.read(buf.pData_, buf.size_);
        if (len != buf.size_) {
            throw Error(kerCallFailed, path, strError(), "FileIo::read");
        }
        return buf;
    }