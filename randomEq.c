/****************************************
 ****************************************
 *****    Act of War - Chapter 5    *****
 ****************************************
 ****************************************/

///// Random EQ Generation for Act of War 5

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "signal.h"
#include "magic.h"
#include "recycle.h"
#include "randomEq.h"

#define PREFIXES 12
#define WEAPONS 24
#define ARMORS 12
#define LEVELS 12
#define WEPSUFFIXES 10
#define EQSUFFIXES 16
#define GOLDSILVER 12
//#define WEARFLAGS 10


//////////////////////////////////////////////////////////////////////
//weapon type, damage type, short description, (name) keywords, long description, number of dice, sides on dice
////

//watch the names and prefixes don't start with a vowel.  Save any a/an confusion if no + mod.
const struct weapons weaponTable[WEAPONS] = {
	{WEAPON_SWORD,	 "stab",	"shortsword",			"short sword shortsword",	"A shortsword has been left here.",			4, 6},
	{WEAPON_SWORD,	 "slice",	"longsword",			"long sword longsword",		"A longsword has been left here.",			5, 7},
	{WEAPON_SWORD,	 "slash",	"greatsword",			"great sword greatsword",	"A greatsword has been left here.",			8, 8},
	{WEAPON_AXE,	 "chop", 	"handaxe",				"hand axe handaxe",			"A handaxe has been left here.",			5, 5},
	{WEAPON_AXE,	 "cleave",	"broadaxe",				"broad axe broadaxe",		"A broadaxe has been left here.",			6, 6},
	{WEAPON_AXE,	 "beating",	"double-bladed axe",	"double axe",				"A double-bladed axe has been left here.",	8, 8},
	{WEAPON_DAGGER,	 "scratch",	"dagger",				"dagger",					"A dagger has been left here.",				3, 4},
	{WEAPON_DAGGER,	 "stab", 	"blade",				"blade dagger",				"A blade has been left here.",				5, 7},
	{WEAPON_DAGGER,	 "slash",	"gauche",				"brass gauche dagger",		"A brass gauche dagger has been left here.",8, 8},
	{WEAPON_MACE,	 "crush",	"club",					"club mace",				"A club has been left here.",				11, 5},
	{WEAPON_MACE,	 "pound", 	"mace",					"mace",						"A mace has been left here.",				14, 4},
	{WEAPON_MACE,	 "smash",	"maul",					"maul mace",				"A maul has been left here.",				15, 4},
	{WEAPON_SPEAR,	 "stab", 	"spear",				"spear",					"A spear has been left here.",				1, 14},
	{WEAPON_SPEAR,	 "beating",	"pilum",				"pilum spear",				"A pilum has been left here.",				2, 13},
	{WEAPON_SPEAR,	 "thrust", 	"lance",				"lance spear",				"A lance has been left here.",				8, 8},
	{WEAPON_POLEARM, "stab",	"halberd",				"halberd polearm",			"A halberd has been left here.",			5, 12},
	{WEAPON_POLEARM, "slash",	"voulge",				"voulge polearm",			"A voulge has been left here.",				4, 16},
	{WEAPON_POLEARM, "chop",	"trident",				"trident polearm",			"A trident has been left here.",			8, 8},
	{WEAPON_FLAIL,	 "pound",	"thresher",				"thresher flail",			"A thresher has been left here.",			6, 3},
	{WEAPON_FLAIL,	 "beating",	"flail",				"flail",					"A flail has been left here.",				6, 5},
	{WEAPON_FLAIL,	 "smash",	"morningstar",			"morningstar flail",		"A morningstar has been left here.",		8, 8},
	{WEAPON_WHIP,	 "slap",	"whip",					"whip",						"A whip has been left here.",				5, 4},
	{WEAPON_WHIP,	 "beating",	"bullwhip",				"bull whip bullwhip",		"A bullwhip has been left here.",			5, 6},
	{WEAPON_WHIP,	 "crush",	"chainwhip",			"chain whip chainwhip",		"A chainwhip has been left here.",			8, 8}//,
//	{WEAPON_EPEE,    "scratch", "foil",					"foil",						"A sharpened foil has been left here.",		4, 6},
//	{WEAPON_EPEE,	 "stab",	"rapier",				"long rapier",				"A long and slender rapier is left here.",	5, 7},
//	{WEAPON_EPEE,	 "thrust",  "sabre",				"long sabre",				"A long sabre has been left here.",			8, 8},
//	{WEAPON_CLAW,    "scratch", "claw",                 "iron claw",                "A sharp iron claw lies here.",             5, 5},
//	{WEAPON_CLAW,    "pound",   "glove",				"heavy glove",              "A heavily-padded fighting glove is here.", 8, 7},
//	{WEAPON_CLAW,    "smash",   "knuckle",              "metal knuckle",            "A metallic knuckle lies here.",            8, 8},
//	{WEAPON_STAFF,	 "thrust",	"quarterstaff",			"quarterstaff",				"A quarterstaff has been left here.",		3, 9},
//	{WEAPON_STAFF,	 "smash",	"oak staff",			"oak staff",				"A beautifully crafted oak staff lies here.", 6, 6},
//	{WEAPON_STAFF,	 "beating", "long staff",			"long staff",				"A very long staff lies here.",				8, 8}
};

