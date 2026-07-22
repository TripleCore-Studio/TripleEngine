# 0002: Renderer Owns Present/Swap

## Status
Accepted

## Context
The engine window (`IWindow`, core-owned) previously called `swapBuffers()`
directly. This works for OpenGL (`glfwSwapBuffers`), but doesn't generalize —
DirectX/Vulkan present through the graphics API itself (`IDXGISwapChain::Present`,
`vkQueuePresent`), not through a windowing call. Keeping present() on the
window would mean `IWindow` needs backend-specific knowledge, defeating the
purpose of the `IRenderer` module boundary.

## Decision
`IRenderer` owns presentation via an explicit `present()` method, called by
core after `endFrame()`. `IWindow` no longer performs the buffer swap itself.

## Reasons
- Keeps windowing (`IWindow`) fully decoupled from the graphics backend
- Matches how DX/Vulkan actually expose presentation (through the graphics
  API/swapchain, not the OS window)
- Separates "finish processing this frame's commands" (`endFrame`) from
  "show the result to the user" (`present`) — distinct responsibilities,
  useful if frame recording/execution is ever decoupled (e.g. multi-threaded
  rendering with a frame of latency)

## Consequences
- `IWindow::swapBuffers()` is unused/removed; core calls `renderer->present()` instead
- Every `IRenderer` backend must implement `present()`, even trivial ones (GL: `glfwSwapBuffers`)