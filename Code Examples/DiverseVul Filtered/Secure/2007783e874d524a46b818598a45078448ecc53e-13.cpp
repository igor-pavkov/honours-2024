  t_go_generator(t_program* program,
                 const std::map<std::string, std::string>& parsed_options,
                 const std::string& option_string)
    : t_generator(program) {
    (void)option_string;
    std::map<std::string, std::string>::const_iterator iter;


    gen_thrift_import_ = DEFAULT_THRIFT_IMPORT;
    gen_package_prefix_ = "";
    package_flag = "";
    read_write_private_ = false;
    ignore_initialisms_ = false;
    for( iter = parsed_options.begin(); iter != parsed_options.end(); ++iter) {
      if( iter->first.compare("package_prefix") == 0) {
        gen_package_prefix_ = (iter->second);
      } else if( iter->first.compare("thrift_import") == 0) {
        gen_thrift_import_ = (iter->second);
      } else if( iter->first.compare("package") == 0) {
        package_flag = (iter->second);
      } else if( iter->first.compare("read_write_private") == 0) {
        read_write_private_ = true;
      } else if( iter->first.compare("ignore_initialisms") == 0) {
        ignore_initialisms_ =  true;
      } else {
        throw "unknown option go:" + iter->first; 
      }
    }

    out_dir_base_ = "gen-go";
  }