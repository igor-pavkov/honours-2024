    void FileIo::Impl::copyXattrFrom(const FileIo&)
#endif
    {
#if defined(__APPLE__)
# if defined(EXV_UNICODE_PATH)
#  error No xattr API for MacOS X with unicode support
# endif
        ssize_t namebufSize = ::listxattr(src.p_->path_.c_str(), 0, 0, 0);
        if (namebufSize < 0) {
            throw Error(kerCallFailed, src.p_->path_, strError(), "listxattr");
        }
        if (namebufSize == 0) {
            // No extended attributes in source file
            return;
        }
        char* namebuf = new char[namebufSize];
        if (::listxattr(src.p_->path_.c_str(), namebuf, namebufSize, 0) != namebufSize) {
            throw Error(kerCallFailed, src.p_->path_, strError(), "listxattr");
        }
        for (ssize_t namebufPos = 0; namebufPos < namebufSize;) {
            const char *name = namebuf + namebufPos;
            namebufPos += strlen(name) + 1;
            const ssize_t valueSize = ::getxattr(src.p_->path_.c_str(), name, 0, 0, 0, 0);
            if (valueSize < 0) {
                throw Error(kerCallFailed, src.p_->path_, strError(), "getxattr");
            }
            char* value = new char[valueSize];
            if (::getxattr(src.p_->path_.c_str(), name, value, valueSize, 0, 0) != valueSize) {
                throw Error(kerCallFailed, src.p_->path_, strError(), "getxattr");
            }
// #906.  Mountain Lion 'sandbox' terminates the app when we call setxattr
#ifndef __APPLE__
#ifdef  EXIV2_DEBUG_MESSAGES
            EXV_DEBUG << "Copying xattr \"" << name << "\" with value size " << valueSize << "\n";
#endif
            if (::setxattr(path_.c_str(), name, value, valueSize, 0, 0) != 0) {
                throw Error(kerCallFailed, path_, strError(), "setxattr");
            }
            delete [] value;
#endif
        }
        delete [] namebuf;
#else
        // No xattr support for this platform.
#endif
    } // FileIo::Impl::copyXattrFrom