Q_GUI_EXPORT QPainterPath qt_painterPathFromVectorPath(const QVectorPath &path)
{
    const qreal *points = path.points();
    const QPainterPath::ElementType *types = path.elements();

    QPainterPath p;
    if (types) {
        int id = 0;
        for (int i=0; i<path.elementCount(); ++i) {
            switch(types[i]) {
            case QPainterPath::MoveToElement:
                p.moveTo(QPointF(points[id], points[id+1]));
                id+=2;
                break;
            case QPainterPath::LineToElement:
                p.lineTo(QPointF(points[id], points[id+1]));
                id+=2;
                break;
            case QPainterPath::CurveToElement: {
                QPointF p1(points[id], points[id+1]);
                QPointF p2(points[id+2], points[id+3]);
                QPointF p3(points[id+4], points[id+5]);
                p.cubicTo(p1, p2, p3);
                id+=6;
                break;
            }
            case QPainterPath::CurveToDataElement:
                ;
                break;
            }
        }
    } else {
        p.moveTo(QPointF(points[0], points[1]));
        int id = 2;
        for (int i=1; i<path.elementCount(); ++i) {
            p.lineTo(QPointF(points[id], points[id+1]));
            id+=2;
        }
    }
    if (path.hints() & QVectorPath::WindingFill)
        p.setFillRule(Qt::WindingFill);

    return p;
}