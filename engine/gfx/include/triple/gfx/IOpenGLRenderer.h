#ifndef I_OPENGL_RENDERER_H
#define I_OPENGL_RENDERER_H

#include "IRenderer.h"

namespace triple::gfx {
	class IOpenGLRenderer : public virtual IRenderer {
	  public:
		virtual bool initGlad(void *loader) = 0;
	};
} // namespace triple::gfx

#endif // I_OPENGL_RENDERER_H
