#include "Player.h"
#include "Weapon.h"

#include <stent.h>

#include <stdio.h>

int main(int argc, char* argv[])
{
  REF(Player) player = {};
  REF(Weapon) weapon = {};
  REF(Player) enemy = {};

  player = PlayerCreate();
  weapon = PlayerWeapon(player);
  enemy = PlayerCreate();
  PlayerSetTarget(player, enemy);

  printf("Player: %p\n", GET(player));
  printf("Weapon: %p\n", GET(weapon));

  FREE(enemy);
  printf("Player Target: %p\n", TRYGET(PlayerTarget(player)));
  FREE(player);

  printf("Player: %p\n", TRYGET(player));
  printf("Weapon: %p\n", TRYGET(weapon));

  StentStats();
  StentCleanup();

  return 0;
}
