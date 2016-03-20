#include "Player.h"
#include "Weapon.h"

#include <stdio.h>
#include <stdlib.h>

struct Player
{
  int health;
  REF(Player) target;
  REF(Weapon) weapon;
};

REF(Player) PlayerCreate()
{
  REF(Player) rtn = {};

  rtn = CALLOC(Player);
  GET(rtn)->health = 100;
  GET(rtn)->weapon = WeaponCreate(rtn);

  return rtn;
}

void PlayerDestroy(REF(Player) ctx)
{
  WeaponDestroy(GET(ctx)->weapon);
  FREE(ctx);
}

void PlayerSetTarget(REF(Player) ctx, REF(Player) target)
{
  GET(ctx)->target = target;
}

REF(Weapon) PlayerWeapon(REF(Player) ctx)
{
  return GET(ctx)->weapon;
}

REF(Player) PlayerTarget(REF(Player) ctx)
{
  return GET(ctx)->target;
}
