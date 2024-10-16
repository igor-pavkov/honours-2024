void QPaintEngineEx::clip(const QRegion &region, Qt::ClipOperation op)
{
    const auto rectsInRegion = region.rectCount();
    if (rectsInRegion == 1) {
        clip(*region.begin(), op);
    } else if (rectsInRegion <= 32) {
        qreal pts[2*32*4];
        int pos = 0;
        for (QRect r : region) {
            qreal x1 = r.x();
            qreal y1 = r.y();
            qreal x2 = r.x() + r.width();
            qreal y2 = r.y() + r.height();

            pts[pos++] = x1;
            pts[pos++] = y1;

            pts[pos++] = x2;
            pts[pos++] = y1;

            pts[pos++] = x2;
            pts[pos++] = y2;

            pts[pos++] = x1;
            pts[pos++] = y2;
        }
        QVectorPath vp(pts, rectsInRegion * 4, qpaintengineex_rect4_types_32);
        clip(vp, op);
    } else {
        QVarLengthArray<qreal> pts(rectsInRegion * 2 * 4);
        QVarLengthArray<QPainterPath::ElementType> types(rectsInRegion * 4);
        int ppos = 0;
        int tpos = 0;

        for (QRect r : region) {
            qreal x1 = r.x();
            qreal y1 = r.y();
            qreal x2 = r.x() + r.width();
            qreal y2 = r.y() + r.height();

            pts[ppos++] = x1;
            pts[ppos++] = y1;

            pts[ppos++] = x2;
            pts[ppos++] = y1;

            pts[ppos++] = x2;
            pts[ppos++] = y2;

            pts[ppos++] = x1;
            pts[ppos++] = y2;

            types[tpos++] = QPainterPath::MoveToElement;
            types[tpos++] = QPainterPath::LineToElement;
            types[tpos++] = QPainterPath::LineToElement;
            types[tpos++] = QPainterPath::LineToElement;
        }

        QVectorPath vp(pts.data(), rectsInRegion * 4, types.data());
        clip(vp, op);
    }

}