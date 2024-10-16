    inline int _sprintf(char *const s, const char *const format, ...) {
      cimg::mutex(6);
      va_list args;
      va_start(args,format);
      const int result = std::vsprintf(s,format,args);
      va_end(args);
      cimg::mutex(6,0);
      return result;
    }