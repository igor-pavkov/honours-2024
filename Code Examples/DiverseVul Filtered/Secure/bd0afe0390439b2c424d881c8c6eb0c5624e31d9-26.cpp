    void BasicIo::readOrThrow(byte* buf, size_t rcount) {
      const size_t nread = read(buf, rcount);
      enforce(nread == rcount, kerInputDataReadFailed);
      enforce(!error(), kerInputDataReadFailed);
    }