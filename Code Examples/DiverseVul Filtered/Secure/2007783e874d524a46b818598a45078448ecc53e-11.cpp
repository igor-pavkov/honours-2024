void t_go_generator::generate_service_server(t_service* tservice) {
  // Generate the dispatch methods
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  string extends = "";
  string extends_processor = "";
  string extends_processor_new = "";
  string serviceName(publicize(tservice->get_name()));

  if (tservice->get_extends() != NULL) {
    extends = type_name(tservice->get_extends());
    size_t index = extends.rfind(".");

    if (index != string::npos) {
      extends_processor = extends.substr(0, index + 1) + publicize(extends.substr(index + 1))
                          + "Processor";
      extends_processor_new = extends.substr(0, index + 1) + "New"
                              + publicize(extends.substr(index + 1)) + "Processor";
    } else {
      extends_processor = publicize(extends) + "Processor";
      extends_processor_new = "New" + extends_processor;
    }
  }

  string pServiceName(privatize(tservice->get_name()));
  // Generate the header portion
  string self(tmp("self"));

  if (extends_processor.empty()) {
    f_types_ << indent() << "type " << serviceName << "Processor struct {" << endl;
    f_types_ << indent() << "  processorMap map[string]thrift.TProcessorFunction" << endl;
    f_types_ << indent() << "  handler " << serviceName << endl;
    f_types_ << indent() << "}" << endl << endl;
    f_types_ << indent() << "func (p *" << serviceName
               << "Processor) AddToProcessorMap(key string, processor thrift.TProcessorFunction) {"
               << endl;
    f_types_ << indent() << "  p.processorMap[key] = processor" << endl;
    f_types_ << indent() << "}" << endl << endl;
    f_types_ << indent() << "func (p *" << serviceName
               << "Processor) GetProcessorFunction(key string) "
                  "(processor thrift.TProcessorFunction, ok bool) {" << endl;
    f_types_ << indent() << "  processor, ok = p.processorMap[key]" << endl;
    f_types_ << indent() << "  return processor, ok" << endl;
    f_types_ << indent() << "}" << endl << endl;
    f_types_ << indent() << "func (p *" << serviceName
               << "Processor) ProcessorMap() map[string]thrift.TProcessorFunction {" << endl;
    f_types_ << indent() << "  return p.processorMap" << endl;
    f_types_ << indent() << "}" << endl << endl;
    f_types_ << indent() << "func New" << serviceName << "Processor(handler " << serviceName
               << ") *" << serviceName << "Processor {" << endl << endl;
    f_types_
        << indent() << "  " << self << " := &" << serviceName
        << "Processor{handler:handler, processorMap:make(map[string]thrift.TProcessorFunction)}"
        << endl;

    for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
      string escapedFuncName(escape_string((*f_iter)->get_name()));
      f_types_ << indent() << "  " << self << ".processorMap[\"" << escapedFuncName << "\"] = &"
                 << pServiceName << "Processor" << publicize((*f_iter)->get_name())
                 << "{handler:handler}" << endl;
    }

    string x(tmp("x"));
    f_types_ << indent() << "return " << self << endl;
    f_types_ << indent() << "}" << endl << endl;
    f_types_ << indent() << "func (p *" << serviceName
               << "Processor) Process(iprot, oprot thrift.TProtocol) (success bool, err "
                  "thrift.TException) {" << endl;
    f_types_ << indent() << "  name, _, seqId, err := iprot.ReadMessageBegin()" << endl;
    f_types_ << indent() << "  if err != nil { return false, err }" << endl;
    f_types_ << indent() << "  if processor, ok := p.GetProcessorFunction(name); ok {" << endl;
    f_types_ << indent() << "    return processor.Process(seqId, iprot, oprot)" << endl;
    f_types_ << indent() << "  }" << endl;
    f_types_ << indent() << "  iprot.Skip(thrift.STRUCT)" << endl;
    f_types_ << indent() << "  iprot.ReadMessageEnd()" << endl;
    f_types_ << indent() << "  " << x
               << " := thrift.NewTApplicationException(thrift.UNKNOWN_METHOD, \"Unknown function "
                  "\" + name)" << endl;
    f_types_ << indent() << "  oprot.WriteMessageBegin(name, thrift.EXCEPTION, seqId)" << endl;
    f_types_ << indent() << "  " << x << ".Write(oprot)" << endl;
    f_types_ << indent() << "  oprot.WriteMessageEnd()" << endl;
    f_types_ << indent() << "  oprot.Flush()" << endl;
    f_types_ << indent() << "  return false, " << x << endl;
    f_types_ << indent() << "" << endl;
    f_types_ << indent() << "}" << endl << endl;
  } else {
    f_types_ << indent() << "type " << serviceName << "Processor struct {" << endl;
    f_types_ << indent() << "  *" << extends_processor << endl;
    f_types_ << indent() << "}" << endl << endl;
    f_types_ << indent() << "func New" << serviceName << "Processor(handler " << serviceName
               << ") *" << serviceName << "Processor {" << endl;
    f_types_ << indent() << "  " << self << " := &" << serviceName << "Processor{"
               << extends_processor_new << "(handler)}" << endl;

    for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
      string escapedFuncName(escape_string((*f_iter)->get_name()));
      f_types_ << indent() << "  " << self << ".AddToProcessorMap(\"" << escapedFuncName
                 << "\", &" << pServiceName << "Processor" << publicize((*f_iter)->get_name())
                 << "{handler:handler})" << endl;
    }

    f_types_ << indent() << "  return " << self << endl;
    f_types_ << indent() << "}" << endl << endl;
  }

  // Generate the process subfunctions
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    generate_process_function(tservice, *f_iter);
  }

  f_types_ << endl;
}