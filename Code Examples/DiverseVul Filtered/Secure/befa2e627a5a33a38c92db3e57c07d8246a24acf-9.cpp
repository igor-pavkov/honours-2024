  static void parent()
  {
    using namespace XrdCl;
    Log         *log         = DefaultEnv::GetLog();
    Env         *env         = DefaultEnv::GetEnv();
    ForkHandler *forkHandler = DefaultEnv::GetForkHandler();
    env->UnLock();

    pid_t pid = getpid();
    log->Debug( UtilityMsg, "In the parent fork handler for process %d", pid );

    //--------------------------------------------------------------------------
    // Run the fork handler if it's enabled
    //--------------------------------------------------------------------------
    int runForkHandler = DefaultRunForkHandler;
    env->GetInt( "RunForkHandler", runForkHandler );
    if( runForkHandler )
    {
      log->SetPid(pid);
      forkHandler->Parent();
    }
  }