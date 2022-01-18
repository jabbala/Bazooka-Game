#include "SFML-2.5.1/include/SFML/Graphics.hpp"
#include "Enemy.h"


Enemy::Enemy() {
	m_speed = 0.0f;
}
Enemy::~Enemy() {}

void Enemy::init(std::string textureName, sf::Vector2f position, float speed)
{
	m_speed = speed;
	m_position = position;

	//Load a texure
	m_texture.loadFromFile(textureName.c_str());
	
	// Create Sprite and Attach a Texture
	m_sprite.setTexture(m_texture);
	m_sprite.setPosition(m_position);
	m_sprite.setOrigin(m_texture.getSize().x / 2, m_texture.getSize().y / 2);
}
void Enemy::update(float dt)
{
	m_sprite.move(m_speed * dt, 0);
}

sf::Sprite Enemy::getSprite() 
{
	return m_sprite;
}