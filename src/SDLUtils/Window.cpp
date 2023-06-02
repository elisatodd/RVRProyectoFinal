#include "Window.h"

#include <cassert>

Window::Window() :
		Window("RVR Proyecto Final", 600, 400) {
}

Window::Window(std::string window_title, int width, int height) :
		m_window_title(window_title),
		m_width(width),
		m_height(height) {

	initWindow();
	initSDLExtensions();
}

Window::~Window() {
	closeSDLExtensions();
	closeWindow();
}

void Window::initWindow() {
	// initialise SDL
	int sdlInit_ret = SDL_Init(SDL_INIT_EVERYTHING);
	assert(sdlInit_ret == 0);

	// Create window
	m_window = SDL_CreateWindow(m_window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_SHOWN);
	assert(m_window != nullptr);

	// Create the renderer
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	assert(m_renderer != nullptr);
}

void Window::initSDLExtensions() {
	// initialize SDL_ttf
	int ttfInit_r = TTF_Init();
	assert(ttfInit_r == 0);

	// initialize SDL_image
	int imgInit_ret = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
	assert(imgInit_ret != 0);
}

void Window::closeWindow() {
	// destroy renderer and window
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);

	SDL_Quit(); // quit SDL
}

void Window::closeSDLExtensions() {
	IMG_Quit(); // quit SDL_image
	TTF_Quit(); // quit SDL_ttf
}