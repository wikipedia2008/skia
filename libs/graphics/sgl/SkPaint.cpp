#include "SkPaint.h"
#include "SkColorFilter.h"
#include "SkFontHost.h"
#include "SkMaskFilter.h"
#include "SkPathEffect.h"
#include "SkRasterizer.h"
#include "SkShader.h"
#include "SkScalerContext.h"
#include "SkStroke.h"
#include "SkTextLayout.h"
#include "SkTypeface.h"
#include "SkXfermode.h"

#define SK_DefaultTextSize		SkIntToScalar(12)

SkPaint::SkPaint()
{
	fTypeface	= NULL;
	fTextSize	= SK_DefaultTextSize;
	fTextScaleX	= SK_Scalar1;
	fTextSkewX	= 0;

	fPathEffect	= NULL;
	fShader		= NULL;
	fXfermode	= NULL;
	fMaskFilter	= NULL;
    fColorFilter = NULL;
    fTextLayout = NULL;
    fRasterizer = NULL;

	fColor		= SK_ColorBLACK;
	fWidth		= 0;
	fMiterLimit	= SK_DefaultMiterLimit;
	fFlags		= 0;
	fCapType	= kDefault_Cap;
	fJoinType	= kDefault_Join;
	fFilterType	= kNo_FilterType;
	fTextAlign	= kLeft_Align;
	fStyle		= kFill_Style;
}

SkPaint::SkPaint(const SkPaint& src)
{
	memcpy(this, &src, sizeof(src));

	fTypeface->safeRef();
	fPathEffect->safeRef();
	fShader->safeRef();
	fXfermode->safeRef();
	fMaskFilter->safeRef();
	fColorFilter->safeRef();
    fTextLayout->safeRef();
    fRasterizer->safeRef();
}

SkPaint::~SkPaint()
{
	fTypeface->safeUnref();
	fPathEffect->safeUnref();
	fShader->safeUnref();
	fXfermode->safeUnref();
	fMaskFilter->safeUnref();
	fColorFilter->safeUnref();
    fTextLayout->safeUnref();
    fRasterizer->safeUnref();
}

SkPaint& SkPaint::operator=(const SkPaint& src)
{
	SkASSERT(&src);

	src.fTypeface->safeRef();
	src.fPathEffect->safeRef();
	src.fShader->safeRef();
	src.fXfermode->safeRef();
	src.fMaskFilter->safeRef();
	src.fColorFilter->safeRef();
	src.fTextLayout->safeRef();
	src.fRasterizer->safeRef();

	fTypeface->safeUnref();
	fPathEffect->safeUnref();
	fShader->safeUnref();
	fXfermode->safeUnref();
	fMaskFilter->safeUnref();
	fColorFilter->safeUnref();
	fTextLayout->safeUnref();
	fRasterizer->safeUnref();

	memcpy(this, &src, sizeof(src));

	return *this;
}

int operator==(const SkPaint& a, const SkPaint& b)
{
	return memcmp(&a, &b, sizeof(a)) == 0;
}

void SkPaint::reset()
{
	SkPaint	init;

	*this = init;
}

void SkPaint::setFlags(U32 flags)
{
	fFlags = SkToU8(flags);
}

void SkPaint::setAntiAliasOn(bool doAA)
{
	this->setFlags(SkSetClear32(fFlags, doAA, kAntiAlias_Shift));
}

void SkPaint::setLinearTextOn(bool doLinearText)
{
	this->setFlags(SkSetClear32(fFlags, doLinearText, kLinearText_Shift));
}

void SkPaint::setUnderlineTextOn(bool doUnderline)
{
	this->setFlags(SkSetClear32(fFlags, doUnderline, kUnderlineText_Shift));
}

void SkPaint::setStrikeThruTextOn(bool doStrikeThru)
{
	this->setFlags(SkSetClear32(fFlags, doStrikeThru, kStrikeThruText_Shift));
}

void SkPaint::setFakeBoldTextOn(bool doFakeBold)
{
	this->setFlags(SkSetClear32(fFlags, doFakeBold, kFakeBoldText_Shift));
}

