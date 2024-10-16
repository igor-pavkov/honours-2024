EnvInitializer::EnvInitializer ()
{
  if( counter++ == 0 ) XrdCl::DefaultEnv::Initialize();
}