double ompl::geometric::VFRRT::determineMeanNorm()
{
    ompl::base::State *rstate = si_->allocState();
    double sum = 0.;
    for (unsigned int i = 0; i < magic::VFRRT_MEAN_NORM_SAMPLES; i++)
    {
        sampler_->sampleUniform(rstate);
        sum += vf_(rstate).norm();
    }
    si_->freeState(rstate);
    return sum / magic::VFRRT_MEAN_NORM_SAMPLES;
}