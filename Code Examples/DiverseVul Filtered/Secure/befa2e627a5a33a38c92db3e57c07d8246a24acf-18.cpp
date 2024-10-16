  CheckSumManager *DefaultEnv::GetCheckSumManager()
  {
    if( unlikely( !sCheckSumManager ) )
    {
      XrdSysMutexHelper scopedLock( sInitMutex );
      if( !sCheckSumManager )
        sCheckSumManager = new CheckSumManager();
    }
    return sCheckSumManager;
  }