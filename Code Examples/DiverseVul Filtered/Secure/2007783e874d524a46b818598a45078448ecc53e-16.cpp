void t_go_generator::generate_service_remote(t_service* tservice) {
  vector<t_function*> functions = tservice->get_functions();
  t_service* parent = tservice->get_extends();

  // collect inherited functions
  while (parent != NULL) {
    vector<t_function*> p_functions = parent->get_functions();
    functions.insert(functions.end(), p_functions.begin(), p_functions.end());
    parent = parent->get_extends();
  }

  vector<t_function*>::iterator f_iter;
  string f_remote_name = package_dir_ + "/" + underscore(service_name_) + "-remote/"
                         + underscore(service_name_) + "-remote.go";
  ofstream f_remote;
  f_remote.open(f_remote_name.c_str());
  string service_module = get_real_go_module(program_);
  string::size_type loc;

  while ((loc = service_module.find(".")) != string::npos) {
    service_module.replace(loc, 1, 1, '/');
  }
  if (!gen_package_prefix_.empty()) {
    service_module = gen_package_prefix_ + service_module;
  }

  string unused_protection;

  f_remote << go_autogen_comment();
  f_remote << indent() << "package main" << endl << endl;
  f_remote << indent() << "import (" << endl;
  f_remote << indent() << "        \"flag\"" << endl;
  f_remote << indent() << "        \"fmt\"" << endl;
  f_remote << indent() << "        \"math\"" << endl;
  f_remote << indent() << "        \"net\"" << endl;
  f_remote << indent() << "        \"net/url\"" << endl;
  f_remote << indent() << "        \"os\"" << endl;
  f_remote << indent() << "        \"strconv\"" << endl;
  f_remote << indent() << "        \"strings\"" << endl;
  f_remote << indent() << "        \"" + gen_thrift_import_ + "\"" << endl;
  f_remote << indent() << render_included_programs(unused_protection);
  f_remote << indent() << "        \"" << service_module << "\"" << endl;
  f_remote << indent() << ")" << endl;
  f_remote << indent() << endl;
  f_remote << indent() << unused_protection; // filled in render_included_programs()
  f_remote << indent() << endl;
  f_remote << indent() << "func Usage() {" << endl;
  f_remote << indent() << "  fmt.Fprintln(os.Stderr, \"Usage of \", os.Args[0], \" "
                          "[-h host:port] [-u url] [-f[ramed]] function [arg1 [arg2...]]:\")"
           << endl;
  f_remote << indent() << "  flag.PrintDefaults()" << endl;
  f_remote << indent() << "  fmt.Fprintln(os.Stderr, \"\\nFunctions:\")" << endl;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    f_remote << "  fmt.Fprintln(os.Stderr, \"  " << (*f_iter)->get_returntype()->get_name() << " "
             << (*f_iter)->get_name() << "(";
    t_struct* arg_struct = (*f_iter)->get_arglist();
    const std::vector<t_field*>& args = arg_struct->get_members();
    vector<t_field*>::const_iterator a_iter;
    std::vector<t_field*>::size_type num_args = args.size();
    bool first = true;

    for (std::vector<t_field*>::size_type i = 0; i < num_args; ++i) {
      if (first) {
        first = false;
      } else {
        f_remote << ", ";
      }

      f_remote << args[i]->get_type()->get_name() << " " << args[i]->get_name();
    }

    f_remote << ")\")" << endl;
  }

  f_remote << indent() << "  fmt.Fprintln(os.Stderr)" << endl;
  f_remote << indent() << "  os.Exit(0)" << endl;
  f_remote << indent() << "}" << endl;
  f_remote << indent() << endl;
  f_remote << indent() << "func main() {" << endl;
  indent_up();
  f_remote << indent() << "flag.Usage = Usage" << endl;
  f_remote << indent() << "var host string" << endl;
  f_remote << indent() << "var port int" << endl;
  f_remote << indent() << "var protocol string" << endl;
  f_remote << indent() << "var urlString string" << endl;
  f_remote << indent() << "var framed bool" << endl;
  f_remote << indent() << "var useHttp bool" << endl;
  f_remote << indent() << "var parsedUrl url.URL" << endl;
  f_remote << indent() << "var trans thrift.TTransport" << endl;
  f_remote << indent() << "_ = strconv.Atoi" << endl;
  f_remote << indent() << "_ = math.Abs" << endl;
  f_remote << indent() << "flag.Usage = Usage" << endl;
  f_remote << indent() << "flag.StringVar(&host, \"h\", \"localhost\", \"Specify host and port\")"
           << endl;
  f_remote << indent() << "flag.IntVar(&port, \"p\", 9090, \"Specify port\")" << endl;
  f_remote << indent() << "flag.StringVar(&protocol, \"P\", \"binary\", \""
                          "Specify the protocol (binary, compact, simplejson, json)\")" << endl;
  f_remote << indent() << "flag.StringVar(&urlString, \"u\", \"\", \"Specify the url\")" << endl;
  f_remote << indent() << "flag.BoolVar(&framed, \"framed\", false, \"Use framed transport\")"
           << endl;
  f_remote << indent() << "flag.BoolVar(&useHttp, \"http\", false, \"Use http\")" << endl;
  f_remote << indent() << "flag.Parse()" << endl;
  f_remote << indent() << endl;
  f_remote << indent() << "if len(urlString) > 0 {" << endl;
  f_remote << indent() << "  parsedUrl, err := url.Parse(urlString)" << endl;
  f_remote << indent() << "  if err != nil {" << endl;
  f_remote << indent() << "    fmt.Fprintln(os.Stderr, \"Error parsing URL: \", err)" << endl;
  f_remote << indent() << "    flag.Usage()" << endl;
  f_remote << indent() << "  }" << endl;
  f_remote << indent() << "  host = parsedUrl.Host" << endl;
  f_remote << indent() << "  useHttp = len(parsedUrl.Scheme) <= 0 || parsedUrl.Scheme == \"http\""
           << endl;
  f_remote << indent() << "} else if useHttp {" << endl;
  f_remote << indent() << "  _, err := url.Parse(fmt.Sprint(\"http://\", host, \":\", port))"
           << endl;
  f_remote << indent() << "  if err != nil {" << endl;
  f_remote << indent() << "    fmt.Fprintln(os.Stderr, \"Error parsing URL: \", err)" << endl;
  f_remote << indent() << "    flag.Usage()" << endl;
  f_remote << indent() << "  }" << endl;
  f_remote << indent() << "}" << endl;
  f_remote << indent() << endl;
  f_remote << indent() << "cmd := flag.Arg(0)" << endl;
  f_remote << indent() << "var err error" << endl;
  f_remote << indent() << "if useHttp {" << endl;
  f_remote << indent() << "  trans, err = thrift.NewTHttpClient(parsedUrl.String())" << endl;
  f_remote << indent() << "} else {" << endl;
  f_remote << indent() << "  portStr := fmt.Sprint(port)" << endl;
  f_remote << indent() << "  if strings.Contains(host, \":\") {" << endl;
  f_remote << indent() << "         host, portStr, err = net.SplitHostPort(host)" << endl;
  f_remote << indent() << "         if err != nil {" << endl;
  f_remote << indent() << "                 fmt.Fprintln(os.Stderr, \"error with host:\", err)"
           << endl;
  f_remote << indent() << "                 os.Exit(1)" << endl;
  f_remote << indent() << "         }" << endl;
  f_remote << indent() << "  }" << endl;
  f_remote << indent() << "  trans, err = thrift.NewTSocket(net.JoinHostPort(host, portStr))"
           << endl;
  f_remote << indent() << "  if err != nil {" << endl;
  f_remote << indent() << "    fmt.Fprintln(os.Stderr, \"error resolving address:\", err)" << endl;
  f_remote << indent() << "    os.Exit(1)" << endl;
  f_remote << indent() << "  }" << endl;
  f_remote << indent() << "  if framed {" << endl;
  f_remote << indent() << "    trans = thrift.NewTFramedTransport(trans)" << endl;
  f_remote << indent() << "  }" << endl;
  f_remote << indent() << "}" << endl;
  f_remote << indent() << "if err != nil {" << endl;
  f_remote << indent() << "  fmt.Fprintln(os.Stderr, \"Error creating transport\", err)" << endl;
  f_remote << indent() << "  os.Exit(1)" << endl;
  f_remote << indent() << "}" << endl;
  f_remote << indent() << "defer trans.Close()" << endl;
  f_remote << indent() << "var protocolFactory thrift.TProtocolFactory" << endl;
  f_remote << indent() << "switch protocol {" << endl;
  f_remote << indent() << "case \"compact\":" << endl;
  f_remote << indent() << "  protocolFactory = thrift.NewTCompactProtocolFactory()" << endl;
  f_remote << indent() << "  break" << endl;
  f_remote << indent() << "case \"simplejson\":" << endl;
  f_remote << indent() << "  protocolFactory = thrift.NewTSimpleJSONProtocolFactory()" << endl;
  f_remote << indent() << "  break" << endl;
  f_remote << indent() << "case \"json\":" << endl;
  f_remote << indent() << "  protocolFactory = thrift.NewTJSONProtocolFactory()" << endl;
  f_remote << indent() << "  break" << endl;
  f_remote << indent() << "case \"binary\", \"\":" << endl;
  f_remote << indent() << "  protocolFactory = thrift.NewTBinaryProtocolFactoryDefault()" << endl;
  f_remote << indent() << "  break" << endl;
  f_remote << indent() << "default:" << endl;
  f_remote << indent() << "  fmt.Fprintln(os.Stderr, \"Invalid protocol specified: \", protocol)"
           << endl;
  f_remote << indent() << "  Usage()" << endl;
  f_remote << indent() << "  os.Exit(1)" << endl;
  f_remote << indent() << "}" << endl;
  f_remote << indent() << "client := " << package_name_ << ".New" << publicize(service_name_)
           << "ClientFactory(trans, protocolFactory)" << endl;
  f_remote << indent() << "if err := trans.Open(); err != nil {" << endl;
  f_remote << indent() << "  fmt.Fprintln(os.Stderr, \"Error opening socket to \", "
                          "host, \":\", port, \" \", err)" << endl;
  f_remote << indent() << "  os.Exit(1)" << endl;
  f_remote << indent() << "}" << endl;
  f_remote << indent() << endl;
  f_remote << indent() << "switch cmd {" << endl;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* arg_struct = (*f_iter)->get_arglist();
    const std::vector<t_field*>& args = arg_struct->get_members();
    vector<t_field*>::const_iterator a_iter;
    std::vector<t_field*>::size_type num_args = args.size();
    string funcName((*f_iter)->get_name());
    string pubName(publicize(funcName));
    string argumentsName(publicize(funcName + "_args", true));
    f_remote << indent() << "case \"" << escape_string(funcName) << "\":" << endl;
    indent_up();
    f_remote << indent() << "if flag.NArg() - 1 != " << num_args << " {" << endl;
    f_remote << indent() << "  fmt.Fprintln(os.Stderr, \"" << escape_string(pubName) << " requires "
             << num_args << " args\")" << endl;
    f_remote << indent() << "  flag.Usage()" << endl;
    f_remote << indent() << "}" << endl;

    for (std::vector<t_field*>::size_type i = 0; i < num_args; ++i) {
      int flagArg = i + 1;
      t_type* the_type(args[i]->get_type());
      t_type* the_type2(get_true_type(the_type));

      if (the_type2->is_enum()) {
        f_remote << indent() << "tmp" << i << ", err := (strconv.Atoi(flag.Arg(" << flagArg << ")))"
                 << endl;
        f_remote << indent() << "if err != nil {" << endl;
        f_remote << indent() << "  Usage()" << endl;
        f_remote << indent() << " return" << endl;
        f_remote << indent() << "}" << endl;
        f_remote << indent() << "argvalue" << i << " := " << package_name_ << "."
                 << publicize(the_type->get_name()) << "(tmp" << i << ")" << endl;
      } else if (the_type2->is_base_type()) {
        t_base_type::t_base e = ((t_base_type*)the_type2)->get_base();
        string err(tmp("err"));

        switch (e) {
        case t_base_type::TYPE_VOID:
          break;

        case t_base_type::TYPE_STRING:
          if (((t_base_type*)the_type2)->is_binary()) {
            f_remote << indent() << "argvalue" << i << " := []byte(flag.Arg(" << flagArg << "))"
                     << endl;
          } else {
            f_remote << indent() << "argvalue" << i << " := flag.Arg(" << flagArg << ")" << endl;
          }
          break;

        case t_base_type::TYPE_BOOL:
          f_remote << indent() << "argvalue" << i << " := flag.Arg(" << flagArg << ") == \"true\""
                   << endl;
          break;

        case t_base_type::TYPE_I8:
          f_remote << indent() << "tmp" << i << ", " << err << " := (strconv.Atoi(flag.Arg("
                   << flagArg << ")))" << endl;
          f_remote << indent() << "if " << err << " != nil {" << endl;
          f_remote << indent() << "  Usage()" << endl;
          f_remote << indent() << "  return" << endl;
          f_remote << indent() << "}" << endl;
          f_remote << indent() << "argvalue" << i << " := int8(tmp" << i << ")" << endl;
          break;

        case t_base_type::TYPE_I16:
          f_remote << indent() << "tmp" << i << ", " << err << " := (strconv.Atoi(flag.Arg("
                   << flagArg << ")))" << endl;
          f_remote << indent() << "if " << err << " != nil {" << endl;
          f_remote << indent() << "  Usage()" << endl;
          f_remote << indent() << "  return" << endl;
          f_remote << indent() << "}" << endl;
          f_remote << indent() << "argvalue" << i << " := int16(tmp" << i << ")" << endl;
          break;

        case t_base_type::TYPE_I32:
          f_remote << indent() << "tmp" << i << ", " << err << " := (strconv.Atoi(flag.Arg("
                   << flagArg << ")))" << endl;
          f_remote << indent() << "if " << err << " != nil {" << endl;
          f_remote << indent() << "  Usage()" << endl;
          f_remote << indent() << "  return" << endl;
          f_remote << indent() << "}" << endl;
          f_remote << indent() << "argvalue" << i << " := int32(tmp" << i << ")" << endl;
          break;

        case t_base_type::TYPE_I64:
          f_remote << indent() << "argvalue" << i << ", " << err
                   << " := (strconv.ParseInt(flag.Arg(" << flagArg << "), 10, 64))" << endl;
          f_remote << indent() << "if " << err << " != nil {" << endl;
          f_remote << indent() << "  Usage()" << endl;
          f_remote << indent() << "  return" << endl;
          f_remote << indent() << "}" << endl;
          break;

        case t_base_type::TYPE_DOUBLE:
          f_remote << indent() << "argvalue" << i << ", " << err
                   << " := (strconv.ParseFloat(flag.Arg(" << flagArg << "), 64))" << endl;
          f_remote << indent() << "if " << err << " != nil {" << endl;
          f_remote << indent() << "  Usage()" << endl;
          f_remote << indent() << "  return" << endl;
          f_remote << indent() << "}" << endl;
          break;

        default:
          throw("Invalid base type in generate_service_remote");
        }

        // f_remote << publicize(args[i]->get_name()) << "(strconv.Atoi(flag.Arg(" << flagArg <<
        // ")))";
      } else if (the_type2->is_struct()) {
        string arg(tmp("arg"));
        string mbTrans(tmp("mbTrans"));
        string err1(tmp("err"));
        string factory(tmp("factory"));
        string jsProt(tmp("jsProt"));
        string err2(tmp("err"));
        std::string tstruct_name(publicize(the_type->get_name()));
        std::string tstruct_module( module_name(the_type));
        if(tstruct_module.empty()) {
          tstruct_module = package_name_;
        }

        f_remote << indent() << arg << " := flag.Arg(" << flagArg << ")" << endl;
        f_remote << indent() << mbTrans << " := thrift.NewTMemoryBufferLen(len(" << arg << "))"
                 << endl;
        f_remote << indent() << "defer " << mbTrans << ".Close()" << endl;
        f_remote << indent() << "_, " << err1 << " := " << mbTrans << ".WriteString(" << arg << ")"
                 << endl;
        f_remote << indent() << "if " << err1 << " != nil {" << endl;
        f_remote << indent() << "  Usage()" << endl;
        f_remote << indent() << "  return" << endl;
        f_remote << indent() << "}" << endl;
        f_remote << indent() << factory << " := thrift.NewTSimpleJSONProtocolFactory()" << endl;
        f_remote << indent() << jsProt << " := " << factory << ".GetProtocol(" << mbTrans << ")"
                 << endl;
        f_remote << indent() << "argvalue" << i << " := " << tstruct_module << ".New" << tstruct_name
                 << "()" << endl;
        f_remote << indent() << err2 << " := argvalue" << i << "." << read_method_name_ <<  "(" << jsProt << ")" << endl;
        f_remote << indent() << "if " << err2 << " != nil {" << endl;
        f_remote << indent() << "  Usage()" << endl;
        f_remote << indent() << "  return" << endl;
        f_remote << indent() << "}" << endl;
      } else if (the_type2->is_container() || the_type2->is_xception()) {
        string arg(tmp("arg"));
        string mbTrans(tmp("mbTrans"));
        string err1(tmp("err"));
        string factory(tmp("factory"));
        string jsProt(tmp("jsProt"));
        string err2(tmp("err"));
        std::string argName(publicize(args[i]->get_name()));
        f_remote << indent() << arg << " := flag.Arg(" << flagArg << ")" << endl;
        f_remote << indent() << mbTrans << " := thrift.NewTMemoryBufferLen(len(" << arg << "))"
                 << endl;
        f_remote << indent() << "defer " << mbTrans << ".Close()" << endl;
        f_remote << indent() << "_, " << err1 << " := " << mbTrans << ".WriteString(" << arg << ")"
                 << endl;
        f_remote << indent() << "if " << err1 << " != nil { " << endl;
        f_remote << indent() << "  Usage()" << endl;
        f_remote << indent() << "  return" << endl;
        f_remote << indent() << "}" << endl;
        f_remote << indent() << factory << " := thrift.NewTSimpleJSONProtocolFactory()" << endl;
        f_remote << indent() << jsProt << " := " << factory << ".GetProtocol(" << mbTrans << ")"
                 << endl;
        f_remote << indent() << "containerStruct" << i << " := " << package_name_ << ".New"
                 << argumentsName << "()" << endl;
        f_remote << indent() << err2 << " := containerStruct" << i << ".ReadField" << (i + 1) << "("
                 << jsProt << ")" << endl;
        f_remote << indent() << "if " << err2 << " != nil {" << endl;
        f_remote << indent() << "  Usage()" << endl;
        f_remote << indent() << "  return" << endl;
        f_remote << indent() << "}" << endl;
        f_remote << indent() << "argvalue" << i << " := containerStruct" << i << "." << argName
                 << endl;
      } else {
        throw("Invalid argument type in generate_service_remote");
      }

      if (the_type->is_typedef()) {
        std::string typedef_module( module_name(the_type));
        if(typedef_module.empty()) {
          typedef_module = package_name_;
        }
        f_remote << indent() << "value" << i << " := " << typedef_module << "."
                 << publicize(the_type->get_name()) << "(argvalue" << i << ")" << endl;
      } else {
        f_remote << indent() << "value" << i << " := argvalue" << i << endl;
      }
    }

    f_remote << indent() << "fmt.Print(client." << pubName << "(";
    bool argFirst = true;

    for (std::vector<t_field*>::size_type i = 0; i < num_args; ++i) {
      if (argFirst) {
        argFirst = false;
      } else {
        f_remote << ", ";
      }

      if (args[i]->get_type()->is_enum()) {
        f_remote << "value" << i;
      } else if (args[i]->get_type()->is_base_type()) {
        t_base_type::t_base e = ((t_base_type*)(args[i]->get_type()))->get_base();

        switch (e) {
        case t_base_type::TYPE_VOID:
          break;

        case t_base_type::TYPE_STRING:
        case t_base_type::TYPE_BOOL:
        case t_base_type::TYPE_I8:
        case t_base_type::TYPE_I16:
        case t_base_type::TYPE_I32:
        case t_base_type::TYPE_I64:
        case t_base_type::TYPE_DOUBLE:
          f_remote << "value" << i;
          break;

        default:
          throw("Invalid base type in generate_service_remote");
        }

        // f_remote << publicize(args[i]->get_name()) << "(strconv.Atoi(flag.Arg(" << flagArg <<
        // ")))";
      } else {
        f_remote << "value" << i;
      }
    }

    f_remote << "))" << endl;
    f_remote << indent() << "fmt.Print(\"\\n\")" << endl;
    f_remote << indent() << "break" << endl;
    indent_down();
  }

  f_remote << indent() << "case \"\":" << endl;
  f_remote << indent() << "  Usage()" << endl;
  f_remote << indent() << "  break" << endl;
  f_remote << indent() << "default:" << endl;
  f_remote << indent() << "  fmt.Fprintln(os.Stderr, \"Invalid function \", cmd)" << endl;
  f_remote << indent() << "}" << endl;
  indent_down();
  f_remote << indent() << "}" << endl;
  // Close service file
  f_remote.close();
  format_go_output(f_remote_name);
#ifndef _MSC_VER
  // Make file executable, love that bitwise OR action
  chmod(f_remote_name.c_str(),
        S_IRUSR | S_IWUSR | S_IXUSR
#ifndef _WIN32
        | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
#endif
        );
#endif
}