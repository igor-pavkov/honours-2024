void OGRKMLLayer::ResetReading()
{
    iNextKMLId_ = 0;
    nLastAsked = -1;
    nLastCount = -1;
}