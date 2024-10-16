  void DefaultEnv::SetLogMask( const std::string &level,
                               const std::string &mask )
  {
    Log *log = GetLog();
    MaskTranslator translator;
    uint64_t topicMask = translator.translateMask( mask );

    if( level == "All" )
    {
      log->SetMask( Log::ErrorMsg,   topicMask );
      log->SetMask( Log::WarningMsg, topicMask );
      log->SetMask( Log::InfoMsg,    topicMask );
      log->SetMask( Log::DebugMsg,   topicMask );
      log->SetMask( Log::DumpMsg,    topicMask );
      return;
    }

    log->SetMask( level, topicMask );
  }