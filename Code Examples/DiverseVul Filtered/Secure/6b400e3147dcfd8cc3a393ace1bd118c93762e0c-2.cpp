void QPaintEngineEx::clip(const QRect &r, Qt::ClipOperation op)
{
    qreal right = r.x() + r.width();
    qreal bottom = r.y() + r.height();
    qreal pts[] = { qreal(r.x()), qreal(r.y()),
                    right, qreal(r.y()),
                    right, bottom,
                    qreal(r.x()), bottom,
                    qreal(r.x()), qreal(r.y()) };
    QVectorPath vp(pts, 5, nullptr, QVectorPath::RectangleHint);
    clip(vp, op);
}