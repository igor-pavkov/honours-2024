void t_go_generator::init_generator() {
  // Make output directory
  string module = get_real_go_module(program_);
  string target = module;
  package_dir_ = get_out_dir();

  // This set is taken from https://github.com/golang/lint/blob/master/lint.go#L692
  commonInitialisms.insert("API");
  commonInitialisms.insert("ASCII");
  commonInitialisms.insert("CPU");
  commonInitialisms.insert("CSS");
  commonInitialisms.insert("DNS");
  commonInitialisms.insert("EOF");
  commonInitialisms.insert("GUID");
  commonInitialisms.insert("HTML");
  commonInitialisms.insert("HTTP");
  commonInitialisms.insert("HTTPS");
  commonInitialisms.insert("ID");
  commonInitialisms.insert("IP");
  commonInitialisms.insert("JSON");
  commonInitialisms.insert("LHS");
  commonInitialisms.insert("QPS");
  commonInitialisms.insert("RAM");
  commonInitialisms.insert("RHS");
  commonInitialisms.insert("RPC");
  commonInitialisms.insert("SLA");
  commonInitialisms.insert("SMTP");
  commonInitialisms.insert("SSH");
  commonInitialisms.insert("TCP");
  commonInitialisms.insert("TLS");
  commonInitialisms.insert("TTL");
  commonInitialisms.insert("UDP");
  commonInitialisms.insert("UI");
  commonInitialisms.insert("UID");
  commonInitialisms.insert("UUID");
  commonInitialisms.insert("URI");
  commonInitialisms.insert("URL");
  commonInitialisms.insert("UTF8");
  commonInitialisms.insert("VM");
  commonInitialisms.insert("XML");
  commonInitialisms.insert("XSRF");
  commonInitialisms.insert("XSS");

  // names of read and write methods
  if (read_write_private_) {
    read_method_name_ = "read";
    write_method_name_ = "write";
  } else {
    read_method_name_ = "Read";
    write_method_name_ = "Write";
  }

  while (true) {
    // TODO: Do better error checking here.
    MKDIR(package_dir_.c_str());

    if (module.empty()) {
      break;
    }

    string::size_type pos = module.find('.');

    if (pos == string::npos) {
      package_dir_ += "/";
      package_dir_ += module;
      package_name_ = module;
      module.clear();
    } else {
      package_dir_ += "/";
      package_dir_ += module.substr(0, pos);
      module.erase(0, pos + 1);
    }
  }

  string::size_type loc;

  while ((loc = target.find(".")) != string::npos) {
    target.replace(loc, 1, 1, '/');
  }

  // Make output files
  f_types_name_ = package_dir_ + "/" + program_name_ + ".go";
  f_types_.open(f_types_name_.c_str());

  f_consts_name_ = package_dir_ + "/" + program_name_ + "-consts.go";
  f_consts_.open(f_consts_name_.c_str());

  vector<t_service*> services = program_->get_services();
  vector<t_service*>::iterator sv_iter;

  for (sv_iter = services.begin(); sv_iter != services.end(); ++sv_iter) {
    string service_dir = package_dir_ + "/" + underscore((*sv_iter)->get_name()) + "-remote";
    MKDIR(service_dir.c_str());
  }

  // Print header
  f_types_ << go_autogen_comment() << go_package() << render_includes(false);

  f_consts_ << go_autogen_comment() << go_package() << render_includes(true);

  f_const_values_ << endl << "func init() {" << endl;

  // Create file for the GoUnusedProtection__ variable
  string f_unused_prot_name_ = package_dir_ + "/" + "GoUnusedProtection__.go";
  ofstream f_unused_prot_;
  f_unused_prot_.open(f_unused_prot_name_.c_str());
  f_unused_prot_ << go_autogen_comment() << go_package() << render_import_protection();
  f_unused_prot_.close();
}