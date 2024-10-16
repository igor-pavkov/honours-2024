void t_go_generator::generate_service_interface(t_service* tservice) {
  string extends = "";
  string extends_if = "";
  string serviceName(publicize(tservice->get_name()));
  string interfaceName = serviceName;

  if (tservice->get_extends() != NULL) {
    extends = type_name(tservice->get_extends());
    size_t index = extends.rfind(".");

    if (index != string::npos) {
      extends_if = "\n" + indent() + "  " + extends.substr(0, index + 1)
                   + publicize(extends.substr(index + 1)) + "\n";
    } else {
      extends_if = "\n" + indent() + publicize(extends) + "\n";
    }
  }

  f_types_ << indent() << "type " << interfaceName << " interface {" << extends_if;
  indent_up();
  generate_go_docstring(f_types_, tservice);
  vector<t_function*> functions = tservice->get_functions();

  if (!functions.empty()) {
    f_types_ << endl;
    vector<t_function*>::iterator f_iter;

    for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
      generate_go_docstring(f_types_, (*f_iter));
      f_types_ << indent() << function_signature_if(*f_iter, "", true) << endl;
    }
  }

  indent_down();
  f_types_ << indent() << "}" << endl << endl;
}