  TransportManager *DefaultEnv::GetTransportManager()
  {
    if( unlikely( !sTransportManager ) )
    {
      XrdSysMutexHelper scopedLock( sInitMutex );
      if( !sTransportManager )
        sTransportManager = new TransportManager();
    }
    return sTransportManager;
  }