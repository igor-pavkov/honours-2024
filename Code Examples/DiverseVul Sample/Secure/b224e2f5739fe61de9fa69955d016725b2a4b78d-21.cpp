  bool matches(SplashFontFileID *id) override {
    return ((SplashOutFontFileID *)id)->r == r;
  }