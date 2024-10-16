  void DefaultEnv::SetLogLevel( const std::string &level )
  {
    Log *log = GetLog();
    log->SetLevel( level );
  }