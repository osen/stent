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

  PlayerDestroy(enemy);
  printf("Player Target: %p\n", GET(PlayerTarget(player)));
  PlayerDestroy(player);

  printf("Player: %p\n", GET(player));
  printf("Weapon: %p\n", GET(weapon));

  RefStats();
  RefCleanup();

  return 0;
}
