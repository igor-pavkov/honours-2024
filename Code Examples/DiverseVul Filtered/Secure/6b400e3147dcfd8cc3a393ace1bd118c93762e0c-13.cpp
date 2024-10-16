QPaintEngineExPrivate::QPaintEngineExPrivate()
    : dasher(&stroker),
      strokeHandler(nullptr),
      activeStroker(nullptr),
      strokerPen(Qt::NoPen)
{
}