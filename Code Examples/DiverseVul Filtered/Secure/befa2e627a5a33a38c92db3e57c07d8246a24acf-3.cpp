  Monitor *DefaultEnv::GetMonitor()
  {
    if( unlikely( !sMonitorInitialized ) )
    {
      XrdSysMutexHelper scopedLock( sInitMutex );
      if( !sMonitorInitialized )
      {
        //----------------------------------------------------------------------
        // Check the environment settings
        //----------------------------------------------------------------------
        Env *env = GetEnv();
        Log *log = GetLog();
        sMonitorInitialized = true;
        std::string monitorLib = DefaultClientMonitor;
        env->GetString( "ClientMonitor", monitorLib );
        if( monitorLib.empty() )
        {
          log->Debug( UtilityMsg, "Monitor library name not set. No "
                      "monitoring" );
          return 0;
        }

        std::string monitorParam = DefaultClientMonitorParam;
        env->GetString( "ClientMonitorParam", monitorParam );

        log->Debug( UtilityMsg, "Initializing monitoring, lib: %s, param: %s",
                    monitorLib.c_str(), monitorParam.c_str() );

        //----------------------------------------------------------------------
        // Loading the plugin
        //----------------------------------------------------------------------
        char *errBuffer = new char[4000];
        sMonitorLibHandle = new XrdOucPinLoader(
                                 errBuffer, 4000, &XrdVERSIONINFOVAR( XrdCl ),
                                 "monitor", monitorLib.c_str() );

        typedef XrdCl::Monitor *(*MonLoader)(const char *, const char *);
        MonLoader loader;
        loader = (MonLoader)sMonitorLibHandle->Resolve( "XrdClGetMonitor", -1 );
        if( !loader )
        {
          log->Error( UtilityMsg, "Unable to initialize user monitoring: %s",
                      errBuffer );
          delete [] errBuffer;
          sMonitorLibHandle->Unload();
          delete sMonitorLibHandle; sMonitorLibHandle = 0;
          return 0;
        }

        //----------------------------------------------------------------------
        // Instantiating the monitor object
        //----------------------------------------------------------------------
        const char *param = monitorParam.empty() ? 0 : monitorParam.c_str();
        sMonitor = (*loader)( XrdSysUtils::ExecName(), param );

        if( !sMonitor )
        {
          log->Error( UtilityMsg, "Unable to initialize user monitoring: %s",
                      errBuffer );
          delete [] errBuffer;
          sMonitorLibHandle->Unload();
          delete sMonitorLibHandle; sMonitorLibHandle = 0;
          return 0;
        }
        log->Debug( UtilityMsg, "Successfully initialized monitoring from: %s",
                    monitorLib.c_str() );
        delete [] errBuffer;
      }
    }
    return sMonitor;
  }