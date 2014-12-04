// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#if MOAI_WITH_LIBPNG
  #include <png.h>
#endif

#include <moai-sim/MOAIImage.h>
#include <moai-sim/MOAIGfxDevice.h>
#include <float.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@lua	bleedRect
	@text	'Bleeds' the interior of the rectangle out by one pixel.

	@in		MOAIImage self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
int MOAIImage::_bleedRect ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	ZLIntRect rect;
	
	rect.mXMin	= state.GetValue < int >( 2, 0 );
	rect.mYMin	= state.GetValue < int >( 3, 0 );
	rect.mXMax	= state.GetValue < int >( 4, 0 );
	rect.mYMax	= state.GetValue < int >( 5, 0 );
	
	self->BleedRect ( rect );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	compare
	@text	Compares the image to another image.
	
	@in		MOAIImage self
	@in		MOAIImage other
	@out	boolean areEqual	A value indicating whether the images are equal.
*/

int MOAIImage::_compare ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UU" )

	MOAIImage* image = state.GetLuaObject < MOAIImage >( 2, true );

	lua_pushboolean ( state, self->Compare ( *image ));

	return 1;
}

//----------------------------------------------------------------//
/**	@lua	convert
	@text	Return a copy of the image with a new color format. Not
			all provided formats are supported by OpenGL. 'nil' may be
			passed for either value, in which case the format will match
			the original.

	@in		MOAIImage self
	@opt	number colorFmt		One of MOAIImage.COLOR_FMT_A_1, MOAIImage.COLOR_FMT_A_4, MOAIImage.COLOR_FMT_A_8,
								MOAIImage.COLOR_FMT_RGB_888, MOAIImage.COLOR_FMT_RGB_565, MOAIImage.COLOR_FMT_RGBA_5551,
								MOAIImage.COLOR_FMT_RGBA_4444, COLOR_FMT_RGBA_8888
	@opt	number pixelFmt		One of MOAIImage.PIXEL_FMT_TRUECOLOR, MOAIImage.PIXEL_FMT_INDEX_4, MOAIImage.PIXEL_FMT_INDEX_8
	@out	MOAIImage image		Copy of the image initialized with given format.
*/
int MOAIImage::_convert ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	u32 colorFmt = state.GetValue < u32 >( 2, self->mColorFormat );
	u32 pixelFmt = state.GetValue < u32 >( 3, self->mPixelFormat );
	
	MOAIImage* image = new MOAIImage ();
	image->Convert ( *self, ( ZLColor::ColorFormat )colorFmt, ( PixelFormat )pixelFmt );
	image->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	copy
	@text	Copies an image.

	@in		MOAIImage self
	@out	MOAIImage image		Copy of the image.
*/
int MOAIImage::_copy ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	MOAIImage* image = new MOAIImage ();
	image->Copy ( *self );
	image->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	copyBits
	@text	Copy a section of one image to another.

	@in		MOAIImage self
	@in		MOAIImage source	Source image.
	@in		number srcX			X location in source image.
	@in		number srcY			Y location in source image.
	@in		number destX		X location in destination image.
	@in		number destY		Y location in destination image.
	@in		number width		Width of section to copy.
	@in		number height		Height of section to copy.
	@out	nil
*/
int MOAIImage::_copyBits ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UUNNNNNN" )
	
	MOAIImage* image = state.GetLuaObject < MOAIImage >( 2, true );
	if ( !image ) {
		return 0;
	}
	
	int srcX	= state.GetValue < int >( 3, 0 );
	int srcY	= state.GetValue < int >( 4, 0 );
	
	int destX	= state.GetValue < int >( 5, 0 );
	int destY	= state.GetValue < int >( 6, 0 );
	
	int width	= state.GetValue < int >( 7, 0 );
	int height	= state.GetValue < int >( 8, 0 );
	
	self->Blit ( *image, srcX, srcY, destX, destY, width, height );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	copyRect
	@text	Copy a section of one image to another. Accepts two rectangles.
			Rectangles may be of different size and proportion. Section of
			image may also be flipped horizontally or vertically by
			reversing min/max of either rectangle.

	@in		MOAIImage self
	@in		MOAIImage source	Source image.
	@in		number srcXMin
	@in		number srcYMin
	@in		number srcXMax
	@in		number srcYMax
	@in		number destXMin
	@in		number destYMin
	@opt	number destXMax		Default value is destXMin + srcXMax - srcXMin;
	@opt	number destYMax		Default value is destYMin + srcYMax - srcYMin;
	@opt	number filter		One of MOAIImage.FILTER_LINEAR, MOAIImage.FILTER_NEAREST.
								Default value is MOAIImage.FILTER_LINEAR.
	@out	nil
*/
int MOAIImage::_copyRect ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UUNNNNNN" )
	
	MOAIImage* image = state.GetLuaObject < MOAIImage >( 2, true );
	if ( !image ) {
		return 0;
	}
	
	ZLIntRect srcRect;
	srcRect.mXMin = state.GetValue < int >( 3, 0 );
	srcRect.mYMin = state.GetValue < int >( 4, 0 );
	srcRect.mXMax = state.GetValue < int >( 5, 0 );
	srcRect.mYMax = state.GetValue < int >( 6, 0 );
	
	ZLIntRect destRect;
	destRect.mXMin = state.GetValue < int >( 7, 0 );
	destRect.mYMin = state.GetValue < int >( 8, 0 );
	destRect.mXMax = state.GetValue < int >( 9, destRect.mXMin + srcRect.Width ());
	destRect.mYMax = state.GetValue < int >( 10, destRect.mYMin + srcRect.Height ());
	
	u32 filter = state.GetValue < u32 >( 11, MOAIImage::FILTER_LINEAR );
	
	ZLColorBlendFunc blendFunc;
	blendFunc.mEquation = ZLColor::BLEND_EQ_NONE;
	
	if ( state.CheckParams ( 12, "NN", false )) {
		blendFunc.mSrcFactor	= ( ZLColor::BlendFactor )state.GetValue < u32 >( 12, ( u32 )ZLColor::BLEND_FACTOR_SRC_ALPHA );
		blendFunc.mDstFactor	= ( ZLColor::BlendFactor )state.GetValue < u32 >( 13, ( u32 )ZLColor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA );
		blendFunc.mEquation		= ( ZLColor::BlendEquation )state.GetValue < u32 >( 14, ( u32 )ZLColor::BLEND_EQ_ADD );
	}

	self->CopyRect ( *image, srcRect, destRect, filter, blendFunc );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	desaturate
	@text	Convert image to grayscale.

	@in		MOAIImage self
	@opt	rY
	@opt	gY
	@opt	bY
	@opt	K
	@out	nil
*/
int MOAIImage::_desaturate ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	float rY	= state.GetValue < float >( 2, LUMA_709_R );
	float gY	= state.GetValue < float >( 3, LUMA_709_G );
	float bY	= state.GetValue < float >( 4, LUMA_709_B );
	float K		= state.GetValue < float >( 5, 1.0f );
	
	self->Desaturate ( *self, rY, gY, bY, K );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	fillCircle
	@text	Draw a filled circle.

	@in		number x
	@in		number y
	@in		number radius
	@opt	number r			Default value is 0.
	@opt	number g			Default value is 0.
	@opt	number b			Default value is 0.
	@opt	number a			Default value is 0.
	@out	nil
*/
int MOAIImage::_fillCircle ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNNN" )
	
	float x0	= state.GetValue < float >( 2, 0.0f );
	float y0	= state.GetValue < float >( 3, 0.0f );
	float r		= state.GetValue < float >( 4, 0.0f );
	u32 color	= state.GetColor32 ( 5, 0.0f, 0.0f, 0.0f, 0.0f );
	
	self->FillCircle ( x0, y0, r, color );
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	fillRect
	@text	Fill a rectangle in the image with a solid color.

	@in		MOAIImage self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@opt	number r			Default value is 0.
	@opt	number g			Default value is 0.
	@opt	number b			Default value is 0.
	@opt	number a			Default value is 0.
	@out	nil
*/
int MOAIImage::_fillRect ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNNNN" )

	ZLIntRect rect = state.GetRect < int >( 2 );
	u32 color = state.GetColor32 ( 6, 0.0f, 0.0f, 0.0f, 0.0f );

	self->FillRect ( rect, color );

	return 0;
}

//----------------------------------------------------------------//
/**	@lua	gammaCorrection
	@text	Apply gamma correction.

	@in		MOAIImage self
	@opt	gamma			Default value is 1.
	@out	nil
*/
int MOAIImage::_gammaCorrection ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	float gamma = state.GetValue < float >( 2, 1 );
	
	self->GammaCorrection ( *self, gamma );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	generateOutlineFromSDF
	@text	Given a rect, and min and max distance values, transform
			to a binary image where 0 means not on the outline and
			1 means part of the outline

	@in		MOAIImage self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@opt	number distMin
	@opt	number distMax
	@opt	number r			Default value is 1.
	@opt	number g			Default value is 1.
	@opt	number b			Default value is 1.
	@opt	number a			Default value is 1.
	@out	nil
 */