//////////////////////////////////////////////////////////////////////
//wear location, short description, (name) keywords, long description, piercing, slashing, bashing, exotic (magic)
////

const struct armors armorTable[ARMORS] = {
	{ITEM_WEAR_FINGER,	"bone ring",			"bone ring bonering",		"A ring has been left here.",						2, 2, 2, 1},
	{ITEM_WEAR_SHIELD,	"buckler",			"buckler",					"A buckler has been left here.",					8, 8, 8, 5},
	{ITEM_WEAR_FEET,	"platemail boots",	"platemail plated boots",	"Some plated boots have been left here.",			5, 5, 5, 3},
	{ITEM_WEAR_HANDS,	"chainmail gloves",	"chainmail gloves",			"Some chain gloves have been left here.",			5, 5, 5, 3},
	{ITEM_WEAR_HEAD,	"steel helmet",		"steel helmet heavy",		"A heavy helmet has been left here.",				6, 6, 6, 4}, //5
	{ITEM_WEAR_NECK,	"{ygolden{x necklace",	"golden necklace",			"A beautiful golden necklace has been left here.",	2, 1, 1, 3},
	{ITEM_WEAR_WRIST,	"hematite bracer",  "hematite bracer",			"A dark colored metal bracer lies here.",			1, 4, 4, 3},
	{ITEM_WEAR_FLOAT,	"{mpurple{x orb",	"purple orb",				"A mystical purple orb floats above the ground.",	1, 1, 1, 7},
//	{WEAR_BACK,			"{Dblack{x cape",	"black cape",				"A blackened leather cape lies here in a pile.",	2, 5, 5, 4},
	{ITEM_WEAR_HANDS,	"steel gauntlets",  "steel gauntlets",			"A pair of steel gauntlets lie here.",				5, 5, 1, 2}, //10
	{ITEM_WEAR_WAIST,	"leather belt",		"leather belt",				"A belt is coiled up and left here.",				4, 1, 5, 5},
	{ITEM_WEAR_ARMS,	"{ygolden{x shoulder plates", "golden shoulder plates", "A pair of gold shoulder plates lie here.",		5, 5, 1, 3},
	{ITEM_WEAR_ABOUT,	"royal shroud",		"royal shroud",				"A beautiful royal shroud lies here.",				1, 2, 5, 3}	

};

const struct levels levelTable[LEVELS] = {
	{0, 5,    "-3 ",	-8},
	{6, 10,   "-2 ",	-4},
	{11, 18,  "-1 ",	-2},
	{19, 45,  "",		0},
	{46, 60,  "+1 ",	2},
	{61, 70,  "+2 ",	4},
	{71, 78,  "+3 ",	5},
	{79, 85,  "+4 ",	6},
	{86, 91,  "+5 ",	7},
	{92, 95,  "+6 ",	8},
	{96, 98,  "+7 ",	9},
	{99, 100, "+8 ",	10}
};


