#include "HUD.h"
#include "../entities/Player.h"
#include "../entities/weapons/Weapon.h"

HUD::HUD()
{
}

bool HUD::load()
{
    if (!font.openFromFile("assets/fonts/default.ttf"))
        return false;

    pauseHint.emplace(font, "Press ESC to pause", 18);
    pauseHint->setPosition({ 10.f, 10.f });
    pauseHint->setFillColor(sf::Color::White);

    moneyText.emplace(font, "", 18);
    moneyText->setPosition({ 10.f, 60.f });
    moneyText->setFillColor(sf::Color::White);

    scoreText.emplace(font, "", 18);
    scoreText->setPosition({ 10.f, 85.f });
    scoreText->setFillColor(sf::Color::White);
    weaponText.emplace(font, "Weapon: -", 18);
    weaponText->setPosition({ 10.f, 110.f });
    weaponText->setFillColor(sf::Color::White);

    ammoText.emplace(font, "Ammo: - / -", 18);
    ammoText->setPosition({ 10.f, 135.f });
    ammoText->setFillColor(sf::Color::White);

    hpBack.setSize({ 200.f, 16.f });
    hpBack.setPosition({ 10.f, 34.f });
    hpBack.setFillColor(sf::Color(60, 60, 60));

    hpFront.setSize({ 200.f, 16.f });
    hpFront.setPosition({ 10.f, 34.f });
    hpFront.setFillColor(sf::Color::Red);
    hpText.emplace(font, "HP: 100 / 100", 18);
    hpText->setPosition({ 220.f, 31.f });
    hpText->setFillColor(sf::Color::White);
    return true;
}
void HUD::update(const Player& player)
{
    moneyText->setString(
        "Money: " + std::to_string(player.getMoney()));

    scoreText->setString(
        "Score: " + std::to_string(player.getScore()));
    const Weapon* weapon = player.getCurrentWeapon();

    if (weapon)
    {
        weaponText->setString(
            "Weapon: " + weapon->getName());

        ammoText->setString(
            "Ammo: " +
            std::to_string(weapon->getCurrentAmmo()) +
            " / " +
            std::to_string(weapon->getReserveAmmo()));
    }
    else
    {
        weaponText->setString("Weapon: -");
        ammoText->setString("Ammo: - / -");
    }

    hpText->setString(
        "HP: " +
        std::to_string(static_cast<int>(player.getHealth())) +
        " / " +
        std::to_string(static_cast<int>(player.getMaxHealth()))
    );
    float pct = player.getHealth() / player.getMaxHealth();

    hpFront.setSize({ 200.f * pct, 16.f });
}

void HUD::render(sf::RenderTarget& target)
{
    target.setView(target.getDefaultView());

    if (pauseHint) target.draw(*pauseHint);
    if (moneyText) target.draw(*moneyText);
    if (scoreText) target.draw(*scoreText);
    if (weaponText) target.draw(*weaponText);
    if (ammoText) target.draw(*ammoText);
    target.draw(hpBack);
    target.draw(hpFront);
    if (hpText) target.draw(*hpText);

}