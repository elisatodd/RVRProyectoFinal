#ifndef Window_H
#define Window_H

#include <string>
#include <map>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Singleton.h"
#include "SDLUtils.h"

class Window: public Singleton<Window> {

	friend Singleton<Window>;

public:

	virtual ~Window();

	Window(Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(Window&) = delete;
	Window& operator=(Window&&) = delete;

	inline SDL_Window* SDLWindow() {
		return m_window;
	}

	// access to the underlying SDL_Renderer -- needed when creating textures
	// other than those initialized in this class
	inline SDL_Renderer* renderer() {
		return m_renderer;
	}

	// clear the renderer with a given SDL_Color
	inline void clearRenderer(SDL_Color bg = build_sdlcolor(0xFFFFFFF)) {
		SDL_SetRenderDrawColor(m_renderer, COLOREXP(bg));
		SDL_RenderClear(m_renderer);
	}

	// present the current content of the renderer
	inline void presentRenderer() {
		SDL_RenderPresent(m_renderer);
	}

	inline int width() {
		return m_width;
	}

	inline int height() {
		return m_height;
	}

	// toggle to full-screen/Window mode
	inline void toggleFullScreen() {
		auto flags = SDL_GetWindowFlags(m_window);
		if (flags & SDL_WINDOW_FULLSCREEN) {
			SDL_SetWindowFullscreen(m_window, 0);
		} else {
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
		}
	}

	// show the cursor when mouse is over the Window
	inline void showCursor() {
		SDL_ShowCursor(1);
	}

	// hide the cursor when mouse is over the Window
	inline void hideCursor() {
		SDL_ShowCursor(0);
	}

	// Access to real time
	inline Uint32 currRealTime() const {
		return SDL_GetTicks();
	}

private:
	Window();
	Window(std::string window_title, int width, int height);

	void initWindow();
	void closeWindow();
	void initSDLExtensions(); // initialize resources (fonts, textures, audio, etc.)
	void closeSDLExtensions();

	std::string m_window_title;
	int m_width;
	int m_height;

	SDL_Window* m_window; // reference to SDL window
	SDL_Renderer* m_renderer; // reference to SDL renderer
};
#endif

// This macro defines a compact way for using the singleton Window, instead of
// writing Window::instance()->method() we write Window().method()
inline Window& Window() {
	return *Window::instance();
}