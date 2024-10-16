void t_go_generator::generate_service_client(t_service* tservice) {
  string extends = "";
  string extends_field = "";
  string extends_client = "";
  string extends_client_new = "";
  string serviceName(publicize(tservice->get_name()));

  if (tservice->get_extends() != NULL) {
    extends = type_name(tservice->get_extends());
    size_t index = extends.rfind(".");

    if (index != string::npos) {
      extends_client = extends.substr(0, index + 1) + publicize(extends.substr(index + 1))
                       + "Client";
      extends_client_new = extends.substr(0, index + 1) + "New"
                           + publicize(extends.substr(index + 1)) + "Client";
    } else {
      extends_client = publicize(extends) + "Client";
      extends_client_new = "New" + extends_client;
    }
  }

  extends_field = extends_client.substr(extends_client.find(".") + 1);

  generate_go_docstring(f_types_, tservice);
  f_types_ << indent() << "type " << serviceName << "Client struct {" << endl;
  indent_up();

  if (!extends_client.empty()) {
    f_types_ << indent() << "*" << extends_client << endl;
  } else {
    f_types_ << indent() << "Transport thrift.TTransport" << endl;
    f_types_ << indent() << "ProtocolFactory thrift.TProtocolFactory" << endl;
    f_types_ << indent() << "InputProtocol thrift.TProtocol" << endl;
    f_types_ << indent() << "OutputProtocol thrift.TProtocol" << endl;
    f_types_ << indent() << "SeqId int32" << endl;
    /*f_types_ << indent() << "reqs map[int32]Deferred" << endl*/;
  }

  indent_down();
  f_types_ << indent() << "}" << endl << endl;
  // Constructor function
  f_types_ << indent() << "func New" << serviceName
             << "ClientFactory(t thrift.TTransport, f thrift.TProtocolFactory) *" << serviceName
             << "Client {" << endl;
  indent_up();
  f_types_ << indent() << "return &" << serviceName << "Client";

  if (!extends.empty()) {
    f_types_ << "{" << extends_field << ": " << extends_client_new << "Factory(t, f)}";
  } else {
    indent_up();
    f_types_ << "{Transport: t," << endl;
    f_types_ << indent() << "ProtocolFactory: f," << endl;
    f_types_ << indent() << "InputProtocol: f.GetProtocol(t)," << endl;
    f_types_ << indent() << "OutputProtocol: f.GetProtocol(t)," << endl;
    f_types_ << indent() << "SeqId: 0," << endl;
    /*f_types_ << indent() << "Reqs: make(map[int32]Deferred)" << endl*/;
    indent_down();
    f_types_ << indent() << "}" << endl;
  }

  indent_down();
  f_types_ << indent() << "}" << endl << endl;
  // Constructor function
  f_types_
      << indent() << "func New" << serviceName
      << "ClientProtocol(t thrift.TTransport, iprot thrift.TProtocol, oprot thrift.TProtocol) *"
      << serviceName << "Client {" << endl;
  indent_up();
  f_types_ << indent() << "return &" << serviceName << "Client";

  if (!extends.empty()) {
    f_types_ << "{" << extends_field << ": " << extends_client_new << "Protocol(t, iprot, oprot)}"
               << endl;
  } else {
    indent_up();
    f_types_ << "{Transport: t," << endl;
    f_types_ << indent() << "ProtocolFactory: nil," << endl;
    f_types_ << indent() << "InputProtocol: iprot," << endl;
    f_types_ << indent() << "OutputProtocol: oprot," << endl;
    f_types_ << indent() << "SeqId: 0," << endl;
    /*f_types_ << indent() << "Reqs: make(map[int32]interface{})" << endl*/;
    indent_down();
    f_types_ << indent() << "}" << endl;
  }

  indent_down();
  f_types_ << indent() << "}" << endl << endl;
  // Generate client method implementations
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* arg_struct = (*f_iter)->get_arglist();
    const vector<t_field*>& fields = arg_struct->get_members();
    vector<t_field*>::const_iterator fld_iter;
    string funname = publicize((*f_iter)->get_name());
    // Open function
    generate_go_docstring(f_types_, (*f_iter));
    f_types_ << indent() << "func (p *" << serviceName << "Client) "
               << function_signature_if(*f_iter, "", true) << " {" << endl;
    indent_up();
    /*
    f_types_ <<
      indent() << "p.SeqId += 1" << endl;
    if (!(*f_iter)->is_oneway()) {
      f_types_ <<
        indent() << "d := defer.Deferred()" << endl <<
        indent() << "p.Reqs[p.SeqId] = d" << endl;
    }
    */
    f_types_ << indent() << "if err = p.send" << funname << "(";
    bool first = true;

    for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
      if (first) {
        first = false;
      } else {
        f_types_ << ", ";
      }

      f_types_ << variable_name_to_go_name((*fld_iter)->get_name());
    }

    f_types_ << "); err != nil { return }" << endl;

    if (!(*f_iter)->is_oneway()) {
      f_types_ << indent() << "return p.recv" << funname << "()" << endl;
    } else {
      f_types_ << indent() << "return" << endl;
    }

    indent_down();
    f_types_ << indent() << "}" << endl << endl;
    f_types_ << indent() << "func (p *" << serviceName << "Client) send"
               << function_signature(*f_iter) << "(err error) {" << endl;
    indent_up();
    std::string argsname = publicize((*f_iter)->get_name() + "_args", true);
    // Serialize the request header
    f_types_ << indent() << "oprot := p.OutputProtocol" << endl;
    f_types_ << indent() << "if oprot == nil {" << endl;
    f_types_ << indent() << "  oprot = p.ProtocolFactory.GetProtocol(p.Transport)" << endl;
    f_types_ << indent() << "  p.OutputProtocol = oprot" << endl;
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "p.SeqId++" << endl;
    f_types_ << indent() << "if err = oprot.WriteMessageBegin(\"" << (*f_iter)->get_name()
               << "\", " << ((*f_iter)->is_oneway() ? "thrift.ONEWAY" : "thrift.CALL")
               << ", p.SeqId); err != nil {" << endl;
    indent_up();
    f_types_ << indent() << "  return" << endl;
    indent_down();
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "args := " << argsname << "{" << endl;

    for (fld_iter = fields.begin(); fld_iter != fields.end(); ++fld_iter) {
      f_types_ << indent() << publicize((*fld_iter)->get_name()) << " : "
                 << variable_name_to_go_name((*fld_iter)->get_name()) << "," << endl;
    }
    f_types_ << indent() << "}" << endl;

    // Write to the stream
    f_types_ << indent() << "if err = args." << write_method_name_ << "(oprot); err != nil {" << endl;
    indent_up();
    f_types_ << indent() << "  return" << endl;
    indent_down();
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "if err = oprot.WriteMessageEnd(); err != nil {" << endl;
    indent_up();
    f_types_ << indent() << "  return" << endl;
    indent_down();
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "return oprot.Flush()" << endl;
    indent_down();
    f_types_ << indent() << "}" << endl << endl;

    if (!(*f_iter)->is_oneway()) {
      std::string resultname = publicize((*f_iter)->get_name() + "_result", true);
      // Open function
      f_types_ << endl << indent() << "func (p *" << serviceName << "Client) recv"
                 << publicize((*f_iter)->get_name()) << "() (";

      if (!(*f_iter)->get_returntype()->is_void()) {
        f_types_ << "value " << type_to_go_type((*f_iter)->get_returntype()) << ", ";
      }

      f_types_ << "err error) {" << endl;
      indent_up();
      // TODO(mcslee): Validate message reply here, seq ids etc.
      string error(tmp("error"));
      string error2(tmp("error"));
      f_types_ << indent() << "iprot := p.InputProtocol" << endl;
      f_types_ << indent() << "if iprot == nil {" << endl;
      f_types_ << indent() << "  iprot = p.ProtocolFactory.GetProtocol(p.Transport)" << endl;
      f_types_ << indent() << "  p.InputProtocol = iprot" << endl;
      f_types_ << indent() << "}" << endl;
      f_types_ << indent() << "method, mTypeId, seqId, err := iprot.ReadMessageBegin()" << endl;
      f_types_ << indent() << "if err != nil {" << endl;
      f_types_ << indent() << "  return" << endl;
      f_types_ << indent() << "}" << endl;
      f_types_ << indent() << "if method != \"" << (*f_iter)->get_name() << "\" {" << endl;
      f_types_ << indent() << "  err = thrift.NewTApplicationException("
                 << "thrift.WRONG_METHOD_NAME, \"" << (*f_iter)->get_name()
                 << " failed: wrong method name\")" << endl;
      f_types_ << indent() << "  return" << endl;
      f_types_ << indent() << "}" << endl;
      f_types_ << indent() << "if p.SeqId != seqId {" << endl;
      f_types_ << indent() << "  err = thrift.NewTApplicationException("
                 << "thrift.BAD_SEQUENCE_ID, \"" << (*f_iter)->get_name()
                 << " failed: out of sequence response\")" << endl;
      f_types_ << indent() << "  return" << endl;
      f_types_ << indent() << "}" << endl;
      f_types_ << indent() << "if mTypeId == thrift.EXCEPTION {" << endl;
      f_types_ << indent() << "  " << error
                 << " := thrift.NewTApplicationException(thrift.UNKNOWN_APPLICATION_EXCEPTION, "
                    "\"Unknown Exception\")" << endl;
      f_types_ << indent() << "  var " << error2 << " error" << endl;
      f_types_ << indent() << "  " << error2 << ", err = " << error << ".Read(iprot)" << endl;
      f_types_ << indent() << "  if err != nil {" << endl;
      f_types_ << indent() << "    return" << endl;
      f_types_ << indent() << "  }" << endl;
      f_types_ << indent() << "  if err = iprot.ReadMessageEnd(); err != nil {" << endl;
      f_types_ << indent() << "    return" << endl;
      f_types_ << indent() << "  }" << endl;
      f_types_ << indent() << "  err = " << error2 << endl;
      f_types_ << indent() << "  return" << endl;
      f_types_ << indent() << "}" << endl;
      f_types_ << indent() << "if mTypeId != thrift.REPLY {" << endl;
      f_types_ << indent() << "  err = thrift.NewTApplicationException("
                 << "thrift.INVALID_MESSAGE_TYPE_EXCEPTION, \"" << (*f_iter)->get_name()
                 << " failed: invalid message type\")" << endl;
      f_types_ << indent() << "  return" << endl;
      f_types_ << indent() << "}" << endl;
      f_types_ << indent() << "result := " << resultname << "{}" << endl;
      f_types_ << indent() << "if err = result." << read_method_name_ << "(iprot); err != nil {" << endl;
      f_types_ << indent() << "  return" << endl;
      f_types_ << indent() << "}" << endl;
      f_types_ << indent() << "if err = iprot.ReadMessageEnd(); err != nil {" << endl;
      f_types_ << indent() << "  return" << endl;
      f_types_ << indent() << "}" << endl;

      t_struct* xs = (*f_iter)->get_xceptions();
      const std::vector<t_field*>& xceptions = xs->get_members();
      vector<t_field*>::const_iterator x_iter;

      for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
        const std::string pubname = publicize((*x_iter)->get_name());

        f_types_ << indent() << "if result." << pubname << " != nil {" << endl;
        f_types_ << indent() << "  err = result." << pubname << endl;
        f_types_ << indent() << "  return " << endl;
        f_types_ << indent() << "}";

        if ((x_iter + 1) != xceptions.end()) {
          f_types_ << " else ";
        } else {
          f_types_ << endl;
        }
      }

      // Careful, only return _result if not a void function
      if (!(*f_iter)->get_returntype()->is_void()) {
        f_types_ << indent() << "value = result.GetSuccess()" << endl;
      }

      f_types_ << indent() << "return" << endl;
      // Close function
      indent_down();
      f_types_ << indent() << "}" << endl << endl;
    }
  }

  // indent_down();
  f_types_ << endl;
}