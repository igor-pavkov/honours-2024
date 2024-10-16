string t_go_generator::render_includes(bool consts) {
  const vector<t_program*>& includes = program_->get_includes();
  string result = "";
  string unused_prot = "";

  string local_namespace = program_->get_namespace("go");
  for (size_t i = 0; i < includes.size(); ++i) {
    if (!local_namespace.empty() && local_namespace == includes[i]->get_namespace("go")) {
      continue;
    }

    string go_module = get_real_go_module(includes[i]);
    size_t found = 0;
    for (size_t j = 0; j < go_module.size(); j++) {
      // Import statement uses slashes ('/') in namespace
      if (go_module[j] == '.') {
        go_module[j] = '/';
        found = j + 1;
      }
    }

    result += "\t\"" + gen_package_prefix_ + go_module + "\"\n";
    unused_prot += "var _ = " + go_module.substr(found) + ".GoUnusedProtection__\n";
  }

  if (includes.size() > 0) {
    result += "\n";
  }

  return go_imports_begin(consts) + result + go_imports_end() + unused_prot;
}