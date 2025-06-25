#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>

const int WIDTH = 800, HEIGHT = 600;
const int SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

class SnakeGame {
    sf::RenderWindow window;
    std::vector<SnakeSegment> snake;
    Direction dir;
    sf::RectangleShape segmentShape;
    sf::Vector2i apple;
    sf::RectangleShape appleShape;
    sf::SoundBuffer eatBuffer;
    sf::Sound eatSound;
    sf::Music bgMusic;
    int score = 0;
    float speed = 0.2f;
    sf::Font font;
    sf::Text scoreText;

public:
    SnakeGame() : window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game") {
        window.setFramerateLimit(60);
        dir = RIGHT;
        snake.push_back(SnakeSegment(10, 10));

        segmentShape.setSize(sf::Vector2f(SIZE - 2, SIZE - 2));
        segmentShape.setFillColor(sf::Color::Green);

        appleShape.setSize(sf::Vector2f(SIZE - 2, SIZE - 2));
        appleShape.setFillColor(sf::Color::Red);
        placeApple();

        eatBuffer.loadFromFile("eat.wav");
        eatSound.setBuffer(eatBuffer);

        bgMusic.openFromFile("bg.wav");
        bgMusic.setLoop(true);
        bgMusic.play();

        font.loadFromFile("arial.ttf");
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        updateScore();
    }

    void placeApple() {
        apple.x = rand() % (WIDTH / SIZE);
        apple.y = rand() % (HEIGHT / SIZE);
    }

    void updateScore() {
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setPosition(10, 10);
    }

    void run() {
        sf::Clock clock;
        float timer = 0;
        while (window.isOpen()) {
            float time = clock.restart().asSeconds();
            timer += time;

            sf::Event e;
            while (window.pollEvent(e)) {
                if (e.type == sf::Event::Closed)
                    window.close();
                if (e.type == sf::Event::KeyPressed) {
                    if (e.key.code == sf::Keyboard::Up && dir != DOWN) dir = UP;
                    else if (e.key.code == sf::Keyboard::Down && dir != UP) dir = DOWN;
                    else if (e.key.code == sf::Keyboard::Left && dir != RIGHT) dir = LEFT;
                    else if (e.key.code == sf::Keyboard::Right && dir != LEFT) dir = RIGHT;
                }
            }

            if (timer >= speed) {
                timer = 0;
                moveSnake();
                checkCollision();
            }

            draw();
        }
    }

    void moveSnake() {
        for (int i = snake.size() - 1; i > 0; --i)
            snake[i] = snake[i - 1];

        switch (dir) {
            case UP: snake[0].y--; break;
            case DOWN: snake[0].y++; break;
            case LEFT: snake[0].x--; break;
            case RIGHT: snake[0].x++; break;
        }

        if (snake[0].x == apple.x && snake[0].y == apple.y) {
            snake.push_back(snake.back());
            score += 10;
            updateScore();
            eatSound.play();
            placeApple();
            if (speed > 0.05f) speed -= 0.01f; // Increase difficulty
        }
    }

    void checkCollision() {
        // Wall
        if (snake[0].x < 0 || snake[0].y < 0 || snake[0].x >= WIDTH / SIZE || snake[0].y >= HEIGHT / SIZE)
            gameOver();
        // Self
        for (size_t i = 1; i < snake.size(); ++i)
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
                gameOver();
    }

    void gameOver() {
        bgMusic.stop();
        sf::Text overText("Game Over! Score: " + std::to_string(score), font, 30);
        overText.setFillColor(sf::Color::Red);
        overText.setPosition(WIDTH / 4, HEIGHT / 2);
        window.clear();
        window.draw(overText);
        window.display();
        sf::sleep(sf::seconds(3));
        window.close();
    }

    void draw() {
        window.clear();

        // Draw apple
        appleShape.setPosition(apple.x * SIZE, apple.y * SIZE);
        window.draw(appleShape);

        // Draw snake
        for (auto& s : snake) {
            segmentShape.setPosition(s.x * SIZE, s.y * SIZE);
            window.draw(segmentShape);
        }

        window.draw(scoreText);
        window.display();
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    SnakeGame game;
    game.run();
    return 0;
}
