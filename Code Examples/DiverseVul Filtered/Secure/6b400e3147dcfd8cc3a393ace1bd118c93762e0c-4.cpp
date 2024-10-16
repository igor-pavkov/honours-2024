QPaintEngineEx::QPaintEngineEx()
    : QPaintEngine(*new QPaintEngineExPrivate, AllFeatures)
{
    extended = true;
}