//////////////////////////////////////////////////////////////////////
//upper level, lower level, prefix name, effect1, effect1 amount, effect2, effect2 amount, effect3, effect3 amount
//
const struct eqPrefix prefixTable[PREFIXES] = {
	{0,  6,	 "cursed ",		APPLY_HIT, -50, APPLY_MANA, -50,	APPLY_MOVE, -50},
	{7,  12, "crude ",		APPLY_HIT, -20, APPLY_MANA, -20,	APPLY_NONE, 0}, 
	{13, 20, "mediocre ",	APPLY_HIT, -5,  APPLY_MOVE, -10,	APPLY_NONE, 0},
	{21, 50, "",			APPLY_NONE, 0,	APPLY_NONE, 0,		APPLY_NONE, 0},
	{51, 62, "reinforced ",	APPLY_HIT, 5,	APPLY_MANA, 10,		APPLY_NONE, 0},
	{63, 72, "superior ",	APPLY_HIT, 15,	APPLY_MOVE, 20,		APPLY_MOVE, 20}, 
	{73, 80, "blessed ",	APPLY_HIT, 20,	APPLY_MANA, 30,		APPLY_NONE, 0},
	{81, 87, "splendid ",	APPLY_HIT, 20,	APPLY_MOVE, 80,		APPLY_NONE, 0},
	{88, 93, "mighty ",		APPLY_HIT, 25,	APPLY_MANA, 25,		APPLY_MOVE, 25},
	{94, 96, "godly ",		APPLY_HIT, 50,	APPLY_MANA, 50,		APPLY_MOVE, 50},
	{97, 98, "spunky ",		APPLY_HIT, 100,	APPLY_MANA, -30,	APPLY_MOVE,	-30},
	{99, 100,"dandy ",		APPLY_HIT, 30,	APPLY_MANA, 45,		APPLY_MOVE,	-100}
};

//////////////////////////////////////////////////////////////////////
//upper level, lower level, damage type, weapon suffix name, damage type, flag1, flag2, flag3, flag4
//
const struct wepSuffix wepSuffixTable[WEPSUFFIXES] = {
	{0,	 20,  "",			"",					0,					0,					0,					0},
//	{21, 27,  "magic",      " of {Cmagic{X",        WEAPON_MAGEBLADE,   0,                  0,                  0},
//	{28, 34,  "drain",      " of {Dleeching{X",     WEAPON_VULPINE,     0,					0,					0},
//	{35, 40,  "magic",      " of {Melements{X",     WEAPON_ELEMENTAL,   0,                  0,                  0},
	{41, 48,  "slice",		" of {Wsevering{x",		WEAPON_SHARP,		0,					0,					0},
	{49, 56,  "frbite",		" of {cfreezing{x",		WEAPON_FROST,		0,					0,					0},
	{57, 64,  "flbite",		" of {rfire{x",			WEAPON_FLAMING,		0,					0,					0},
	{65, 70,  "grep",		" of {mdraining{x",		WEAPON_VAMPIRIC,	0,					0,					0},
	{71, 76,  "shbite",		" of {Ylightning{x",	WEAPON_SHOCKING,	0,					0,					0},
	{77, 82,  "wrath",		" of {Rdestruction{x",	WEAPON_VAMPIRIC,	WEAPON_FLAMING,		0,					0},
	{83, 88,  "digestion",	" of {Dpain{x",			WEAPON_SHARP,		WEAPON_SHOCKING,	0,					0},
	{89, 95,  "unholy",		" of {ytorment{x",		WEAPON_FLAMING,		WEAPON_FROST,		WEAPON_SHOCKING,	0},
	{96, 100, "divine",		" of {gsupremacy{x",	WEAPON_SHARP,	WEAPON_VAMPIRIC,		WEAPON_FLAMING,		0}
};


