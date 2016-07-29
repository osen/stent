#include <stent.h>

REF(Player);
REFDEF(Weapon);

REF(Weapon) WeaponCreate(REF(Player) owner);
void WeaponDestroy(REF(Weapon) ctx);
