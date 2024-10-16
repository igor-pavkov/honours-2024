    static void readOrThrow(BasicIo& iIo, byte* buf, long rcount, ErrorCode err) {
      const long nread = iIo.read(buf, rcount);
      enforce(nread == rcount, err);
      enforce(!iIo.error(), err);
    }