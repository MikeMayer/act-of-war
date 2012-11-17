/****************************************
 ****************************************
 *****    Act of War - Chapter 5    *****
 ****************************************
 ****************************************/

///// Random EQ Generation for Act of War 5

int RandEqRoll args ( (int lvl, int multi) );
int getMulti args ( (CHAR_DATA * ch) );
int modFuzz args ( (int amount) );
void addRandomItemsToCorpse args ( (OBJ_DATA * corpse, CHAR_DATA * ch, int multi) );
void addAffect args ( (AFFECT_DATA *paf, OBJ_DATA * obj, int apply, int amount) );

extern const struct weapons weaponTable[];
extern const struct armors armorTable[];
extern const struct levels levelTable[];
extern const struct eqPrefix prefixTable[];
extern const struct wepSuffix wepSuffixTable[];
extern const struct eqSuffix eqSuffixTable[];
extern const struct wearFlags wearTable[];


struct weapons
{
	int weaponType;
	char *damageType;
	char *name;
	char *keys;
	char *desc;
	int numOfDice;
	int diceSize;
};

struct armors
{
	int wearLoc;
	char *name;
	char *keys;
	char *desc;
	int ac1;
	int ac2;
	int ac3;
	int ac4;
};

struct levels
{
	int rollLow;
	int rollHigh;
	char *name;
	int amount;	
};

struct	eqPrefix
{
    int rollLow;
	int rollHigh;
	char *name;
	int effectType1;
	int effectMod1;
	int effectType2;
	int effectMod2;
	int effectType3;
	int effectMod3;
};

struct	wepSuffix
{
    int rollLow;
	int rollHigh;
	char *damageType;
	char *name;
	int vect1;
	int vect2;
	int vect3;
	int vect4;
};

struct eqSuffix
{
	int rollLow;
	int rollHigh;
	char *name;
	int apply;
	int sn;
};
	
struct goldSilver
{	
	int rollLow;
	int rollHigh;
	int gold;
	int silver;
};

struct wearFlags
{
	int rollLow;
	int rollHigh;
	char *name;
	char *keys;
	int   wearLoc1;
	int   wearLoc2;
	int   extraFlag;
};
