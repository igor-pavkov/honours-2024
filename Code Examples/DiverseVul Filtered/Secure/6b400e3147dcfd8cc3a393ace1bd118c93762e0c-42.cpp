void QPaintEngineEx::draw(const QVectorPath &path)
{
    const QBrush &brush = state()->brush;
    if (qbrush_style(brush) != Qt::NoBrush)
        fill(path, brush);

    const QPen &pen = state()->pen;
    if (qpen_style(pen) != Qt::NoPen && qbrush_style(qpen_brush(pen)) != Qt::NoBrush)
        stroke(path, pen);
}