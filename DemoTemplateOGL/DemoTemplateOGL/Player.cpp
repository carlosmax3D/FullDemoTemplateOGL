#include "Player.h"
#include <algorithm> // Para std::min y std::max

// 🧱 Constructor privado
Player::Player(float maxHealth) {
    this->maxHealth = maxHealth;
    this->health = maxHealth;
}

// 🧠 Método estático — devuelve siempre la misma instancia
Player& Player::getInstance() {
    static Player instance; // Crea una sola vez
    return instance;
}

// ==== Getters ====
float Player::getHealth() const {
    return health;
}

float Player::getMaxHealth() const {
    return maxHealth;
}

// ==== Lógica de vida ====
void Player::takeDamage(float amount) {
    health = std::max(0.0f, health - amount);
}

void Player::heal(float amount) {
    health = std::min(maxHealth, health + amount);
}

bool Player::isAlive() const {
    return health > 0.0f;
}