//////////////////////////////////////////////////////////////////////
//upper level, lower level, eq suffix name, aff_flag, sn (spell slot number)
//
const struct eqSuffix eqSuffixTable[EQSUFFIXES] = {
	{0,  3,		" of blindness",			AFF_BLIND,				4},
	{4,  6,     " of paralysis",			AFF_WEB,	            0},
	{7,  11,	" of misdeeds",				AFF_CURSE,				17},
	{12, 16,	" of malice",				AFF_FAERIE_FIRE,		72},
	{17, 22,	" of sluggishness",			AFF_SLOW,				515},
	{23, 27,	" of disease",				AFF_PLAGUE,				503},
	{28, 72,	"",							0,						0},
	{73, 77,	" of inner sight",			AFF_DETECT_ALIGNMENT,   513},
	{78, 83,	" with bad intentions",		AFF_PROTECT_GOOD,		514},
	{84, 87,	" of airwalking",			AFF_FLYING,				56},
	{88, 90,	" of phase shift",			AFF_PASS_DOOR,			74},
	{91, 93,	" of awareness",			AFF_INFRARED,		    77},
	{94, 95,	" of illusion",				0,			    		0},
	{96, 97,	" of stealth",				AFF_SNEAK,				653},
	{98, 99,	" of true sight",			AFF_DETECT_INVIS,		19},
	{99, 100,	" of protection",			AFF_REGENERATION,		0}
};

//////////////////////////////////////////////////////////////////////
//upper level, lower level, gold, silver
//
const struct goldSilver goldSilverTable[GOLDSILVER] = {
	{0, 3,			5,		100		},
	{4, 10,			15,		200		},
	{11, 15,		22,		300		},
	{16, 20,		30,		400		},
	{21, 30,		40,		600		},
	{31, 50,		70,		700		},
	{51, 55,		80,		800		},
	{56, 70,		100,	900		},
	{71, 80,		300,	600		},
	{81, 90,		1000,	1000	},
	{91, 94,		3000,	5000	},
	{95, 100,		6000,	1000	}
	
};

//////////////////////////////////////////////////////////////////////
//upper level, lower level, name, keywords, item flag1, item flag2, extra flag
//
//const struct wearFlags wearTable[WEARFLAGS] = {
//	{0,  1,	"Caster's",		"Magical",		ITEM_MAGES,		0,		0},
//	{2, 3,	"Tyrgan ",		" Useful ",		ITEM_TYRGA,		0,		ITEM_BLESS},
//	{4, 5,	"Karanan ",		" Useful ",		ITEM_KARANA,	0,		0},
//	{6, 7,	"Sahgrinian ",	" Useful ",		ITEM_Sahgrin,	0,		ITEM_EVIL},
//	{8, 9,	"Sylvan ",		" Useful ",		ITEM_SYLVA,		0,		0},
//	{10, 11,"Thief's ",	    " Useful ",		ITEM_THIEF,		0,		0},
//	{12, 13,"Fighter's ",	" Useful ",		ITEM_MELEE,		0,		0},
//	{14, 15,"Healer's ",	" Useful ",		ITEM_HEALER,	0,		ITEM_BLESS},
//	{16, 17,"Useful ",		" Useful ",		ITEM_UNICLASS,	0,		0},
//	{18, 100,"",	"",	0,			0,		0}
//};

int RandEqRoll (int lvl, int multi){
	long value = lvl * 100;
	value = value * number_range(1,43);
	value = value /30;
	value = value * multi;
	value = value /10000;
	value = UMIN(100,value);
	value = UMAX(0,value);	
	return value;
}

int getMulti(CHAR_DATA * ch){

	if (ch->random_drop_rating > 0)
	{
		return ch->random_drop_rating+80;
	}
	return 0;
}

int modFuzz(int amount){
	return amount + number_range(-(amount/3), amount/3);
}


