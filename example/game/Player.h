#include <stent.h>

REF(Weapon);
DECLARE(Player);

REF(Player) PlayerCreate();
void PlayerDestroy(REF(Player) ctx);
void PlayerSetTarget(REF(Player) ctx, REF(Player) target);
REF(Weapon) PlayerWeapon(REF(Player) ctx);
REF(Player) PlayerTarget(REF(Player) ctx);