int	MOAIImage::_generateOutlineFromSDF( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNNNN" )
	
	ZLIntRect rect = state.GetRect < int >( 2 );
	float distMin = state.GetValue < float >( 6, 0.46f );
	float distMax = state.GetValue < float >( 7, 0.5f );
	float r		= state.GetValue < float >( 8, 1.0f );
	float g		= state.GetValue < float >( 9, 1.0f );
	float b		= state.GetValue < float >( 10, 1.0f );
	float a		= state.GetValue < float >( 11, 1.0f );
	
	self->GenerateOutlineFromSDF ( rect, distMin, distMax, r, g, b, a );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	generateSDF
	@text	Given a rect, creates a signed distance field from it
 
	@in		MOAIImage self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
 */
int MOAIImage::_generateSDF( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNNNN" )
	
	ZLIntRect rect = state.GetRect <int>( 2 );
	
	self->GenerateSDF ( rect );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	generateSDFDeadReckoning
	@text	Given a rect, creates a signed distance field from it 
			using dead reckoning technique

	@in		MOAIImage self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@opt	number threshold default is 256
	@out	nil
 */
int MOAIImage::_generateSDFDeadReckoning( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNNNN" )
	
	ZLIntRect rect = state.GetRect <int>( 2 );
	u32 threshold = state.GetValue < u32 >( 6, 256 );
	
	self->GenerateSDFDeadReckoning ( rect, threshold );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	getColor32
	@text	Returns a 32-bit packed RGBA value from the image for a
			given pixel coordinate.

	@in		MOAIImage self
	@in		number x
	@in		number y
	@out	number color
*/
int MOAIImage::_getColor32 ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNN" )
	
	u32 x		= state.GetValue < u32 >( 2, 0 );
	u32 y		= state.GetValue < u32 >( 3, 0 );

	u32 color = self->GetColor ( x, y );
	lua_pushnumber ( state, color );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	getFormat
	@text	Returns the color format of the image.

	@in		MOAIImage self
	@out	number colorFormat
*/
int MOAIImage::_getFormat ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	lua_pushnumber ( state, self->GetColorFormat ());
	
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	getRGBA
	@text	Returns an RGBA color as four floating point values.

	@in		MOAIImage self
	@in		number x
	@in		number y
	@out	number r
	@out	number g
	@out	number b
	@out	number a
*/
int MOAIImage::_getRGBA ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNN" )
	
	u32 x		= state.GetValue < u32 >( 2, 0 );
	u32 y		= state.GetValue < u32 >( 3, 0 );

	u32 color = self->GetColor ( x, y );
	
	ZLColorVec colorVec;
	colorVec.SetRGBA ( color );
	
	lua_pushnumber ( state, colorVec.mR );
	lua_pushnumber ( state, colorVec.mG );
	lua_pushnumber ( state, colorVec.mB );
	lua_pushnumber ( state, colorVec.mA );
	
	return 4;
}

//----------------------------------------------------------------//
/**	@lua	getSize
	@text	Returns the width and height of the image.

	@in		MOAIImage self
	@out	number width
	@out	number height
*/
int MOAIImage::_getSize ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	u32 width	= self->GetWidth ();
	u32 height	= self->GetHeight ();

	lua_pushnumber ( state, width );
	lua_pushnumber ( state, height );
	
	return 2;
}

//----------------------------------------------------------------//
/**	@lua	init
	@text	Initializes the image with a width, height and color format.

	@in		MOAIImage self
	@in		number width
	@in		number height
	@opt	number colorFmt		One of MOAIImage.COLOR_FMT_A_1, MOAIImage.COLOR_FMT_A_4, MOAIImage.COLOR_FMT_A_8,
								MOAIImage.COLOR_FMT_RGB_888, MOAIImage.COLOR_FMT_RGB_565, MOAIImage.COLOR_FMT_RGBA_5551,
								MOAIImage.COLOR_FMT_RGBA_4444, COLOR_FMT_RGBA_8888.
								Default value is MOAIImage.COLOR_FMT_RGBA_8888.
	@out	nil
*/
int MOAIImage::_init ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )

	MOAIImage* srcImage	= state.GetLuaObject < MOAIImage >( 2, false );
	if  ( srcImage ) {
	
		self->Init ( *srcImage );
	}
	else {

		u32 width		= state.GetValue < u32 >( 2, 0 );
		u32 height		= state.GetValue < u32 >( 3, 0 );
		u32 colorFmt	= state.GetValue < u32 >( 4, ZLColor::RGBA_8888 );

		self->Init ( width, height, ( ZLColor::ColorFormat )colorFmt, TRUECOLOR );
	}
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	load
	@text	Loads an image from an image file.
			Depending on the build configuration, the following file formats are supported:
			PNG, JPG, WebP.

	@in		MOAIImage self
	@in		string filename
	@opt	number transform	One of MOAIImage.POW_TWO, One of MOAIImage.QUANTIZE,
								One of MOAIImage.TRUECOLOR, One of MOAIImage.PREMULTIPLY_ALPHA
	@out	nil
*/
int MOAIImage::_load ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "US" )

	cc8* filename	= state.GetValue < cc8* >( 2, "" );
	u32 transform	= state.GetValue < u32 >( 3, 0 );

	self->Load ( filename, transform );

	return 0;
}

//----------------------------------------------------------------//
/**	@lua	loadFromBuffer
	@text	Loads an image from a buffer.

	@in		MOAIImage self
	@in		MOAIDataBuffer buffer		Buffer containing the image
	@opt	number transform			One of MOAIImage.POW_TWO, One of MOAIImage.QUANTIZE,
										One of MOAIImage.TRUECOLOR, One of MOAIImage.PREMULTIPLY_ALPHA
	@out	nil
*/
int MOAIImage::_loadFromBuffer ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UU" )

	MOAIDataBuffer* buffer = state.GetLuaObject < MOAIDataBuffer >( 2, true );
	u32 transform = state.GetValue < u32 >( 3, 0 );
	if ( buffer ) {
		void* bytes = 0;
		size_t size = 0;
		ZLByteStream stream;

		buffer->Lock ( &bytes, &size );
	
		stream.SetBuffer ( bytes, size );
		stream.SetLength ( size );

		self->Load ( stream, transform );

		buffer->Unlock();
	}

	return 0;
}

//----------------------------------------------------------------//
/**	@lua	mix
	@text	Transforms each color by a 4x4 matrix. The default value is a
			4x4 identity matrix. The transformation 'remixes' the image's
			channels: each new channel value is given by the sum of channels
			as weighted by the corresponding row of the matrix. For example,
			to remix the blue channel: b = r*b1 + g*b2 + b*b3 + a*b4. A row value
			for b of (0, 0, 1, 0) would be the identity: b = b. A row value for
			b of (1, 0, 0, 0) would replace b with r: b=r. A row value for
			b of (0.5, 0.5, 0, 0) would replace b with an even blend of r and g:
			b = r*05 + b*0.5. In this fashion, all channels of the image
			may be rearranged or blended.
	
	@in		MOAIImage self
	@opt	r1, r2, r3, r4
	@opt	g1, g2, g3, g4
	@opt	b1, b2, b3, b4
	@opt	a1, a2, a3, a4
	@opt	K					Default value is 1.
	@out	nil
*/
int MOAIImage::_mix ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )

	ZLMatrix4x4 mtx;

	mtx.m [ ZLMatrix4x4::C0_R0 ]		= state.GetValue < float >( 2, 1.0f );
	mtx.m [ ZLMatrix4x4::C1_R0 ]		= state.GetValue < float >( 3, 0.0f );
	mtx.m [ ZLMatrix4x4::C2_R0 ]		= state.GetValue < float >( 4, 0.0f );
	mtx.m [ ZLMatrix4x4::C3_R0 ]		= state.GetValue < float >( 5, 0.0f );
	
	mtx.m [ ZLMatrix4x4::C0_R1 ]		= state.GetValue < float >( 6, 0.0f );
	mtx.m [ ZLMatrix4x4::C1_R1 ]		= state.GetValue < float >( 7, 1.0f );
	mtx.m [ ZLMatrix4x4::C2_R1 ]		= state.GetValue < float >( 8, 0.0f );
	mtx.m [ ZLMatrix4x4::C3_R1 ]		= state.GetValue < float >( 9, 0.0f );
	
	mtx.m [ ZLMatrix4x4::C0_R2 ]		= state.GetValue < float >( 10, 0.0f );
	mtx.m [ ZLMatrix4x4::C1_R2 ]		= state.GetValue < float >( 11, 0.0f );
	mtx.m [ ZLMatrix4x4::C2_R2 ]		= state.GetValue < float >( 12, 1.0f );
	mtx.m [ ZLMatrix4x4::C3_R2 ]		= state.GetValue < float >( 13, 0.0f );
	
	mtx.m [ ZLMatrix4x4::C0_R3 ]		= state.GetValue < float >( 14, 0.0f );
	mtx.m [ ZLMatrix4x4::C1_R3 ]		= state.GetValue < float >( 15, 0.0f );
	mtx.m [ ZLMatrix4x4::C2_R3 ]		= state.GetValue < float >( 16, 0.0f );
	mtx.m [ ZLMatrix4x4::C3_R3 ]		= state.GetValue < float >( 17, 1.0f );
	
	float K								= state.GetValue < float >( 18, 1.0f );
	
	self->Mix ( *self, mtx, K );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@lua	padToPow2
	@text	Copies an image and returns a new image padded to the next
			power of 2 along each dimension. Original image will be
			in the upper left hand corner of the new image.

	@in		MOAIImage self
	@out	MOAIImage image		Copy of the image padded to the next nearest power
								of two along each dimension.
*/
int MOAIImage::_padToPow2 ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )
	
	MOAIImage* image = new MOAIImage ();
	image->PadToPow2 ( *self );
	image->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	resize
	@text	Copies the image to an image with a new size.

	@in		MOAIImage self
	@in		number width		New width of the image.
	@in		number height		New height of the image.
	@opt	number filter		One of MOAIImage.FILTER_LINEAR, MOAIImage.FILTER_NEAREST.
								Default value is MOAIImage.FILTER_LINEAR.
	@out	MOAIImage image
*/
int MOAIImage::_resize ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNN" )
	
	u32 width	= state.GetValue < u32 >( 2, 0 );
	u32 height	= state.GetValue < u32 >( 3, 0 );
	u32 filter	= state.GetValue < u32 >( 4, MOAIImage::FILTER_LINEAR );
	
	ZLIntRect srcRect;
	ZLIntRect destRect;
	
	srcRect.Init ( 0, 0, self->mWidth, self->mHeight );
	destRect.Init ( 0, 0, width, height );
	
	MOAIImage* image = new MOAIImage ();
	image->Init ( width, height, self->mColorFormat, self->mPixelFormat );
	image->CopyRect ( *self, srcRect, destRect, filter );
	image->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	resizeCanvas
	@text	Copies the image to a canvas with a new size. If the canvas
			is larger than the original image, the extra pixels will be
			initialized with 0. Pass in a new frame or just a new width
			and height. Negative values are permitted for the frame.

	@overload

		@in		MOAIImage self
		@in		number width		New width of the image.
		@in		number height		New height of the image.
		@out	MOAIImage image

	@overload

		@in		MOAIImage self
		@in		number xMin
		@in		number yMin
		@in		number xMax
		@in		number yMax
		@out	MOAIImage image
*/
int MOAIImage::_resizeCanvas ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNN" )
	
	ZLIntRect rect;
	
	if ( state.CheckParams ( 4, "NN" )) {
		rect.mXMin	= state.GetValue < int >( 2, 0 );
		rect.mYMin	= state.GetValue < int >( 3, 0 );
		rect.mXMax	= state.GetValue < int >( 4, 0 );
		rect.mYMax	= state.GetValue < int >( 5, 0 );
	}
	else {
		rect.mXMin	= 0;
		rect.mYMin	= 0;
		rect.mXMax	= state.GetValue < int >( 2, 0 );
		rect.mYMax	= state.GetValue < int >( 3, 0 );
	}
	
	MOAIImage* image = new MOAIImage ();
	image->ResizeCanvas ( *self, rect );
	image->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@lua	setColor32
	@text	Sets 32-bit the packed RGBA value for a given pixel
			coordinate. Parameter will be converted to the native format
			of the image.

	@in		MOAIImage self
	@in		number x
	@in		number y
	@in		number color
	@out	nil
*/
int MOAIImage::_setColor32 ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNNN" )

	u32 x		= state.GetValue < u32 >( 2, 0 );
	u32 y		= state.GetValue < u32 >( 3, 0 );
	u32 color	= state.GetValue < u32 >( 4, 0 );

	self->SetColor ( x, y, color );

	return 0;
}

