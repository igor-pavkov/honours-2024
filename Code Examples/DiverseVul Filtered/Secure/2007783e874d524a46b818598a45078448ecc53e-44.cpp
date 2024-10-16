void t_go_generator::generate_process_function(t_service* tservice, t_function* tfunction) {
  // Open function
  string processorName = privatize(tservice->get_name()) + "Processor"
                         + publicize(tfunction->get_name());
  string argsname = publicize(tfunction->get_name() + "_args", true);
  string resultname = publicize(tfunction->get_name() + "_result", true);
  // t_struct* xs = tfunction->get_xceptions();
  // const std::vector<t_field*>& xceptions = xs->get_members();
  vector<t_field*>::const_iterator x_iter;
  f_types_ << indent() << "type " << processorName << " struct {" << endl;
  f_types_ << indent() << "  handler " << publicize(tservice->get_name()) << endl;
  f_types_ << indent() << "}" << endl << endl;
  f_types_ << indent() << "func (p *" << processorName
             << ") Process(seqId int32, iprot, oprot thrift.TProtocol) (success bool, err "
                "thrift.TException) {" << endl;
  indent_up();
  f_types_ << indent() << "args := " << argsname << "{}" << endl;
  f_types_ << indent() << "if err = args." << read_method_name_ <<  "(iprot); err != nil {" << endl;
  f_types_ << indent() << "  iprot.ReadMessageEnd()" << endl;
  if (!tfunction->is_oneway()) {
    f_types_ << indent()
               << "  x := thrift.NewTApplicationException(thrift.PROTOCOL_ERROR, err.Error())"
               << endl;
    f_types_ << indent() << "  oprot.WriteMessageBegin(\"" << escape_string(tfunction->get_name())
               << "\", thrift.EXCEPTION, seqId)" << endl;
    f_types_ << indent() << "  x.Write(oprot)" << endl;
    f_types_ << indent() << "  oprot.WriteMessageEnd()" << endl;
    f_types_ << indent() << "  oprot.Flush()" << endl;
  }
  f_types_ << indent() << "  return false, err" << endl;
  f_types_ << indent() << "}" << endl << endl;
  f_types_ << indent() << "iprot.ReadMessageEnd()" << endl;

  if (!tfunction->is_oneway()) {
    f_types_ << indent() << "result := " << resultname << "{}" << endl;
  }
  bool need_reference = type_need_reference(tfunction->get_returntype());
  if (!tfunction->is_oneway() && !tfunction->get_returntype()->is_void()) {
    f_types_ << "var retval " << type_to_go_type(tfunction->get_returntype()) << endl;
  }

  f_types_ << indent() << "var err2 error" << endl;
  f_types_ << indent() << "if ";

  if (!tfunction->is_oneway()) {
    if (!tfunction->get_returntype()->is_void()) {
      f_types_ << "retval, ";
    }
  }

  // Generate the function call
  t_struct* arg_struct = tfunction->get_arglist();
  const std::vector<t_field*>& fields = arg_struct->get_members();
  vector<t_field*>::const_iterator f_iter;
  f_types_ << "err2 = p.handler." << publicize(tfunction->get_name()) << "(";
  bool first = true;

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if (first) {
      first = false;
    } else {
      f_types_ << ", ";
    }

    f_types_ << "args." << publicize((*f_iter)->get_name());
  }

  f_types_ << "); err2 != nil {" << endl;

  t_struct* exceptions = tfunction->get_xceptions();
  const vector<t_field*>& x_fields = exceptions->get_members();
  if (!x_fields.empty()) {
    f_types_ << indent() << "switch v := err2.(type) {" << endl;

    vector<t_field*>::const_iterator xf_iter;

    for (xf_iter = x_fields.begin(); xf_iter != x_fields.end(); ++xf_iter) {
      f_types_ << indent() << "  case " << type_to_go_type(((*xf_iter)->get_type())) << ":"
                 << endl;
      f_types_ << indent() << "result." << publicize((*xf_iter)->get_name()) << " = v" << endl;
    }

    f_types_ << indent() << "  default:" << endl;
  }

  if (!tfunction->is_oneway()) {
    f_types_ << indent() << "  x := thrift.NewTApplicationException(thrift.INTERNAL_ERROR, "
                              "\"Internal error processing " << escape_string(tfunction->get_name())
               << ": \" + err2.Error())" << endl;
    f_types_ << indent() << "  oprot.WriteMessageBegin(\"" << escape_string(tfunction->get_name())
               << "\", thrift.EXCEPTION, seqId)" << endl;
    f_types_ << indent() << "  x.Write(oprot)" << endl;
    f_types_ << indent() << "  oprot.WriteMessageEnd()" << endl;
    f_types_ << indent() << "  oprot.Flush()" << endl;
  }

  f_types_ << indent() << "  return true, err2" << endl;

  if (!x_fields.empty()) {
    f_types_ << indent() << "}" << endl;
  }

  f_types_ << indent() << "}"; // closes err2 != nil

  if (!tfunction->is_oneway()) {
    if (!tfunction->get_returntype()->is_void()) {
      f_types_ << " else {" << endl; // make sure we set Success retval only on success
      indent_up();
      f_types_ << indent() << "result.Success = ";
      if (need_reference) {
        f_types_ << "&";
      }
      f_types_ << "retval" << endl;
      indent_down();
      f_types_ << "}" << endl;
    } else {
      f_types_ << endl;
    }
    f_types_ << indent() << "if err2 = oprot.WriteMessageBegin(\""
               << escape_string(tfunction->get_name()) << "\", thrift.REPLY, seqId); err2 != nil {"
               << endl;
    f_types_ << indent() << "  err = err2" << endl;
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "if err2 = result." << write_method_name_ << "(oprot); err == nil && err2 != nil {" << endl;
    f_types_ << indent() << "  err = err2" << endl;
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "if err2 = oprot.WriteMessageEnd(); err == nil && err2 != nil {"
               << endl;
    f_types_ << indent() << "  err = err2" << endl;
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "if err2 = oprot.Flush(); err == nil && err2 != nil {" << endl;
    f_types_ << indent() << "  err = err2" << endl;
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "if err != nil {" << endl;
    f_types_ << indent() << "  return" << endl;
    f_types_ << indent() << "}" << endl;
    f_types_ << indent() << "return true, err" << endl;
  } else {
    f_types_ << endl;
    f_types_ << indent() << "return true, nil" << endl;
  }
  indent_down();
  f_types_ << indent() << "}" << endl << endl;
}