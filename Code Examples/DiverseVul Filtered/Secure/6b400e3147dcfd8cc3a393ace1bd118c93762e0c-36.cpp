QPainterState *QPaintEngineEx::createState(QPainterState *orig) const
{
    if (!orig)
        return new QPainterState;
    return new QPainterState(orig);
}