//----------------------------------------------------------------//
/**	@lua	setRGBA
	@text	Sets a color using RGBA floating point values.

	@in		MOAIImage self
	@in		number x
	@in		number y
	@in		number r
	@in		number g
	@in		number b
	@opt	number a	Default value is 1.
	@out	nil
*/
int MOAIImage::_setRGBA ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "UNN" )

	u32 x		= state.GetValue < u32 >( 2, 0 );
	u32 y		= state.GetValue < u32 >( 3, 0 );
	
	float r		= state.GetValue < float >( 4, 0.0f );
	float g		= state.GetValue < float >( 5, 0.0f );
	float b		= state.GetValue < float >( 6, 0.0f );
	float a		= state.GetValue < float >( 7, 1.0f );

	self->SetColor ( x, y, ZLColor::PackRGBA ( r, g, b, a ));

	return 0;
}

//----------------------------------------------------------------//
/**	@lua	simpleThreshold
	@text	This is a 'naive' threshold implementation that forces image color channels
			to 0 or 1 based on a per-channel threshold value. The channel value must be
			entirely greater that the threshold to be promoted to a value of 1. This means
			a threshold value of 1 will always result in a channel value of 0.

	@in		MOAIImage self
	@opt	number r	Default value is 0.
	@opt	number g	Default value is 0.
	@opt	number b	Default value is 0.
	@opt	number a	Default value is 0.
	@out	nil
*/
int MOAIImage::_simpleThreshold ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "U" )

	float r		= state.GetValue < float >( 2, 0.0f );
	float g		= state.GetValue < float >( 3, 0.0f );
	float b		= state.GetValue < float >( 4, 0.0f );
	float a		= state.GetValue < float >( 5, 0.0f );

	self->SimpleThreshold ( *self, r, g, b, a );

	return 0;
}

//----------------------------------------------------------------//
/**	@lua	writePNG
	@text	Write image to a PNG file.

	@in		MOAIImage self
	@in		string filename
	@out	nil
*/
int MOAIImage::_writePNG ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIImage, "US" )
	
	#if MOAI_WITH_LIBPNG
		cc8* filename = state.GetValue < cc8* >( 2, "" );
		
		ZLFileStream stream;
		stream.OpenWrite ( filename );
		self->WritePNG ( stream );
	#endif
	
	return 0;
}

//================================================================//
// MOAIImage
//================================================================//

//----------------------------------------------------------------//
MOAIImage* MOAIImage::AffirmImage ( MOAILuaState& state, int idx ) {

	MOAIImage* image = 0;
	
	if ( state.IsType ( idx, LUA_TUSERDATA )) {
		image = state.GetLuaObject < MOAIImage >( idx, false );
	}
	
	if ( state.IsType ( idx, LUA_TSTRING )) {
	
		cc8* filename = state.GetValue < cc8* >( idx, "" );
		if ( ZLFileSys::CheckFileExists ( filename )) {
	
			image = new MOAIImage ();
			if ( !image->Load ( filename )) {
				// TODO: report error
				delete image;
				image = 0;
			}
		}
	}
	return image;
}

//----------------------------------------------------------------//
void MOAIImage::Alloc () {

	if ( this->mBitmap ) {
		free ( this->mBitmap );
		this->mBitmap = 0;
	}

	if ( this->mPalette ) {
		free ( this->mPalette );
		this->mPalette = 0;
	}
	
	u32 bitmapSize = this->GetBitmapSize ();
	if ( bitmapSize ) {
		this->mBitmap = malloc ( bitmapSize );
	}
	
	u32 paletteSize = this->GetPaletteSize ();
	if ( paletteSize ) {
		this->mPalette = malloc ( paletteSize );
		memset ( this->mPalette, 0, paletteSize );
	}
}

//----------------------------------------------------------------//
void MOAIImage::BleedRect ( ZLIntRect rect ) {

	rect.Bless ();

	int xMin = rect.mXMin;
	int yMin = rect.mYMin;
	int xMax = rect.mXMax;
	int yMax = rect.mYMax;

	int width = ( int )this->mWidth;
	int height = ( int )this->mHeight;

	// clamp min to 0
	xMin = ( xMin < 0 ) ? 0 : xMin;
	yMin = ( yMin < 0 ) ? 0 : yMin;
	
	// clamp max to image width, height
	xMax = ( xMax > width ) ? width : xMax;
	yMax = ( yMax > height ) ? height : yMax;

	if ( xMax <= xMin ) return;
	if ( yMax <= yMin ) return;

	if ( xMin >= width ) return;
	if ( yMin >= height ) return;
	
	if ( xMax <= 0 ) return;
	if ( yMax <= 0 ) return;
	
	// copy the wings (left and right)
	if (( xMin > 0 ) || ( xMax < width )) {
		for ( int y = yMin; y < yMax; ++y ) {
		
			// left wing
			if ( xMin > 0 ) {
				this->SetPixel ( xMin - 1, y, this->GetPixel ( xMin, y ));
			}
			
			// right wing
			if ( xMax < width ) {
				this->SetPixel ( xMax, y, this->GetPixel ( xMax - 1, y ));
			}
		}
	}
	
	// expand and clamp left and right
	xMin = ( xMin > 1 ) ? xMin - 1 : 0;
	xMax = ( xMax < width ) ? xMax + 1 : width;
	
	u32 bitDepth = this->GetPixelDepthInBits ();
	
	if ( yMin > 0 ) {
		ZLBitBuffer::Blit ( this->GetRowAddr ( yMin - 1 ), xMin, this->GetRowAddr ( yMin ), xMin, xMax - xMin, bitDepth );
	}
	
	if ( yMax < height ) {
		ZLBitBuffer::Blit ( this->GetRowAddr ( yMax ), xMin, this->GetRowAddr ( yMax - 1 ), xMin, xMax - xMin, bitDepth );
	}
}

//----------------------------------------------------------------//
void MOAIImage::Blit ( const MOAIImage& image, int srcX, int srcY, int destX, int destY, int width, int height ) {

	int srcXMax = srcX + width;
	int srcYMax = srcY + height;

	if ( srcX < 0 ) {
		destX -= srcX;
		width += srcX;
		srcX = 0;
	}
	
	if ( srcY < 0 ) {
		destY -= srcY;
		height += srcY;
		srcY = 0;
	}
	
	if ( srcXMax > ( int )image.mWidth ) {
		width -= srcXMax - ( int )image.mWidth;
	}
	
	if ( srcYMax > ( int )image.mHeight ) {
		height -= srcYMax - ( int )image.mHeight;
	}
	
	int destXMax = destX + width;
	int destYMax = destY + height;
	
	if ( destX < 0 ) {
		srcX -= destX;
		width += destX;
		destX = 0;
	}
	
	if ( destY < 0 ) {
		srcY -= destY;
		height += destY;
		destY = 0;
	}
	
	if ( destXMax > ( int )this->mWidth ) {
		width -= destXMax - ( int )this->mWidth;
	}
	
	if ( destYMax > ( int )this->mHeight ) {
		height -= destYMax - ( int )this->mHeight;
	}
	
	if ( width <= 0 ) return;
	if ( height <= 0 ) return;
	
	if ( srcX >= ( int )image.mWidth ) return;
	if ( srcY >= ( int )image.mHeight ) return;
	
	if ( destX >= ( int )this->mWidth ) return;
	if ( destY >= ( int )this->mHeight ) return;
	
	u32 pixelDepth = this->GetPixelDepthInBits ();
	
	for ( int y = 0; y < height; ++y ) {
		
		const void* srcRow = image.GetRowAddr ( y + srcY );
		void* destRow = this->GetRowAddr ( y + destY );
		
		ZLBitBuffer::Blit ( destRow, destX, srcRow, srcX, width, pixelDepth );
	}
}

//----------------------------------------------------------------//
void MOAIImage::ComparePixel( ZLIntVec2D** grid, ZLIntVec2D& p, int x, int y, int offsetX, int offsetY, int width, int height ) {
	
	// Need to check for boundaries unless there is a 1 pixel gutter
	ZLIntVec2D otherPixel; //= grid[y + offsetY][x + offsetX];
	
	int newX = x + offsetX;
	int newY = y + offsetY;
	
	if ( newX >= 0 && newY >= 0 && newX < width && newY < height ) {
		otherPixel = grid[newY][newX];
	}
	else {
		otherPixel.mX = 9999;
		otherPixel.mY = 9999;
	}
		
	otherPixel.mX += offsetX;
	otherPixel.mY += offsetY;
	
	if ( otherPixel.LengthSquared() < p.LengthSquared() )
		p = otherPixel;
}

//----------------------------------------------------------------//
void MOAIImage::CalculateSDF( ZLIntVec2D** grid, int width, int height ) {
	
	// Pass 0
	for ( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			ZLIntVec2D p = grid[y][x];
			this->ComparePixel( grid, p, x, y, -1,  0, width, height );
			this->ComparePixel( grid, p, x, y,  0, -1, width, height );
			this->ComparePixel( grid, p, x, y, -1, -1, width, height );
			this->ComparePixel( grid, p, x, y,  1, -1, width, height );
			grid[y][x] = p;
		}
		
		for ( int x = width - 1; x >= 0; x-- ) {
			ZLIntVec2D p = grid[y][x];
			this->ComparePixel( grid, p, x, y, 1, 0, width, height );
			grid[y][x] = p;
		}
	}
	
	// Pass 1
	for ( int y = height - 1; y >= 0; y-- ) {
		for ( int x = width - 1; x >= 0; x-- ) {
			ZLIntVec2D p = grid[y][x];
			this->ComparePixel( grid, p, x, y,  1,  0, width, height );
			this->ComparePixel( grid, p, x, y,  0,  1, width, height );
			this->ComparePixel( grid, p, x, y, -1,  1, width, height );
			this->ComparePixel( grid, p, x, y,  1,  1, width, height );
			grid[y][x] = p;
		}
		
		for ( int x = 0; x < width; x++ ) {
			ZLIntVec2D p = grid[y][x];
			this->ComparePixel( grid, p, x, y, -1, 0, width, height );
			grid[y][x] = p;
		}
	}
}

//----------------------------------------------------------------//
void MOAIImage::Clear () {

	if ( this->mBitmap ) {
		free ( this->mBitmap );
	}
	
	if ( this->mPalette ) {
		free ( this->mPalette );
	}
	
	this->mColorFormat	= ZLColor::CLR_FMT_UNKNOWN;
	this->mPixelFormat	= PXL_FMT_UNKNOWN;

	this->mWidth		= 0;
	this->mHeight		= 0;
	
	this->mBitmap		= 0;
	this->mPalette		= 0;
}

//----------------------------------------------------------------//
void MOAIImage::ClearBitmap () {

	if ( this->mBitmap ) {
		memset ( this->mBitmap, 0, this->GetBitmapSize ());
	}
}

//----------------------------------------------------------------//
void MOAIImage::ClearRect ( ZLIntRect rect ) {

	rect.Bless ();
	
	ZLIntRect bounds = this->GetBounds ();
	bounds.Clip ( rect );
	
	int width = rect.Width ();
	
	if ( !width ) return;
	if ( !rect.Height ()) return;

	u32 pixelDepth = this->GetPixelDepthInBits ();
	
	for ( int y = rect.mYMin; y < rect.mYMax; ++y ) {
		ZLBitBuffer::Clear ( this->GetRowAddr ( y ), pixelDepth, width, pixelDepth );
	}
}

