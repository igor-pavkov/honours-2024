void QPaintEngineEx::drawStaticTextItem(QStaticTextItem *staticTextItem)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    if (staticTextItem->numGlyphs == 0)
        return;

    QFontEngine *fontEngine = staticTextItem->fontEngine();
    fontEngine->addGlyphsToPath(staticTextItem->glyphs, staticTextItem->glyphPositions,
                                staticTextItem->numGlyphs, &path, { });
    if (!path.isEmpty()) {
        QPainterState *s = state();
        QPainter::RenderHints oldHints = s->renderHints;
        bool changedHints = false;
        if (bool(oldHints & QPainter::TextAntialiasing)
            && !bool(fontEngine->fontDef.styleStrategy & QFont::NoAntialias)
            && !bool(oldHints & QPainter::Antialiasing)) {
            s->renderHints |= QPainter::Antialiasing;
            renderHintsChanged();
            changedHints = true;
        }

        fill(qtVectorPathForPath(path), s->pen.brush());

        if (changedHints) {
            s->renderHints = oldHints;
            renderHintsChanged();
        }
    }
}