
#ifndef GAINPUTDEBUGRENDERER_H_
#define GAINPUTDEBUGRENDERER_H_

namespace gainput
{

/// Interface for debug rendering of input device states.
class GAINPUT_LIBEXPORT DebugRenderer
{
public:
	virtual void DrawCircle(float x, float y, float radius) = 0;

	virtual void DrawLine(float x1, float y1, float x2, float y2) = 0;

	virtual void DrawText(float x, float y, const char* const text) = 0;
};

}

#endif