void SkPaint::setStyle(Style style)
{
	SkASSERT((unsigned)style < kStyleCount);
	fStyle = style;
}

void SkPaint::setColor(SkColor color)
{
	fColor = color;
}

void SkPaint::setAlpha(U8CPU a)
{
	fColor = SkColorSetARGB(a, SkColorGetR(fColor), SkColorGetG(fColor), SkColorGetB(fColor));
}

void SkPaint::setARGB(U8CPU a, U8CPU r, U8CPU g, U8CPU b)
{
	fColor = SkColorSetARGB(a, r, g, b);
}

void SkPaint::setStrokeWidth(SkScalar width)
{
	SkASSERT(width >= 0);
	fWidth = width;
}

void SkPaint::setStrokeMiter(SkScalar limit)
{
	SkASSERT(limit >= 0);
	fMiterLimit = limit;
}

void SkPaint::setStrokeCap(Cap ct)
{
	SkASSERT((unsigned)ct < kCapCount);
	fCapType = SkToU8(ct);
}

void SkPaint::setStrokeJoin(Join jt)
{
	SkASSERT((unsigned)jt < kJoinCount);
	fJoinType = SkToU8(jt);
}

void SkPaint::setFilterType(FilterType ft)
{
	SkASSERT((unsigned)ft < kFilterTypeCount);
	fFilterType = SkToU8(ft);
}

//////////////////////////////////////////////////////////////////

void SkPaint::setTextAlign(Align align)
{
	SkASSERT((unsigned)align < kAlignCount);
	fTextAlign = SkToU8(align);
}

void SkPaint::setTextSize(SkScalar ts)
{
	SkASSERT(ts > 0);
	fTextSize = ts;
}

void SkPaint::setTextScaleX(SkScalar scaleX)
{
	fTextScaleX = scaleX;
}

void SkPaint::setTextSkewX(SkScalar skewX)
{
	fTextSkewX = skewX;
}

///////////////////////////////////////////////////////////////////////////////////////

SkTextLayout* SkPaint::setTextLayout(SkTextLayout* layout)
{
    SkRefCnt_SafeAssign(fTextLayout, layout);
    return layout;
}

SkTypeface* SkPaint::setTypeface(SkTypeface* font)
{
	SkRefCnt_SafeAssign(fTypeface, font);
	return font;
}

SkRasterizer* SkPaint::setRasterizer(SkRasterizer* r)
{
    SkRefCnt_SafeAssign(fRasterizer, r);
    return r;
}

///////////////////////////////////////////////////////////////////////////////////////

#include "SkGlyphCache.h"
#include "SkUtils.h"

class SkAutoRestorePaintTextSizeAndFrame {
public:
	SkAutoRestorePaintTextSizeAndFrame(const SkPaint* paint) : fPaint((SkPaint*)paint)
	{
		fTextSize = paint->getTextSize();
		fStyle = paint->getStyle();
		fPaint->setStyle(SkPaint::kFill_Style);
	}
	~SkAutoRestorePaintTextSizeAndFrame()
	{
		fPaint->setStyle(fStyle);
		fPaint->setTextSize(fTextSize);
	}
    
private:
	SkPaint*		fPaint;
	SkScalar		fTextSize;
	SkPaint::Style	fStyle;
};

static SkScalar measure_text(const SkPaint& paint, SkGlyphCache* cache,
                             SkUnicodeWalkerProc proc, const char* text, size_t byteLength,
                             int* count)
{
	SkASSERT(count);

	SkFixed         x = 0;
	int             n;
    SkTextLayout*   layout = paint.getTextLayout();
    
    if (layout)
    {
        SkAutoSTMalloc<32, SkTextLayout::Rec>   storage(byteLength);
        SkTextLayout::Rec*                      rec = storage.get();

        n = layout->layout(paint, text, byteLength, proc, rec);
        for (int i = 0; i < n; i++)
        {
            // should pass rec[i].glyphID when we have it
            x += cache->getMetrics(rec[i].charCode()).fAdvanceX + SkScalarToFixed(rec[i].fDeltaAdvance);
        }
    }
    else
    {
        const char*	stop = (const char*)text + byteLength;
        for (n = 0; text < stop; n++)
        {
            x += cache->getMetrics(proc(&text)).fAdvanceX;
        }
        SkASSERT(text == stop);
    }
	*count = n;
	return SkFixedToScalar(x);
}

