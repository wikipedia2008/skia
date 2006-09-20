#include "SkNinePatch.h"
#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkPaint.h"

#define USE_TRACEx

#ifdef USE_TRACE
    static bool gTrace;
#endif

static void* getSubAddr(const SkBitmap& bm, int x, int y)
{
    SkASSERT((unsigned)x < bm.width());
    SkASSERT((unsigned)y < bm.height());
    
    switch (bm.getConfig()) {
    case SkBitmap::kNo_Config:
    case SkBitmap::kA1_Config:
        SkASSERT(!"unsupported config for ninepatch");
        break;
    case SkBitmap::kA8_Config:
    case SkBitmap:: kIndex8_Config:
        break;
    case SkBitmap::kRGB_565_Config:
        x <<= 1;
        break;
	case SkBitmap::kARGB_8888_Config:
        x <<= 2;
        break;
    default:
        break;
    }
    return (char*)bm.getPixels() + x + y * bm.rowBytes();
}

static void drawPatch(SkCanvas* canvas, const SkRect16& src, const SkRect& dst,
                      const SkBitmap& bitmap, const SkPaint& paint)
{
#ifdef USE_TRACE
    if (gTrace) SkDebugf("======== ninepatch src [%d %d %d,%d] dst[%g %g %g,%g]\n",
        src.fLeft, src.fTop, src.width(), src.height(),
        SkScalarToFloat(dst.fLeft), SkScalarToFloat(dst.fTop),
        SkScalarToFloat(dst.width()), SkScalarToFloat(dst.height()));
#endif

    SkBitmap    tmp;
    
    tmp.setConfig(bitmap.getConfig(), src.width(), src.height(), bitmap.rowBytes());
    tmp.setPixels(getSubAddr(bitmap, src.fLeft, src.fTop));

    SkMatrix    matrix;
    SkRect      tmpSrc;
    tmpSrc.set(0, 0, SkIntToScalar(tmp.width()), SkIntToScalar(tmp.height()));
    matrix.setRectToRect(tmpSrc, dst, SkMatrix::kFill_ScaleToFit);

    canvas->save();
//    canvas->clipRect(dst);    try not to use, as it will suck visually if we're rotated (and make us way-slow)
    canvas->concat(matrix);
    canvas->drawBitmap(tmp, 0, 0, paint);
    canvas->restore();
}

static bool pixel_is_transparent(const SkBitmap& bm, int x, int y)
{
    switch (bm.getConfig()) {
    case SkBitmap::kARGB_8888_Config:
        return 0 == *bm.getAddr32(x, y);
    default:
        return false;
    }
}

static void drawColumn(SkCanvas* canvas, SkRect16* src, SkRect* dst,
                       const SkRect& bounds, const SkRect16& mar,
                       const SkBitmap& bitmap, const SkPaint& paint)
{
// upper row
    src->fTop = 0;
    src->fBottom = mar.fTop;
    dst->fTop = bounds.fTop;
    dst->fBottom = bounds.fTop + SkIntToScalar(mar.fTop);
    drawPatch(canvas, *src, *dst, bitmap, paint);  
// mid row
    src->fTop = src->fBottom;
    src->fBottom = bitmap.height() - mar.fBottom;
    dst->fTop = dst->fBottom;
    dst->fBottom = bounds.fBottom - SkIntToScalar(mar.fBottom);
    if (src->width() != 1 || src->height() != 1 ||
        !pixel_is_transparent(bitmap, src->fLeft, src->fTop))
    {
        drawPatch(canvas, *src, *dst, bitmap, paint);
    }
    else
    {
    //    SkDEBUGF(("========= Skip transparent center of ninepatch\n"));
    }
// lower row
    src->fTop = src->fBottom;
    src->fBottom = bitmap.height();
    dst->fTop = dst->fBottom;
    dst->fBottom = bounds.fBottom;
    drawPatch(canvas, *src, *dst, bitmap, paint);  
}

void SkNinePatch::Draw(SkCanvas* canvas, const SkRect& bounds,
                     const SkBitmap& bitmap, const SkRect16& margin,
                     const SkPaint* paint)
{
#ifdef USE_TRACE
    if (10 == margin.fLeft) gTrace = true;
#endif

    SkASSERT(canvas);

#ifdef USE_TRACE
    if (gTrace) SkDebugf("======== ninepatch bounds [%g %g]\n", SkScalarToFloat(bounds.width()), SkScalarToFloat(bounds.height()));
    if (gTrace) SkDebugf("======== ninepatch paint bm [%d,%d]\n", bitmap.width(), bitmap.height());
    if (gTrace) SkDebugf("======== ninepatch margin [%d,%d,%d,%d]\n", margin.fLeft, margin.fTop, margin.fRight, margin.fBottom);
#endif


    if (bounds.isEmpty() ||
        bitmap.width() == 0 || bitmap.height() == 0 || bitmap.getPixels() == NULL ||
        paint && paint->getXfermode() == NULL && paint->getAlpha() == 0)
    {
#ifdef USE_TRACE
        if (gTrace) SkDebugf("======== abort ninepatch draw\n");
#endif
        return;
    }

    SkPaint defaultPaint;
    if (NULL == paint)
        paint = &defaultPaint;

    SkRect      dst;
    SkRect16    src, mar;

    mar.set(SkMax32(margin.fLeft, 0), SkMax32(margin.fTop, 0),
            SkMax32(margin.fRight, 0), SkMax32(margin.fBottom, 0));

// left column
    src.fLeft = 0;
    src.fRight = mar.fLeft;
    dst.fLeft = bounds.fLeft;
    dst.fRight = bounds.fLeft + SkIntToScalar(mar.fLeft);
    drawColumn(canvas, &src, &dst, bounds, mar, bitmap, *paint);
// mid column
    src.fLeft = src.fRight;
    src.fRight = bitmap.width() - mar.fRight;
    dst.fLeft = dst.fRight;
    dst.fRight = bounds.fRight - SkIntToScalar(mar.fRight);
    drawColumn(canvas, &src, &dst, bounds, mar, bitmap, *paint);
// right column
    src.fLeft = src.fRight;
    src.fRight = bitmap.width();
    dst.fLeft = dst.fRight;
    dst.fRight = bounds.fRight;
    drawColumn(canvas, &src, &dst, bounds, mar, bitmap, *paint);

#ifdef USE_TRACE
    gTrace = false;
#endif
}

void SkNinePatch::Draw(SkCanvas* canvas, const SkRect& bounds,
                     const SkBitmap& bitmap, int cx, int cy,
                     const SkPaint* paint)
{
    SkRect16    marginRect;
    
    marginRect.set(cx, cy, bitmap.width() - cx - 1, bitmap.height() - cy - 1);

    SkNinePatch::Draw(canvas, bounds, bitmap, marginRect, paint);
}