//----------------------------------------------------------------//
bool MOAIImage::Convert ( const MOAIImage& image, ZLColor::ColorFormat colorFmt, PixelFormat pixelFmt ) {
	
	if (( pixelFmt != TRUECOLOR ) && ( pixelFmt != image.mPixelFormat )) {
		// TODO: we don't yet support conversion to indexed color or between indexed color formats
		// TODO: log an error
		this->Clear ();
		return false;
	}
	
	// if everything's the same, just copy it over and bail
	if (( colorFmt == image.mColorFormat ) && ( pixelFmt == image.mPixelFormat )) {
		if ( this != &image ) {
			this->Copy ( image );
		}
		return true;
	}

	MOAIImage newImage;
	newImage.Init ( image.mWidth, image.mHeight, colorFmt, pixelFmt );
	
	if ( pixelFmt == TRUECOLOR ) {
	
		// we want this in truecolor
		if ( image.mPixelFormat != TRUECOLOR ) {
		
			// we're converting from indexed color, so read and set every pixel
			for ( u32 y = 0; y < image.mHeight; ++y ) {
				for ( u32 x = 0; x < image.mWidth; ++x ) {
					newImage.SetColor ( x, y, image.GetColor ( x, y ));
				}
			}
		}
		else {
		
			// it's already truecolor, so we're just converting the color format
			// we'll use the helper method in ZLColor which is a little more efficient
			for ( u32 y = 0; y < image.mHeight; ++y ) {
				const void* srcRow = image.GetRowAddr ( y );
				void* destRow = newImage.GetRowAddr ( y );
				ZLColor::Convert ( destRow, newImage.mColorFormat, srcRow, image.mColorFormat, image.mWidth );
			}
		}
	}
	else {
	
		// pixel format is indexed color, so just convert the palette
		u32 total = image.GetPaletteCount ();
		ZLColor::Convert ( newImage.mPalette, newImage.mColorFormat, image.mPalette, image.mColorFormat, total );
	}
	
	this->Take ( newImage );
	return true;
}

//----------------------------------------------------------------//
bool MOAIImage::Compare ( const MOAIImage& image ) {

	if (( this->mWidth != image.mWidth ) || ( this->mHeight != image.mHeight )) {
		return false;
	}

	float r [ 2 ][ 4 ] = {{ 0 }};
	float g [ 2 ][ 4 ] = {{ 0 }};
	float b [ 2 ][ 4 ] = {{ 0 }};
	float a [ 2 ][ 4 ] = {{ 0 }};

	for ( u32 i = 0; i < this->mHeight; i++ ) {
		for ( u32 j = 0; j < this->mWidth; j++ ) {
			ZLColorVec color1, color2;
			color1.SetRGBA ( this->GetColor ( j, i ));
			color2.SetRGBA ( image.GetColor ( j, i ));

			r [ 0 ][ u32 ( ZLFloat::Max ( color1.mR * 4 - 1, 0 ))]++;
			g [ 0 ][ u32 ( ZLFloat::Max ( color1.mG * 4 - 1, 0 ))]++;
			b [ 0 ][ u32 ( ZLFloat::Max ( color1.mB * 4 - 1, 0 ))]++;
			a [ 0 ][ u32 ( ZLFloat::Max ( color1.mA * 4 - 1, 0 ))]++;

			r [ 1 ][ u32 ( ZLFloat::Max ( color2.mR * 4 - 1, 0 ))]++;
			g [ 1 ][ u32 ( ZLFloat::Max ( color2.mG * 4 - 1, 0 ))]++;
			b [ 1 ][ u32 ( ZLFloat::Max ( color2.mB * 4 - 1, 0 ))]++;
			a [ 1 ][ u32 ( ZLFloat::Max ( color2.mA * 4 - 1, 0 ))]++;
		}
	}

	for ( u32 i = 0; i < 2; i++ ) {
		for ( u32 j = 0; j < 4; j++ ) {
			r [ i ][ j ] = r [ i ][ j ] / ( this->mWidth * this->mHeight );
			g [ i ][ j ] = g [ i ][ j ] / ( this->mWidth * this->mHeight );
			b [ i ][ j ] = b [ i ][ j ] / ( this->mWidth * this->mHeight );
			a [ i ][ j ] = a [ i ][ j ] / ( this->mWidth * this->mHeight );
		}
	}

	for ( u32 i = 0; i < 4; i++ ) {
		r [ 0 ][ i ] = ZLFloat::Abs ( r [ 0 ][ i ] - r [ 1 ][ i ]);
		g [ 0 ][ i ] = ZLFloat::Abs ( g [ 0 ][ i ] - g [ 1 ][ i ]);
		b [ 0 ][ i ] = ZLFloat::Abs ( b [ 0 ][ i ] - b [ 1 ][ i ]);
		a [ 0 ][ i ] = ZLFloat::Abs ( a [ 0 ][ i ] - a [ 1 ][ i ]);
	}

	float similar = 0;

	for ( u32 i = 0; i < 4; i++ ) {
		similar += ( r [ 0 ][ i ] + g [ 0 ][ i ] + b [ 0 ][ i ] + a [ 0 ][ i ]);
	}

	return similar < .02;
}

//----------------------------------------------------------------//
void MOAIImage::Copy ( const MOAIImage& image ) {

	this->Init ( image.mWidth, image.mHeight, image.mColorFormat, image.mPixelFormat );
	
	size_t bitmapSize = this->GetBitmapSize ();
	if ( bitmapSize ) {
		memcpy ( this->mBitmap, image.mBitmap, bitmapSize );
	}
	
	size_t paletteSize = this->GetPaletteSize ();
	if ( paletteSize ) {
		memcpy ( this->mPalette, image.mPalette, paletteSize );
	}
}

//----------------------------------------------------------------//
void MOAIImage::CopyRect ( const MOAIImage& image, ZLIntRect srcRect, ZLIntRect destRect, u32 filter ) {

	ZLColorBlendFunc blendFunc;
	blendFunc.mEquation = ZLColor::BLEND_EQ_NONE;
	this->CopyRect ( image, srcRect, destRect, filter, blendFunc );
}

//----------------------------------------------------------------//
void MOAIImage::CopyRect ( const MOAIImage& image, ZLIntRect srcRect, ZLIntRect destRect, u32 filter, const ZLColorBlendFunc& blendFunc ) {

	if (( this->mPixelFormat != TRUECOLOR ) && ( this->mPixelFormat != TRUECOLOR )) return; // TODO: warn about this case

	float scale;

	bool xFlip = srcRect.IsXFlipped () != destRect.IsXFlipped ();
	bool yFlip = srcRect.IsYFlipped () != destRect.IsYFlipped ();

	srcRect.Bless ();
	destRect.Bless ();

	// if everything's the same size and format, we can do a simple blit
	if ( !( xFlip || yFlip )) {
		if (( blendFunc.mEquation == ZLColor::BLEND_EQ_NONE ) && ( this->mPixelFormat == image.mPixelFormat ) && ( this->mColorFormat == image.mColorFormat )) {
			if (( srcRect.Width () == destRect.Width ()) && ( srcRect.Height () == destRect.Height ())) {
				this->Blit ( image, srcRect.mXMin, srcRect.mYMin, destRect.mXMin, destRect.mYMin, srcRect.Width (), srcRect.Height ());
				return;
			}
		}
	}

	// prepare the rectangles
	ZLIntRect srcBounds;
	srcBounds.Init ( 0, 0, image.mWidth, image.mHeight );
	
	ZLIntRect destBounds;
	destBounds.Init ( 0, 0, this->mWidth, this->mHeight );

	if ( !srcRect.Overlap ( srcBounds )) return;
	if ( !destRect.Overlap ( destBounds )) return;
	
	ZLIntRect srcClipA = srcRect;
	ZLIntRect srcClipB = srcRect;
	
	ZLIntRect destClipA = destRect;
	ZLIntRect destClipB = destRect;

	// get the rects clipped against their respective bounds
	srcBounds.Clip ( srcClipA );
	destBounds.Clip ( destClipA );
	
	// now we need to get each rect's subrect adjusted for the *other* rect's clip
	scale = ( float )srcClipA.Width () / ( float )srcRect.Width ();
	if ( scale < 1.0f ) {
		
		int offset = ( int )floor (( srcClipA.mXMin - srcRect.mXMin ) / scale );
		int width = ( int )floor ( destClipB.Width () * scale );
		
		if ( xFlip ) {
			destClipB.mXMax -= offset;
			destClipB.mXMin = destClipB.mXMax - width;
		}
		else {
			destClipB.mXMin += offset;
			destClipB.mXMax = destClipB.mXMin + width;
		}
	}
	
	scale = ( float )srcClipA.Height () / ( float )srcRect.Height ();
	if ( scale < 1.0f ) {
	
		int offset = ( int )floor (( srcClipA.mYMin - srcRect.mYMin ) / scale );
		int height = ( int )floor ( destClipB.Height () * scale );
	
		if ( yFlip ) {
			destClipB.mYMax -= offset;
			destClipB.mYMin = destClipB.mYMax - height;
		}
		else {
			destClipB.mYMin += offset;
			destClipB.mYMax = destClipB.mYMin + height;
		}
	}
	
	scale = ( float )destClipA.Width () / ( float )destRect.Width ();
	if ( scale < 1.0f ) {
	
		int offset = ( int )floor (( destClipA.mXMin - destRect.mXMin ) / scale );
		int width = ( int )floor ( srcClipB.Width () * scale );
	
		if ( xFlip ) {
			srcClipB.mXMax -= offset;
			srcClipB.mXMin = srcClipB.mXMax - width;
		}
		else {
			srcClipB.mXMin += offset;
			srcClipB.mXMax = srcClipB.mXMin + width;
		}
	}
	
	scale = ( float )destClipA.Height () / ( float )destRect.Height ();
	if ( scale < 1.0f ) {
	
		int offset = ( int )floor (( destClipA.mYMin - destRect.mYMin ) / scale );
		int height = ( int )floor ( srcClipB.Height () * scale );
	
		if ( yFlip ) {
			srcClipB.mYMax -= offset;
			srcClipB.mYMin = srcClipB.mYMax - height;
		}
		else {
			srcClipB.mYMin += offset;
			srcClipB.mYMax = srcClipB.mYMin + height;
		}
	}
	
	// the final rects are the intersection of clipA and clipB
	srcRect = srcClipA;
	if ( !srcRect.Intersect ( srcClipB, srcRect )) return;
	
	destRect = destClipA;
	if ( !destRect.Intersect ( destClipB, destRect )) return;

	// TODO: if *image == this *and* srcRect, destRect overlap, we need to copy srcRect out of image
	bool useCopyImage = false;
	MOAIImage copyImage;
	
	if (( this == &image ) && srcRect.OverlapExcludeEdge ( destRect )) {
		
		ZLIntRect newSrcRect = srcRect;
		newSrcRect.Offset ( -newSrcRect.mXMin, -newSrcRect.mYMin );
		
		copyImage.Init ( newSrcRect.mXMax, newSrcRect.mYMax, image.mColorFormat, image.mPixelFormat );
		copyImage.Blit ( image, srcRect.mXMin, srcRect.mYMin, newSrcRect.mXMin, newSrcRect.mYMin, newSrcRect.mXMax, newSrcRect.mYMax );
		
		srcRect = newSrcRect;
		useCopyImage = true;
	}

	const MOAIImage& srcImage = useCopyImage ? copyImage : image;

	// now set up the copy
	int srcWidth = srcRect.Width ();
	int srcHeight = srcRect.Height ();
	
	int destWidth = destRect.Width ();
	int destHeight = destRect.Height ();
	
	float xSrcStep = ( float )srcWidth / ( float )destWidth;
	float ySrcStep = ( float )srcHeight / ( float )destHeight;
	
	int xSrcSampleSteps = ( int )ceilf ( xSrcStep );
	int ySrcSampleSteps = ( int )ceilf ( ySrcStep );
	
	float sampleAverage = 1.0f / ( float )( xSrcSampleSteps * ySrcSampleSteps );
	
	float xSampleStep = xSrcStep / ( float )xSrcSampleSteps;
	float ySampleStep = ySrcStep / ( float )ySrcSampleSteps;
	
	float xSrcOrigin = ( float )srcRect.mXMin;
	float ySrcOrigin = ( float )srcRect.mYMin;
	
	if ( xFlip ) {
		xSrcOrigin = ( float )srcRect.mXMax;
		xSampleStep = -xSampleStep;
		xSrcStep = -xSrcStep;
	}
	
	if ( yFlip ) {
		ySrcOrigin = ( float )srcRect.mYMax;
		ySampleStep = -ySampleStep;
		ySrcStep = -ySrcStep;
	}
	
	if ( this->mPixelFormat != TRUECOLOR ) {
	
		// TODO: warn about this case
	
		assert ( this->mColorFormat == srcImage.mColorFormat );
		
		for ( int y = 0; y < destHeight; ++y ) {
			for ( int x = 0; x < destWidth; ++x ) {
				
				float xPixel = floorf ( xSrcOrigin + (( float )x * xSrcStep ) + 0.5f );
				float yPixel = floorf ( ySrcOrigin + (( float )y * ySrcStep ) + 0.5f );
				
				u32 pixel = srcImage.GetPixel ( xPixel, yPixel );
				this->SetPixel ( xPixel, yPixel, pixel );
			}
		}
	}
	else if (( filter == FILTER_NEAREST ) || (( xSrcSampleSteps == 1 ) && ( ySrcSampleSteps == 1 ))) {
	
		for ( int y = 0; y < destHeight; ++y ) {
			for ( int x = 0; x < destWidth; ++x ) {
				
				float xSample = xSrcOrigin + (( float )x * xSrcStep );
				float ySample = ySrcOrigin + (( float )y * ySrcStep );
				
				u32 color = srcImage.SampleColor ( xSample, ySample, filter );
				this->SetColor ( destRect.mXMin + x, destRect.mYMin + y, color, blendFunc );
			}
		}
	}
	else {
	
		for ( int y = 0; y < destHeight; ++y ) {
			for ( int x = 0; x < destWidth; ++x ) {
				
				float r = 0.0f;
				float g = 0.0f;
				float b = 0.0f;
				float a = 0.0f;
				
				float xSample = xSrcOrigin + (( float )x * xSrcStep );
				float ySample = ySrcOrigin + (( float )y * ySrcStep );
				
				for ( int ys = 0; ys < ySrcSampleSteps; ++ys ) {
					for ( int xs = 0; xs < xSrcSampleSteps; ++xs ) {
					
						u32 color = srcImage.SampleColor ( xSample + (( float )xs * xSampleStep ), ySample + (( float )ys * ySampleStep ), filter );
						
						ZLColorVec vec ( color );
						
						r += vec.mR;
						g += vec.mG;
						b += vec.mB;
						a += vec.mA;
					}
				}
				
				r *= sampleAverage;
				g *= sampleAverage;
				b *= sampleAverage;
				a *= sampleAverage;
				
				this->SetColor ( destRect.mXMin + x, destRect.mYMin + y, ZLColor::PackRGBA ( r, g, b, a ), blendFunc );
			}
		}
	}
}

