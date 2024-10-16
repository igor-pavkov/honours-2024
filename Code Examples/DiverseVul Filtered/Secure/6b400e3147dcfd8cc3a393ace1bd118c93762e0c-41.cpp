void QPaintEngineExPrivate::replayClipOperations()
{
    Q_Q(QPaintEngineEx);

    QPainter *p = q->painter();
    if (!p || !p->d_ptr)
        return;

    const QList<QPainterClipInfo> &clipInfo = p->d_ptr->state->clipInfo;

    QTransform transform = q->state()->matrix;

    for (const QPainterClipInfo &info : clipInfo) {

        if (info.matrix != q->state()->matrix) {
            q->state()->matrix = info.matrix;
            q->transformChanged();
        }

        switch (info.clipType) {
        case QPainterClipInfo::RegionClip:
            q->clip(info.region, info.operation);
            break;
        case QPainterClipInfo::PathClip:
            q->clip(info.path, info.operation);
            break;
        case QPainterClipInfo::RectClip:
            q->clip(info.rect, info.operation);
            break;
        case QPainterClipInfo::RectFClip: {
            qreal right = info.rectf.x() + info.rectf.width();
            qreal bottom = info.rectf.y() + info.rectf.height();
            qreal pts[] = { info.rectf.x(), info.rectf.y(),
                            right, info.rectf.y(),
                            right, bottom,
                            info.rectf.x(), bottom };
            QVectorPath vp(pts, 4, nullptr, QVectorPath::RectangleHint);
            q->clip(vp, info.operation);
            break;
            }
        }
    }

    if (transform != q->state()->matrix) {
        q->state()->matrix = transform;
        q->transformChanged();
    }
}