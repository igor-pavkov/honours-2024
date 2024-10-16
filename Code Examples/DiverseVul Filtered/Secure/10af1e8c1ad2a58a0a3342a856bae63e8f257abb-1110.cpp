    inline int _vsnprintf(char *const s, const size_t size, const char *const format, va_list ap) {
      int result = -1;
      cimg::mutex(6);
      if (size) result = _vsnprintf_s(s,size,_TRUNCATE,format,ap);
      if (result==-1) result = _vscprintf(format,ap);
      cimg::mutex(6,0);
      return result;
    }