SkScalar SkPaint::privateMeasureText(SkUnicodeWalkerProc textProc,
                                     const char* text, size_t byteLength,
                                     SkScalar* above, SkScalar* below) const
{
	SkASSERT(text != NULL || byteLength == 0);

	SkScalar                            scale = 0;
    SkAutoRestorePaintTextSizeAndFrame	restore(this);

	if (this->isLinearTextOn())
	{
		scale = fTextSize / kCanonicalTextSizeForPaths;
		// this gets restored by restore
		((SkPaint*)this)->setTextSize(SkIntToScalar(kCanonicalTextSizeForPaths));
	}

	SkAutoGlyphCache	autoCache(*this, NULL);
	SkGlyphCache*		cache = autoCache.getCache();

	if (above || below)
	{
		SkPoint	abovePt, belowPt;
		cache->getLineHeight(&abovePt, &belowPt);
		if (scale)
		{
			abovePt.fY = SkScalarMul(abovePt.fY, scale);
			belowPt.fY = SkScalarMul(belowPt.fY, scale);
		}
		if (above)
			*above = abovePt.fY;
		if (below)
			*below = belowPt.fY;
	}
    
    SkScalar width = 0;
    
    if (byteLength)
    {
        int count;
        width = measure_text(*this, cache, textProc, text, byteLength, &count);

        if (scale)
            width = SkScalarMul(width, scale);
    }
    return width;
}

SkScalar SkPaint::measureText(const char utf8[], size_t length,
							  SkScalar* above, SkScalar* below) const
{
    return this->privateMeasureText((SkUnicodeWalkerProc)SkUTF8_NextUnichar, utf8, length, above, below);
}

SkScalar SkPaint::measureText16(const U16 utf16[], size_t numberOf16BitValues,
							  SkScalar* above, SkScalar* below) const
{
    return this->privateMeasureText((SkUnicodeWalkerProc)SkUTF16_NextUnichar, (const char*)utf16, numberOf16BitValues << 1, above, below);
}

////////////////////////////////////////////////////////////////////////////////////////////

int SkPaint::privateGetTextWidths(const char text[], size_t byteLength,
                                  SkScalar widths[], SkUnicodeWalkerProc proc) const
{
	SkASSERT(text != NULL && byteLength > 0);

	SkAutoRestorePaintTextSizeAndFrame	restore(this);
	SkScalar							scale = 0;

	if (this->isLinearTextOn())
	{
		scale = fTextSize / kCanonicalTextSizeForPaths;
		// this gets restored by restore
		((SkPaint*)this)->setTextSize(SkIntToScalar(kCanonicalTextSizeForPaths));
	}

	SkAutoGlyphCache	autoCache(*this, NULL);
	SkGlyphCache*		cache = autoCache.getCache();

    SkScalar*   w = widths;
	const char*	stop = (const char*)text + byteLength;
    if (scale) {
        while (text < stop)
            *w++ = SkScalarMul(SkFixedToScalar(cache->getMetrics(proc(&text)).fAdvanceX), scale);
    }
    else {
        while (text < stop)
            *w++ = SkFixedToScalar(cache->getMetrics(proc(&text)).fAdvanceX);
    }
    return w - widths;  // count
}

int SkPaint::getTextWidths(const char text[], size_t byteLength, SkScalar widths[]) const
{
    if (0 == byteLength)
        return 0;
    
    if (NULL == widths)
        return SkUTF8_CountUnichars(text, byteLength);
    
    int count = this->privateGetTextWidths(text, byteLength, widths, (SkUnicodeWalkerProc)SkUTF8_NextUnichar);
    SkASSERT(SkUTF8_CountUnichars(text, byteLength) == count);
    return count;
}

