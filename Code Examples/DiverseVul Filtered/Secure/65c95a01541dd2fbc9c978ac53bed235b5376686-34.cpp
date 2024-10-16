static bool addPattern(zip* zipStruct, const String& pattern, const Array& options,
                       std::string path, int64_t flags, bool glob) {
  std::string removePath;
  if (options->exists(String("remove_path"))) {
    auto var = options->get(String("remove_path"));
    if (var.isString()) {
      removePath.append(var.asCStrRef().c_str());
    }
  }

  bool removeAllPath = false;
  if (options->exists(String("remove_all_path"))) {
    auto var = options->get(String("remove_all_path"));
    if (var.isBoolean()) {
      removeAllPath = var.asBooleanVal();
    }
  }

  std::string addPath;
  if (options->exists(String("add_path"))) {
    auto var = options->get(String("add_path"));
    if (var.isString()) {
      addPath.append(var.asCStrRef().c_str());
    }
  }

  Array files;
  if (glob) {
    auto match = HHVM_FN(glob)(pattern, flags);
    if (match.isArray()) {
      files = match.toArrRef();
    } else {
      return false;
    }
  } else {
    if (path[path.size() - 1] != '/') {
      path.push_back('/');
    }
    auto allFiles = HHVM_FN(scandir)(path);
    if (allFiles.isArray()) {
      files = allFiles.toArrRef();
    } else {
      return false;
    }
  }

  std::string dest;
  auto pathLen = path.size();

  for (ArrayIter it(files); it; ++it) {
    auto var = it.second();
    if (!var.isString()) {
      return false;
    }

    auto source = var.asCStrRef();
    if (HHVM_FN(is_dir)(source)) {
      continue;
    }

    if (!glob) {
      auto var = preg_match(pattern, source);
      if (var.isInteger()) {
        if (var.asInt64Val() == 0) {
          continue;
        }
      } else {
        return false;
      }
    }

    dest.resize(0);
    dest.append(source.c_str());

    if (removeAllPath) {
      auto index = dest.rfind('/');
      if (index != std::string::npos) {
        dest.erase(0, index + 1);
      }
    } else if (!removePath.empty()) {
      auto index = dest.find(removePath);
      if (index == 0) {
        dest.erase(0, removePath.size());
      }
    }

    if (!addPath.empty()) {
      dest.insert(0, addPath);
    }

    path.resize(pathLen);
    path.append(source.c_str());

    if (!addFile(zipStruct, path.c_str(), dest.c_str())) {
      return false;
    }
  }

  zip_error_clear(zipStruct);
  return true;
}