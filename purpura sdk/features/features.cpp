#include "visuals/visuals.h"
#include "misc/misc.h"
#include "engine_prediction/engine_prediction.h"
#include "legit/legit_lag_comp.h"
#include "legit/legit_aimbot.h"
#include "legit/legit_antiaim.h"
#include "legit/triggerbot.h"
#include "skins/skinchanger.h"
#include "rage/ragebot.h"

namespace features {
	visuals* vis = new visuals;
	miscellaneous* misc = new miscellaneous;
	engine_prediction* engine_pred = new engine_prediction;
	legit_lag_comp* lag_comp = new legit_lag_comp;
	legit_aimbot* legit = new legit_aimbot;
	legit_antiaim* legit_aa = new legit_antiaim;
	legit_triggerbot* triggerbot = new legit_triggerbot;
	skinchanger* skins = new skinchanger;
	ragebot* rage = new ragebot;
}