#ifndef HIBIKI_EHBKPLAYERAPPENDABILITYTYPE_H
#define HIBIKI_EHBKPLAYERAPPENDABILITYTYPE_H

enum EHbkPlayerAppendAbilityType : char {
	Action_None,
	Action_BeatHit,
	Action_SPAttack,
	Action_Dodge,
	Action_AirDodge,
	Action_RhythmDodge,
	Action_Magnet,
	Action_MagnetBackstab,
	Action_Parry,
	Action_AirParry,
	Action_DirectionalParry,
	Action_PartnerCall,
};

inline const char* AbilityToString(EHbkPlayerAppendAbilityType t) {
	switch(t) {
		case Action_None: return "None";
		case Action_BeatHit: return "Action_BeatHit";
		case Action_SPAttack: return "Action_SPAttack";
		case Action_Dodge: return "Action_Dodge";
		case Action_AirDodge: return "Action_AirDodge";
		case Action_RhythmDodge: return "Action_RhythmDodge";
		case Action_Magnet: return "Action_Magnet";
		case Action_MagnetBackstab: return "Action_MagnetBackstab";
		case Action_Parry: return "Action_Parry";
		case Action_AirParry: return "Action_AirParry";
		case Action_DirectionalParry: return "Action_DirectionalParry";
		case Action_PartnerCall: return "Action_PartnerCall";
		default: return "Invalid";
	}
}

#endif
