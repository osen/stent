#include "Weapon.h"
#include "Player.h"

#include <stdlib.h>

struct Weapon
{
  REF(Player) owner;
  int ammo;
};

REF(Weapon) WeaponCreate(REF(Player) owner)
{
  REF(Weapon) rtn = {};

  rtn = CALLOC(Weapon);
  GET(rtn)->owner = owner;
  GET(rtn)->ammo = 50;

  return rtn;
}

void WeaponDestroy(REF(Weapon) ctx)
{
  FREE(ctx);
}
