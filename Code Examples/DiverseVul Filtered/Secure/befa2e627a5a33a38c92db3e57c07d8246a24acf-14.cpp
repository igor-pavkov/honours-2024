  void DefaultEnv::ReInitializeLogging()
  {
    delete sLog;
    sLog = new Log();
    SetUpLog();
  }