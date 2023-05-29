#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <cassert>

#include <SDL2/SDL.h>

#include "Vector2D.h"

class GameObject {
public:
	GameObject() : m_transform(), m_size(), m_rotation(0), m_texture(nullptr), m_texture_size() {
		m_enabled = true;
	}

	virtual ~GameObject();

	virtual void handleInput(const SDL_Event &event) {};
	virtual void update() {};

	inline bool isEnabled() const {
		return m_enabled;
	}

	inline bool setEnabled(bool e) {
		return m_enabled = e;
	}

	virtual void render();

	void setTransform(const int x, const int y);

	Vector2D getTransform() const;

	void setTransform(Vector2D tr);

	void setRotation(const float rot);

	void setSize(const int w, const int h);

	Vector2D getSize() const;

	void setTexture(const std::string& path);

protected:

	bool m_enabled;

	Vector2D m_transform;
	Vector2D m_size;
	float m_rotation;

	SDL_Texture* m_texture;
	Vector2D m_texture_size;
    
	void render(const SDL_Rect &src, const SDL_Rect &dest,
            const SDL_Point *p = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
};
#endif