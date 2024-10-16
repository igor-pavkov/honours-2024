Eigen::VectorXd ompl::geometric::VFRRT::getNewDirection(const base::State *qnear, const base::State *qrand)
{
    // Set vrand to be the normalized vector from qnear to qrand
    Eigen::VectorXd vrand(vfdim_);
    for (unsigned int i = 0; i < vfdim_; i++)
        vrand[i] = *si_->getStateSpace()->getValueAddressAtIndex(qrand, i) -
                   *si_->getStateSpace()->getValueAddressAtIndex(qnear, i);
    vrand /= si_->distance(qnear, qrand);

    // Get the vector at qnear, and normalize
    Eigen::VectorXd vfield = vf_(qnear);
    const double lambdaScale = vfield.norm();
    // In the case where there is no vector field present, vfield.norm() == 0,
    // return the direction of the random state.
    if (lambdaScale < std::numeric_limits<float>::epsilon())
        return vrand;
    vfield /= lambdaScale;
    // Sample a weight from the distribution
    const double omega = biasedSampling(vrand, vfield, lambdaScale);
    // Determine updated direction
    return computeAlphaBeta(omega, vrand, vfield);
}