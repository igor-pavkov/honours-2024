void ompl::geometric::VFRRT::setup()
{
    RRT::setup();
    vfdim_ = si_->getStateSpace()->getValueLocations().size();
}