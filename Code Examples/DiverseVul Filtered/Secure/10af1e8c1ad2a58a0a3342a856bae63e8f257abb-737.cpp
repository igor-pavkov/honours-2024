    inline int _snprintf(char *const s, const size_t size, const char *const format, ...) {
      va_list ap;
      va_start(ap,format);
      const int result = _vsnprintf(s,size,format,ap);
      va_end(ap);
      return result;
    }