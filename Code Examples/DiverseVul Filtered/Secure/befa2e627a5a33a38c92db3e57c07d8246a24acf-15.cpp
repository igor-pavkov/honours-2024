  bool DefaultEnv::SetLogFile( const std::string &filepath )
  {
    Log *log = GetLog();
    LogOutFile *out = new LogOutFile();

    if( out->Open( filepath ) )
    {
      log->SetOutput( out );
      return true;
    }

    delete out;
    return false;
  }