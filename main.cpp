// Shooter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "SFML-2.5.1\include\SFML\Graphics.hpp"
#include "SFML-2.5.1\include\SFML\Audio.hpp"

#include "Hero.h"
#include "Enemy.h"
#include "Rocket.h"


sf::Vector2f viewSize(1024, 768);
sf::VideoMode vm(viewSize.x, viewSize.y);
sf::RenderWindow window(vm, "Shooter Game", sf::Style::Default);

void spawnEnemy();
void shoot();
bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2);
void reset();

sf::Texture skyTexture;
sf::Sprite skySprite;


sf::Texture bgTexture;
sf::Sprite bgSprite;

Hero hero;

std::vector<Enemy*> enemies;
std::vector<Rocket*> rockets;

float currentTime;
float prevTime = 0.0f;

int score;
bool gameover = true;

//Text
sf::Font headingFont;
sf::Text headingText;

// Score
sf::Font scoreFont;
sf::Text scoreText;

// Tutorial Text
sf::Text tutorialText;



// Audio
sf::Music bgMusic;

sf::SoundBuffer fireBuffer;
sf::SoundBuffer hitBuffer;

sf::Sound fireSound(fireBuffer);
sf::Sound hitSound(hitBuffer);

void init()
{
    skyTexture.loadFromFile("Assets/graphics/sky.png");
    skySprite.setTexture(skyTexture);

    bgTexture.loadFromFile("Assets/graphics/bg.png");
    bgSprite.setTexture(bgTexture);

    // Audio
    bgMusic.openFromFile("Assets/audio/bgMusic.ogg");
    bgMusic.play();

    fireBuffer.loadFromFile("Assets/audio/fire.ogg");
    hitBuffer.loadFromFile("Assets/audio/hit.ogg");

    // Load Font
    headingFont.loadFromFile("Assets/fonts/SnackerComic.ttf");

    // Text
    headingText.setFont(headingFont);
    headingText.setString("Tiny Bazooka");
    headingText.setCharacterSize(84);
    headingText.setFillColor(sf::Color::Red);
    sf::FloatRect headingBounds = headingText.getLocalBounds();
    headingText.setOrigin(headingBounds.width / 2, headingBounds.height / 2);
    headingText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

    scoreFont.loadFromFile("Assets/fonts/arial.ttf");
    scoreText.setFont(scoreFont);
    scoreText.setString("Score:0");
    scoreText.setCharacterSize(45);
    scoreText.setFillColor(sf::Color::Red);
    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
    scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));
    
    tutorialText.setFont(scoreFont);
    tutorialText.setString("Press Down Arrow to Fire and Start Game, Up Arrow to Jump");
    tutorialText.setCharacterSize(35);
    tutorialText.setFillColor(sf::Color::Red);

    sf::FloatRect tutorialBounds = tutorialText.getLocalBounds();
    tutorialText.setOrigin(tutorialBounds.width / 2, tutorialBounds.height / 2);
    tutorialText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.20f));

    hero.init("Assets/graphics/heroAnim.png",4, 1.0f, sf::Vector2f(viewSize.x * 0.25f, viewSize.y * 0.5f), 200);

    srand((int)time(0));
}

void draw()
{
    window.draw(skySprite);
    window.draw(bgSprite);
    if (gameover)
    {
        window.draw(headingText);
        window.draw(tutorialText);
    }
    else
    {
        window.draw(scoreText);
    }
    
    window.draw(hero.getSprite());
    for (Enemy* enemy : enemies) window.draw(enemy->getSprite());
    for (Rocket* rocket : rockets) window.draw(rocket->getSprite());
}

void updateInput()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Up) hero.jump(750.0f);
            if (event.key.code == sf::Keyboard::Down)
            {
                if (gameover)
                {
                    gameover = false;
                    reset();
                }
                else
                {
                    shoot();
                }
            }

        }
        if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed) window.close();
    }
}

void update(float dt)
{

    hero.update(dt);
    currentTime += dt;
    //Spawn Enemies
    if (currentTime >= (prevTime + 1.125f)) 
    {
        spawnEnemy();
        prevTime = currentTime;
    }

    // Update Enemies 
    for (int i = 0; i < enemies.size(); i++) 
    {
        Enemy* enemy = enemies[i];
        enemy->update(dt);
        if (enemy->getSprite().getPosition().x < 0)
        {
            enemies.erase(enemies.begin() + i);
            delete(enemy);
            gameover = true;
        }
    }

    // Update Enemies
    for (int i = 0; i < rockets.size(); i++)
    {
        Rocket* rocket = rockets[i];
        rocket->update(dt);
        if (rocket->getSprite().getPosition().x > viewSize.x)
        {
            rockets.erase(rockets.begin() + i);
            delete(rocket);
        }
    }

    // Check collisions between rocket and enemy
    for (int i = 0; i < rockets.size(); i++) {
        for (int j = 0; j < enemies.size(); j++) {
            Rocket* rocket = rockets[i];
            Enemy* enemy = enemies[j];
               
            if (checkCollision(rocket->getSprite(), enemy->getSprite()))
            {
                hitSound.play();
                score++;
                std::string finalScore = "Score:" + std::to_string(score);
                scoreText.setString(finalScore);

                sf::FloatRect scoreBounds = scoreText.getLocalBounds();
                scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
                scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

                rockets.erase(rockets.begin() + i);
                enemies.erase(enemies.begin() + j);
                delete(rocket);
                delete(enemy);
            }
        }
    }
}

int main()
{
    init();
    sf::Clock clock;
    while (window.isOpen()) {
        updateInput();
        sf::Time dt = clock.restart();
        if(!gameover) update(dt.asSeconds());
        window.clear(sf::Color::Red);
        draw();
        window.display();
    }
    return 0;
}

void spawnEnemy()
{
    int randLoc = rand() % 3;
    sf::Vector2f enemyPos;
    float speed;
    switch (randLoc)
    {
    case 0:
        enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.75f);
        speed = -400;
        break;

    case 1:
        enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.60f);
        speed = -550;
        break;

    case 2:
        enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.40f);
        speed = -650;
        break;
    default:
        std::cout << "Incorrect y value " << std::endl;
        return;
    }

    Enemy* enemy = new Enemy();
    enemy->init("Assets/graphics/enemy.png", enemyPos, speed);
    enemies.push_back(enemy);
}

void shoot()
{
    Rocket* rocket = new Rocket();
    rocket->init("Assets/graphics/rocket.png", hero.getSprite().getPosition(), 400.0f);
    rockets.push_back(rocket);
    fireSound.play();
}

bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2)
{
    sf::FloatRect shape1 = sprite1.getGlobalBounds();
    sf::FloatRect shape2 = sprite2.getGlobalBounds();
    if (shape1.intersects(shape2))  return true;
    else return false;
}

void reset()
{
    score = 0;
    currentTime = 0.0f;
    prevTime = 0.0;
    scoreText.setString("Score:0");
    for (Enemy* enemy : enemies) delete(enemy);
    for (Rocket* rocket : rockets) delete(rocket);
    enemies.clear();
    rockets.clear();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
