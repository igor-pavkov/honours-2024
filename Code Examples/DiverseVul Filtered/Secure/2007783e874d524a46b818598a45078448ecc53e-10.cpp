void t_go_generator::generate_enum(t_enum* tenum) {
  std::ostringstream to_string_mapping, from_string_mapping;
  std::string tenum_name(publicize(tenum->get_name()));
  generate_go_docstring(f_types_, tenum);
  f_types_ << "type " << tenum_name << " int64" << endl << "const (" << endl;

  to_string_mapping << indent() << "func (p " << tenum_name << ") String() string {" << endl;
  to_string_mapping << indent() << "  switch p {" << endl;

  from_string_mapping << indent() << "func " << tenum_name << "FromString(s string) (" << tenum_name
                      << ", error) {" << endl;
  from_string_mapping << indent() << "  switch s {" << endl;

  vector<t_enum_value*> constants = tenum->get_constants();
  vector<t_enum_value*>::iterator c_iter;
  int value = -1;

  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    value = (*c_iter)->get_value();

    string iter_std_name(escape_string((*c_iter)->get_name()));
    string iter_name((*c_iter)->get_name());
    f_types_ << indent() << "  " << tenum_name << "_" << iter_name << ' ' << tenum_name << " = "
             << value << endl;
    // Dictionaries to/from string names of enums
    to_string_mapping << indent() << "  case " << tenum_name << "_" << iter_name << ": return \""
                      << iter_std_name << "\"" << endl;

    if (iter_std_name != escape_string(iter_name)) {
      from_string_mapping << indent() << "  case \"" << iter_std_name << "\", \""
                          << escape_string(iter_name) << "\": return " << tenum_name << "_"
                          << iter_name << ", nil " << endl;
    } else {
      from_string_mapping << indent() << "  case \"" << iter_std_name << "\": return " << tenum_name
                          << "_" << iter_name << ", nil " << endl;
    }
  }

  to_string_mapping << indent() << "  }" << endl;
  to_string_mapping << indent() << "  return \"<UNSET>\"" << endl;
  to_string_mapping << indent() << "}" << endl;
  from_string_mapping << indent() << "  }" << endl;
  from_string_mapping << indent() << "  return " << tenum_name << "(0),"
                      << " fmt.Errorf(\"not a valid " << tenum_name << " string\")" << endl;
  from_string_mapping << indent() << "}" << endl;

  f_types_ << ")" << endl << endl << to_string_mapping.str() << endl << from_string_mapping.str()
           << endl << endl;

  // Generate a convenience function that converts an instance of an enum
  // (which may be a constant) into a pointer to an instance of that enum
  // type.
  f_types_ << "func " << tenum_name << "Ptr(v " << tenum_name << ") *" << tenum_name
           << " { return &v }" << endl << endl;

  // Generate MarshalText
  f_types_ << "func (p " << tenum_name << ") MarshalText() ([]byte, error) {" << endl;
  f_types_ << "return []byte(p.String()), nil" << endl;
  f_types_ << "}" << endl << endl;

  // Generate UnmarshalText
  f_types_ << "func (p *" << tenum_name << ") UnmarshalText(text []byte) error {" << endl;
  f_types_ << "q, err := " << tenum_name << "FromString(string(text))" << endl;
  f_types_ << "if (err != nil) {" << endl << "return err" << endl << "}" << endl;
  f_types_ << "*p = q" << endl;
  f_types_ << "return nil" << endl;
  f_types_ << "}" << endl << endl;

  // Generate Scan for sql.Scanner interface
  f_types_ << "func (p *" << tenum_name << ") Scan(value interface{}) error {" <<endl;
  f_types_ << "v, ok := value.(int64)" <<endl;
  f_types_ << "if !ok {" <<endl;
  f_types_ << "return errors.New(\"Scan value is not int64\")" <<endl;
  f_types_ << "}" <<endl;
  f_types_ << "*p = " << tenum_name << "(v)" << endl;
  f_types_ << "return nil" << endl;
  f_types_ << "}" << endl << endl;

  // Generate Value for driver.Valuer interface
  f_types_ << "func (p * " << tenum_name << ") Value() (driver.Value, error) {" <<endl;
  f_types_ << "  if p == nil {" << endl;
  f_types_ << "    return nil, nil" << endl;
  f_types_ << "  }" << endl;
  f_types_ << "return int64(*p), nil" << endl;
  f_types_ << "}" << endl;

}