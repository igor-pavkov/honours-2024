bool QPaintEngineExPrivate::hasClipOperations() const
{
    Q_Q(const QPaintEngineEx);

    QPainter *p = q->painter();
    if (!p || !p->d_ptr)
        return false;

    return !p->d_ptr->state->clipInfo.isEmpty();
}