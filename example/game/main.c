#include "Player.h"
#include "Weapon.h"

#include <stent.h>

#include <stdio.h>

int main(int argc, char* argv[])
{
  REF(Player) player = {};
  REF(Weapon) weapon = {};
  REF(Player) enemy = {};
  REF(Player) attachTest = {};

  player = PlayerCreate();
  weapon = PlayerWeapon(player);
  enemy = PlayerCreate();
  PlayerSetTarget(player, enemy);

  printf("Player: %p\n", GET(player));
  printf("Weapon: %p\n", GET(weapon));

  ATTACH(attachTest, NULL);
  //ATTACH(attachTest, GET(weapon));
  printf("Attach Test 1: %p\n", GET(attachTest));

  ATTACH(attachTest, GET(player));
  printf("Attach Test 2: %p\n", GET(attachTest));

  PlayerDestroy(enemy);
  printf("Player Target: %p\n", GET(PlayerTarget(player)));
  PlayerDestroy(player);

  printf("Player: %p\n", GET(player));
  printf("Weapon: %p\n", GET(weapon));
  printf("Attach Test: %p\n", GET(attachTest));

  RefStats();
  RefCleanup();

  return 0;
}
