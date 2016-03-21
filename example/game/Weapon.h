#include <stent.h>

REF(Player);
DECLARE(Weapon);

REF(Weapon) WeaponCreate(REF(Player) owner);
void WeaponDestroy(REF(Weapon) ctx);
