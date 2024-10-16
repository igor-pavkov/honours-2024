string t_go_generator::module_name(t_type* ttype) {
  t_program* program = ttype->get_program();

  if (program != NULL && program != program_) {
    if (program->get_namespace("go").empty() || 
        program_->get_namespace("go").empty() || 
        program->get_namespace("go") != program_->get_namespace("go")) {
      string module(get_real_go_module(program));
      // for namespaced includes, only keep part after dot.
      size_t dot = module.rfind('.');
      if (dot != string::npos) {
        module = module.substr(dot + 1);
      }
      return module;
    }
  }

  return "";
}