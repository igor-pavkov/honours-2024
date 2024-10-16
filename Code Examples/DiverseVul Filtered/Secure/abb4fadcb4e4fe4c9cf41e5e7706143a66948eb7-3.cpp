ompl::geometric::VFRRT::VFRRT(const base::SpaceInformationPtr &si, VectorField vf, double exploration,
                              double initial_lambda, unsigned int update_freq)
  : RRT(si)
  , vf_(std::move(vf))
  , explorationSetting_(exploration)
  , lambda_(initial_lambda)
  , nth_step_(update_freq)
{
    setName("VFRRT");
    maxDistance_ = si->getStateValidityCheckingResolution();
}