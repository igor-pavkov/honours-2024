  void DefaultEnv::SetUpLog()
  {
    Log *log = GetLog();

    //--------------------------------------------------------------------------
    // Check if the log level has been defined in the environment
    //--------------------------------------------------------------------------
    char *level = getenv( "XRD_LOGLEVEL" );
    if( level )
      log->SetLevel( level );

    //--------------------------------------------------------------------------
    // Check if we need to log to a file
    //--------------------------------------------------------------------------
    char *file = getenv( "XRD_LOGFILE" );
    if( file )
    {
      LogOutFile *out = new LogOutFile();
      if( out->Open( file ) )
        log->SetOutput( out );
      else
        delete out;
    }

    //--------------------------------------------------------------------------
    // Log mask defaults
    //--------------------------------------------------------------------------
    MaskTranslator translator;
    log->SetMask( Log::DumpMsg, translator.translateMask( "All|^PollerMsg" ) );

    //--------------------------------------------------------------------------
    // Initialize the topic mask
    //--------------------------------------------------------------------------
    char *logMask = getenv( "XRD_LOGMASK" );
    if( logMask )
    {
      uint64_t mask = translator.translateMask( logMask );
      log->SetMask( Log::ErrorMsg,   mask );
      log->SetMask( Log::WarningMsg, mask );
      log->SetMask( Log::InfoMsg,    mask );
      log->SetMask( Log::DebugMsg,   mask );
      log->SetMask( Log::DumpMsg,    mask );
    }

    logMask = getenv( "XRD_LOGMASK_ERROR" );
    if( logMask ) log->SetMask( Log::ErrorMsg, translator.translateMask( logMask ) );

    logMask = getenv( "XRD_LOGMASK_WARNING" );
    if( logMask ) log->SetMask( Log::WarningMsg, translator.translateMask( logMask ) );

    logMask = getenv( "XRD_LOGMASK_INFO" );
    if( logMask ) log->SetMask( Log::InfoMsg, translator.translateMask( logMask ) );

    logMask = getenv( "XRD_LOGMASK_DEBUG" );
    if( logMask ) log->SetMask( Log::DebugMsg, translator.translateMask( logMask ) );

    logMask = getenv( "XRD_LOGMASK_DUMP" );
    if( logMask ) log->SetMask( Log::DumpMsg, translator.translateMask( logMask ) );

    //--------------------------------------------------------------------------
    // Set up the topic strings
    //--------------------------------------------------------------------------
    log->SetTopicName( AppMsg,             "App" );
    log->SetTopicName( UtilityMsg,         "Utility" );
    log->SetTopicName( FileMsg,            "File" );
    log->SetTopicName( PollerMsg,          "Poller" );
    log->SetTopicName( PostMasterMsg,      "PostMaster" );
    log->SetTopicName( XRootDTransportMsg, "XRootDTransport" );
    log->SetTopicName( TaskMgrMsg,         "TaskMgr" );
    log->SetTopicName( XRootDMsg,          "XRootD" );
    log->SetTopicName( FileSystemMsg,      "FileSystem" );
    log->SetTopicName( AsyncSockMsg,       "AsyncSock" );
    log->SetTopicName( JobMgrMsg,          "JobMgr" );
    log->SetTopicName( PlugInMgrMsg,       "PlugInMgr" );
  }