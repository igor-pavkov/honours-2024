  void run_with_new_reader() {
    BundleReader reader(Env::Default(), reader_prefix);
    if (!reader.status().ok()) {
      status = reader.status();
      return;
    }

    status = run(&reader);
  }