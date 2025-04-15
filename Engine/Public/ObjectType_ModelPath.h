#pragma once
#include "Engine_Defines.h"

enum AllObjectType {
	TYPE_TILE,
	TYPE_UNIT,
	TYPE_DECO,
	TYPE_ITEM,
	TYPE_Interactive,
	TYPE_Light,

	TYPE_END
};

enum UnitType {
	UNIT_DefaultMutant,
	UNIT_Assassin,
	UNIT_Enforcer,
	UNIT_CortexReaver,
	UNIT_Elite,
	UNIT_END
};

enum InteractiveType {
	INTER_AlarmLight,
	INTER_Door,
	INTER_Elevator,
	INTER_LaserButton,
	INTER_RespawnChamber,
	INTER_END
};

static _bool IsSame(const _wstring& szName, const _tchar* szName2) {
	if (lstrcmp(szName.c_str(), szName2) == 0) {
		return true;
	}
	else {
		return false;
	}
}

static UnitType GetType_Name(const _wstring& szModelName) {
	if (lstrcmp(szModelName.c_str(), TEXT("Prototype_Component_Model_Assassin")) == 0) {
		return UNIT_Assassin;
	}
	else if (lstrcmp(szModelName.c_str(), TEXT("Prototype_Component_Model_CortexReaver")) == 0) {
		return UNIT_CortexReaver;
	}
	else if (lstrcmp(szModelName.c_str(), TEXT("Prototype_Component_Model_DefaultMutant")) == 0) {
		return UNIT_DefaultMutant;
	}
	else if (lstrcmp(szModelName.c_str(), TEXT("Prototype_Component_Model_Enforcer")) == 0) {
		return UNIT_Enforcer;
	}
	else if (lstrcmp(szModelName.c_str(), TEXT("Prototype_Component_Model_CyborgElite")) == 0) {
		return UNIT_Elite;
	}
	else {
		return UNIT_END;
	}
}

static InteractiveType GetTypeInterative_Name(const _wstring& szModelName) {
	if (IsSame(szModelName, TEXT("Prototype_Component_Model_AlarmLight"))) {
		return INTER_AlarmLight;
	}
	else if (IsSame(szModelName, TEXT("Prototype_Component_Model_BridgeDoor"))) {
		return INTER_Door;
	}
	else if (IsSame(szModelName, TEXT("Prototype_Component_Model_Elevator"))) {
		return INTER_Elevator;
	}
	else if (IsSame(szModelName, TEXT("Prototype_Component_Model_LaserButton"))) {
		return INTER_LaserButton;
	}
	else if (IsSame(szModelName, TEXT("Prototype_Component_Model_RespawnChamber"))) {
		return INTER_RespawnChamber;
	}
}