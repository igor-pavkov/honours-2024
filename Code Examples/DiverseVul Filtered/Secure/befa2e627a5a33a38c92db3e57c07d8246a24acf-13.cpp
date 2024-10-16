  void DefaultEnv::Finalize()
  {
    if( sPostMaster )
    {
      sPostMaster->Stop();
      sPostMaster->Finalize();
      delete sPostMaster;
      sPostMaster = 0;
    }

    delete sTransportManager;
    sTransportManager = 0;

    delete sCheckSumManager;
    sCheckSumManager = 0;

    delete sMonitor;
    sMonitor = 0;

    if( sMonitorLibHandle )
      sMonitorLibHandle->Unload();

    delete sMonitorLibHandle;
    sMonitorLibHandle = 0;

    delete sForkHandler;
    sForkHandler = 0;

    delete sFileTimer;
    sFileTimer = 0;

    delete sPlugInManager;
    sPlugInManager = 0;

    delete sEnv;
    sEnv = 0;

    delete sLog;
    sLog = 0;
  }