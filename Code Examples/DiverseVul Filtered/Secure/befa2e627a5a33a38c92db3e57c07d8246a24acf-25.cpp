  PostMaster *DefaultEnv::GetPostMaster()
  {
    PostMaster* postMaster = AtomicGet(sPostMaster);

    if( unlikely( !postMaster ) )
    {
      XrdSysMutexHelper scopedLock( sInitMutex );
      postMaster = AtomicGet(sPostMaster);

      if( postMaster )
        return postMaster;

      postMaster = new PostMaster();

      if( !postMaster->Initialize() )
      {
        delete postMaster;
        postMaster = 0;
        return 0;
      }

      if( !postMaster->Start() )
      {
        postMaster->Finalize();
        delete postMaster;
        postMaster = 0;
        return 0;
      }

      sForkHandler->RegisterPostMaster( postMaster );
      postMaster->GetTaskManager()->RegisterTask( sFileTimer, time(0), false );
      AtomicCAS(sPostMaster, sPostMaster, postMaster);
    }

    return postMaster;
  }