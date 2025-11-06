#pragma once
#ifndef PLAYER_H
#define PLAYER_H

class Player {
private:
    float health;      // Vida actual del jugador
    float maxHealth;   // Vida máxima

    // 🔒 Constructor privado (solo accesible por getInstance)
    Player(float maxHealth = 100.0f);

    // 🔒 Copia y asignación deshabilitadas
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

public:
    // 🧠 Método estático para obtener la instancia única
    static Player& getInstance();

    // ==== Getters ====
    float getHealth() const;
    float getMaxHealth() const;

    // ==== Funciones de vida ====
    void takeDamage(float amount);  // Recibe daño
    void heal(float amount);        // Recupera vida
    bool isAlive() const;           // Verifica si sigue vivo
};

#endif
