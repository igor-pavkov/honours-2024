  virtual req::ptr<File> open(const String& filename,
                              const String& mode,
                              int options,
                              const req::ptr<StreamContext>& context) {
    std::string url(filename.c_str());
    auto pound = url.find('#');
    if (pound == std::string::npos) {
      return nullptr;
    }

    // 6 is the position after zip://
    auto path = url.substr(6, pound - 6);
    auto file = url.substr(pound + 1);

    if (path.empty() || file.empty()) {
      return nullptr;
    }

    int err;
    auto z = _zip_open(path, 0, &err);
    if (z == nullptr) {
      return nullptr;
    }

    return req::make<ZipStream>(z, file);
  }