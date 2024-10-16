    inline int _snprintf(char *const s, const size_t n, const char *const format, ...) {
      cimg::mutex(6);
      va_list args;
      va_start(args,format);
      const int result = std::vsnprintf(s,n,format,args);
      va_end(args);
      cimg::mutex(6,0);
      return result;
    }