#include <iostream>
#include <SFML/Graphics.hpp>
#include "solver.hpp"
#include "renderer.hpp"
#include "utils/number_generator.hpp"
#include "utils/math.hpp"


static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * Math::PI);
    const float b = sin(t + 0.66f * 2.0f * Math::PI);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}


int32_t main(int32_t, char*[])
{
    // Create window
    constexpr int32_t window_width  = 1000;
    constexpr int32_t window_height = 1000;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Verlet", sf::Style::Default, settings);
    const uint32_t frame_rate = 60;
    window.setFramerateLimit(frame_rate);

    Solver   solver;
    Renderer renderer{window};

    // Solver configuration
    solver.setConstraint({static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f}, 450.0f);
    solver.setSubStepsCount(8);
    solver.setSimulationUpdateRate(frame_rate);

    // Set simulation attributes
    const float        object_spawn_delay    = 0.001f;
    const uint32_t     max_objects_count     = 3;
    float              object_size           = 15.0f;
    float              dtheta                = 2*Math::PI / max_objects_count;
    float              theta                 = 0;
    sf::Vector2f       offset                = {500.0f, 500.0f};
    float              spawn_point_radius    = 120.0f;
    float              spawn_speed           = 100.1f;

    sf::Clock clock;
    // Main loop
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
        }

        if (solver.getObjectsCount() < max_objects_count && clock.getElapsedTime().asSeconds() >= object_spawn_delay) {
            clock.restart();
            sf::Vector2f spawn_point = spawn_point_radius * sf::Vector2f{cos(theta), sin(theta)};
            sf::Vector2f spawn_point_speed = spawn_speed * sf::Vector2f{sin(theta), -cos(theta)};
            auto& object = solver.addObject(spawn_point + offset, object_size);
            solver.setObjectVelocity(object, spawn_point_speed);
            const float t      = solver.getTime();
            object.color = getRainbow(t*30);
            theta += dtheta;
        }

        solver.update();
        window.clear(sf::Color::White);
        renderer.render(solver);
		window.display();
    }

    return 0;
}
