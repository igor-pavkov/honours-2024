void QPaintEngineEx::drawLines(const QLine *lines, int lineCount)
{
    int elementCount = lineCount << 1;
    while (elementCount > 0) {
        int count = qMin(elementCount, 32);

        qreal pts[64];
        int count2 = count<<1;
        for (int i=0; i<count2; ++i)
            pts[i] = ((const int *) lines)[i];

        QVectorPath path(pts, count, qpaintengineex_line_types_16, QVectorPath::LinesHint);
        stroke(path, state()->pen);

        elementCount -= 32;
        lines += 16;
    }
}