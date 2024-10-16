QPaintEngineEx::QPaintEngineEx(QPaintEngineExPrivate &data)
    : QPaintEngine(data, AllFeatures)
{
    extended = true;
}