//----------------------------------------------------------------//
void MOAIImage::Desaturate ( const MOAIImage& image, float rY, float gY, float bY, float K ) {

	if ( this != &image ) {
		this->Copy ( image );
	}

	if ( this->mPixelFormat == TRUECOLOR ) {
		for ( u32 y = 0; y < this->mHeight; y++ ) {
			ZLColor::Desaturate ( this->GetRowAddr ( y ), this->mColorFormat, this->mWidth, rY, gY, bY, K );
		}
	}
	else {
		ZLColor::Desaturate ( this->GetPalette (), this->mColorFormat, this->GetPaletteCount (), rY, gY, bY, K );
	}
}

//----------------------------------------------------------------//
void MOAIImage::DrawLine(int p1x, int p1y, int p2x, int p2y, u32 color)
{
    int F, x, y;
	
	// Swap points if p1 is on the right of p2
    if ( p1x > p2x ) {
        swap ( p1x, p2x );
        swap ( p1y, p2y );
    }
	
    // Handle trivial cases separately for algorithm speed up.
    // Trivial case 1: m = +/-INF (Vertical line)
    if ( p1x == p2x ) {
	
		// Swap y-coordinates if p1 is above p2
        if (p1y > p2y) {
            swap ( p1y, p2y );
        }
		
        x = p1x;
        y = p1y;
        while ( y <= p2y ) {
            this->SetColor ( x, y, color );
            y++;
        }
        return;
    }
	
    // Trivial case 2: m = 0 (Horizontal line)
    else if ( p1y == p2y ) {
        x = p1x;
        y = p1y;
		
        while ( x <= p2x ) {
            this->SetColor ( x, y, color );
            x++;
        }
        return;
    }
	
    int dy				= p2y - p1y;  // y-increment from p1 to p2
    int dx				= p2x - p1x;  // x-increment from p1 to p2
    int dy2				= ( dy << 1 );  // dy << 1 == 2*dy
    int dx2				= ( dx << 1 );
    int dy2_minus_dx2	= dy2 - dx2;  // precompute constant for speed up
    int dy2_plus_dx2	= dy2 + dx2;
	
	// m >= 0
    if ( dy >= 0 ) {
	
        // Case 1: 0 <= m <= 1 (Original case)
        if ( dy <= dx ) {
            F = dy2 - dx;    // initial F
			
            x = p1x;
            y = p1y;
            while ( x <= p2x ) {
			
                this->SetColor ( x, y, color );
                if ( F <= 0 ) {
                    F += dy2;
                }
                else {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        }
        else { // Case 2: 1 < m < INF (Mirror about y=x line replace all dy by dx and dx by dy)
			
            F = dx2 - dy;    // initial F
			
            y = p1y;
            x = p1x;
            while ( y <= p2y ) {
			
                this->SetColor ( x, y, color );
				
                if ( F <= 0 ) {
                    F += dx2;
                }
                else {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    }
    else { // m < 0
	
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if ( dx >= -dy ) {
            F = -dy2 - dx;    // initial F
			
            x = p1x;
            y = p1y;
            while ( x <= p2x ) {
                this->SetColor ( x, y, color );
                if ( F <= 0 ) {
                    F -= dy2;
                }
                else {
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        }
        else { // Case 4: -INF < m < -1 (Mirror about x-axis and mirror about y=x line, replace all dx by -dy and dy by dx)
			
            F = dx2 + dy;    // initial F
			
            y = p1y;
            x = p1x;
            while ( y >= p2y ) {
                this->SetColor ( x, y, color );
                if ( F <= 0 ) {
                    F += dx2;
                }
                else {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}

//----------------------------------------------------------------//
// TODO: should fill ellipse (not just circle)
void MOAIImage::FillCircle ( float centerX, float centerY, float xRad, u32 color ) {
	
	int radius = ( int )xRad;
	int x0 = ( int )centerX;
	int y0 = ( int )centerY;
	
	int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;
	
	this->DrawLine ( x0, y0 + radius, x0, y0 - radius, color );
    this->DrawLine ( x0 + radius, y0, x0 - radius, y0, color );
	
    while ( x < y ) {
    
		//ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if ( f >= 0 ) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		
		this->DrawLine ( x0 - x, y0 + y, x0 + x, y0 + y, color );
		this->DrawLine ( x0 - x, y0 - y, x0 + x, y0 - y, color );
		this->DrawLine ( x0 + y, y0 + x, x0 - y, y0 + x, color );
		this->DrawLine ( x0 + y, y0 - x, x0 - y, y0 - x, color );
    }
}

//----------------------------------------------------------------//
void MOAIImage::FillRect ( ZLIntRect rect, u32 color ) {

	if ( !color ) {
		this->ClearRect ( rect );
		return;
	}

	rect.Bless ();
	
	ZLIntRect bounds = this->GetBounds ();
	bounds.Clip ( rect );
	
	if ( !rect.Width ()) return;
	if ( !rect.Height ()) return;
	
	for ( int y = rect.mYMin; y < rect.mYMax; ++y ) {
		this->DrawLine ( rect.mXMin, y, rect.mXMax, y, color );
	}
}

//----------------------------------------------------------------//
void MOAIImage::GammaCorrection ( const MOAIImage& image, float gamma ) {

	if ( this != &image ) {
		this->Copy ( image );
	}

	if ( this->mPixelFormat == TRUECOLOR ) {
		for ( u32 y = 0; y < this->mHeight; y++ ) {
			ZLColor::GammaCorrection ( this->GetRowAddr ( y ), this->mColorFormat, this->mWidth, gamma );
		}
	}
	else {
		ZLColor::GammaCorrection ( this->GetPalette (), this->mColorFormat, this->GetPaletteCount (), gamma );
	}
}

//----------------------------------------------------------------//
void MOAIImage::GenerateOutlineFromSDF ( ZLIntRect rect, float distMin, float distMax, float r, float g, float b, float a ) {
	u32 width = rect.Width () + 1;
	u32 height = rect.Height () + 1;
	
	for ( int y = 0; y < height; ++y ) {
		for ( int x = 0; x < width; ++x ) {
			u32 color = this->GetColor ( x + rect.mXMin, y + rect.mYMin );
			ZLColorVec colorVec;
			colorVec.SetRGBA ( color );
			
			if ( colorVec.mA >= distMin && colorVec.mA < distMax ) {
				colorVec.mR = r;
				colorVec.mG = g;
				colorVec.mB = b;
				colorVec.mA = a;
			}
			else {
				colorVec.mA = 0;
			}
			
			this->SetColor ( x + rect.mXMin, y + rect.mYMin, colorVec.PackRGBA () );
		}
	}
}

//----------------------------------------------------------------//
void MOAIImage::GenerateSDF ( ZLIntRect rect ) {
	
	// Plus one because rect goes to exact end
	int width = rect.Width() + 1;
	int height = rect.Height() + 1;
	
	ZLIntVec2D** grid1 = new ZLIntVec2D* [height];
	ZLIntVec2D** grid2 = new ZLIntVec2D* [height];
	int** gridDistance = new int* [height];
	
	for ( int i = 0; i < height; ++i ) {
		grid1[i] = new ZLIntVec2D[width];
		grid2[i] = new ZLIntVec2D[width];
		gridDistance[i] = new int[width];
	}
	
	ZLIntVec2D inside(0, 0);
	ZLIntVec2D empty(9999, 9999);
	
	// Set up the initial grid
	for ( int y = 0; y < height; ++y ) {
		for ( int x = 0; x < width; ++x ) {
			ZLColorVec colorVec;
			u32 color = this->GetColor(x + rect.mXMin, y + rect.mYMin);
			colorVec.SetRGBA(color);
			// Points inside get marked with a dx/dy of zero.
			// Points outside get marked with an infinitely large distance.
			if ( colorVec.mA == 0.0f ) {
				grid1[y][x] = inside;
				grid2[y][x] = empty;
			}
			else {
				grid2[y][x] = inside;
				grid1[y][x] = empty;
			}
		}
	}
	
	CalculateSDF( grid1, width, height );
	CalculateSDF( grid2, width, height );
		
	int maxDist = INT_MIN;
	int minDist = INT_MAX;
	
	for( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			// Calculate the actual distance from the dx/dy
			int dist1 = (int)( grid1[y][x].Length() );
			int dist2 = (int)( grid2[y][x].Length() );
			int dist = dist1 - dist2;
			gridDistance[y][x] = dist;

			if (minDist > dist)
				minDist = dist;
			if (maxDist < dist)
				maxDist = dist;
		}
	}
	
	// Hard coded spread factor for testing, need to pass in!
	int range = 60;

	// Have to scale the distance value from minDis - maxDis to 0 - 1
	for( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			
			float scaledDistVal = gridDistance[y][x];
			scaledDistVal = ( scaledDistVal + 30 ) / range;
			ZLColorVec colorVec;
			colorVec.Set ( 0, 0, 0, scaledDistVal );
			this->SetColor ( x + rect.mXMin, y + rect.mYMin, colorVec.PackRGBA() );

		}
	}
	
	
	for ( int i = 0; i < height; i++ ) {
  		delete [] grid1[i];
		delete [] grid2[i];
		delete [] gridDistance[i];
  	}
	
	delete [] grid1;
	delete [] grid2;
	delete [] gridDistance;
}

//----------------------------------------------------------------//
void MOAIImage::GenerateSDFDeadReckoning( ZLIntRect rect, int threshold ) {
	
	// Specified in the paper
	// d1 is horizontal pixel distance, d2 is diagonal pixel distance
	const int d1 = 3;
	const int d2 = 4;
	
	// Plus one because rect goes to exact end
	int width = rect.Width () + 1;
	int height = rect.Height () + 1;
	int size = height * width;
	
	// Treating 1d array as 2d
	float* distanceMap = ( float * ) malloc ( sizeof ( float ) * size );
	int* binaryMap = ( int * ) malloc ( sizeof ( int ) * size );
	
	if ( distanceMap == NULL || binaryMap == NULL ) {
		printf("ERROR: Out of memory\n");
		return;
	}
	
	// Init the binary map and distance map
	for ( int y = 0; y < height; ++y ) {
		for ( int x = 0; x < width; ++x ) {
			
			u32 color = this->GetColor ( x + rect.mXMin, y + rect.mYMin );
			ZLColorVec colorVec;
			colorVec.SetRGBA (color);
			//printf("color: %f, %f, %f, %f\n", colorVec.mR, colorVec.mG, colorVec.mB, colorVec.mA);
			if ( colorVec.mA > 0.5f ) {
				binaryMap[y * width + x] = 1;
			}
			else {
				binaryMap[y * width + x] = 0;
			}
			
			distanceMap[y * width + x] = FLT_MAX;
		}
	}
	
	// Looking for the edge
	for ( int y = 1; y < height - 1; ++y ) {
		for ( int x = 1; x < width - 1; ++x ) {
			
			int currentVal = binaryMap[y * width + x];
			if ( binaryMap[y * width + (x - 1)] != currentVal ||
				 binaryMap[y * width + (x + 1)] != currentVal ||
				 binaryMap[(y - 1) * width + x] != currentVal ||
				 binaryMap[(y + 1) * width + x] != currentVal ) {
				distanceMap[y * width + x] = 0;
			}
		}
	}
	
	
	// Perform first pass
	for ( int y = 1; y < height - 1; ++y ) {
		for (int x = 1; x < width - 1; ++x ) {
			
			float *pCurDistVal = &distanceMap[y * width + x];
			
			if ( distanceMap[(y - 1) * width + (x - 1)] + d2 < *pCurDistVal )
				*pCurDistVal = distanceMap[(y - 1) * width + (x - 1)] + d2;
			
			if ( distanceMap[(y - 1) * width + x] + d1 < *pCurDistVal)
				*pCurDistVal = distanceMap[(y - 1) * width + x] + d1;
			
			if ( distanceMap[(y - 1) * width + (x + 1)] + d2 < *pCurDistVal )
				*pCurDistVal = distanceMap[(y - 1) * width + (x + 1)] + d2;
			
			if ( distanceMap[y * width + (x - 1)] + d1 < *pCurDistVal )
				*pCurDistVal = distanceMap[y * width + (x - 1)] + d1;
		}
	}
	
	// Perform second pass
	for ( int y = height - 2; y > 0; --y ) {
		for ( int x = width - 2; x > 0; --x ) {
			
			float *pCurDistVal = &distanceMap[y * width + x];
			
			if ( distanceMap[y * width + (x + 1)] + d1 < *pCurDistVal)
				*pCurDistVal = distanceMap[y * width + (x + 1)] + d1;
			
			if ( distanceMap[(y + 1) * width + (x - 1)] + d2 < *pCurDistVal)
				*pCurDistVal = distanceMap[(y + 1) * width + (x - 1)] + d2;
			
			if ( distanceMap[(y + 1) * width + x] + d1 < *pCurDistVal)
				*pCurDistVal = distanceMap[(y + 1) * width + x] + d1;
			
			if ( distanceMap[(y + 1) * width + (x + 1)] + d2 < *pCurDistVal)
				*pCurDistVal = distanceMap[(y + 1) * width + (x + 1)] + d2;
		}
	}
	
	// Indicate inside and outside
	for ( int y = height - 1; y > 0; --y ) {
		for ( int x = width - 1; x > 0; --x ) {
			
			if ( binaryMap[y * width + x] == 0 )
				distanceMap[y * width + x] *= -1;
		}
	}
	
	// Hard coded spread factor for testing, need to pass in!
	int half = threshold / 2;
	
	// Have to scale the distance value from minDis - maxDis to 0 - 1
	for( int y = 0; y < height; y++ ) {
		for ( int x = 0; x < width; x++ ) {
			
			float scaledDistVal = distanceMap[y * width + x];
			scaledDistVal = ( scaledDistVal + half ) / threshold;
			
			// If distance is more than the max threshold specified, snap to 0
			if ( scaledDistVal < 0 )
				scaledDistVal = 0;
			
			ZLColorVec colorVec;
			colorVec.Set ( 0, 0, 0, scaledDistVal );
			this->SetColor ( x + rect.mXMin, y + rect.mYMin, colorVec.PackRGBA ());
			
		}
	}
	
	free ( binaryMap );
	free ( distanceMap );
}

//----------------------------------------------------------------//
u32 MOAIImage::GetBitmapSize () const {

	return this->GetRowSize () * this->mHeight;
}

//----------------------------------------------------------------//
ZLIntRect MOAIImage::GetBounds () {

	ZLIntRect bounds;
	bounds.Init ( 0, 0, this->mWidth, this->mHeight );
	return bounds;
}

//----------------------------------------------------------------//
u32 MOAIImage::GetColor ( u32 x, u32 y ) const {

	if ( !this->mBitmap ) return 0;

	if ( this->mPixelFormat == TRUECOLOR ) {
		return ZLColor::ConvertToRGBA ( this->GetPixel ( x, y ), this->mColorFormat );
	}
	else {
		return this->GetPaletteColor ( this->GetPixel ( x, y ));
	}

	return 0;	
}

//----------------------------------------------------------------//
u32 MOAIImage::GetDataSize () const {

	return this->GetPaletteSize () + this->GetBitmapSize ();
}

//----------------------------------------------------------------//
u32 MOAIImage::GetMinPowerOfTwo ( u32 size ) {

	if ( MOAIImage::IsPow2 ( size )) return size;

	u32 pow2 = 1;
	while ( pow2 < size ) pow2 = pow2 << 0x01;
	return pow2;
}

//----------------------------------------------------------------//
u32 MOAIImage::GetPaletteColor ( u32 i ) const {

	u32 total = this->GetPaletteCount ();
	if ( i < total ) {
		u32 pixel = ZLBitBuffer::GetValue ( this->mPalette, i, ZLColor::GetDepthInBits ( this->mColorFormat ));
		return ZLColor::ConvertToRGBA ( pixel, this->mColorFormat );
	}
	return 0;
}

//----------------------------------------------------------------//
u32 MOAIImage::GetPaletteCount () const {

	switch ( this->mPixelFormat ) {
		case TRUECOLOR:		return 0;
		case INDEX_4:		return 16;
		case INDEX_8:		return 256;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
u32 MOAIImage::GetPaletteSize () const {
	
	return ZLBitBuffer::CalculateSize ( ZLColor::GetDepthInBits ( this->mColorFormat ), this->GetPaletteCount ());
}

//----------------------------------------------------------------//
u32 MOAIImage::GetPixel ( u32 x, u32 y ) const {
	
	if (( x >= this->mWidth ) || ( y >= this->mHeight )) return 0;
	
	return ZLBitBuffer::GetValue ( this->GetRowAddr ( y ), x, this->GetPixelDepthInBits ());
}

//----------------------------------------------------------------//
u32 MOAIImage::GetPixelDepthInBits () const {

	switch ( this->mPixelFormat ) {
		case TRUECOLOR:		return ZLColor::GetDepthInBits ( this->mColorFormat );
		case INDEX_4:		return 4;
		case INDEX_8:		return 8;
		default:			break;
	}
	return 0;
}

//----------------------------------------------------------------//
ZLIntRect MOAIImage::GetRect () {

	ZLIntRect rect;
	rect.Init ( 0, 0, this->mWidth, this->mHeight );
	return rect;
}

//----------------------------------------------------------------//
void* MOAIImage::GetRowAddr ( u32 y ) {

	return ( void* )(( size_t )this->mBitmap + ( this->GetRowSize () * y ));
}

//----------------------------------------------------------------//
const void* MOAIImage::GetRowAddr ( u32 y ) const {

	return ( const void* )(( size_t )this->mBitmap + ( this->GetRowSize () * y ));
}

//----------------------------------------------------------------//
size_t MOAIImage::GetRowSize () const {

	return ZLBitBuffer::CalculateSize ( this->GetPixelDepthInBits (), this->mWidth );
}

//----------------------------------------------------------------//
void MOAIImage::GetSubImage ( const MOAIImage& image, ZLIntRect rect ) {

	int width = rect.Width ();
	int height = rect.Height ();
	
	this->Init ( width, height, image.mColorFormat, image.mPixelFormat );
	this->Blit ( image, rect.mXMin, rect.mYMin, 0, 0, width, height );
}

//----------------------------------------------------------------//
void MOAIImage::Init ( const MOAIImage& image ) {

	this->Init ( image.mWidth, image.mHeight, image.mColorFormat, image.mPixelFormat );
}

//----------------------------------------------------------------//
void MOAIImage::Init ( u32 width, u32 height, ZLColor::ColorFormat colorFmt, PixelFormat pixelFmt ) {

	this->Clear ();

	this->mColorFormat = colorFmt;
	this->mPixelFormat = pixelFmt;

	this->mWidth = width;
	this->mHeight = height;
	
	this->Alloc ();
	this->ClearBitmap ();
	
	this->OnImageStatusChanged ( this->IsOK ());
}

//----------------------------------------------------------------//
void MOAIImage::Init ( void* bitmap, u32 width, u32 height, ZLColor::ColorFormat colorFmt ) {

	this->Clear ();

	if ( !bitmap ) return;

	this->mPixelFormat = TRUECOLOR;
	this->mColorFormat = colorFmt;

	this->mWidth = width;
	this->mHeight = height;
	
	this->Alloc ();
	u32 size = this->GetBitmapSize ();
	memcpy ( this->mBitmap, bitmap, size );
	
	this->OnImageStatusChanged ( this->IsOK ());
}

//----------------------------------------------------------------//
bool MOAIImage::IsPow2 () {

	return ( MOAIImage::IsPow2 ( this->mWidth ) && MOAIImage::IsPow2 ( this->mHeight ));
}

//----------------------------------------------------------------//
bool MOAIImage::IsPow2 ( u32 n ) {

	return (( n == 1 ) || (( n & ( n - 1 )) == 0 ));
}

//----------------------------------------------------------------//
bool MOAIImage::Load ( cc8* filename, u32 transform ) {

	this->Clear ();
	
	ZLFileStream stream;
	if ( stream.OpenRead ( filename )) {
		this->Load ( stream, transform );
		stream.Close ();
		this->OnImageStatusChanged ( this->IsOK ());
	}
	else {
		MOAILog ( NULL, MOAILogMessages::MOAI_FileOpenError_S, filename );
	}
	return this->IsOK ();
}

//----------------------------------------------------------------//
bool MOAIImage::Load ( ZLStream& stream, u32 transform ) {
	UNUSED ( stream );
	UNUSED ( transform );

	this->Clear ();
	
	#if MOAI_WITH_LIBPNG
		if ( MOAIImage::IsPng ( stream )) {
			this->LoadPng ( stream, transform );
			this->OnImageStatusChanged ( this->IsOK ());
			return this->IsOK ();
		}
	#endif
	
	#if MOAI_WITH_LIBJPG
		if ( MOAIImage::IsJpg ( stream )) {
			this->LoadJpg ( stream, transform );
			this->OnImageStatusChanged ( this->IsOK ());
			return this->IsOK ();
		}
	#endif
	
	#if MOAI_WITH_LIBWEBP
		if ( MOAIImage::IsWebP ( stream )) {
			this->LoadWebP ( stream, transform );
			this->OnImageStatusChanged ( this->IsOK ());
			return this->IsOK ();
		}
	#endif
	
	return false;
}

//----------------------------------------------------------------//
bool MOAIImage::IsOK () {

	return ( this->mBitmap != 0 );
}

//----------------------------------------------------------------//
bool MOAIImage::MipReduce () {

	if (( this->mWidth == 0 ) || ( this->mHeight == 0 )) return false;
	if (( this->mWidth <= 1 ) && ( this->mHeight <= 1 )) return false;

	u32 width = this->mWidth & ~1;
	u32 height = this->mHeight & ~1;

	MOAIImage nextMip;

	if ( width == 0 ) {
	
		nextMip.Init ( 1, height >> 1, this->mColorFormat, this->mPixelFormat );
		
		for ( u32 y = 0; y < height; y += 2 ) {
			
			u32 c0 = this->GetColor ( 0, y );
			u32 c1 = this->GetColor ( 0, y + 1 );

			u32 result = ZLColor::Average ( c0, c1 );
			nextMip.SetColor ( 0, y >> 1, result );
		}
	}
	else if ( height == 0 ) {
	
		nextMip.Init ( width >> 1, 1, this->mColorFormat, this->mPixelFormat );
	
		for ( u32 x = 0; x < width; x += 2 ) {
			
			u32 c0 = this->GetColor ( x, 0 );
			u32 c1 = this->GetColor ( x + 1, 0 );

			u32 result = ZLColor::Average ( c0, c1 );
			nextMip.SetColor ( x >> 1, 0, result );
		}
	}
	else {
		
		nextMip.Init ( width >> 1, height >> 1, this->mColorFormat, this->mPixelFormat );

		for ( u32 y0 = 0; y0 < height; y0 += 2 ) {
			for ( u32 x0 = 0; x0 < width; x0 += 2 ) {
				
				u32 x1 = x0 + 1;
				u32 y1 = y0 + 1;
				
				u32 c0 = this->GetColor ( x0, y0 );
				u32 c1 = this->GetColor ( x1, y0 );
				u32 c2 = this->GetColor ( x0, y1 );
				u32 c3 = this->GetColor ( x1, y1 );

				u32 result = ZLColor::Average ( c0, c1, c2, c3 );
				nextMip.SetColor ( x0 >> 1, y0 >> 1, result );
			}
		}
	}
	this->Take ( nextMip );
	return true;
}

//----------------------------------------------------------------//
void MOAIImage::Mix ( const MOAIImage& image, const ZLMatrix4x4& mtx, float K ) {
	
	if ( this != &image ) {
		this->Copy ( image );
	}

	if ( this->mPixelFormat == TRUECOLOR ) {
		for ( u32 y = 0; y < this->mHeight; y++ ) {
			ZLColor::Mix ( this->GetRowAddr ( y ), this->mColorFormat, this->mWidth, mtx, K );
		}
	}
	else {
		ZLColor::Mix ( this->GetPalette (), this->mColorFormat, this->GetPaletteCount (), mtx, K );
	}
}

//----------------------------------------------------------------//
MOAIImage::MOAIImage () :
	mPixelFormat ( PXL_FMT_UNKNOWN ),
	mColorFormat ( ZLColor::CLR_FMT_UNKNOWN ),
	mWidth ( 0 ),
	mHeight ( 0 ),
	mBitmap ( 0 ),
	mPalette ( 0 ) {
	
	RTTI_SINGLE ( MOAILuaObject )
}

//----------------------------------------------------------------//
MOAIImage::~MOAIImage () {

	this->Clear ();
}

//----------------------------------------------------------------//
void MOAIImage::OnImageStatusChanged ( bool isOK ) {
	UNUSED ( isOK );
}

//----------------------------------------------------------------//
void MOAIImage::PadToPow2 ( const MOAIImage& image ) {

	ZLIntRect canvas;
	canvas.mXMin = 0;
	canvas.mYMin = 0;
	canvas.mXMax = this->GetMinPowerOfTwo ( image.GetWidth ());
	canvas.mYMax = this->GetMinPowerOfTwo ( image.GetHeight ());
	
	this->ResizeCanvas ( image, canvas );
}

//----------------------------------------------------------------//
void MOAIImage::PremultiplyAlpha ( const MOAIImage& image ) {

	if ( this != &image ) {
		this->Copy ( image );
	}

	if ( this->mPixelFormat == TRUECOLOR ) {
		for ( u32 y = 0; y < this->mHeight; ++y ) {
			ZLColor::PremultiplyAlpha ( this->GetRowAddr ( y ), this->mColorFormat, this->mWidth );
		}
	}
	else {
		ZLColor::PremultiplyAlpha ( this->mPalette, this->mColorFormat, this->GetPaletteCount ());
	}
}

//----------------------------------------------------------------//
void MOAIImage::RegisterLuaClass ( MOAILuaState& state ) {
	
	state.SetField ( -1, "FILTER_LINEAR",				( u32 )MOAIImage::FILTER_LINEAR );
	state.SetField ( -1, "FILTER_NEAREST",				( u32 )MOAIImage::FILTER_NEAREST );
	
	state.SetField ( -1, "POW_TWO",						( u32 )MOAIImageTransform::POW_TWO );
	state.SetField ( -1, "QUANTIZE",					( u32 )MOAIImageTransform::QUANTIZE );
	state.SetField ( -1, "TRUECOLOR",					( u32 )MOAIImageTransform::TRUECOLOR );
	state.SetField ( -1, "PREMULTIPLY_ALPHA",			( u32 )MOAIImageTransform::PREMULTIPLY_ALPHA );
	
	state.SetField ( -1, "PIXEL_FMT_TRUECOLOR",			( u32 )TRUECOLOR );
	state.SetField ( -1, "PIXEL_FMT_INDEX_4",			( u32 )INDEX_4 );
	state.SetField ( -1, "PIXEL_FMT_INDEX_8",			( u32 )INDEX_8 );
	
	state.SetField ( -1, "COLOR_FMT_A_1",				( u32 )ZLColor::A_1 );
	state.SetField ( -1, "COLOR_FMT_A_4",				( u32 )ZLColor::A_4 );
	state.SetField ( -1, "COLOR_FMT_A_8",				( u32 )ZLColor::A_8 );
	state.SetField ( -1, "COLOR_FMT_RGB_888",			( u32 )ZLColor::RGB_888 );
	state.SetField ( -1, "COLOR_FMT_RGB_565",			( u32 )ZLColor::RGB_565 );
	state.SetField ( -1, "COLOR_FMT_RGBA_5551",			( u32 )ZLColor::RGBA_5551 );
	state.SetField ( -1, "COLOR_FMT_RGBA_4444",			( u32 )ZLColor::RGBA_4444 );
	state.SetField ( -1, "COLOR_FMT_RGBA_8888",			( u32 )ZLColor::RGBA_8888 );
	
	state.SetField ( -1, "BLEND_EQ_ADD",						( u32 )ZLColor::BLEND_EQ_ADD );
	state.SetField ( -1, "BLEND_EQ_NONE",						( u32 )ZLColor::BLEND_EQ_NONE );
	state.SetField ( -1, "BLEND_EQ_SUBTRACT",					( u32 )ZLColor::BLEND_EQ_SUBTRACT );
	
	state.SetField ( -1, "BLEND_FACTOR_ONE",					( u32 )ZLColor::BLEND_FACTOR_ONE);
	state.SetField ( -1, "BLEND_FACTOR_ZERO",					( u32 )ZLColor::BLEND_FACTOR_ZERO );
	state.SetField ( -1, "BLEND_FACTOR_DST_ALPHA",				( u32 )ZLColor::BLEND_FACTOR_DST_ALPHA );
	state.SetField ( -1, "BLEND_FACTOR_DST_COLOR",				( u32 )ZLColor::BLEND_FACTOR_DST_COLOR);
	state.SetField ( -1, "BLEND_FACTOR_ONE_MINUS_DST_ALPHA",	( u32 )ZLColor::BLEND_FACTOR_ONE_MINUS_DST_ALPHA );
	state.SetField ( -1, "BLEND_FACTOR_ONE_MINUS_DST_COLOR",	( u32 )ZLColor::BLEND_FACTOR_ONE_MINUS_DST_COLOR );
	state.SetField ( -1, "BLEND_FACTOR_ONE_MINUS_SRC_ALPHA",	( u32 )ZLColor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA );
	state.SetField ( -1, "BLEND_FACTOR_ONE_MINUS_SRC_COLOR",	( u32 )ZLColor::BLEND_FACTOR_ONE_MINUS_SRC_COLOR );
	state.SetField ( -1, "BLEND_FACTOR_SRC_ALPHA",				( u32 )ZLColor::BLEND_FACTOR_SRC_ALPHA );
	state.SetField ( -1, "BLEND_FACTOR_SRC_COLOR",				( u32 )ZLColor::BLEND_FACTOR_SRC_COLOR );
}

//----------------------------------------------------------------//
void MOAIImage::RegisterLuaFuncs ( MOAILuaState& state ) {
	UNUSED ( state );

	luaL_Reg regTable [] = {
		{ "bleedRect",					_bleedRect },
		{ "compare",					_compare },
		{ "convert",					_convert },
		{ "convertColors",				_convert }, // back compat
		{ "convertToGrayScale",			_desaturate }, // back compat
		{ "copy",						_copy },
		{ "copyBits",					_copyBits },
		{ "copyRect",					_copyRect },
		{ "desaturate",					_desaturate },
		{ "fillCircle",					_fillCircle },
		{ "fillRect",					_fillRect },
		{ "generateOutlineFromSDF",		_generateOutlineFromSDF },
		{ "generateSDF",				_generateSDF },
		{ "generateSDFDeadReckoning",	_generateSDFDeadReckoning },
		{ "getColor32",					_getColor32 },
		{ "getFormat",					_getFormat },
		{ "getRGBA",					_getRGBA },
		{ "getSize",					_getSize },
		{ "init",						_init },
		{ "load",						_load },
		{ "loadFromBuffer",				_loadFromBuffer },
		{ "mix",						_mix },
		{ "padToPow2",					_padToPow2 },
		{ "resize",						_resize },
		{ "resizeCanvas",				_resizeCanvas },
		{ "setColor32",					_setColor32 },
		{ "setRGBA",					_setRGBA },
		{ "simpleThreshold",			_simpleThreshold },
		{ "writePNG",					_writePNG },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIImage::ResizeCanvas ( const MOAIImage& image, ZLIntRect rect ) {

	// TODO: specify sides (a la photoshop)

	rect.Bless ();
	
	int width = rect.Width ();
	int height = rect.Height ();
	
	MOAIImage newImage;
	newImage.Init (( u32 )width, ( u32 )height, image.mColorFormat, image.mPixelFormat );
	
	ZLIntRect srcRect;
	srcRect.mXMin = -rect.mXMin;
	srcRect.mYMin = -rect.mYMin;
	srcRect.mXMax = ( int )image.GetWidth () + srcRect.mXMin;
	srcRect.mYMax = ( int )image.GetHeight () + srcRect.mYMin;
	
	rect.Offset ( -rect.mXMin, -rect.mYMin );
	
	if ( srcRect.Overlap ( rect )) {
	
		u32 beginSpan = 0;
		u32 leftSize = 0;
		
		if ( srcRect.mXMin > 0 ) {
			beginSpan = srcRect.mXMin;
			leftSize = beginSpan;
		}
		
		u32 endSpan = width;
		u32 rightSize = 0;
		
		if ( srcRect.mXMax < width ) {
			endSpan = srcRect.mXMax;
			rightSize = width - endSpan;
		}
		
		u32 spanSize = endSpan - beginSpan;
		
		u32 bitDepth = this->GetPixelDepthInBits ();
		
		u32 srcRowSize = image.GetRowSize ();
		u32 srcRowXOff = srcRect.mXMin < 0 ? -srcRect.mXMin : 0;
		
		for ( int y = 0; y < height; ++y ) {
		
			void* destRow = newImage.GetRowAddr ( y );
		
			if (( y < srcRect.mYMin ) || ( y >= srcRect.mYMax )) {
			
				ZLBitBuffer::Clear ( destRow, bitDepth, 0, newImage.mWidth );
			}
			else {
			
				if ( leftSize ) {
					ZLBitBuffer::Clear ( destRow, bitDepth, 0, leftSize );
				}
				
				if ( spanSize ) {
					const void* srcRow = image.GetRowAddr ( y - srcRect.mYMin );
					ZLBitBuffer::Blit ( destRow, leftSize, srcRow, srcRowXOff, spanSize, bitDepth );
				}
				
				if ( rightSize ) {
					ZLBitBuffer::Clear ( destRow, bitDepth, leftSize + spanSize, rightSize );
				}
			}
		}
	}
	else {
		newImage.ClearBitmap ();
	}
	this->Take ( newImage );
}

//----------------------------------------------------------------//
u32 MOAIImage::SampleColor ( float x, float y, u32 filter ) const {

	u32 x0 = ( u32 )floorf ( x );
	u32 y0 = ( u32 )floorf ( y );
	
	u32 x1 = x0 + 1;
	u32 y1 = y0 + 1;
	
	if ( x1 >= this->mWidth ) {
		x1 = this->mWidth - 1;
	}
	
	if ( y1 >= this->mHeight ) {
		y1 = this->mHeight - 1;
	}
	
	u32 c0 = this->GetColor ( x0, y0 );
	u32 c1 = this->GetColor ( x1, y0 );
	u32 c2 = this->GetColor ( x0, y1 );
	u32 c3 = this->GetColor ( x1, y1 );

	u8 xt = ( u8 )(( x - ( float )x0 ) * 255.0f );
	u8 yt = ( u8 )(( y - ( float )y0 ) * 255.0f );
	
	if ( filter == FILTER_LINEAR ) {
		return ZLColor::BilerpFixed ( c0, c1, c2, c3, xt, yt );
	}
	return ZLColor::NearestNeighbor ( c0, c1, c2, c3, xt, yt );
}

//----------------------------------------------------------------//
void MOAIImage::SerializeIn ( MOAILuaState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIImage::SerializeOut ( MOAILuaState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAIImage::SetColor ( u32 x, u32 y, u32 color ) {

	if ( this->mPixelFormat != TRUECOLOR ) return;

	if ( y > this->mHeight ) return;
	if ( x > this->mWidth ) return;

	u32 pixel = ZLColor::ConvertFromRGBA ( color, this->mColorFormat );
	this->SetPixel ( x, y, pixel );
}

//----------------------------------------------------------------//
void MOAIImage::SetColor ( u32 x, u32 y, u32 color, const ZLColorBlendFunc& blendFunc ) {

	if ( this->mPixelFormat != TRUECOLOR ) return;
	
	if ( y > this->mHeight ) return;
	if ( x > this->mWidth ) return;

	if ( blendFunc.mEquation != ZLColor::BLEND_EQ_NONE ) {
		u32 dstColor = this->GetColor ( x, y );
		color = ZLColor::Blend ( color, dstColor, blendFunc );
	}
	u32 pixel = ZLColor::ConvertFromRGBA ( color, this->mColorFormat );
	this->SetPixel ( x, y, pixel );
}

//----------------------------------------------------------------//
void MOAIImage::SetPaletteColor ( u32 idx, u32 rgba ) {

	u32 total = this->GetPaletteCount ();
	if ( idx < total ) {
		
		u32 color = ZLColor::ConvertFromRGBA ( rgba, this->mColorFormat );
		u32 colorDepth = ZLColor::GetDepthInBits ( this->mColorFormat );
		
		ZLBitBuffer::SetValue ( this->mPalette, color, idx, colorDepth );
	}
}

//----------------------------------------------------------------//
void MOAIImage::SetPixel ( u32 x, u32 y, u32 pixel ) {

	if (( x >= this->mWidth ) || ( y >= this->mHeight )) return;
	
	ZLBitBuffer::SetValue ( this->GetRowAddr ( y ), pixel, x, this->GetPixelDepthInBits ());
}

//----------------------------------------------------------------//
void MOAIImage::SimpleThreshold ( const MOAIImage& image, float rT, float gT, float bT, float aT ) {

	if ( this != &image ) {
		this->Copy ( image );
	}

	u32 threshold = ZLColor::ConvertFromRGBA ( ZLColor::PackRGBA ( rT, gT, bT, aT ), this->mColorFormat );
	
	if ( this->mPixelFormat == TRUECOLOR ) {
		for ( u32 y = 0; y < this->mHeight; ++y ) {
			ZLColor::SimpleThreshold ( this->GetRowAddr ( y ), this->mColorFormat, this->mWidth, threshold );
		}
	}
	else {
		ZLColor::SimpleThreshold ( this->mPalette, this->mColorFormat, this->GetPaletteCount (), threshold );
	}
}

//----------------------------------------------------------------//
void MOAIImage::Take ( MOAIImage& image ) {

	this->Clear ();

	this->mColorFormat	= image.mColorFormat;
	this->mPixelFormat	= image.mPixelFormat;

	this->mWidth		= image.mWidth;
	this->mHeight		= image.mHeight;
	
	this->mBitmap		= image.mBitmap;
	this->mPalette		= image.mPalette;

	// kill the data before clear
	image.mBitmap = 0;
	image.mPalette = 0;
	image.Clear ();
}

//----------------------------------------------------------------//
void MOAIImage::Transform ( u32 transform ) {
	
	if ( !transform ) return;
	
	if ( transform & MOAIImageTransform::PREMULTIPLY_ALPHA ) {
		this->PremultiplyAlpha ( *this );
	}
	
	ZLColor::ColorFormat colorFormat = this->mColorFormat;
	PixelFormat pixelFormat = this->mPixelFormat;
	
	if ( transform & MOAIImageTransform::QUANTIZE ) {
	
		if ( this->mColorFormat == ZLColor::RGB_888 ) {
			colorFormat = ZLColor::RGB_565;
		}
		
		if ( this->mColorFormat == ZLColor::RGBA_8888 ) {
			colorFormat = ZLColor::RGBA_4444;
		}
	}
	
	if ( transform & MOAIImageTransform::TRUECOLOR ) {
		pixelFormat = TRUECOLOR;
	}
	
	this->Convert ( *this, colorFormat, pixelFormat );
	
	if ( transform & MOAIImageTransform::POW_TWO ) {
		this->PadToPow2 ( *this );
	}
}
