#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

const unsigned int W1_WIDTH = 1280;
const unsigned int W1_HEIGHT = 720;
const int NUM_POINTS = 1280;
const double C = 1.0;
const double DX = 0.5;
const double DT = 0.1;
const double DAMPING = 0.9999;
const float THICKNESS = 3.0f;
const double MOUSE_VALUE = 720.0;

const unsigned int WIN_W = 1280;
const unsigned int WIN_H = 720;

const int GRID_W = 640;
const int GRID_H = 360;
const double DAMPING2 = 0.995;

sf::Vector2i getGridPos(sf::RenderWindow &window) {
  sf::Vector2i mousePos = sf::Mouse::getPosition(window);
  sf::Vector2u windowSize = window.getSize();

  int x = (int)((mousePos.x / (double)windowSize.x) * GRID_W);
  int y = (int)((mousePos.y / (double)windowSize.y) * GRID_H);

  if (x < 0) x = 0;
  if (x >= GRID_W) x = GRID_W - 1;
  if (y < 0) y = 0;
  if (y >= GRID_H) y = GRID_H - 1;

  return {x, y};
}

int main() {
  #ifdef _WIN32
  SetConsoleOutputCP(65001);
  #endif

  bool showError = false;

  while (true) {
    int decision = 0;

    while (true) {
      std::cout << "\033[2J\033[1;1H";

      std::cout << "\n";
      std::cout << "  "
                   "\u2554\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2557\n";
      std::cout << "  \u2551            WAVE SIMULATION            \u2551\n";
      std::cout << "  "
                   "\u2560\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2563\n";
      std::cout << "  \u2551                                       \u2551\n";
      std::cout << "  \u2551   [1]  1D Wave Simulation             \u2551\n";
      std::cout << "  \u2551   [2]  2D Wave Simulation             \u2551\n";
      std::cout << "  \u2551                                       \u2551\n";
      std::cout << "  \u2551   [0]  Quit                           \u2551\n";
      std::cout << "  \u2551                                       \u2551\n";
      std::cout << "  "
                   "\u255A\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550\u2550"
                   "\u2550\u2550\u2550\u2550\u255D\n";
      std::cout << "\n";

      if (showError) {
        std::cout << "  >> Error: Invalid Input! Please type 1, 2, or 0.\n";
        showError = false;
      }

      std::cout << "  Select Option > ";

      if (!(std::cin >> decision)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        showError = true;
        continue;
      }

      if (decision == 0)
        return 0;
      if (decision == 1 || decision == 2)
        break;

      showError = true;
    }

    sf::RenderWindow window;
    if (decision == 1)
      window.create(sf::VideoMode({W1_WIDTH, W1_HEIGHT}),
                    "1D Wave (Press Q to Menu, R to Reset)");
    else
      window.create(sf::VideoMode({WIN_W, WIN_H}),
                    "2D Wave (Press Q to Menu, C to Clear, R to Reset)");

    window.setFramerateLimit(60);

    if (decision == 1) {
      std::vector<double> u(NUM_POINTS, 0.0);
      std::vector<double> u_prev(NUM_POINTS, 0.0);
      std::vector<double> u_next(NUM_POINTS, 0.0);

      double courant_sq = (C * DT / DX) * (C * DT / DX);

      sf::RectangleShape lineSegment;
      float radius = THICKNESS / 2.0f;
      sf::CircleShape joint(radius);
      joint.setOrigin({radius, radius});

      while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
          if (event->is<sf::Event::Closed>()) {
            window.close();
            return 0;
          }

          if (const auto *keyPress = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPress->code == sf::Keyboard::Key::Q) {
              window.close();
            }
            if (keyPress->code == sf::Keyboard::Key::R) {
              std::fill(u.begin(), u.end(), 0.0);
              std::fill(u_prev.begin(), u_prev.end(), 0.0);
              std::fill(u_next.begin(), u_next.end(), 0.0);
            }
          }

          if (const auto *mousePress =
                  event->getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2u wSize = window.getSize();
            int center = (int)((mousePress->position.x / (double)wSize.x) * NUM_POINTS);
            
            int inputRadius = 15;
            for (int i = -inputRadius; i <= inputRadius; ++i) {
              int target = center + i;
              if (target > 1 && target < NUM_POINTS - 1) {
                double dist = i / (double)inputRadius;
                double shape = std::exp(-3.0 * dist * dist);
                double val = MOUSE_VALUE * shape;
                u[target] += val;
                u_prev[target] += val;
              }
            }
          }
        }

        int subSteps = 8;
        while (subSteps--) {
          for (int i = 1; i < NUM_POINTS - 1; ++i) {
            u_next[i] = 2 * u[i] - u_prev[i] +
                        courant_sq * (u[i + 1] - 2 * u[i] + u[i - 1]);
            u_next[i] *= DAMPING;
          }
          u_next[0] = 0;
          u_next[NUM_POINTS - 1] = 0;
          u_prev = u;
          u = u_next;
        }

        window.clear(sf::Color::Black);

        for (int i = 0; i < NUM_POINTS - 1; ++i) {
          float x1 = (float)i / NUM_POINTS * W1_WIDTH;
          float y1 = (W1_HEIGHT / 2.0f) - (float)u[i];
          float x2 = (float)(i + 1) / NUM_POINTS * W1_WIDTH;
          float y2 = (W1_HEIGHT / 2.0f) - (float)u[i + 1];

          sf::Vector2f p1(x1, y1);
          sf::Vector2f p2(x2, y2);
          sf::Vector2f diff = p2 - p1;

          float length = std::sqrt(diff.x * diff.x + diff.y * diff.y);
          sf::Angle angle = sf::radians(std::atan2(diff.y, diff.x));

          double val = std::abs(u[i]);
          double limit = 350.0;
          double ratio = std::min(val / limit, 1.0);

          sf::Color segmentColor;
          segmentColor.a = 255;

          if (ratio < 0.3) {
            double t = ratio / 0.3;
            segmentColor.r = static_cast<std::uint8_t>(255 * (1.0 - t));
            segmentColor.g = static_cast<std::uint8_t>(255 * (1.0 - t));
            segmentColor.b = 255;
          } else {
            double t = (ratio - 0.3) / 0.7;
            segmentColor.r = static_cast<std::uint8_t>(255 * t);
            segmentColor.g = 0;
            segmentColor.b = static_cast<std::uint8_t>(255 * (1.0 - t));
          }

          lineSegment.setSize({length, THICKNESS});
          lineSegment.setPosition(p1);
          lineSegment.setRotation(angle);
          lineSegment.setFillColor(segmentColor);
          window.draw(lineSegment);
          joint.setPosition(p1);
          joint.setFillColor(segmentColor);
          window.draw(joint);
        }

        float lastX = (float)(NUM_POINTS - 1) / NUM_POINTS * W1_WIDTH;
        float lastY = (W1_HEIGHT / 2.0f) - (float)u[NUM_POINTS - 1];
        joint.setPosition({lastX, lastY});
        window.draw(joint);

        window.display();
      }
    } 
    else {
      std::vector<double> u(GRID_W * GRID_H, 0.0);
      std::vector<double> u_prev(GRID_W * GRID_H, 0.0);
      std::vector<double> u_next(GRID_W * GRID_H, 0.0);
      std::vector<bool> walls(GRID_W * GRID_H, false);
      std::vector<std::uint8_t> pixels(GRID_W * GRID_H * 4);

      sf::Texture texture;
      if (!texture.resize({(unsigned int)GRID_W, (unsigned int)GRID_H}))
        return -1;
      
      texture.setSmooth(true);

      sf::Sprite sprite(texture);
      
      sf::Vector2u wSize = window.getSize();
      sprite.setScale({(float)wSize.x / GRID_W, (float)wSize.y / GRID_H});

      while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
          if (event->is<sf::Event::Closed>()) {
            window.close();
            return 0;
          }
          
          if (event->is<sf::Event::Resized>()) {
              sf::Vector2u newSize = window.getSize();
              sprite.setScale({(float)newSize.x / GRID_W, (float)newSize.y / GRID_H});
              window.setView(sf::View(sf::FloatRect({0, 0}, {(float)newSize.x, (float)newSize.y})));
          }

          if (const auto *keyPress = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPress->code == sf::Keyboard::Key::Q) {
              window.close();
            }
            if (keyPress->code == sf::Keyboard::Key::C) {
              std::fill(u.begin(), u.end(), 0.0);
              std::fill(u_prev.begin(), u_prev.end(), 0.0);
              std::fill(u_next.begin(), u_next.end(), 0.0);
            }
            if (keyPress->code == sf::Keyboard::Key::R) {
              std::fill(u.begin(), u.end(), 0.0);
              std::fill(u_prev.begin(), u_prev.end(), 0.0);
              std::fill(u_next.begin(), u_next.end(), 0.0);
              std::fill(walls.begin(), walls.end(), false);
            }
          }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
          sf::Vector2i p = getGridPos(window);
          int radius = 2;
          for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
              int tx = p.x + x;
              int ty = p.y + y;
              if (tx >= 0 && tx < GRID_W && ty >= 0 && ty < GRID_H) {
                walls[ty * GRID_W + tx] = true;
                u[ty * GRID_W + tx] = 0;
                u_prev[ty * GRID_W + tx] = 0;
              }
            }
          }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
          sf::Vector2i p = getGridPos(window);
          int radius = 3;
          for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
              int tx = p.x + x;
              int ty = p.y + y;
              if (tx > 0 && tx < GRID_W - 1 && ty > 0 && ty < GRID_H - 1) {
                if (!walls[ty * GRID_W + tx]) {
                  u[ty * GRID_W + tx] += 500.0;
                  u_prev[ty * GRID_W + tx] += 500.0;
                }
              }
            }
          }
        }

        for (int y = 1; y < GRID_H - 1; ++y) {
          for (int x = 1; x < GRID_W - 1; ++x) {
            int i = y * GRID_W + x;
            if (walls[i]) {
              u_next[i] = 0.0;
              continue;
            }
            double val = (u[i - 1] + u[i + 1] + u[i - GRID_W] + u[i + GRID_W] -
                          4 * u[i]);
            u_next[i] = 2 * u[i] - u_prev[i] + val * 0.5;
            u_next[i] *= DAMPING2;
          }
        }
        u_prev = u;
        u = u_next;

        for (int i = 0; i < GRID_W * GRID_H; ++i) {
          int pixelIdx = i * 4;
          if (walls[i]) {
            pixels[pixelIdx + 0] = 50;
            pixels[pixelIdx + 1] = 55;
            pixels[pixelIdx + 2] = 60;
            pixels[pixelIdx + 3] = 255;
            continue;
          }

          double val = u[i];
          double brightness = std::clamp(std::abs(val) * 3.0, 0.0, 255.0);
          std::uint8_t iBright = static_cast<std::uint8_t>(brightness);
          std::uint8_t r = 0, g = 0, b = 0;

          if (val > 0) {
            r = static_cast<std::uint8_t>(brightness * 0.1);
            g = static_cast<std::uint8_t>(brightness * 0.8);
            b = iBright;
          } else {
            r = iBright;
            g = static_cast<std::uint8_t>(brightness * 0.5);
            b = static_cast<std::uint8_t>(brightness * 0.1);
          }

          pixels[pixelIdx + 0] = r;
          pixels[pixelIdx + 1] = g;
          pixels[pixelIdx + 2] = b;
          pixels[pixelIdx + 3] = 255;
        }

        texture.update(pixels.data());
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
      }
    }
  }
  return 0;
}