void addAffect(AFFECT_DATA *paf, OBJ_DATA* obj, int apply, int amount){
	paf = new_affect();
	paf->where = TO_OBJECT;
	paf->type = -1; // Izlude 10-06-04, random eq no longer shows up on aff. (type = -1, rather than 0)
	paf->level = 90;
	paf->duration = -1;
	paf->location = apply;
	paf->modifier = amount;
	paf->bitvector = 0;
	paf->next = obj->affected;
	obj->affected = paf;
}

void addWear(OBJ_DATA* obj, int wear){

obj->wear_flags += wear;

}

void addRandomItemsToCorpse(OBJ_DATA * corpse, CHAR_DATA * ch, int multi){
	//lets make some eq
	//how many items to drop

	int numToDrop = 0;
	int percentageRoll = number_range(0,100);
	int count = 0;
	if (multi == number_range(1, 99)){
		if (percentageRoll > 60)
			numToDrop++;
		if (percentageRoll > 95)
			numToDrop++;
	}
	if (multi == number_range(100, 114)){
		if (percentageRoll > 45)
			numToDrop++;
		if (percentageRoll > 85)
			numToDrop++;
		if (percentageRoll > 97)
			numToDrop++;
	}
	if (multi == number_range(115, 125)){
		if (percentageRoll > 35)
			numToDrop++;
		if (percentageRoll > 80)
			numToDrop++;
		if (percentageRoll > 92)
			numToDrop++;
		if (percentageRoll > 98)
			numToDrop++;
	}
	if (multi > 125){
		if (percentageRoll > 0)
			numToDrop++;
		if (percentageRoll > 50)
			numToDrop++;
		if (percentageRoll > 76)
			numToDrop++;
		if (percentageRoll > 88)
			numToDrop++;
		if (percentageRoll > 95)
			numToDrop++;
		if (percentageRoll > 98)
			numToDrop++;
	}
	if (numToDrop > 0){
		int i;	
		for (i = 0; i < numToDrop; i++){
			OBJ_DATA *eq;
			eq = create_object(get_obj_index(OBJ_VNUM_RANDEQ), 0);

			//what type is it and what is it??
			int item = 0;
			int gold, silver;
			//int wearRoll = RandEqRoll(ch->level, multi);
			char money[SmSL], money2[SmSL], money3[SmSL];//, wearKey[SmSL];
			switch (number_range(0,2)){
				case (0)://weaponzor
					item = number_range(0,WEAPONS-1);
					eq->name = str_dup(weaponTable[item].keys);
					eq->value[0] = weaponTable[item].weaponType;
					eq->short_descr = str_dup(weaponTable[item].name);
					eq->description = str_dup(weaponTable[item].desc);
					eq->value[1] = weaponTable[item].numOfDice;
					eq->value[2] = weaponTable[item].diceSize;
					eq->value[3] = attack_lookup(weaponTable[item].damageType);
			
//			// Insert wearflag prefix here
//			for (count = 0; count < WEARFLAGS; count++){
//				if (wearRoll >= wearTable[count].rollLow && wearRoll <= wearTable[count].rollHigh){
//						eq->wear_flags += wearTable[count].wearLoc1;
//						eq->wear_flags += wearTable[count].wearLoc2;
//						eq->extra_flags += wearTable[count].extraFlag;
//								}
//					}
//					break;

				case (1)://armor
					item = number_range(0,ARMORS-1);
					eq->item_type = ITEM_ARMOR;
					eq->wear_flags = ITEM_TAKE + armorTable[item].wearLoc;
					eq->name = str_dup(armorTable[item].keys);
					eq->short_descr = str_dup(armorTable[item].name);
					eq->description = str_dup(armorTable[item].desc);
					eq->value[0] = armorTable[item].ac1;
					eq->value[1] = armorTable[item].ac2;
					eq->value[2] = armorTable[item].ac3;
					eq->value[3] = armorTable[item].ac4;
					break;
				case (2)://monies
					gold = modFuzz(goldSilverTable[item].gold);
					silver = modFuzz(goldSilverTable[item].silver);
					item = number_range(0, GOLDSILVER-1);
					eq->item_type = ITEM_MONEY;
					eq->wear_flags = ITEM_TAKE;
					sprintf(money,  "%d gold %d silver coins", gold, silver); 
					sprintf(money2, "A pile of %d gold and %d silver coins", gold, silver);
					sprintf(money3, "A pile of %d gold and %d silver coins lies here.", gold, silver);
					eq->name = str_dup(money);
					eq->short_descr = str_dup(money2);
					eq->description = str_dup(money3);
					eq->value[1] = gold;
					eq->value[0] = silver;
					numToDrop++;
				default:
					break;
			}						
			
			if (eq->item_type == ITEM_MONEY)
			{
				obj_to_obj(eq, corpse);
				return;
			}

			//add prefix
			int prefixRoll = RandEqRoll(ch->level, multi);	
			
			//clarity check, we don't want a -50 godly item
			int modRoll = RandEqRoll(ch->level, multi);
			while (modRoll + 30 < prefixRoll || modRoll - 30 > prefixRoll){
				modRoll = RandEqRoll(ch->level, multi);
			}
	
		//For Short Descs
			char *objOrigDesc;	
			char modString[SmSL];
			char objNewDesc[SmSL];
			char objBackup[SmSL];
			objOrigDesc = eq->short_descr;


			AFFECT_DATA *paf;
			paf = new_affect();
			for (count = 0; count < LEVELS; count++){
				if (modRoll >= levelTable[count].rollLow && modRoll <= levelTable[count].rollHigh){
					if (eq->wear_flags == ITEM_TAKE + ITEM_WEAR_FEET ||
						eq->wear_flags == ITEM_TAKE + ITEM_WEAR_ARMS ||
						eq->wear_flags == ITEM_TAKE + ITEM_WEAR_LEGS ||
						eq->wear_flags == ITEM_TAKE){
							sprintf( modString, "Some ");
					} else if (eq->wear_flags == ITEM_TAKE + ITEM_WEAR_HANDS) {
						sprintf(modString, "A pair of ");
					} else {
						sprintf( modString, "A ");
					}
					strcat(modString, levelTable[count].name);

					if(eq->item_type == ITEM_WEAPON){
						//hitdam
						addAffect(paf, eq, APPLY_DAMROLL, levelTable[count].amount);
						addAffect(paf, eq, APPLY_HITROLL, levelTable[count].amount);
					} else {
						switch(number_range(0,3)){
							case (0):
								addAffect(paf, eq, APPLY_SAVING_SPELL, (-1 * (levelTable[count].amount)/2));
								break;
							case (1):
								addAffect(paf, eq, APPLY_SAVING_SPELL, (-1 * (levelTable[count].amount)/2));
								break;
							case (2):
								addAffect(paf, eq, APPLY_SAVING_SPELL, (-1 * (levelTable[count].amount)/2));
								break;
							case (3):
								addAffect(paf, eq, APPLY_AC, (-1 * (levelTable[count].amount)));
								break;
							default:
								break;
						}
					}
				}
			}
			
			for (count = 0; count < PREFIXES; count++){
				if (prefixRoll >= prefixTable[count].rollLow && prefixRoll <= prefixTable[count].rollHigh){
					strcat(modString, prefixTable[count].name);
					if (prefixTable[count].effectType1 != APPLY_NONE){
						addAffect(paf, eq, prefixTable[count].effectType1, modFuzz(prefixTable[count].effectMod1));
					}
					if (prefixTable[i].effectType2 != APPLY_NONE){
						addAffect(paf, eq, prefixTable[count].effectType2, modFuzz(prefixTable[count].effectMod2));
					}
					if (prefixTable[i].effectType3 != APPLY_NONE){
						addAffect(paf, eq, prefixTable[count].effectType3, modFuzz(prefixTable[count].effectMod3));
					}
				}
			}

			strcpy(objBackup, objOrigDesc);
			strcpy(objNewDesc, modString);
			strcat(objNewDesc, objBackup);	
			free_string( eq->short_descr );
			eq->short_descr = str_dup( objNewDesc );
			objOrigDesc = eq->short_descr;

			
			AFFECT_DATA af;
			int suffixRoll = RandEqRoll(ch->level, multi);
			if(eq->item_type == ITEM_WEAPON){
				for (count = 0; count < WEPSUFFIXES; count++){
					if (suffixRoll >= wepSuffixTable[count].rollLow && suffixRoll <= wepSuffixTable[count].rollHigh){
						strcpy(objNewDesc, objOrigDesc);
						strcpy(objBackup, wepSuffixTable[count].name);
						strcat(objNewDesc, objBackup);	
						free_string( eq->short_descr );
						eq->short_descr = str_dup( objNewDesc );
						if (wepSuffixTable[count].vect1 > 0){
							af.where     = TO_WEAPON;
							af.type      = 0;
							af.level     = 90;
							af.duration  = -1;
							af.location  = 0;
							af.modifier  = 0;
							af.bitvector = wepSuffixTable[count].vect1;
							affect_to_obj(eq,&af);
							if (wepSuffixTable[count].rollHigh > 40)
							{
							eq->value[3] = attack_lookup(wepSuffixTable[count].damageType);
							}
						}
						if (wepSuffixTable[count].vect2 > 0){
							af.where     = TO_WEAPON;
							af.type      = 0;
							af.level     = 90;
							af.duration  = -1;
							af.location  = 0;
							af.modifier  = 0;
							af.bitvector = wepSuffixTable[count].vect2;
							affect_to_obj(eq,&af);
							if (wepSuffixTable[count].rollHigh > 40)
							{
							eq->value[3] = attack_lookup(wepSuffixTable[count].damageType);
							}
						}
						if (wepSuffixTable[count].vect3 > 0){
							af.where     = TO_WEAPON;
							af.type      = 0;
							af.level     = 90;
							af.duration  = -1;
							af.location  = 0;
							af.modifier  = 0;
							af.bitvector = wepSuffixTable[count].vect3;
							affect_to_obj(eq,&af);
							if (wepSuffixTable[count].rollHigh > 40)
							{
							eq->value[3] = attack_lookup(wepSuffixTable[count].damageType);
							}
						}
						if (wepSuffixTable[count].vect4 > 0){
							af.where     = TO_WEAPON;
							af.type      = 0;
							af.level     = 90;
							af.duration  = -1;
							af.location  = 0;
							af.modifier  = 0;
							af.bitvector = wepSuffixTable[count].vect4;
							affect_to_obj(eq,&af);
							if (wepSuffixTable[count].rollHigh > 40)
							{
							eq->value[3] = attack_lookup(wepSuffixTable[count].damageType);
							}
						}			
					}				
				}
			} else {
				for (count = 0; count < EQSUFFIXES; count++){
					if (suffixRoll >= eqSuffixTable[count].rollLow && suffixRoll <= eqSuffixTable[count].rollHigh){
						strcpy(objNewDesc, objOrigDesc);
						strcpy(objBackup, eqSuffixTable[count].name);
						strcat(objNewDesc, objBackup);	
						free_string( eq->short_descr );
						eq->short_descr = str_dup( objNewDesc );
						if (eqSuffixTable[count].apply > 0){
							af.where     = TO_AFFECTS;
							af.type      = eqSuffixTable[count].sn;
							af.level     = 120;
							af.duration  = -1;
							af.location  = 0;
							af.modifier  = 0;
							af.bitvector = eqSuffixTable[count].apply;
							affect_to_obj(eq,&af);
						}
					}
				}
			}
			
			//Bug prevention - Akuji
			if ((eq != NULL) && (corpse != NULL))
			{
				obj_to_obj(eq, corpse);
			}

			//Support to randomly generate eq for immortals
			// - Akuji
			if (corpse == NULL)
			{
				obj_to_char(eq, ch);
			}
                        modString[0] = '\0';
                        objNewDesc[0] = '\0';
		}
	}
}

