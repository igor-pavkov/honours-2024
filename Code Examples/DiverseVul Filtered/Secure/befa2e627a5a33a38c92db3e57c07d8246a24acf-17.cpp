  DefaultEnv::DefaultEnv()
  {
    Log *log = GetLog();

    //--------------------------------------------------------------------------
    // Declate the variables to be processed
    //--------------------------------------------------------------------------
    std::vector<EnvVarHolder<int> >         varsInt;
    std::vector<EnvVarHolder<std::string> > varsStr;
    REGISTER_VAR_INT( varsInt, "ConnectionWindow",     DefaultConnectionWindow     );
    REGISTER_VAR_INT( varsInt, "ConnectionRetry",      DefaultConnectionRetry      );
    REGISTER_VAR_INT( varsInt, "RequestTimeout",       DefaultRequestTimeout       );
    REGISTER_VAR_INT( varsInt, "StreamTimeout",        DefaultStreamTimeout        );
    REGISTER_VAR_INT( varsInt, "SubStreamsPerChannel", DefaultSubStreamsPerChannel );
    REGISTER_VAR_INT( varsInt, "TimeoutResolution",    DefaultTimeoutResolution    );
    REGISTER_VAR_INT( varsInt, "StreamErrorWindow",    DefaultStreamErrorWindow    );
    REGISTER_VAR_INT( varsInt, "RunForkHandler",       DefaultRunForkHandler       );
    REGISTER_VAR_INT( varsInt, "RedirectLimit",        DefaultRedirectLimit        );
    REGISTER_VAR_INT( varsInt, "WorkerThreads",        DefaultWorkerThreads        );
    REGISTER_VAR_INT( varsInt, "CPChunkSize",          DefaultCPChunkSize          );
    REGISTER_VAR_INT( varsInt, "CPParallelChunks",     DefaultCPParallelChunks     );
    REGISTER_VAR_INT( varsInt, "DataServerTTL",        DefaultDataServerTTL        );
    REGISTER_VAR_INT( varsInt, "LoadBalancerTTL",      DefaultLoadBalancerTTL      );
    REGISTER_VAR_INT( varsInt, "CPInitTimeout",        DefaultCPInitTimeout        );
    REGISTER_VAR_INT( varsInt, "CPTPCTimeout",         DefaultCPTPCTimeout         );
    REGISTER_VAR_INT( varsInt, "TCPKeepAlive",         DefaultTCPKeepAlive         );
    REGISTER_VAR_INT( varsInt, "TCPKeepAliveTime",     DefaultTCPKeepAliveTime     );
    REGISTER_VAR_INT( varsInt, "TCPKeepAliveInterval", DefaultTCPKeepAliveInterval );
    REGISTER_VAR_INT( varsInt, "TCPKeepProbes",        DefaultTCPKeepAliveProbes   );
    REGISTER_VAR_INT( varsInt, "MultiProtocol",        DefaultMultiProtocol        );
    REGISTER_VAR_INT( varsInt, "ParallelEvtLoop",      DefaultParallelEvtLoop      );
    REGISTER_VAR_INT( varsInt, "MetalinkProcessing",   DefaultMetalinkProcessing   );
    REGISTER_VAR_INT( varsInt, "LocalMetalinkFile",    DefaultLocalMetalinkFile    );
    REGISTER_VAR_INT( varsInt, "XCpBlockSize",         DefaultXCpBlockSize    );

    REGISTER_VAR_STR( varsStr, "PollerPreference",     DefaultPollerPreference     );
    REGISTER_VAR_STR( varsStr, "ClientMonitor",        DefaultClientMonitor        );
    REGISTER_VAR_STR( varsStr, "ClientMonitorParam",   DefaultClientMonitorParam   );
    REGISTER_VAR_STR( varsStr, "NetworkStack",         DefaultNetworkStack         );
    REGISTER_VAR_STR( varsStr, "PlugIn",               DefaultPlugIn               );
    REGISTER_VAR_STR( varsStr, "PlugInConfDir",        DefaultPlugInConfDir        );
    REGISTER_VAR_STR( varsStr, "ReadRecovery",         DefaultReadRecovery         );
    REGISTER_VAR_STR( varsStr, "WriteRecovery",        DefaultWriteRecovery        );
    REGISTER_VAR_STR( varsStr, "GlfnRedirector",       DefaultGlfnRedirector       );

    //--------------------------------------------------------------------------
    // Process the configuration files
    //--------------------------------------------------------------------------
    std::map<std::string, std::string> config, userConfig;
    Status st = Utils::ProcessConfig( config, "/etc/xrootd/client.conf" );

    if( !st.IsOK() )
      log->Warning( UtilityMsg, "Unable to process global config file: %s",
                    st.ToString().c_str() );

    XrdSysPwd pwdHandler;
    passwd *pwd = pwdHandler.Get( getuid() );
    if( pwd )
    {
      std::string userConfigFile = pwd->pw_dir;
      userConfigFile += "/.xrootd/client.conf";

      st = Utils::ProcessConfig( userConfig, userConfigFile );

      if( !st.IsOK() )
        log->Debug( UtilityMsg, "Unable to process user config file: %s",
                    st.ToString().c_str() );
    }
    else
      log->Debug( UtilityMsg, "Unable to find user home directory." );

    std::map<std::string, std::string>::iterator it;

    for( it = config.begin(); it != config.end(); ++it )
      log->Dump( UtilityMsg, "[Global config] \"%s\" = \"%s\"",
                 it->first.c_str(), it->second.c_str() );

    for( it = userConfig.begin(); it != userConfig.end(); ++it )
    {
      config[it->first] = it->second;
      log->Dump( UtilityMsg, "[User config] \"%s\" = \"%s\"",
                 it->first.c_str(), it->second.c_str() );
    }

    for( it = config.begin(); it != config.end(); ++it )
      log->Debug( UtilityMsg, "[Effective config] \"%s\" = \"%s\"",
                  it->first.c_str(), it->second.c_str() );

    //--------------------------------------------------------------------------
    // Monitoring settings
    //--------------------------------------------------------------------------
    char *tmp = strdup( XrdSysUtils::ExecName() );
    char *appName = basename( tmp );
    PutString( "AppName", appName );
    free( tmp );
    ImportString( "AppName", "XRD_APPNAME" );
    PutString( "MonInfo", "" );
    ImportString( "MonInfo", "XRD_MONINFO" );

    //--------------------------------------------------------------------------
    // Process ints
    //--------------------------------------------------------------------------
    for( size_t i = 0; i < varsInt.size(); ++i )
    {
      PutInt( varsInt[i].name, varsInt[i].def );

      it = config.find( varsInt[i].name );
      if( it != config.end() )
      {
        char *endPtr = 0;
        int value = (int)strtol( it->second.c_str(), &endPtr, 0 );
        if( *endPtr )
          log->Warning( UtilityMsg, "Unable to set %s to %s: not a proper "
                        "integer", varsInt[i].name.c_str(),
                        it->second.c_str() );
        else
          PutInt( varsInt[i].name, value );
      }

      std::string name = "XRD_" + varsInt[i].name;
      std::transform( name.begin(), name.end(), name.begin(), ::toupper );
      ImportInt( varsInt[i].name, name );
    }

    //--------------------------------------------------------------------------
    // Process strings
    //--------------------------------------------------------------------------
    for( size_t i = 0; i < varsStr.size(); ++i )
    {
      PutString( varsStr[i].name, varsStr[i].def );

      it = config.find( varsStr[i].name );
      if( it != config.end() )
        PutString( varsStr[i].name, it->second );

      std::string name = "XRD_" + varsStr[i].name;
      std::transform( name.begin(), name.end(), name.begin(), ::toupper );
      ImportString( varsStr[i].name, name );
    }

    //--------------------------------------------------------------------------
    // Register fork handlers
    //--------------------------------------------------------------------------
    pthread_atfork( prepare, parent, child );
  }