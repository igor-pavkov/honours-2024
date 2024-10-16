    void XPathIo::ReadStdin() {
        if (isatty(fileno(stdin)))
            throw Error(kerInputDataReadFailed);

#ifdef _O_BINARY
        // convert stdin to binary
        if (_setmode(_fileno(stdin), _O_BINARY) == -1)
            throw Error(kerInputDataReadFailed);
#endif

        char readBuf[100*1024];
        std::streamsize readBufSize = 0;
        do {
            std::cin.read(readBuf, sizeof(readBuf));
            readBufSize = std::cin.gcount();
            if (readBufSize > 0) {
                write((byte*)readBuf, (long)readBufSize);
            }
        } while(readBufSize);
    }