int SkPaint::getTextWidths16(const uint16_t text[], size_t numberOf16BitValues, SkScalar widths[]) const
{
    if (0 == numberOf16BitValues)
        return 0;
    
    if (NULL == widths)
        return SkUTF16_CountUnichars(text, numberOf16BitValues);
    
    int count = this->privateGetTextWidths((const char*)text, numberOf16BitValues << 1, widths, (SkUnicodeWalkerProc)SkUTF16_NextUnichar);
    SkASSERT(SkUTF16_CountUnichars(text, numberOf16BitValues) == count);
    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////

SkScalar SkPaint::ascent() const
{
    SkScalar above;
    (void)this->measureText(NULL, 0, &above, NULL);
    return above;
}

SkScalar SkPaint::descent() const
{
    SkScalar below;
    (void)this->measureText(NULL, 0, NULL, &below);
    return below;
}

#include "SkDraw.h"

void SkPaint::privateGetTextPath(SkUnicodeWalkerProc textProc, const char text[], size_t length, SkScalar x, SkScalar y, SkPath* path) const
{
	SkASSERT(length == 0 || text != NULL);
	if (text == NULL || length == 0 || path == NULL)
		return;
    SkASSERT(textProc);

	SkTextToPathIter	iter(textProc, text, length, *this, false, true);
	SkMatrix			matrix;
	SkScalar			prevXPos = 0;

	matrix.setScale(iter.getPathScale(), iter.getPathScale(), 0, 0);
	matrix.postTranslate(x, y);
	path->reset();

	SkScalar		xpos;
	const SkPath*	iterPath;
	while ((iterPath = iter.next(&xpos)) != NULL)
	{
		matrix.postTranslate(xpos - prevXPos, 0);
		path->addPath(*iterPath, matrix);
		prevXPos = xpos;
	}
}

void SkPaint::getTextPath(const char text[], size_t length, SkScalar x, SkScalar y, SkPath* path) const
{
    this->privateGetTextPath((SkUnicodeWalkerProc)SkUTF8_NextUnichar, text, length, x, y, path);
}

void SkPaint::getText16Path(const U16 text[], size_t numberOf16BitValues, SkScalar x, SkScalar y, SkPath* path) const
{
    this->privateGetTextPath((SkUnicodeWalkerProc)SkUTF16_NextUnichar, (const char*)text, numberOf16BitValues << 1, x, y, path);
}

static void add_flattenable(SkDescriptor* desc, U32 tag, U32 len, SkFlattenable* obj)
{
	SkFlattenable::Factory fact = obj->getFactory();
	SkASSERT(fact);

	SkWBuffer   buffer(desc->addEntry(tag, sizeof(void*) + len, NULL), sizeof(void*) + len);
 
    buffer.writePtr((const void*)fact);
    obj->flatten(buffer);
    SkASSERT(buffer.pos() == buffer.size());
}

/*
 *  interpolates to find the right value for key, in the function represented by the 'length' number of pairs: (keys[i], values[i])
    inspired by a desire to change the multiplier for thickness in fakebold
    therefore, i assumed number of pairs (length) will be small, so a linear search is sufficient
    repeated keys are allowed for discontinuous functions (so long as keys is monotonically increasing), and if 
    	key is the value of a repeated scalar in keys, the first one will be used 
	- this may change if a binary search is used
	- also, this ensures that there is no divide by zero (an assert also checks for that)
*/
static SkScalar interpolate(SkScalar key, const SkScalar keys[], const SkScalar values[], int length)
{

    SkASSERT(length > 0);
    SkASSERT(keys != NULL);    
    SkASSERT(values != NULL);
#ifdef SK_DEBUG
    for (int i = 1; i < length; i++)
        SkASSERT(keys[i] >= keys[i-1]);
#endif
    int right = 0;
    while (right < length && key > keys[right])
        right++;
    //could use sentinal values to eliminate conditionals
    //i assume i am not in control of input values, so i want to make it simple
    if (length == right)
        return values[length-1];
    if (0 == right)
        return values[0];
    //otherwise, we interpolate between right-1 and right
    SkScalar rVal = values[right];
    SkScalar lVal = values[right-1];
    SkScalar rightKey = keys[right];
    SkScalar leftKey = keys[right-1];
    SkASSERT(rightKey != leftKey);
    //fractional amount which we will multiply by the difference in the left value and right value
    SkScalar fract = SkScalarDiv(key-leftKey,rightKey-leftKey);
    return lVal + SkScalarMul(fract, rVal-lVal);
}

//used for interpolating in fakeBold
static const SkScalar pointSizes[] = { SkIntToScalar(9), SkIntToScalar(36) };
static const SkScalar multipliers[] = { SK_Scalar1/24, SK_Scalar1/32 };

void SkScalerContext::MakeRec(const SkPaint& paint, const SkMatrix* deviceMatrix, Rec* rec)
{
	SkASSERT(deviceMatrix == NULL || (deviceMatrix->getType() & SkMatrix::kPerspective_Mask) == 0);

	rec->fTextSize = paint.getTextSize();
	rec->fPreScaleX	= paint.getTextScaleX();
	rec->fPreSkewX	= paint.getTextSkewX();

	if (deviceMatrix)
	{
		rec->fPost2x2[0][0] = deviceMatrix->getScaleX();
		rec->fPost2x2[0][1] = deviceMatrix->getSkewX();
		rec->fPost2x2[1][0] = deviceMatrix->getSkewY();
		rec->fPost2x2[1][1] = deviceMatrix->getScaleY();
	}
	else
	{
		rec->fPost2x2[0][0] = rec->fPost2x2[1][1] = SK_Scalar1;
		rec->fPost2x2[0][1] = rec->fPost2x2[1][0] = 0;
	}
    
    SkPaint::Style  style = paint.getStyle();
    SkScalar        strokeWidth = paint.getStrokeWidth();
    
    if (paint.isFakeBoldTextOn())
    {
        SkScalar fakeBoldScale = interpolate(paint.getTextSize(), pointSizes, multipliers, 2);
        SkScalar extra = SkScalarMul(paint.getTextSize(), fakeBoldScale);
        
        if (style == SkPaint::kFill_Style)
        {
            style = SkPaint::kStrokeAndFill_Style;
            strokeWidth = extra;    // ignore paint's strokeWidth if it was "fill"
        }
        else
            strokeWidth += extra;
    }

	if (style != SkPaint::kFill_Style && strokeWidth > 0)
	{
		rec->fFrameWidth = strokeWidth;
		rec->fMiterLimit = paint.getStrokeMiter();
		rec->fFrameAndFill = SkToU8(style == SkPaint::kStrokeAndFill_Style);
		rec->fStrokeJoin = SkToU8(paint.getStrokeJoin());
	}
	else
	{
		rec->fFrameWidth = 0;
		rec->fMiterLimit = 0;
		rec->fFrameAndFill = false;
		rec->fStrokeJoin = 0;
	}

	rec->fUseHints = SkToU8(!paint.isLinearTextOn());
	rec->fDoAA = SkToU8(paint.isAntiAliasOn());
}

SkGlyphCache* SkPaint::detachCache(const SkMatrix* deviceMatrix) const
{
	SkScalerContext::Rec	rec;

	SkScalerContext::MakeRec(*this, deviceMatrix, &rec);

	size_t			descSize = sizeof(rec);
	int				entryCount = 2;	// scalerrec + typeface
	SkTypeface*		tf = this->getTypeface();
	size_t			tfSize = 0;
	SkPathEffect*	pe = this->getPathEffect();
	size_t			peLen = 0;
	SkMaskFilter*	mf = this->getMaskFilter();
	size_t			mfLen = 0;
	SkRasterizer*	ra = this->getRasterizer();
	size_t			raLen = 0;

    // we always do this, even if tf is NULL
    tfSize = SkFontHost::FlattenTypeface(tf, NULL);
    descSize += tfSize;

	if (pe)
	{
		if (pe->getFactory())
		{
            SkWBuffer   buffer;
			pe->flatten(buffer);
            peLen = buffer.pos();
			descSize += sizeof(SkFlattenable::Factory) + peLen;
			entryCount += 1;
			rec.fDoAA = true;	// force antialiasing when we do the scan conversion
		}
		else
			pe = NULL;
	}
	if (mf)
	{
		if (mf->getFactory())
		{
            SkWBuffer   buffer;
            mf->flatten(buffer);
			mfLen = buffer.pos();
			descSize += sizeof(SkFlattenable::Factory) + mfLen;
			entryCount += 1;
			rec.fDoAA = true;	// force antialiasing with maskfilters
		}
		else
			mf = NULL;
	}
	if (ra)
	{
		if (ra->getFactory())
		{
            SkWBuffer   buffer;
            ra->flatten(buffer);
			raLen = buffer.pos();
			descSize += sizeof(SkFlattenable::Factory) + raLen;
			entryCount += 1;
			rec.fDoAA = true;	// force antialiasing when we do the scan conversion
		}
		else
			ra = NULL;
	}
	descSize += SkDescriptor::ComputeOverhead(entryCount);

	SkAutoDescriptor	ad(descSize);
	SkDescriptor*		desc = ad.getDesc();

	desc->init();
	desc->addEntry(kRec_SkDescriptorTag, sizeof(rec), &rec);

    // we always do this, even if tf is NULL
    {
        SkDEBUGCODE(size_t tfSize2 = ) SkFontHost::FlattenTypeface(tf, desc->addEntry(kTypeface_SkDescriptorTag, tfSize, NULL));
		SkASSERT(tfSize2 == tfSize);
	}

	if (pe)
		add_flattenable(desc, kPathEffect_SkDescriptorTag, peLen, pe);
	if (mf)
		add_flattenable(desc, kMaskFilter_SkDescriptorTag, mfLen, mf);
	if (ra)
		add_flattenable(desc, kRasterizer_SkDescriptorTag, raLen, ra);

	SkASSERT(descSize == desc->getLength());
	desc->computeChecksum();

	return SkGlyphCache::DetachCache(desc);
}

//////////////////////////////////////////////////////////////////

SkShader* SkPaint::setShader(SkShader* shader)
{
	SkRefCnt_SafeAssign(fShader, shader);
	return shader;
}

SkColorFilter* SkPaint::setColorFilter(SkColorFilter* filter)
{
	SkRefCnt_SafeAssign(fColorFilter, filter);
	return filter;
}

SkXfermode* SkPaint::setXfermode(SkXfermode* mode)
{
	SkRefCnt_SafeAssign(fXfermode, mode);
	return mode;
}

SkXfermode* SkPaint::setPorterDuffXfermode(SkPorterDuff::Mode mode)
{
    fXfermode->safeUnref();
    fXfermode = SkPorterDuff::CreateXfermode(mode);
	return fXfermode;
}

SkPathEffect* SkPaint::setPathEffect(SkPathEffect* effect)
{
	SkRefCnt_SafeAssign(fPathEffect, effect);
	return effect;
}

SkMaskFilter* SkPaint::setMaskFilter(SkMaskFilter* filter)
{
	SkRefCnt_SafeAssign(fMaskFilter, filter);
	return filter;
}

////////////////////////////////////////////////////////////////////////////////////////

bool SkPaint::getFillPath(const SkPath& src, SkPath* dst) const
{
    SkPath          effectPath, strokePath;
    const SkPath*   path = &src;

	SkScalar width = this->getStrokeWidth();
    
    switch (this->getStyle()) {
    case SkPaint::kFill_Style:
        width = -1; // mark it as no-stroke
        break;
    case SkPaint::kStrokeAndFill_Style:
        if (width == 0)
            width = -1; // mark it as no-stroke
        break;
    case SkPaint::kStroke_Style:
        break;
    default:
        SkASSERT(!"unknown paint style");
    }

	if (this->getPathEffect())
	{
        // lie to the pathEffect if our style is strokeandfill, so that it treats us as just fill
        if (this->getStyle() == SkPaint::kStrokeAndFill_Style)
            width = -1; // mark it as no-stroke

		if (this->getPathEffect()->filterPath(&effectPath, src, &width))
			path = &effectPath;
        
        // restore the width if we earlier had to lie, and if we're still set to no-stroke
        // note: if we're now stroke (width >= 0), then the pathEffect asked for that change
        // and we want to respect that (i.e. don't overwrite their setting for width)
        if (this->getStyle() == SkPaint::kStrokeAndFill_Style && width < 0)
        {
            width = this->getStrokeWidth();
            if (width == 0)
                width = -1;
        }
	}
    
    if (width > 0 && !path->isEmpty())
    {
		SkStroke stroker(*this, width);
		stroker.strokePath(*path, &strokePath);
        path = &strokePath;
	}

    if (path == &src)
        *dst = src;
    else
    {
        SkASSERT(path == &effectPath || path == &strokePath);
        dst->swap(*(SkPath*)path);
    }

    return width != 0;  // return true if we're filled, or false if we're hairline (width == 0)
}

////////////////////////////////////////////////////////////////////////////////////////

static bool has_thick_frame(const SkPaint& paint)
{
	return paint.getStrokeWidth() > 0 && paint.getStyle() != SkPaint::kFill_Style;
}

SkTextToPathIter::SkTextToPathIter( SkUnicodeWalkerProc textProc,
                                    const char text[], size_t length,
                                    const SkPaint& paint,
                                    bool applyStrokeAndPathEffects,
                                    bool forceLinearTextOn)
                                    : fPaint(paint), fTextProc(textProc)
{
	if (forceLinearTextOn)
		fPaint.setLinearTextOn(true);
	fPaint.setMaskFilter(NULL);   // don't want this affecting our path-cache lookup

	if (fPaint.getPathEffect() == NULL && !has_thick_frame(fPaint))
		applyStrokeAndPathEffects = false;

	// can't use our canonical size if we need to apply patheffects/strokes
	if (fPaint.isLinearTextOn() && !applyStrokeAndPathEffects)
	{
		fPaint.setTextSize(SkIntToScalar(SkPaint::kCanonicalTextSizeForPaths));
		fScale = paint.getTextSize() / SkPaint::kCanonicalTextSizeForPaths;
	}
	else
		fScale = SK_Scalar1;
	
	if (!applyStrokeAndPathEffects)
	{
		fPaint.setStyle(SkPaint::kFill_Style);
		fPaint.setPathEffect(NULL);
	}

	fCache = SkGlyphCache::DetachCache(fPaint, NULL);

	SkPaint::Style	style = SkPaint::kFill_Style;
	SkPathEffect*	pe = NULL;

	if (!applyStrokeAndPathEffects)
	{
		style = paint.getStyle();	// restore
		pe = paint.getPathEffect();		// restore
	}
	fPaint.setStyle(style);
	fPaint.setPathEffect(pe);
	fPaint.setMaskFilter(paint.getMaskFilter());	// restore

	// now compute fXOffset if needed

	SkScalar xOffset = 0;
	if (paint.getTextAlign() != SkPaint::kLeft_Align)	// need to measure first
	{
		int		 count;
		SkScalar width = SkScalarMul(measure_text(paint, fCache, textProc, text, length, &count), fScale);
		if (paint.getTextAlign() == SkPaint::kCenter_Align)
			width = SkScalarHalf(width);
		xOffset = -width;
	}
	fXPos = xOffset; // + SkScalarHalf(paint.getTextTracking()); do we need to return the textlayout's first deltaAdvance?
	fPrevAdvance = 0;

	fText = text;
	fStop = text + length;
}

SkTextToPathIter::~SkTextToPathIter()
{
	SkGlyphCache::AttachCache(fCache);
}

const SkPath* SkTextToPathIter::next(SkScalar* xpos)
{
	while (fText < fStop)
	{
		const SkGlyph& glyph = fCache->getMetrics(fTextProc(&fText));

		fXPos += fPrevAdvance;
		fPrevAdvance = SkScalarMul(SkFixedToScalar(glyph.fAdvanceX), fScale);   // + fPaint.getTextTracking();

		if (glyph.fWidth)
		{
			if (xpos)
				*xpos = fXPos;
			return fCache->findPath(glyph.fCharCode);
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

SkTypeface* SkTypeface::Create(const char name[], Style style)
{
	return SkFontHost::CreateTypeface(NULL, name, style);
}

SkTypeface* SkTypeface::CreateFromTypeface(const SkTypeface* family, Style style)
{
	return SkFontHost::CreateTypeface(family, NULL, style);
}

