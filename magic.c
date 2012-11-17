/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"

/* command procedures needed */
DECLARE_DO_FUN(do_look);

/*
 * Local functions.
 */
void say_spell args((CHAR_DATA * ch, int sn));

/* imported functions */
bool remove_obj args((CHAR_DATA * ch, int iWear, bool fReplace));
void wear_obj args((CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace));

/* New Wait State function */
void fwait_state(CHAR_DATA *ch, int npulse)
{
if(IS_AFFECTED2(ch, AFF_LAPSE))
{
 npulse += 6;
ch->wait = UMAX(ch->wait,npulse);
 }else{
ch->wait = UMAX(ch->wait,npulse);
 }
return;
}

/*
 * Lookup a skill by name.
 */
int skill_lookup(const char *name)
{
    int sn;

    for (sn = 0; sn < MAX_SKILL; sn++) {
	if (skill_table[sn].name == NULL)
	    break;
	if (LOWER(name[0]) == LOWER(skill_table[sn].name[0])
	    && !str_prefix(name, skill_table[sn].name))
	    return sn;
    }

    return -1;
}

  int skill_lookup_exact(const char *name)
 {
     int sn;
 
     for (sn = 0; sn < MAX_SKILL; sn++) {
 	if (skill_table[sn].name == NULL)
 	    break;
 	if (LOWER(name[0]) == LOWER(skill_table[sn].name[0])
 	    && !str_cmp(name, skill_table[sn].name))
 	    return sn;
     }
 
     return -1;
 }


int find_spell(CHAR_DATA * ch, const char *name)
{
    /* finds a spell the character can cast if possible */
    int sn, found = -1;

    if (IS_NPC(ch))
	return skill_lookup(name);

    for (sn = 0; sn < MAX_SKILL; sn++) {
	if (skill_table[sn].name == NULL)
	    break;
	if (LOWER(name[0]) == LOWER(skill_table[sn].name[0])
	    && !str_prefix(name, skill_table[sn].name)) {
	    if (found == -1)
		found = sn;
	    if (ch->level >= skill_table[sn].skill_level[ch->class]
		&& ch->pcdata->learned[sn] > 0)
		return sn;
	}
    }
    return found;
}

/*
 * Lookup a skill by slot number.
 * Used for object loading.
 */
int slot_lookup(int slot)
{
    extern bool fBootDb;
    int sn;

    if (slot <= 0)
	return -1;

    for (sn = 0; sn < MAX_SKILL; sn++) {
	if (slot == skill_table[sn].slot)
	    return sn;
    }

    if (fBootDb) {
	bug("Slot_lookup: bad slot %d.", slot);
	abort();
    }
    return -1;
}

/*
 * Utter mystical words for an sn.
 */
void say_spell(CHAR_DATA * ch, int sn)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    CHAR_DATA *rch;
    char *pName;
    int iSyl;
    int length;

    struct syl_type {
	char *old;
	char *new;
    };

    static const struct syl_type syl_table[] =
    {
	{" ", " "},
	{"ar", "abra"},
	{"au", "kada"},
	{"bless", "fido"},
	{"blind", "nose"},
	{"bur", "mosa"},
	{"cu", "judi"},
	{"de", "oculo"},
	{"en", "unso"},
	{"light", "dies"},
	{"lo", "hi"},
	{"mor", "zak"},
	{"move", "sido"},
	{"ness", "lacri"},
	{"ning", "illa"},
	{"per", "duda"},
	{"ra", "gru"},
	{"fresh", "ima"},
	{"re", "candus"},
	{"son", "sabru"},
	{"tect", "infra"},
	{"tri", "cula"},
	{"ven", "nofo"},
	{"a", "a"},
	{"b", "b"},
	{"c", "q"},
	{"d", "e"},
	{"e", "z"},
	{"f", "y"},
	{"g", "o"},
	{"h", "p"},
	{"i", "u"},
	{"j", "y"},
	{"k", "t"},
	{"l", "r"},
	{"m", "w"},
	{"n", "i"},
	{"o", "a"},
	{"p", "s"},
	{"q", "d"},
	{"r", "f"},
	{"s", "g"},
	{"t", "h"},
	{"u", "j"},
	{"v", "z"},
	{"w", "x"},
	{"x", "n"},
	{"y", "l"},
	{"z", "k"},
	{"", ""}
    };

    buf[0] = '\0';
    for (pName = skill_table[sn].name; *pName != '\0'; pName += length) {
	for (iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++) {
	    if (!str_prefix(syl_table[iSyl].old, pName)) {
		strcat(buf, syl_table[iSyl].new);
		break;
	    }
	}

	if (length == 0)
	    length = 1;
    }

    sprintf(buf2, "$n utters the words, '%s'.", buf);
    sprintf(buf, "$n utters the words, '%s'.", skill_table[sn].name);

    for (rch = ch->in_room->people; rch; rch = rch->next_in_room) {
	if (rch != ch)
	    act((!IS_NPC(rch) && ch->class == rch->class) ? buf : buf2,
		ch, NULL, rch, TO_VICT);
    }

    return;
}

/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */

/* 
 * Global var here for use in Coel's skilldump.
 */
int has_saved;

bool saves_spell(int level, CHAR_DATA * victim, int dam_type, int difficulty)
{

//base- same levels, victim is well_dressed
const int base = 75;
const int well_dressed = -50;
const int level_diff_up = 9;
const int level_diff_down = 5;

    int save;
    int random_roll;
    int diff;
int a, b, c, d;
int x, y, z;
//char buf[MAX_STRING_LENGTH];

save = base - (victim->saving_throw - well_dressed);

diff = victim->level - level;
if (diff > 0)
	save += diff * level_diff_up;
else
	save += diff * level_diff_down;

save += difficulty;

x = victim->level;
y = level;
z =victim->saving_throw;
a = save;  
  if (IS_AFFECTED(victim, AFF_BERSERK))
	save += victim->level / 2;

    switch (check_immune(victim, dam_type)) {
    case IS_IMMUNE:
	return TRUE;
    case IS_RESISTANT:
	save += 50;
	break;
    case IS_VULNERABLE:
	save -= 50;
	break;
    }
b = save;
//    if (!IS_NPC(victim) && class_table[victim->class].fMana)
//	save = 10 * save / 9;
    save = URANGE(5, save, 95);
c = save;
    random_roll = number_percent();
d = random_roll;
    if (has_saved > 30000)	/* Sanity check */
	has_saved = 0;
    has_saved += (random_roll < save);
//	sprintf(buf," %d %d %d %d %d %d %d.\n\r", x, y, z, a, b, c, d	);
//	send_to_char(buf, victim);
    return random_roll < save;
}

/* RT save for dispels */

bool saves_dispel(int dis_level, int spell_level, int duration)
{
    int save;

    if (duration == -1)
	spell_level += 5;
    /* very hard to dispel permanent effects */

    save = 50 + (spell_level - dis_level) * 5;
    save = URANGE(5, save, 95);
    return number_percent() < save;
}

/* co-routine for dispel magic and cancellation */

bool check_dispel(int dis_level, CHAR_DATA * victim, int sn)
{
    AFFECT_DATA *af;
    AFFECT_DATA *af2;
    if (is_affected(victim, sn)) {
	for (af = victim->affected; af != NULL; af = af->next) {
	    if (af->type == sn) {
		if (!saves_dispel(dis_level, af->level, af->duration)) {
		    affect_strip(victim, sn);
		    if (skill_table[sn].msg_off) {
			send_to_char(skill_table[sn].msg_off, victim);
			send_to_char("\n\r", victim);
		    }
		    return TRUE;
		} else
		    af->level--;
	    }
	}
    }
    return FALSE;

   if (is_affected(victim, sn)) {
	for (af2 = victim->affected; af2 != NULL; af2 = af2->next) {
	    if (af2->type == sn) {
		if (!saves_dispel(dis_level, af2->level, af2->duration)) {
		    affect_strip(victim, sn);
		    if (skill_table[sn].msg_off) {
			send_to_char(skill_table[sn].msg_off, victim);
			send_to_char("\n\r", victim);
		    }
		    return TRUE;
		} else
		    af->level--;
	    }
	}
    }
    return FALSE;

}

/* for finding mana costs -- temporary version */
int mana_cost(CHAR_DATA * ch, int min_mana, int skilllevel)
{
    if (ch->level + 2 == skilllevel)
		return 50;
	else
		return UMAX(min_mana, (100 / (2 + ch->level - skilllevel)));
}

/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

void do_cast(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *ppl;
    OBJ_DATA *obj;
    void *vo;
    int mana;
    int sn;
    int target;
    int i;

    /*
     * Switched NPC's can cast spells, but others can't.
     */
    if (IS_NPC(ch) && ch->desc == NULL)
	return;

    target_name = one_argument(argument, arg1);
    one_argument(target_name, arg2);

    if (arg1[0] == '\0') {
	send_to_char("Cast which what where?\n\r", ch);
	return;
    }
    if ((sn = find_spell(ch, arg1)) < 1
	|| skill_table[sn].spell_fun == spell_null || skill_table[sn].spell_fun == NULL
	|| (!IS_NPC(ch) && (ch->level < skill_table[sn].skill_level[ch->class]
			    || ch->pcdata->learned[sn] == 0))) {
	send_to_char("You don't know any spells of that name.\n\r", ch);
	return;
    }
    if (IS_SET(ch->in_room->affected_by, AFF_SILENT_ROOM)) {
	send_to_char("You try to speak but nothing happens!\n\r", ch);
	return;

    }
    if (ch->position < skill_table[sn].minimum_position) {
	send_to_char("You can't concentrate enough.\n\r", ch);
	return;
    }
    if (ch->level + 2 == skill_table[sn].skill_level[ch->class])
	mana = 50;
    else
	mana = UMAX(
		       skill_table[sn].min_mana,
		       100 / (2 + ch->level - skill_table[sn].skill_level[ch->class]));

/* Special Mana cost for the Blink spell.
   For every person in room, mana cost is uped by 50
   This is to prevent cheep cheep PK stops
*/

 if (skill_lookup("blink") == sn )
 {
  i = 1;
  for( ppl = ch->in_room->people; ppl != NULL; ppl = ppl->next_in_room )
    {
     if (ppl != ch && !IS_NPC(ppl) && !IS_IMMORTAL(ppl))
      i++;
    }
   mana *= i;
 }
   /*
     * Locate targets.
     */
    victim = NULL;
    obj = NULL;
    vo = NULL;
    target = TARGET_NONE;

    switch (skill_table[sn].target) {
    default:
	bug("Do_cast: bad target for sn %d.", sn);
	return;

    case TAR_IGNORE:
	break;

    case TAR_CHAR_OFFENSIVE:
	if (arg2[0] == '\0') {
	    if ((victim = ch->fighting) == NULL) {
		send_to_char("Cast the spell on whom?\n\r", ch);
		return;
	    }
	} else {
	    if ((victim = get_char_room(ch, target_name)) == NULL) {
		send_to_char("They aren't here.\n\r", ch);
		return;
	    }
	}
/*
   if ( ch == victim )
   {
   send_to_char( "You can't do that to yourself.\n\r", ch );
   return;
   }
 */

    if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOFIGHT))
	return;
    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFIGHT))
	return;



	if (!IS_NPC(ch)) {

	    if (is_safe(ch, victim) && victim != ch) {
		send_to_char("Not on that target.\n\r", ch);
		return;
	    }
	    check_killer(ch, victim);
	}
	if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
	    send_to_char("You can't do that on your own follower.\n\r",
			 ch);
	    return;
	}
	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_CHAR_DEFENSIVE:
	if (arg2[0] == '\0') {
	    victim = ch;
	} else {
	    if ((victim = get_char_room(ch, target_name)) == NULL) {
		send_to_char("They aren't here.\n\r", ch);
		return;
	    }
	}

    if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOFIGHT))
	return;
    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFIGHT))
	return;

	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_CHAR_SELF:
	if (arg2[0] != '\0' && !is_name(target_name, ch->name)) {
	    send_to_char("You cannot cast this spell on another.\n\r", ch);
	    return;
	}
	vo = (void *) ch;
	target = TARGET_CHAR;
	break;

    case TAR_OBJ_INV:
	if (arg2[0] == '\0') {
	    send_to_char("What should the spell be cast upon?\n\r", ch);
	    return;
	}
	if ((obj = get_obj_carry(ch, target_name, ch)) == NULL) {
	    send_to_char("You are not carrying that.\n\r", ch);
	    return;
	}
	vo = (void *) obj;
	target = TARGET_OBJ;
	break;

    case TAR_OBJ_CHAR_OFF:
	if (arg2[0] == '\0') {
	    if ((victim = ch->fighting) == NULL) {
		send_to_char("Cast the spell on whom or what?\n\r", ch);
		return;
	    }
	    target = TARGET_CHAR;
	} else if ((victim = get_char_room(ch, target_name)) != NULL) {
	    target = TARGET_CHAR;
	}
	if (target == TARGET_CHAR) {	/* check the sanity of the attack */
	    if (is_safe_spell(ch, victim, FALSE) && victim != ch) {
		send_to_char("Not on that target.\n\r", ch);
		return;
	    }
	    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) {
		send_to_char("You can't do that on your own follower.\n\r",
			     ch);
		return;
	    }
	    if (!IS_NPC(ch))
		check_killer(ch, victim);

	    vo = (void *) victim;
	} else if ((obj = get_obj_here(ch, target_name)) != NULL) {
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	} else {
	    send_to_char("You don't see that here.\n\r", ch);
	    return;
	}
	break;

    case TAR_OBJ_CHAR_DEF:
	if (arg2[0] == '\0') {
	    vo = (void *) ch;
	    target = TARGET_CHAR;
	} else if ((victim = get_char_room(ch, target_name)) != NULL) {
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOFIGHT))
	return;
    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFIGHT))
	return;
	    vo = (void *) victim;
	    target = TARGET_CHAR;
	} else if ((obj = get_obj_carry(ch, target_name, ch)) != NULL) {
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	} else {
	    send_to_char("You don't see that here.\n\r", ch);
	    return;
	}

	break;
    }

    if (!IS_NPC(ch) && ch->mana < mana) {
	send_to_char("You don't have enough mana.\n\r", ch);
	return;
    }
    if (str_cmp(skill_table[sn].name, "ventriloquate"))
	say_spell(ch, sn);

if (!IS_IMMORTAL(ch))
{
    WAIT_STATE(ch, skill_table[sn].beats);
} else {
	mana = 0;
}

    if (number_percent() > get_skill(ch, sn)) 
{
//	if (IS_SET(ch->act, PLR_SUNWEAK)) {
//	    send_to_char("The bright sun ruins your concentration.\n\r", ch);
//	} else
	    send_to_char("You lost your concentration.\n\r", ch);

if( !  (ch ==  (CHAR_DATA *) vo && skill_table[sn].target == TAR_CHAR_OFFENSIVE))
	check_improve(ch, sn, FALSE, 1);

	ch->mana -= mana / 2;
    } else if (victim != NULL &&
	       !strcmp(class_table[victim->class].name, "barbarian")
	       && number_percent() < victim->level / 4) {
	ch->mana -= mana;
if( !  (ch ==  (CHAR_DATA *) vo && skill_table[sn].target == TAR_CHAR_OFFENSIVE))
	check_improve(ch, sn, TRUE, 1);
	send_to_char("You have resisted a spell.\n\r", victim);
	send_to_char("Your victim has resisted your spell.\n\r", ch);
    } else {
	ch->mana -= mana;
//	if (IS_NPC(ch) || class_table[ch->class].fMana)
//	    /* class has spells */
	    (*skill_table[sn].spell_fun) (sn, ch->level, ch, vo, target);
//	else
//	    (*skill_table[sn].spell_fun) (sn, 3 * ch->level / 4, ch, vo, target);
if( !  (ch ==  (CHAR_DATA *) vo && skill_table[sn].target == TAR_CHAR_OFFENSIVE))
	check_improve(ch, sn, TRUE, 1);
    }

    if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
	 || (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
	&& victim != ch
	&& victim->master != ch) {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for (vch = ch->in_room->people; vch; vch = vch_next) {
	    vch_next = vch->next_in_room;
	    if (victim == vch && victim->fighting == NULL && victim->position >= POS_FIGHTING) {

		check_killer(ch, victim);
		multi_hit(victim, ch, TYPE_UNDEFINED);
		break;
	    }
	}
    }
    return;
}

/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell(int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj)
{
    void *vo;
    int target = TARGET_NONE;

    if (sn <= 0)
	return;

    if (sn >= MAX_SKILL || skill_table[sn].spell_fun == 0) {
	bug("Obj_cast_spell: bad sn %d.", sn);
	return;
    }
    switch (skill_table[sn].target) {
    default:
	bug("Obj_cast_spell: bad target for sn %d.", sn);
	return;

    case TAR_IGNORE:
	vo = NULL;
	break;

    case TAR_CHAR_OFFENSIVE:
	if (victim == NULL)
	    victim = ch->fighting;
	if (victim == NULL) {
	    send_to_char("You can't do that.\n\r", ch);
	    return;
	}
	if (is_safe(ch, victim) && ch != victim) {
	    send_to_char("Something isn't right...\n\r", ch);
	    return;
	}
	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_CHAR_DEFENSIVE:
    case TAR_CHAR_SELF:
	if (victim == NULL)
	    victim = ch;
	vo = (void *) victim;
	target = TARGET_CHAR;
	break;

    case TAR_OBJ_INV:
	if (obj == NULL) {
	    send_to_char("You can't do that.\n\r", ch);
	    return;
	}
	vo = (void *) obj;
	target = TARGET_OBJ;
	break;

    case TAR_OBJ_CHAR_OFF:
	if (victim == NULL && obj == NULL){
	    if (ch->fighting != NULL)
		victim = ch->fighting;
	    else {
		send_to_char("You can't do that.\n\r", ch);
		return;
	    }
        }
	if (victim != NULL) {
	    if (is_safe_spell(ch, victim, FALSE) && ch != victim) {
		send_to_char("Somehting isn't right...\n\r", ch);
		return;
	    }
	    vo = (void *) victim;
	    target = TARGET_CHAR;
	} else {
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	}
	break;

    case TAR_OBJ_CHAR_DEF:
	if (victim == NULL && obj == NULL) {
	    vo = (void *) ch;
	    target = TARGET_CHAR;
	} else if (victim != NULL) {
	    vo = (void *) victim;
	    target = TARGET_CHAR;
	} else {
	    vo = (void *) obj;
	    target = TARGET_OBJ;
	}

	break;
    }


    if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
	 || (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
	&& victim != ch
	&& victim->master != ch) {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for (vch = ch->in_room->people; vch; vch = vch_next) {
	    vch_next = vch->next_in_room;
	    if (victim == vch) {
		check_killer(ch, victim);
                target_name = "";
                (*skill_table[sn].spell_fun) (sn, level, ch, vo, target);
		break;	
	    }
	}
    }
     else {
        target_name = "";
        (*skill_table[sn].spell_fun) (sn, level, ch, vo, target);
        }
    return;
}

//Vashna 07-14-01:Modifying attack spells so they cause less damage.
//Tasydra 07-25-01:You fucked up Cure serious and Cure Critical here. Fixed it, and uped heal to make it worthwhile.
//Serek 07-26-01: I am here to fuck up new shit.. with new spells!
//damage spell dice
#define DAM_ACIDBLAST 	11
#define DAM_BURNINGHANDS	7
#define DAM_CALLLIGHTNING	7
#define DAM_CAUSELIGHT	3
#define DAM_CAUSECRITICAL	8
#define DAM_CAUSESERIOUS	6
#define DAM_CHAINLIGHTNING	4
#define DAM_CHILLTOUCH	6
#define DAM_COLOURSPRAY	8
#define DAM_CURECRITICAL	8
#define DAM_CURELIGHT	6
#define DAM_CURESERIOUS	7
#define DAM_DEMONFIRE	8
#define DAM_EARTHQUAKE	5
#define DAM_FIREBALL	7
#define DAM_FLAMESTRIKE	7
#define DAM_HARMSPELL	10   // ick, a confliction!
#define DAM_HEAL		13
#define DAM_HOLYWORD	7
#define DAM_LIGHTNINGBOLT	7
#define DAM_MAGICMISSILE	7
#define DAM_RAYOFTRUTH	8
#define DAM_SMITE		10
#define DAM_SHOCKINGGRASP	6
#define DAM_ACIDBREATH	15
#define DAM_FIREBREATH	11
#define DAM_TREMOR		14
#define DAM_FIRESTORM	11
#define DAM_FROSTBREATH	10
#define DAM_GASBREATH	7
#define DAM_LIGHTNINGBREATH	17
#define DAM_DEATHCOIL	8
#define DAM_UNHOLYWORD	8
#define DAM_AGE		8
#define DAM_YOUTH		8

/*
 * Spell functions.
 */
void spell_acid_blast(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

//    if(ch == victim)
//	dam = 0;
//    else
   int d = !strcmp(class_table[ch->class].name,"Bard") ? DAM_ACIDBLAST : DAM_ACIDBLAST -2;
    dam = dice(level, d);
    if (saves_spell(level, victim, DAM_ACID, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_ACID, TRUE);
    return;
}

void spell_web(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA *paf;


    if (is_affected(victim, sn)) {
	if (victim == ch)
	    send_to_char("You are already tied down.\n\r", ch);
	else
	    act("$N is already tied down.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (saves_spell(level, victim, DAM_OTHER, SAVE_OFTEN)) {
	send_to_char("You fail.\n\r", ch);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 2;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_WEB;
    affect_to_char(victim, &af);

    if (!IS_NPC(victim))
    {
    paf = affect_find(ch->affected, skill_lookup("noquit"));

    if (paf != NULL)
	affect_remove(ch, paf);

    af.where = TO_AFFECTS;
    af.type = skill_lookup("noquit");
    af.level = 1;
    af.duration = 3;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_NOQUIT;
    affect_to_char(ch, &af);
	}
	
    paf = affect_find(victim->affected, skill_lookup("noquit"));

    if (paf != NULL)	
	affect_remove(victim, paf);

    af.where = TO_AFFECTS;
    af.type = skill_lookup("noquit");
    af.level = 1;
    af.duration = 3;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_NOQUIT;
    affect_to_char(victim, &af);

    send_to_char("Your legs have been COVERED IN STICKY WEBS!!\n\r", victim);
    if (ch != victim)
	act("$N is COVERED IN STICKY WEBS!!", ch, NULL, victim, TO_CHAR);

    check_killer(ch, victim);
    return;

}
void spell_entangle(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA *paf;

    if (!IS_OUTSIDE(ch)) {
	send_to_char("You must be out of doors.\n\r", ch);
	return;
    }

    if (is_affected(victim, sn)) {
	if (victim == ch)
	    send_to_char("You are already wrapped up by something.\n\r", ch);
	else
	    act("$N is already tied down.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (saves_spell(level, victim, DAM_OTHER, SAVE_OFTEN)) {
	send_to_char("You fail.\n\r", ch);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 10;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_WEB;
    affect_to_char(victim, &af);

    if (!IS_NPC(victim))
    {
    paf = affect_find(ch->affected, skill_lookup("noquit"));

    if (paf != NULL)
	affect_remove(ch, paf);

    af.where = TO_AFFECTS;
    af.type = skill_lookup("noquit");
    af.level = 1;
    af.duration = 3;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_NOQUIT;
    affect_to_char(ch, &af);
	}
	
    paf = affect_find(victim->affected, skill_lookup("noquit"));

    if (paf != NULL)
	affect_remove(victim, paf);

    af.where = TO_AFFECTS;
    af.type = skill_lookup("noquit");
    af.level = 1;
    af.duration = 3;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_NOQUIT;
    affect_to_char(victim, &af);

    send_to_char("The ground grows up AROUND YOUR LEGS!!\n\r", victim);
    if (ch != victim)
	act("$N is COATED BY A THICK WEB OF VINES!!", ch, NULL, victim, TO_CHAR);

    check_killer(ch, victim);
    return;

}
void spell_armor(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn)) {
	if (victim == ch)
	    send_to_char("You are already armored.\n\r", ch);
	else
	    act("$N is already armored.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 24;
    af.modifier = -20;
    af.location = APPLY_AC;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("You feel someone protecting you.\n\r", victim);
    if (ch != victim)
	act("$N is protected by your magic.", ch, NULL, victim, TO_CHAR);
    return;
}

void spell_bless(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    /* deal with the object case first */
    if (target == TARGET_OBJ) {
	obj = (OBJ_DATA *) vo;
	if (IS_OBJ_STAT(obj, ITEM_BLESS)) {
	    act("$p is already blessed.", ch, obj, NULL, TO_CHAR);
	    return;
	}
	if (IS_OBJ_STAT(obj, ITEM_EVIL)) {
	    AFFECT_DATA *paf;

	    paf = affect_find(obj->affected, gsn_curse);
	    if (!saves_dispel(level, paf != NULL ? paf->level : obj->level, 0)) {
		if (paf != NULL)
		    affect_remove_obj(obj, paf);
		act("$p glows a pale blue.", ch, obj, NULL, TO_ALL);
		REMOVE_BIT(obj->extra_flags, ITEM_EVIL);
		return;
	    } else {
		act("The evil of $p is too powerful for you to overcome.",
		    ch, obj, NULL, TO_CHAR);
		return;
	    }
	}
	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = 6 + level;
	af.location = APPLY_SAVES;
	af.modifier = 0;
	af.bitvector = ITEM_BLESS;
	affect_to_obj(obj, &af);

	act("$p glows with a holy aura.", ch, obj, NULL, TO_ALL);

	if (obj->wear_loc != WEAR_NONE)
	    ch->saving_throw -= 1;
	return;
    }
    /* character target */
    victim = (CHAR_DATA *) vo;

    if (victim->position == POS_FIGHTING || is_affected(victim, sn)) {
	if (victim == ch)
	    send_to_char("You are already blessed.\n\r", ch);
	else
	    act("$N already has divine favor.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 6 + level;
    af.location = APPLY_HITROLL;
    af.modifier = level / 8;
    af.bitvector = 0;
    affect_to_char(victim, &af);

    af.location = APPLY_SAVING_SPELL;
    af.modifier = 0 - level / 8;
    affect_to_char(victim, &af);
    send_to_char("You feel righteous.\n\r", victim);
    if (ch != victim)
	act("You grant $N the favor of your god.", ch, NULL, victim, TO_CHAR);
    return;
}

void spell_blindness(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_BLIND) || saves_spell(level, victim, DAM_OTHER, SAVE_OFTEN))
	return;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.location = APPLY_HITROLL;
    af.modifier = -4;
    af.duration = 7;
    af.bitvector = AFF_BLIND;
    affect_to_char(victim, &af);
    send_to_char("You are blinded!\n\r", victim);
    act("$n appears to be blinded.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_burning_hands(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_BURNINGHANDS);
    if (saves_spell(level, victim, DAM_FIRE, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_FIRE, TRUE);
    return;
}

void spell_call_lightning(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;

    if (!IS_OUTSIDE(ch)) {
	send_to_char("You must be out of doors.\n\r", ch);
	return;
    }
    if (weather_info.sky < SKY_RAINING) {
	send_to_char("You need bad weather.\n\r", ch);
	return;
    }
    dam = dice(level, DAM_CALLLIGHTNING);

    send_to_char("Lightning strikes your foes!\n\r", ch);
    act("$n calls lightning to strike $s foes!",
	ch, NULL, NULL, TO_ROOM);

    for (vch = char_list; vch != NULL; vch = vch_next) {
	vch_next = vch->next;
	if (vch->in_room == NULL)
	    continue;
	if (vch->in_room == ch->in_room) {
	    if (vch != ch && !is_safe_spell(ch, vch, TRUE))
		damage_old(ch, vch, saves_spell(level, vch, DAM_LIGHTNING, SAVE_ATTACK)
			   ? dam / 2 : dam, sn, DAM_LIGHTNING, TRUE);
	    continue;
	}
	if (vch->in_room->area == ch->in_room->area
	    && IS_OUTSIDE(vch)
	    && IS_AWAKE(vch))
	    send_to_char("Lightning flashes in the sky.\n\r", vch);
    }

    return;
}


void spell_calm(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{

    if (IS_SET(ch->act, PLR_AUTOASSIST)) 
    {
	send_to_char("Autoassist removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOASSIST);
    }
    stop_fighting(ch, FALSE);

}

void spell_cancellation(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    bool found = FALSE;

    level += 2;

    if ((!IS_NPC(ch) && IS_NPC(victim) &&
	 !(IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)) ||
	(IS_NPC(ch) && !IS_NPC(victim))) {
	send_to_char("You failed, try dispel magic.\n\r", ch);
	return;
    }
    if (!IS_NPC(victim) && GET_SIDE(victim) != GET_SIDE(ch)) {
	send_to_char("Use dispel not cancellation.\n\r", ch);
	return;
    }
    /* unlike dispel magic, the victim gets NO save */

    /* begin running through the spells */

    if (check_dispel(level, victim, skill_lookup("armor")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("bless")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("death touch")))
 {
	found = TRUE;
	act("$n no longer looks so white.", victim, NULL, NULL, TO_ROOM);
    }
    if(check_dispel(level, victim, skill_lookup("lapse"))) {
	found = TRUE;
	act("$n no longer looks distorted by time.", victim, NULL, NULL, TO_ROOM);
	}

    if (check_dispel(level, victim, skill_lookup("blindness"))) {
	found = TRUE;
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("calm"))) {
	found = TRUE;
	act("$n no longer looks so peaceful...", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("change sex"))) {
	found = TRUE;
	act("$n looks more like $mself again.", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("charm person"))) {
	found = TRUE;
	act("$n regains $s free will.", victim, NULL, NULL, TO_ROOM);
	stop_follower(victim);
    }
    if (check_dispel(level, victim, skill_lookup("chill touch"))) {
	found = TRUE;
	act("$n looks warmer.", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("curse")))
	found = TRUE;
	
	if (check_dispel(level, victim, skill_lookup("decay")))  // Decay!
	act("$n is no longer is rotting.\n\r", victim, NULL, NULL, TO_ROOM);
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect evil")))
	found = TRUE;
	
	if (check_dispel(level, victim, skill_lookup("mana burn"))){
	found = TRUE;
	act("$n no longer is spraying mana everywhere...", victim, NULL, NULL, TO_ROOM);
}
	if (check_dispel(level, victim, skill_lookup("life drain"))){
	found = TRUE;
	act("$n no longer is being drained of life.", victim, NULL, NULL, TO_ROOM);	
}
    if (check_dispel(level, victim, skill_lookup("detect good")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect hidden")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect invis")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect magic")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("faerie fire"))) {
	act("$n's outline fades.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("fly"))) {
	act("$n falls to the ground!", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("frenzy"))) {
	act("$n no longer looks so wild.", victim, NULL, NULL, TO_ROOM);;
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("giant strength"))) {
	act("$n no longer looks so mighty.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("haste"))) {
	act("$n is no longer moving so quickly.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("infravision")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("invis"))) {
	act("$n fades into existance.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("mass invis"))) {
	act("$n fades into existance.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("pass door")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("protection evil")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("protection good")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("sanctuary"))) {
	act("The white aura around $n's body vanishes.",
	    victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("reflect"))) {
	act("The silver aura around $n's body vanishes.",
	    victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("shield"))) {
	act("The shield protecting $n vanishes.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("sleep")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("slow"))) {
	act("$n is no longer moving so slowly.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("stone skin"))) {
	act("$n's skin regains its normal texture.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("weaken"))) {
	act("$n looks stronger.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("age"))) {
	act("$n looks younger.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("decripify"))) {
	act("$n looks relieved.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
	}
    if (check_dispel(level, victim, skill_lookup("youth"))) {
	act("$n looks older.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (found)
	send_to_char("Ok.\n\r", ch);
    else
	send_to_char("Spell failed.\n\r", ch);
}

void spell_cause_light(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int antiheal;

    antiheal = dice(level, DAM_CAUSELIGHT);
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) 
	{
		victim->hit = UMIN(victim->hit + antiheal, victim->max_hit);
		act("$n grins!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You feel better!!\n\r", victim);
	} 
	else 
	{
		damage_old(ch, (CHAR_DATA *) vo, dice(level, DAM_CAUSELIGHT), sn, DAM_HARM, TRUE);;
	}

    return;
	}


void spell_cause_critical(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int antiheal;

    antiheal = dice(level, DAM_CAUSECRITICAL);
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) 
	{
		victim->hit = UMIN(victim->hit + antiheal, victim->max_hit);
		act("$n grins!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You feel better!!\n\r", victim);
	} 
	else 
	{
		damage_old(ch, (CHAR_DATA *) vo, dice(level, DAM_CAUSECRITICAL), sn, DAM_HARM, TRUE);;
	}

    return;
	}


void spell_cause_serious(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int antiheal;

    antiheal = dice(level, DAM_CAUSESERIOUS);
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) 
	{
		victim->hit = UMIN(victim->hit + antiheal, victim->max_hit);
		act("$n grins!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You feel better!!\n\r", victim);
	} 
	else 
	{
		damage_old(ch, (CHAR_DATA *) vo, dice(level, DAM_CAUSESERIOUS), sn, DAM_HARM, TRUE);;
	}

    return;
	}


void spell_chain_lightning(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *tmp_vict, *last_vict, *next_vict;
    bool found;
    int dam;

    /* first strike */

    act("A lightning bolt leaps from $n's hand and arcs to $N.",
	ch, NULL, victim, TO_ROOM);
    act("A lightning bolt leaps from your hand and arcs to $N.",
	ch, NULL, victim, TO_CHAR);
    act("A lightning bolt leaps from $n's hand and hits you!",
	ch, NULL, victim, TO_VICT);

    dam = dice(level, DAM_CHAINLIGHTNING);
    if (saves_spell(level, victim, DAM_LIGHTNING, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_LIGHTNING, TRUE);
    last_vict = victim;
    level -= 3;			/* decrement damage */

    /* new targets */
    while (level > 0) {
	found = FALSE;
	for (tmp_vict = ch->in_room->people;
	     tmp_vict != NULL;
	     tmp_vict = next_vict) {
	    next_vict = tmp_vict->next_in_room;
	    if (!is_safe_spell(ch, tmp_vict, TRUE) && tmp_vict != last_vict) {
		found = TRUE;
		last_vict = tmp_vict;
		act("The bolt arcs to $n!", tmp_vict, NULL, NULL, TO_ROOM);
		act("The bolt hits you!", tmp_vict, NULL, NULL, TO_CHAR);
		dam = dice(level, DAM_CHAINLIGHTNING);
		if (saves_spell(level, tmp_vict, DAM_LIGHTNING, SAVE_ATTACK))
		    dam /= 2;
		damage_old(ch, tmp_vict, dam, sn, DAM_LIGHTNING, TRUE);
		level -= 3;	/* decrement damage */
	    }
	}			/* end target searching loop */

	if (!found) {		/* no target found, hit the caster */
	    if (ch == NULL)
		return;

	    if (last_vict == ch) {	/* no double hits */
		act("The bolt seems to have fizzled out.", ch, NULL, NULL, TO_ROOM);
		act("The bolt grounds out through your body.",
		    ch, NULL, NULL, TO_CHAR);
		return;
	    }
	    last_vict = ch;
	    act("The bolt arcs to $n...whoops!", ch, NULL, NULL, TO_ROOM);
	    send_to_char("You are struck by your own lightning!\n\r", ch);
	    dam = dice(level, DAM_CHAINLIGHTNING);
	    if (saves_spell(level, ch, DAM_LIGHTNING, 0))
		dam /= 3;
	    damage_old(ch, ch, dam, sn, DAM_LIGHTNING, TRUE);
	    level -= 4;		/* decrement damage */
	    if (ch == NULL)
		return;
	}
	/* now go back and find more targets */
    }
}

void spell_change_sex(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn)) {
	if (victim == ch)
	    send_to_char("You've already been changed.\n\r", ch);
	else
	    act("$N has already had $s(?) sex changed.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (saves_spell(level, victim, DAM_OTHER, SAVE_RARELY - 25))
	return;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 2 * level;
    af.location = APPLY_SEX;
    do {
	af.modifier = number_range(0, 2) - victim->sex;
    }
    while (af.modifier == 0);
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("You feel different.\n\r", victim);
    act("$n doesn't look like $mself anymore...", victim, NULL, NULL, TO_ROOM);
	//set_prename(ch);
    return;
}

void spell_charm_person(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    CHAR_DATA *och;
    int num;

    if (is_safe(ch, victim))
	return;

    if (victim == ch) {
	send_to_char("You like yourself even better!\n\r", ch);
	return;
    }
    if (IS_AFFECTED(victim, AFF_CHARM)
	|| IS_AFFECTED(ch, AFF_CHARM)
	|| level < victim->level
	|| IS_SET(victim->imm_flags, IMM_CHARM)
	|| saves_spell(level, victim, DAM_CHARM, SAVE_NORMAL))
	return;

    if (!IS_NPC(victim))
	return;

    num = 1;

    for (och = char_list; och != NULL; och = och->next) {

	if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch)
	    num++;

    }
    if (num > (ch->level / 15)) {
	send_to_char("You can only have one charmed follower per 15 levels.\n\r", ch);
	return;
    }
    if (IS_SET(victim->in_room->room_flags, ROOM_LAW)) {
	send_to_char(
			"Cant do that here sorry.\n\r", ch);
	return;
    }
    if (victim->master)
	stop_follower(victim);
    add_follower(victim, ch);
    victim->master = ch;
    victim->leader = ch;
    victim->side = ch->side;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 200;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char(victim, &af);
    act("Isn't $n just so nice?", ch, NULL, victim, TO_VICT);
    if (ch != victim)
	act("$N looks at you with adoring eyes.", ch, NULL, victim, TO_CHAR);
    return;
}

void spell_chill_touch(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;


//    if(ch == victim)
//	dam = 0;
//    else
     dam = dice(level, DAM_CHILLTOUCH);
    if (!saves_spell(level, victim, DAM_COLD, SAVE_ATTACK)) {
	act("$n turns blue and shivers.", victim, NULL, NULL, TO_ROOM);
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 6;
	af.location = APPLY_STR;
	af.modifier = -1;
	af.bitvector = 0;
	affect_join(victim, &af);
    } else {
	dam /= 2;
    }

    damage_old(ch, victim, dam, sn, DAM_COLD, TRUE);
    return;
}

void spell_colour_spray(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

 
//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_COLOURSPRAY);
    if (saves_spell(level, victim, DAM_LIGHT, SAVE_ATTACK))
	dam /= 2;
    else
	spell_blindness(skill_lookup("blindness"),
			(level / 3) * 2, ch, (void *) victim, TARGET_CHAR);

    damage_old(ch, victim, dam, sn, DAM_LIGHT, TRUE);
    return;
}

void spell_servant(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *mob;
    CHAR_DATA *och;
    int num;

    num = 1;

    for (och = char_list; och != NULL; och = och->next) {

	if (IS_NPC(och))
	    if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch)
		num++;

    }
    if (num > (ch->level / 15)) {
	send_to_char("You can't have that many.\n\r", ch);
	return;
    }
    mob = create_mobile(get_mob_index(MOB_SERVANT));

    if (mob == NULL) {
	send_to_char("Nothing happens...\n\r", ch);
	return;
    }
    send_to_char("You conjure up a hard working servant.\n\r", ch);
    char_to_room(mob, ch->in_room);
    SET_BIT(mob->affected_by, AFF_CHARM);
    add_follower(mob, ch);
    mob->leader = ch;
    mob->master = ch;

}

void spell_decoy(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *mob;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    mob = NULL;

    if(IS_NPC(victim))
    {
	send_to_char("Not on mobs.\n\r", ch);
	return;
    }

    if( victim->side != ch->side)
    {
	send_to_char("Not on enemies.\n\r", ch);
	return;
    }

    mob = create_mobile(get_mob_index(MOB_DECOY));

    if (mob == NULL) {
	send_to_char("Nothing happens...\n\r", ch);
	return;
    }
    free_string(mob->description);
    mob->description = str_dup(victim->description);

    free_string(mob->long_descr);
    
       if (victim->level >= LVL_LEGEND)
		sprintf(buf, "*Legendary %s* is here.\n\r", victim->name);    

    else
    
    	if (victim->level >= LVL_REGENT)
		sprintf(buf, "*Regent %s* is here.\n\r", victim->name);    

    else 
    
    if (victim->level >= LVL_LORD)
		if (ch->sex == SEX_MALE)
			sprintf(buf, "*Lord %s* is here.\n\r", victim->name);
		else
		    sprintf(buf, "*Lady %s* is here.\n\r", victim->name);
    else 
    
    if (victim->level >= LVL_MASTER)
		if (victim->sex == SEX_MALE)
		    sprintf(buf, "*Master %s* is here.\n\r", victim->name);
		else
			sprintf(buf, "*Mistress %s* is here.\n\r", victim->name);
    else
    
     if (victim->level >= LVL_DUKE)
		if (victim->sex == SEX_MALE)
		    sprintf(buf, "*Duke %s* is here.\n\r", victim->name);
		else
			sprintf(buf, "*Duchess %s* is here.\n\r", victim->name);
    else 
    
    if (victim->level >= LVL_COUNT)
		if (victim->sex == SEX_MALE)
		    sprintf(buf, "*Count %s* is here.\n\r", victim->name);
		else
		    sprintf(buf, "*Countess %s* is here.\n\r", victim->name);
    else 
    
    if (victim->level >= LVL_BARON)
		if (victim->sex == SEX_MALE)
			sprintf(buf, "*Baron %s* is here.\n\r", victim->name);
		else
			sprintf(buf, "*Baroness %s* is here.\n\r", victim->name);
/*	
	set_prename(victim);
	if(victim->level >= LVL_LEGEND)
		sprintf(buf, "*%s %s* is here.\n\r",ch->pcdata->prename, victim->name);
*/
	else
		sprintf(buf, "*A %s from %s* is here.\n\r",
	
	pc_race_table[victim->race].room_name,
	kingdom_table[victim->side].name);

    mob->long_descr = str_dup(buf);

    free_string(mob->short_descr);

   if (victim->level >= LVL_LEGEND)
		sprintf(buf2, "*Legendary %s*", victim->name);    

    else 
    
    if (victim->level >= LVL_REGENT)
		sprintf(buf2, "*Regent %s*", victim->name);    

    else 
    
    if (victim->level >= LVL_LORD)
		if (ch->sex == SEX_MALE)
			sprintf(buf2, "*Lord %s*", victim->name);
		else
		    sprintf(buf2, "*Lady %s*", victim->name);
    else 
    
    if (victim->level >= LVL_MASTER)
		if (victim->sex == SEX_MALE)
		    sprintf(buf2, "*Master %s*", victim->name);
		else
			sprintf(buf2, "*Mistress %s*", victim->name);
    else
    
     if (victim->level >= LVL_DUKE)
		if (victim->sex == SEX_MALE)
		    sprintf(buf2, "*Duke %s*", victim->name);
		else
			sprintf(buf2, "*Duchess %s*", victim->name);
    else 
    
    if (victim->level >= LVL_COUNT)
		if (victim->sex == SEX_MALE)
		    sprintf(buf2, "*Count %s*", victim->name);
		else
		    sprintf(buf2, "*Countess %s*", victim->name);
    else 
    
    if (victim->level >= LVL_BARON)
		if (victim->sex == SEX_MALE)
			sprintf(buf2, "*Baron %s*", victim->name);
		else
			sprintf(buf2, "*Baroness %s*", victim->name);

/*
	set_prename(victim);
	if(victim->level >= LVL_LEGEND)
		sprintf(buf2, "*%s %s*",ch->pcdata->prename, victim->name);
*/
    else
		sprintf(buf2, "*A %s from %s*", pc_race_table[victim->race].room_name,
	
	kingdom_table[victim->side].name);
    mob->short_descr = str_dup(buf2);


    free_string(mob->name);
    sprintf(buf3, "%s %s", victim->name, pc_race_table[victim->race].name);
    mob->name = str_dup(buf3);
    mob->side = victim->side;

    send_to_char("You conjure up a decoy.\n\r", ch);
    char_to_room(mob, victim->in_room);

}

void spell_mount(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *mob;
    mob = NULL;

    if (ch->level <= 20)
	mob = create_mobile(get_mob_index(MOB_LANDSTRIDER));
    else if (ch->level <= 45)
	mob = create_mobile(get_mob_index(MOB_FLAMING_STEED));
    else if (ch->level <= 120)
	mob = create_mobile(get_mob_index(MOB_FLAMING_STEED));

    if (mob == NULL) {
	send_to_char("Nothing happens...\n\r", ch);
	return;
    }
    send_to_char("You conjure up something to ride.\n\r", ch);
    char_to_room(mob, ch->in_room);
    add_follower(mob, ch);
    mob->leader = ch;
    mob->master = ch;
}

void spell_guardian(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *mob;
    CHAR_DATA *och;
    int num;

    num = 1;

    for (och = char_list; och != NULL; och = och->next) {

	if (IS_NPC(och))
	    if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch)
		num++;

    }
    if (num > (ch->level / 15)) {
	send_to_char("You can't have that many.\n\r", ch);
	return;
    }
    mob = create_mobile(get_mob_index(MOB_GUARDIAN));

    if (mob == NULL) {
	send_to_char("Nothing happens...\n\r", ch);
	return;
    }
    send_to_char("You conjure up a guardian.\n\r", ch);
    char_to_room(mob, ch->in_room);
    SET_BIT(mob->affected_by, AFF_CHARM);
    add_follower(mob, ch);
    mob->leader = ch;
    mob->master = ch;
}

void spell_continual_light(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *light;

    if (GET_SIDE(ch) != SIDE_UM && GET_SIDE(ch) != SIDE_CAIRN) {
	if (target_name[0] != '\0') {	/* do a glow on some object */
	    light = get_obj_carry(ch, target_name, ch);

	    if (light == NULL) {
		send_to_char("You don't see that here.\n\r", ch);
		return;
	    }
	    if (IS_OBJ_STAT(light, ITEM_GLOW)) {
		act("$p is already glowing.", ch, light, NULL, TO_CHAR);
		return;
	    }
	    SET_BIT(light->extra_flags, ITEM_GLOW);
	    act("$p glows with a white light.", ch, light, NULL, TO_ALL);
	    return;
	}
	light = create_object(get_obj_index(OBJ_VNUM_LIGHT_BALL), 0);
	obj_to_room(light, ch->in_room);
	act("$n twiddles $s thumbs and $p appears.", ch, light, NULL, TO_ROOM);
	act("You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR);
	return;
    } else {

	if (target_name[0] != '\0') {	/* do a glow on some object */
	    light = get_obj_carry(ch, target_name, ch);

	    if (light == NULL) {
		send_to_char("You don't see that here.\n\r", ch);
		return;
	    }
	    if (IS_OBJ_STAT(light, ITEM_UNGLOW)) {
		act("$p is already dark.", ch, light, NULL, TO_CHAR);
		return;
	    }
	    SET_BIT(light->extra_flags, ITEM_UNGLOW);
	    act("$p glows with a black light.", ch, light, NULL, TO_ALL);
	    return;
	}
	light = create_object(get_obj_index(OBJ_VNUM_DARK_BALL), 0);
	obj_to_room(light, ch->in_room);
	act("$n twiddles $s thumbs and $p appears.", ch, light, NULL, TO_ROOM);
	act("You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR);
	return;
    }
}

void spell_control_weather(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
	int chancebetter;
	int chanceminus;
	
	 switch (weather_info.sky){
	     default:
	     chancebetter = 0;
	     chanceminus = 0;
	     break;
	     
	     case SKY_CLOUDLESS:
	     chancebetter = 0;
	     chanceminus = 60;
	     break;
	     
	     case SKY_CLOUDY:
	     chancebetter = 50;
	     chanceminus = 40;
	     break;
	     
	     case SKY_RAINING:
	     chancebetter = 40;
	     chanceminus = 30;
	     break;
	     
	     case SKY_LIGHTNING:
	     chancebetter = 15;
	     chanceminus = 0;
	     break;
     }
	
    if (!str_cmp(target_name, "better")) {
	    if(chancebetter > number_percent())
 		weather_info.sky -= 1;
 		else{
 		send_to_char("You failed to change the weather.\n\r",ch);
 		return;}
}
//	weather_info.change += dice(level / 3, 4);
    else if (!str_cmp(target_name, "worse")){
    	if(chanceminus > number_percent())
    weather_info.sky += 1;
//	weather_info.change -= dice(level / 3, 4);
    	else{
    	send_to_char("You failed to change the weather.\n\r",ch);
    	return;
	}
	}
    else
	send_to_char("Do you want it to get better or worse?\n\r", ch);

    send_to_char("The wind blows sharply for a moment.\n\r", ch);
    return;
    
    
}

void spell_create_food(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *mushroom;

    mushroom = create_object(get_obj_index(OBJ_VNUM_MUSHROOM), 0);
    mushroom->value[0] = level / 2;
    mushroom->value[1] = level;
    obj_to_char(mushroom, ch);
    act("$n has created a scrumptous mushroom.", ch, mushroom, NULL, TO_ROOM);
    send_to_char("A scrumptous mushroom has appeared in your hands.\n\r", ch);
    return;
}

void spell_create_rose(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *rose;
    rose = create_object(get_obj_index(OBJ_VNUM_ROSE), 0);
    act("$n has created a beautiful red rose.", ch, rose, NULL, TO_ROOM);
    send_to_char("You create a beautiful red rose.\n\r", ch);
    obj_to_char(rose, ch);
    return;
}

void spell_create_spring(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *spring;

    spring = create_object(get_obj_index(OBJ_VNUM_SPRING), 0);
    spring->timer = level;
    obj_to_room(spring, ch->in_room);
    act("$p flows from the ground.", ch, spring, NULL, TO_ROOM);
    act("$p flows from the ground.", ch, spring, NULL, TO_CHAR);
    return;
}

void spell_create_water(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int water;

    if (obj->item_type != ITEM_DRINK_CON) {
	send_to_char("It is unable to hold water.\n\r", ch);
	return;
    }
    if (obj->value[2] != LIQ_WATER && obj->value[1] != 0) {
	send_to_char("It contains some other liquid.\n\r", ch);
	return;
    }
    water = UMIN(
		    level * (weather_info.sky >= SKY_RAINING ? 4 : 2),
		    obj->value[0] - obj->value[1]
	);

    if (water > 0) {
	obj->value[2] = LIQ_WATER;
	obj->value[1] += water;
	if (!is_name("water", obj->name)) {
	    char buf[MAX_STRING_LENGTH];

	    sprintf(buf, "%s water", obj->name);
	    free_string(obj->name);
	    obj->name = str_dup(buf);
	}
	act("$p is filled.", ch, obj, NULL, TO_CHAR);
    }
    return;
}

void spell_cure_blindness(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if (!is_affected(victim, gsn_blindness) &&
!is_affected(victim, skill_lookup("fire breath")) &&
!is_affected(victim, gsn_dirt)) 
{
	if (victim == ch)
	    send_to_char("You aren't blind.\n\r", ch);
	else
	    act("$N doesn't appear to be blinded.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (check_dispel(level, victim, gsn_blindness)) 
    {
	send_to_char("Your vision returns!\n\r", victim);
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
    } else
	send_to_char("Spell failed.\n\r", ch);

    if (check_dispel(level, victim, skill_lookup("fire breath")))
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);

    if (check_dispel(level, victim, gsn_dirt))
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);




}

void spell_cure_critical(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    heal = dice(level, DAM_CURECRITICAL);
    
    if (!IS_AFFECTED(victim, AFF_NOQUIT))
	heal *= 2;
    
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) {
	damage_old(ch, victim, heal/2, sn, DAM_OTHER, TRUE);
	act("$n shrieks!", victim, NULL, NULL, TO_ROOM);
	send_to_char("You shriek in pain!!\n\r", victim);
    } else {
	victim->hit = UMIN(victim->hit + heal, victim->max_hit);
	update_pos(victim);
	send_to_char("You feel better!\n\r", victim);
	if (ch != victim)
	    send_to_char("They feel better.\n\r", ch);
    }
    return;
}

/* RT added to cure plague */
void spell_cure_disease(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if (!is_affected(victim, gsn_plague)) {
	if (victim == ch)
	    send_to_char("You aren't ill.\n\r", ch);
	else
	    act("$N doesn't appear to be diseased.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (check_dispel(level, victim, gsn_plague)) {
	send_to_char("Your sores vanish.\n\r", victim);
	act("$n looks relieved as $s sores vanish.", victim, NULL, NULL, TO_ROOM);
    } else
	send_to_char("Spell failed.\n\r", ch);
}

void spell_cure_light(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    heal = dice(level, DAM_CURELIGHT);
    
    if (!IS_AFFECTED(victim, AFF_NOQUIT))
	heal *= 2;
	
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) {
	damage_old(ch, victim, heal/2, sn, DAM_OTHER, TRUE);
	act("$n shrieks!", victim, NULL, NULL, TO_ROOM);
	send_to_char("You shriek in pain!!\n\r", victim);
    } else {
	victim->hit = UMIN(victim->hit + heal, victim->max_hit);
	update_pos(victim);
	send_to_char("You feel better!\n\r", victim);
	if (ch != victim)
	    send_to_char("Ok.\n\r", ch);
    }
    return;
}

void spell_cure_poison(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if (!is_affected(victim, gsn_poison)) {
	if (victim == ch)
	    send_to_char("You aren't poisoned.\n\r", ch);
	else
	    act("$N doesn't appear to be poisoned.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (check_dispel(level, victim, gsn_poison)) {
	send_to_char("A warm feeling runs through your body.\n\r", victim);
	act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
    } else
	send_to_char("Spell failed.\n\r", ch);
}

void spell_cure_serious(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

    heal = dice(level, DAM_CURESERIOUS);
    
    if (!IS_AFFECTED(victim, AFF_NOQUIT))
	heal *= 2;
    
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) {
	damage_old(ch, victim, heal/2, sn, DAM_OTHER, TRUE);
	act("$n shrieks!", victim, NULL, NULL, TO_ROOM);
	send_to_char("You shriek in pain!!\n\r", victim);
    } else {
	victim->hit = UMIN(victim->hit + heal, victim->max_hit);
	update_pos(victim);
	send_to_char("You feel better!\n\r", victim);
	if (ch != victim)
	    send_to_char("Ok.\n\r", ch);
    }
    return;
}

void spell_curse(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    /* deal with the object case first */
    if (target == TARGET_OBJ) {
	obj = (OBJ_DATA *) vo;
	if (IS_OBJ_STAT(obj, ITEM_EVIL)) {
	    act("$p is already filled with evil.", ch, obj, NULL, TO_CHAR);
	    return;
	}
	if (IS_OBJ_STAT(obj, ITEM_BLESS)) {
	    AFFECT_DATA *paf;

	    paf = affect_find(obj->affected, skill_lookup("bless"));
	    if (!saves_dispel(level, paf != NULL ? paf->level : obj->level, 0)) {
		if (paf != NULL)
		    affect_remove_obj(obj, paf);
		act("$p glows with a red aura.", ch, obj, NULL, TO_ALL);
		REMOVE_BIT(obj->extra_flags, ITEM_BLESS);
		return;
	    } else {
		act("The holy aura of $p is too powerful for you to overcome.",
		    ch, obj, NULL, TO_CHAR);
		return;
	    }
	}
	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = 2 * level;
	af.location = APPLY_SAVES;
	af.modifier = +1;
	af.bitvector = ITEM_EVIL;
	affect_to_obj(obj, &af);

	act("$p glows with a malevolent aura.", ch, obj, NULL, TO_ALL);

	if (obj->wear_loc != WEAR_NONE)
	    ch->saving_throw += 1;
	return;
    }
    /* character curses */
    victim = (CHAR_DATA *) vo;

//if (IS_AFFECTED(victim,AFF_CURSE) || saves_spell(level,victim,DAM_NEGATIVE, 0))
    if (saves_spell(level, victim, DAM_NEGATIVE, -10))
	return;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 2 * level;
//    af.location = APPLY_HITROLL;
//    af.modifier = -1 * (level / 16);
    af.bitvector = AFF_CURSE;
//    affect_join(victim, &af);

    af.location = APPLY_SAVING_SPELL;
    af.modifier = level / 8;
    affect_join(victim, &af);

    send_to_char("You feel unclean.\n\r", victim);
    if (ch != victim)
	act("$N looks very uncomfortable.", ch, NULL, victim, TO_CHAR);
    return;
}

/* RT replacement demonfire spell */

void spell_demonfire(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    /*if ( !IS_NPC(ch) && !IS_EVIL(ch) )
       {
       victim = ch;
       send_to_char("The demons turn upon you!\n\r",ch);
       }

       ch->alignment = UMAX(-1000, ch->alignment - 50);
     */

    if (victim != ch) {
	act("$n calls forth the demons of Hell upon $N!",
	    ch, NULL, victim, TO_ROOM);
	act("$n has assailed you with the demons of Hell!",
	    ch, NULL, victim, TO_VICT);
	send_to_char("You conjure forth the demons of hell!\n\r", ch);
    }

//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_DEMONFIRE);
    if (saves_spell(level, victim, DAM_NEGATIVE, 0))
	dam /= 2;
    spell_curse(gsn_curse, level, ch, (void *) victim, TARGET_CHAR);
    damage_old(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);
}

/*
   void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo,int target )
   {
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_DETECT_ALIGNMENT) )
   {
   if (victim == ch)
   send_to_char("You can already sense evil.\n\r",ch);
   else
   act("$N can already detect evil.",ch,NULL,victim,TO_CHAR);
   return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level      = level;
   af.duration  = level;
   af.modifier  = 0;
   af.location  = APPLY_NONE;
   af.bitvector = AFF_DETECT_ALIGNMENT;
   affect_to_char( victim, &af );
   send_to_char( "Your eyes tingle.\n\r", victim );
   if ( ch != victim )
   send_to_char( "Ok.\n\r", ch );
   return;
   }
 */

void spell_detect_good(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_DETECT_ALIGNMENT)) {
	if (victim == ch)
	    send_to_char("You can already sense good.\n\r", ch);
	else
	    act("$N can already detect good.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_DETECT_ALIGNMENT;
    affect_to_char(victim, &af);
    send_to_char("Your eyes tingle.\n\r", victim);
    if (ch != victim)
	send_to_char("Ok.\n\r", ch);
    return;
}

void spell_detect_hidden(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_DETECT_HIDDEN)) {
	if (victim == ch)
	    send_to_char("You are already as alert as you can be. \n\r", ch);
	else
	    act("$N can already sense hidden lifeforms.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char(victim, &af);
    send_to_char("Your awareness improves.\n\r", victim);
    if (ch != victim)
	send_to_char("Ok.\n\r", ch);
    return;
}

void spell_detect_invis(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_DETECT_INVIS)) {
	if (victim == ch)
	    send_to_char("You can already see invisible.\n\r", ch);
	else
	    act("$N can already see invisible things.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char(victim, &af);
    send_to_char("Your eyes tingle.\n\r", victim);
    if (ch != victim)
	send_to_char("Ok.\n\r", ch);
    return;
}

void spell_detect_magic(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_DETECT_MAGIC)) {
	if (victim == ch)
	    send_to_char("You can already sense magical auras.\n\r", ch);
	else
	    act("$N can already detect magic.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char(victim, &af);
    send_to_char("Your eyes tingle.\n\r", victim);
    if (ch != victim)
	send_to_char("Ok.\n\r", ch);
    return;
}

void spell_detect_poison(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if (obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD) {
	if (obj->value[3] != 0)
	    send_to_char("You smell poisonous fumes.\n\r", ch);
	else
	    send_to_char("It looks delicious.\n\r", ch);
    } else {
	send_to_char("It doesn't look poisoned.\n\r", ch);
    }

    return;
}

void spell_dispel_evil(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if (!IS_NPC(ch) && (IS_EVIL(ch) || ch->side == 2))
	victim = ch;

    if (IS_GOOD(victim) || victim->side != 2) {
	act("The gods protects $N.", ch, NULL, victim, TO_ROOM);
	return;
    }
    if (IS_NEUTRAL(victim)) {
	act("$N does not seem to be affected.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim->hit > (ch->level * 4))
	dam = dice(level, 14);
    else
	dam = UMAX(victim->hit, dice(level, 4));
    if (saves_spell(level, victim, DAM_HOLY, 0))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_HOLY, TRUE);
    return;
}

void spell_dispel_good(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if (!IS_NPC(ch) && (IS_GOOD(ch) || ch->side != 2))
	victim = ch;

    if (IS_EVIL(victim) || victim->side == 2) {
	act("$N is protected by $S evil.", ch, NULL, victim, TO_ROOM);
	return;
    }
    if (IS_NEUTRAL(victim)) {
	act("$N does not seem to be affected.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (victim->hit > (ch->level * 4))
	dam = dice(level, 14);
    else
	dam = UMAX(victim->hit, dice(level, 4));
    if (saves_spell(level, victim, DAM_NEGATIVE, 0))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);
    return;
}

/* modified for enhanced use */

void spell_dispel_magic(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    bool found = FALSE;

    if (saves_spell(level, victim, DAM_OTHER, 0)) {
	send_to_char("You feel a brief tingling sensation.\n\r", victim);
	send_to_char("You failed.\n\r", ch);
	return;
    }
    /* begin running through the spells */

    if (check_dispel(level, victim, skill_lookup("armor")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("bless")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("blindness"))) {
	found = TRUE;
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("calm"))) {
	found = TRUE;
	act("$n no longer looks so peaceful...", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("change sex"))) {
	found = TRUE;
	act("$n looks more like $mself again.", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("chill touch"))) {
	found = TRUE;
	act("$n looks warmer.", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("death touch")))
 {
	found = TRUE;
	act("$n no longer looks so white.", victim, NULL, NULL, TO_ROOM);
    }
    if (check_dispel(level, victim, skill_lookup("curse")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect evil")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect good")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect hidden")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("detect invis")))
	found = TRUE;
	
	if (check_dispel(level, victim, skill_lookup("spirit blow")))
	found = TRUE;
	
	if (check_dispel(level, victim, skill_lookup("enhance aura"))){
	act("$n no longer has increased attack power.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
}

    if (check_dispel(level, victim, skill_lookup("faerie fire"))) {
	act("$n's outline fades.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("fly"))) {
	act("$n falls to the ground!", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("frenzy"))) {
	act("$n no longer looks so wild.", victim, NULL, NULL, TO_ROOM);;
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("giant strength"))) {
	act("$n no longer looks so mighty.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("haste"))) {
	act("$n is no longer moving so quickly.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("infravision")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("invis"))) {
	act("$n fades into existance.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("mass invis"))) {
	act("$n fades into existance.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("pass door")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("protection evil")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("protection good")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("sanctuary"))) {
	act("The white aura around $n's body vanishes.",
	    victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("reflect"))) {
	act("The silver aura around $n's body vanishes.",
	    victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (IS_AFFECTED(victim, AFF_SANCTUARY)
	&& !saves_dispel(level, victim->level, -1)
	&& !is_affected(victim, skill_lookup("sanctuary"))) {
	REMOVE_BIT(victim->affected_by, AFF_SANCTUARY);
	act("The white aura around $n's body vanishes.",
	    victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (IS_AFFECTED(victim, AFF_REFLECT)
	&& !saves_dispel(level, victim->level, -1)) {
	REMOVE_BIT(victim->affected_by, AFF_REFLECT);
	act("The silver aura around $n's body vanishes.",
	    victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    
    if (IS_NPC(victim)
    && IS_SET(victim->affected_by, AFF_REFLECT)) 
    {							//prz work izlude reflect
    	REMOVE_BIT(victim->affected_by, AFF_REFLECT);
    	act("The silver aura around $n's body vanishes.",
	    victim, NULL, NULL, TO_ROOM);
		found = TRUE;
	}
    
    if (check_dispel(level, victim, skill_lookup("shield"))) {
	act("The shield protecting $n vanishes.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("sleep")))
	found = TRUE;

    if (check_dispel(level, victim, skill_lookup("slow"))) {
	act("$n is no longer moving so slowly.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("stone skin"))) {
	act("$n's skin regains its normal texture.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("weaken"))) {
	act("$n looks stronger.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("age"))) {
	act("$n looks younger.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (check_dispel(level, victim, skill_lookup("decripify"))) {
	act("$n looks relieved.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
	}
	if (check_dispel(level, victim, skill_lookup("decay aura"))) {
	act("$n decay aura fades.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
	}
    if (check_dispel(level, victim, skill_lookup("youth"))) {
	act("$n looks older.", victim, NULL, NULL, TO_ROOM);
	found = TRUE;
    }
    if (found)
	send_to_char("Ok.\n\r", ch);
    else
	send_to_char("Spell failed.\n\r", ch);
    return;
}

void spell_earthquake(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char("The earth trembles beneath your feet!\n\r", ch);
    act("$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM);

    for (vch = char_list; vch != NULL; vch = vch_next) {
	vch_next = vch->next;
	if (vch->in_room == NULL)
	    continue;
	if (vch->in_room == ch->in_room) {
	    if (vch != ch && !is_safe_spell(ch, vch, TRUE)){
		if (IS_AFFECTED(vch, AFF_FLYING))
		    damage_old(ch, vch, 0, sn, DAM_BASH, TRUE);
		else
		    damage_old(ch, vch, dice(level, DAM_EARTHQUAKE), sn, DAM_BASH, TRUE);
	    continue;
        }
	}
	if (vch->in_room->area == ch->in_room->area)
	    send_to_char("The earth trembles and shivers.\n\r", vch);
    }

    return;
}

void spell_enchant_armor(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    int result, fail;
    int ac_bonus, added;
    bool ac_found = FALSE;

    if (obj->item_type != ITEM_ARMOR) {
	send_to_char("That isn't an armor.\n\r", ch);
	return;
    }
    if (obj->wear_loc != -1) {
	send_to_char("The item must be carried to be enchanted.\n\r", ch);
	return;
    }
    /* this means they have no bonus */
    ac_bonus = 0;
    fail = 25;			/* base 25% chance of failure */

    /* find the bonuses */

    if (!obj->enchanted)
	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
	    if (paf->location == APPLY_AC) {
		ac_bonus = paf->modifier;
		ac_found = TRUE;
		fail += 5 * (ac_bonus * ac_bonus);
	    } else		/* things get a little harder */
		fail += 20;
	}
    for (paf = obj->affected; paf != NULL; paf = paf->next) {
	if (paf->location == APPLY_AC) {
	    ac_bonus = paf->modifier;
	    ac_found = TRUE;
	    fail += 5 * (ac_bonus * ac_bonus);
	} else			/* things get a little harder */
	    fail += 20;
    }

    /* apply other modifiers */
    fail -= level;

    if (IS_OBJ_STAT(obj, ITEM_BLESS))
	fail -= 15;
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
	fail -= 5;

    fail = URANGE(5, fail, 85);

    result = number_percent();

    /* the moment of truth */
    if (result < (fail / 5)) {	/* item destroyed */
	act("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_CHAR);
	act("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_ROOM);
	extract_obj(obj);
	return;
    }
    if (result < (fail / 3)) {	/* item disenchanted */
	AFFECT_DATA *paf_next;

	act("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR);
	act("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM);
	obj->enchanted = TRUE;

	/* remove all affects */
	for (paf = obj->affected; paf != NULL; paf = paf_next) {
	    paf_next = paf->next;
	    free_affect(paf);
	}
	obj->affected = NULL;

	/* clear all flags */
	obj->extra_flags = 0;
	return;
    }
    if (result <= fail) {	/* failed, no bad result */
	send_to_char("Nothing seemed to happen.\n\r", ch);
	return;
    }
    /* okay, move all the old flags into new vectors if we have to */
    if (!obj->enchanted) {
	AFFECT_DATA *af_new;
	obj->enchanted = TRUE;

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
	    af_new = new_affect();

	    af_new->next = obj->affected;
	    obj->affected = af_new;

	    af_new->where = paf->where;
	    af_new->type = UMAX(0, paf->type);
	    af_new->level = paf->level;
	    af_new->duration = paf->duration;
	    af_new->location = paf->location;
	    af_new->modifier = paf->modifier;
	    af_new->bitvector = paf->bitvector;
	}
    }
    if (result <= (90 - level / 5)) {	/* success! */
	act("$p shimmers with a gold aura.", ch, obj, NULL, TO_CHAR);
	act("$p shimmers with a gold aura.", ch, obj, NULL, TO_ROOM);
	SET_BIT(obj->extra_flags, ITEM_MAGIC);
	added = -1;
    } else {			/* exceptional enchant */
	act("$p glows a brillant gold!", ch, obj, NULL, TO_CHAR);
	act("$p glows a brillant gold!", ch, obj, NULL, TO_ROOM);
	SET_BIT(obj->extra_flags, ITEM_MAGIC);
	SET_BIT(obj->extra_flags, ITEM_GLOW);
	added = -2;
    }

    /* now add the enchantments */

    if (obj->level < LEVEL_HERO)
	obj->level = UMIN(LEVEL_HERO - 1, obj->level + 1);

    if (ac_found) {
	for (paf = obj->affected; paf != NULL; paf = paf->next) {
	    if (paf->location == APPLY_AC) {
		paf->type = sn;
		paf->modifier += added;
		paf->level = UMAX(paf->level, level);
	    }
	}
    } else {			/* add a new affect */
	paf = new_affect();

	paf->where = TO_OBJECT;
	paf->type = sn;
	paf->level = level;
	paf->duration = -1;
	paf->location = APPLY_AC;
	paf->modifier = added;
	paf->bitvector = 0;
	paf->next = obj->affected;
	obj->affected = paf;
    }

}

void spell_enchant_weapon(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;
    int result, fail;
    int hit_bonus, dam_bonus, added;
    bool hit_found = FALSE, dam_found = FALSE;

    if (obj->item_type != ITEM_WEAPON) {
	send_to_char("That isn't a weapon.\n\r", ch);
	return;
    }
    if (obj->wear_loc != -1) {
	send_to_char("The item must be carried to be enchanted.\n\r", ch);
	return;
    }
    /* this means they have no bonus */
    hit_bonus = 0;
    dam_bonus = 0;
    fail = 25;			/* base 25% chance of failure */

    /* find the bonuses */

    if (!obj->enchanted)
	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
	    if (paf->location == APPLY_HITROLL) {
		hit_bonus = paf->modifier;
		hit_found = TRUE;
		fail += 2 * (hit_bonus * hit_bonus);
	    } else if (paf->location == APPLY_DAMROLL) {
		dam_bonus = paf->modifier;
		dam_found = TRUE;
		fail += 2 * (dam_bonus * dam_bonus);
	    } else		/* things get a little harder */
		fail += 25;
	}
    for (paf = obj->affected; paf != NULL; paf = paf->next) {
	if (paf->location == APPLY_HITROLL) {
	    hit_bonus = paf->modifier;
	    hit_found = TRUE;
	    fail += 2 * (hit_bonus * hit_bonus);
	} else if (paf->location == APPLY_DAMROLL) {
	    dam_bonus = paf->modifier;
	    dam_found = TRUE;
	    fail += 2 * (dam_bonus * dam_bonus);
	} else			/* things get a little harder */
	    fail += 25;
    }

    /* apply other modifiers */
    fail -= 3 * level / 2;

    if (IS_OBJ_STAT(obj, ITEM_BLESS))
	fail -= 15;
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
	fail -= 5;

    fail = URANGE(5, fail, 95);

    result = number_percent();

    /* the moment of truth */
    if (result < (fail / 5)) {	/* item destroyed */
	act("$p shivers violently and explodes!", ch, obj, NULL, TO_CHAR);
	act("$p shivers violently and explodeds!", ch, obj, NULL, TO_ROOM);
	extract_obj(obj);
	return;
    }
    if (result < (fail / 2)) {	/* item disenchanted */
	AFFECT_DATA *paf_next;

	act("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR);
	act("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM);
	obj->enchanted = TRUE;

	/* remove all affects */
	for (paf = obj->affected; paf != NULL; paf = paf_next) {
	    paf_next = paf->next;
	    free_affect(paf);
	}
	obj->affected = NULL;

	/* clear all flags */
	obj->extra_flags = 0;
	return;
    }
    if (result <= fail) {	/* failed, no bad result */
	send_to_char("Nothing seemed to happen.\n\r", ch);
	return;
    }
    /* okay, move all the old flags into new vectors if we have to */
    if (!obj->enchanted) {
	AFFECT_DATA *af_new;
	obj->enchanted = TRUE;

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
	    af_new = new_affect();

	    af_new->next = obj->affected;
	    obj->affected = af_new;

	    af_new->where = paf->where;
	    af_new->type = UMAX(0, paf->type);
	    af_new->level = paf->level;
	    af_new->duration = paf->duration;
	    af_new->location = paf->location;
	    af_new->modifier = paf->modifier;
	    af_new->bitvector = paf->bitvector;
	}
    }
    if (result <= (100 - level / 5)) {	/* success! */
	act("$p glows blue.", ch, obj, NULL, TO_CHAR);
	act("$p glows blue.", ch, obj, NULL, TO_ROOM);
	SET_BIT(obj->extra_flags, ITEM_MAGIC);
	added = 1;
    } else {			/* exceptional enchant */
	act("$p glows a brillant blue!", ch, obj, NULL, TO_CHAR);
	act("$p glows a brillant blue!", ch, obj, NULL, TO_ROOM);
	SET_BIT(obj->extra_flags, ITEM_MAGIC);
	SET_BIT(obj->extra_flags, ITEM_GLOW);
	added = 2;
    }

    /* now add the enchantments */

    if (obj->level < LEVEL_HERO - 1)
	obj->level = UMIN(LEVEL_HERO - 1, obj->level + 1);

    if (dam_found) {
	for (paf = obj->affected; paf != NULL; paf = paf->next) {
	    if (paf->location == APPLY_DAMROLL) {
		paf->type = sn;
		paf->modifier += added;
		paf->level = UMAX(paf->level, level);
		if (paf->modifier > 4)
		    SET_BIT(obj->extra_flags, ITEM_HUM);
	    }
	}
    } else {			/* add a new affect */
	paf = new_affect();

	paf->where = TO_OBJECT;
	paf->type = sn;
	paf->level = level;
	paf->duration = -1;
	paf->location = APPLY_DAMROLL;
	paf->modifier = added;
	paf->bitvector = 0;
	paf->next = obj->affected;
	obj->affected = paf;
    }

    if (hit_found) {
	for (paf = obj->affected; paf != NULL; paf = paf->next) {
	    if (paf->location == APPLY_HITROLL) {
		paf->type = sn;
		paf->modifier += added;
		paf->level = UMAX(paf->level, level);
		if (paf->modifier > 4)
		    SET_BIT(obj->extra_flags, ITEM_HUM);
	    }
	}
    } else {			/* add a new affect */
	paf = new_affect();

	paf->type = sn;
	paf->level = level;
	paf->duration = -1;
	paf->location = APPLY_HITROLL;
	paf->modifier = added;
	paf->bitvector = 0;
	paf->next = obj->affected;
	obj->affected = paf;
    }

}

void spell_imbue(int sn,int level,CHAR_DATA *ch, void *vo,int target)
    {
    OBJ_DATA *obj = get_eq_char(ch, WEAR_WIELD);
    AFFECT_DATA af;
    char *objOrigDesc;
    char *affFlag;
    char objNewDesc[MAX_STRING_LENGTH];
    char objBackup[MAX_STRING_LENGTH];
    int result, fail;
    int type;
    if ((obj == NULL) || (obj->item_type != ITEM_WEAPON)){
		send_to_char("You must be wielding a weapon to imbue it.\n\r",ch);
		return;
    }

    type = 0;
    objOrigDesc = obj->description;
    affFlag = NULL;
    
    if (!str_cmp( target_name, "fire")){
		type = 1;
		if (IS_WEAPON_STAT(obj,WEAPON_FLAMING)){
			send_to_char("The weapon is already filled with the power of fire.\n\r",ch);
			return;
		}
	} 

	
		else if (!str_cmp( target_name, "ice")){
			type = 2;
			if (IS_WEAPON_STAT(obj,WEAPON_FROST)){
				send_to_char("The weapon is already filled with the power of ice.\n\r",ch);
				return;
			}
		} 
	

		else if (!str_cmp( target_name, "lightning")){
				type = 3;
				if (IS_WEAPON_STAT(obj,WEAPON_SHOCKING)){
					send_to_char("The weapon is already filled with the power of lightning.\n\r",ch);
					return;
				}
			}
		 
			
		else if (!str_cmp( target_name, "drain")){
					type = 4;
					if (IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC)){
						send_to_char("The weapon is already filled with the power of drain.\n\r",ch);
						return;
					}
				}
			
			else if (!str_cmp( target_name, "leech")){
					type = 5;
					if (IS_WEAPON_STAT(obj,WEAPON_VORPAL)){
						send_to_char("The weapon is already filled with the power of drain.\n\r",ch);
						return;
					}
				}
				
			else if (!str_cmp( target_name, "poison")){
					type = 6;
					if (IS_WEAPON_STAT(obj,WEAPON_POISON)){
						send_to_char("The weapon is already filled with the power of poison.\n\r",ch);
						return;
					}
				}
			
		
/*	}*/

	if (type == 0){
		send_to_char("Imbue it with {RFire{x, {CIce{X, {YLightning{x, {DDrain{x, {cLeech{X, {gPoison{X? \n\r",ch);
		return;
	}

	/*Set our fails*/

	fail = 30; /*default 30% fail rate*/

	if (IS_WEAPON_STAT(obj,WEAPON_FLAMING))
		fail += 25;
	if (IS_WEAPON_STAT(obj,WEAPON_FROST))
		fail += 25;
	if (IS_WEAPON_STAT(obj,WEAPON_SHOCKING))
		fail += 25;
	if (IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC))
		fail += 25;
	if (IS_WEAPON_STAT(obj,WEAPON_VORPAL))
		fail += 15;
	if (IS_WEAPON_STAT(obj,WEAPON_POISON))
		fail += 28;
	if (IS_WEAPON_STAT(obj,WEAPON_SHARP))
		fail += 40;
	if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
		fail -= 20;

    fail -= level/3;

    if (IS_OBJ_STAT(obj,ITEM_BLESS))
		fail -= 10;

    fail = URANGE(5,fail,95);

    result = number_percent();

    if (result < (fail / 2)){
	act("$p gives way to the magic and disintegrates!",ch,obj,NULL,TO_CHAR);
	act("$p gives way to the magic and disintegrates!",ch,obj,NULL,TO_ROOM);
    	extract_obj(obj);
	return;
    }

    if ( result <= fail ){
	act("The energy passed right through $p!",ch,obj,NULL,TO_CHAR);
	return;
    }

    af.where     = TO_WEAPON;
    af.type      = sn;
    af.level     = level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;

    switch(type){
	case 1: af.bitvector = WEAPON_FLAMING;
        	act("$p glows a gentle red.",ch,obj,NULL,TO_CHAR);
		act("$p glows a gentle red.",ch,obj,NULL,TO_ROOM);
                affFlag = "({RF{x)";
	        break;
	case 2: af.bitvector = WEAPON_FROST;
		act("$p shimmers a light blue.",ch,obj,NULL,TO_CHAR);
		act("$p shimmers a light blue.",ch,obj,NULL,TO_ROOM);
                affFlag = "({CI{x)";
		break;
	case 3: af.bitvector = WEAPON_SHOCKING;
		act("$p lets out a sharp crackle.",ch,obj,NULL,TO_CHAR);
		act("$p lets out a sharp crackle.",ch,obj,NULL,TO_ROOM);
                affFlag = "({YL{x)";
		break;
	case 4: af.bitvector = WEAPON_VAMPIRIC;
        	act("$p appears to emit an evil aura.",ch,obj,NULL,TO_CHAR);
		act("$p appears to emit an evil aura.",ch,obj,NULL,TO_ROOM);
                affFlag = "({DD{x)";
                break;
 	
    case 5: af.bitvector = WEAPON_VORPAL;
    act("$p appears to emit an evil aura.",ch,obj,NULL,TO_CHAR);
	act("$p appears to emit an evil aura.",ch,obj,NULL,TO_ROOM);
    affFlag = "({DD{x)";
                break;   
                
    case 6: af.bitvector = WEAPON_POISON;
    act("$p turns sickly and pale.",ch,obj,NULL,TO_CHAR);
	act("$p turns sickly and pale.",ch,obj,NULL,TO_ROOM);
    affFlag = "({cL{x)";
                break;         
    default:
 	 break;
    }
    strcpy(objBackup, objOrigDesc);
    strcpy(objNewDesc, affFlag);
    strcat(objNewDesc, objBackup);
    free_string( obj->description );
    obj->description = str_dup( objNewDesc );
    affect_to_obj(obj,&af);
}

void spell_alchemy(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{

    //the selection,   least to most powerful outcomes in order

    // use of slot 6 indicates potion has special name

    long int selection[][6] =
    {
	{1, -1, -1, -1, -1},	//the almightly potion of donothing
	 {1, 513, -1, -1, -1,},	// "detect alignment"
	 {1, 511, -1, -1, -1},	//create rose
	 {1, 80, -1, -1, -1},	//create spring
	 {1, 12, -1, -1, -1},	//  create food
	 {1, 16, -1, -1, -1},	//  cure light
	 {1, 16, -1, -1, -1},	//  cure light
	 {1, 20, -1, -1, -1},	//  detect magic
	 {1, 43, -1, -1, -1},	// cure poison 
	 {1, 81, -1, -1, -1},	// refresh 
	 {1, 57, -1, -1, -1},	// continual light 
	 {1, 77, -1, -1, -1},	// infravision 
	 {1, 56, -1, -1, -1},	// fly 
	 {1, 1, -1, -1, -1},		// armor
	 {1, 3, -1, -1, -1},		// bless
	 {1, 39, -1, -1, -1},	// giant strength
	 {1, 514, -1, -1, -1},	// protection good
	 {1, 34, -1, -1, -1},	// protection evil
	 {1, 35, -1, -1, -1},	// remove curse
	 {1, 35, -1, -1, -1},	// detect invisibility
	 {1, 301, -1, -1, -1},	// servant
	 {1, 14, -1, -1, -1},	// cure blindness
	 {1, 507, -1, -1, -1},	// cancellation
	 {1, 501, -1, -1, -1},	// cure disease
	 {1, 61, -1, -1, -1},	// cure serious
	 {1, 74, -1, -1, -1},	// pass door
	 {1, 304, -1, -1, -1},	// mount
	 {1, 73, -1, -1, -1},	// faerie fog
	 {1, 29, -1, -1, -1},	// invisibility
	 {1, 35, -1, -1, -1},	// cure critical
	 {1, 2, -1, -1, -1},		// teleport
	 {1, 67, -1, -1, -1},	// shield
	 {1, 504, -1, -1, -1},	// frenzy
	 {1, 61, -1, -1, -1},	// freedom
	 {1, 504, -1, -1, -1},	// decoy
	 {1, 61, -1, -1, -1},	// heal
	 {1, 521, -1, -1, -1},	// farsight
	 {1, 46, -1, -1, -1},	// haven
	 {1, 61, -1, -1, -1},	// detect hidden
	 {1, 69, -1, -1, -1},	// mass invisibility
	 {1, 502, -1, -1, -1},	// haste
	 {1, 61, -1, -1, -1},	// sanctuary
	 {1, 61, -1, -1, -1},	// divinity
	 {1, 66, -1, -1, -1},	// stone skin
	 {1, 66, -1, -1, -1},	// mass healing
	 {1, 504, -1, -1, -1},	// mota
	 {1, 14, -1, -1, -1},	// prism
	 {1, 14, -1, -1, -1},	// reflect

    };
    char unique_name[][80] = {
	{""},
	{"freedom"},
	{"divinity"},
	{"mota"},
	{"mirrors"}
};

    OBJ_DATA *potion, *ingredient;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];


    //configurations, update these when editing
    long int maxLevel = 58;	//   the highest level this cares about, higher lvs will cast at this

    long int skillLvMax = 100;	//highest you can have it practiced to

    long int maxIng = 5;	//highest ingredient

    long int possibilities = sizeof(selection) / sizeof(selection[1]);
    long int ing;		
    long int maxRoll;
    long int minRoll;
    long int chMaxRoll;
    long int chMinRoll = 0;
    long int chSkill = ch->pcdata->learned[sn];
    long int chRange;
    long int quad;
    long int result;
    level = UMIN(level, maxLevel);

//check ingredient
ingredient = get_eq_char(ch, WEAR_HOLD);
    if ( (ingredient == NULL || ingredient->item_type != ITEM_INGREDIENT)) 
    {	send_to_char("You must hold the ingredient you will use.\n\r", ch);
	return;
    }
ing = ingredient->value[0];

if(ingredient->value[1] > 0) ingredient->value[1]--;
if(ingredient->value[1] == 0) 
{
	act("$p vanishes.", ch, ingredient, NULL, TO_CHAR);
	extract_obj(ingredient);

}

    //the possible range
    maxRoll = possibilities * maxLevel * skillLvMax * maxIng;
    minRoll = 1;

    //the characters range
    chMaxRoll = possibilities * level * chSkill * ing;
    chMinRoll = chSkill * ing;
    chRange = chMaxRoll - chMinRoll;

    //the bellCurve
    result = chRange / 4;
    quad = number_range(1, 100);

    if (quad <= 20)		//poor result

	result = 0;
    else if (quad <= 40)	//average result

	result = chRange * 1 / 4;
    else if (quad <= 80)	//good result

	result = chRange * 1 / 2;
    else			//incredible result

	result = chRange * 3 / 4;
    result = number_range(result, result + chRange / 4);

    //convert result to match the possibilities;
    // result = x / ( z / y )
    result = result / (maxRoll / possibilities);

    if (result < 1)
	result = 1;
    if ( result > possibilities)
	result = 1;

    //make the badboy       
    potion = create_object(get_obj_index(OBJ_VNUM_POTION), 0);
    potion->value[0] = level / selection[result][0];
    potion->value[1] = slot_lookup(selection[result][1]);
    potion->value[2] = slot_lookup(selection[result][2]);
    potion->value[3] = slot_lookup(selection[result][3]);
    potion->value[4] = slot_lookup(selection[result][4]);
    obj_to_char(potion, ch);

    free_string(potion->short_descr);
    free_string(potion->name);

if(!selection[result][5])
{
    sprintf(buf, "a potion of %s", skill_table[potion->value[1]].name);
    sprintf(buf2, "potion %s", skill_table[potion->value[1]].name);
}
else
{
    sprintf(buf, "a potion of %s", unique_name[selection[result][5]]);
    sprintf(buf2, "potion %s", unique_name[selection[result][5]]);
}
    potion->short_descr = str_dup(buf);
    potion->name = str_dup(buf2);

    send_to_char("A potion has appeared in your hands.\n\r", ch);

    //all that other stuff
    act("$n has created a potion.", ch, potion, NULL, TO_ROOM);
    return;

}

void spell_scribery(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *scroll, *parchment;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

    long int selection[][6] =
    {
	{1, -1, -1, -1, -1},	//the almightly scroll of donothing
	 {1, 82, -1, -1, -1},	// change sex
	 {1, 8, -1, -1, -1},		// chill touch
	 {1, 33, -1, -1, -1},	// poison
	 {1, 64, -1, -1, -1},	// cause serious
	 {1, 515, -1, -1, -1},	// slow
	 {1, 521, -1, -1, -1},	// call animal
	 {1, 17, -1, -1, -1},	// curse
	 {1, 5, -1, -1, -1},		// burning hands
	 {1, 7, -1, -1, -1},		// charm person
	 {1, 72, -1, -1, -1},	// faerie fire
	 {1, 62, -1, -1, -1},	// cause light
	 {1, 54, -1, -1, -1},	// shocking grasp
	 {1, 26, -1, -1, -1},	// fireball
	 {1, 87, -1, -1, -1},	// nullify
	 {1, 30, -1, -1, -1},	// lightning bolt
	 {1, 49, -1, -1, -1},	// minion
	 {1, 4, -1, -1, -1},		// blindness
	 {1, 45, -1, -1, -1},	// death coil
	 {1, 10, -1, -1, -1},	// colour spray
	 {1, 27, -1, -1, -1},	// harm
	 {1, 65, -1, -1, -1},	// flamestrike
	 {1, 50, -1, -1, -1},	// soul coil
	 {1, 518, -1, -1, -1},	// ray of truth
	 {1, 6, -1, -1, -1},		// call lightning
	 {1, 63, -1, -1, -1},	// cause critical
	 {1, 505, -1, -1, -1},	// demonfire
	 {1, 202, -1, -1, -1},	// frost breath
	 {1, 203, -1, -1, -1},	// gas breath
	 {1, 700, -1, -1, -1},	// smite
	 {1, 555, -1, -1, -1},	// agony
	 {1, 48, -1, -1, -1},	// death touch
	 {1, 70, -1, -1, -1},	// acid blast
	 {1, 200, -1, -1, -1},	// acid breath
	 {1, 500, -1, -1, -1},	// chain lightning
	 {1, 25, -1, -1, -1},	// energy drain
	 {1, 59, -1, -1, -1},	// dispel magic
	 {1, 201, -1, -1, -1},	// fire breath
	 {1, 312, -1, -1, -1},	// firestorm
	 {1, 204, -1, -1, -1},	// lightning breath
	 {1, 300, -1, -1, -1},	// web
	 {1, 310, -1, -1, -1},	// entangle
	 {1, 506, -1, -1, -1},	// holy word
	 {1, 47, -1, -1, -1},	// unholy word
//	 {1, 555, 50, 204, 312, 1},	// pain
//	 {1, 300, 310, 300, 310, 2},	// ouch
    };

    char unique_name[][80] = {
	{""},
	{"pain"},
	{"ouch"},
};


    //configurations, update these when editing
    long int maxLevel = 58;	//   the highest level this cares about, higher lvs will cast at this

    long int skillLvMax = 100;	//highest you can have it practiced to

    long int maxIng = 10;	//highest ingredient

    long int possibilities = sizeof(selection) / sizeof(selection[1]);
    long int ing;

    long int maxRoll;
    long int minRoll;
    long int chMaxRoll;
    long int chMinRoll = 0;
    long int chSkill = ch->pcdata->learned[sn];
    long int chRange;
    long int quad;
    long int result;
    level = UMIN(level, maxLevel);


//check ingredient
parchment = get_eq_char(ch, WEAR_HOLD);
    if ( (parchment == NULL || parchment->item_type != ITEM_PARCHMENT)) 
    {	send_to_char("You must hold the parchment you will use.\n\r", ch);
	return;
    }
ing = parchment->value[0];

if(parchment->value[1] > 0) parchment->value[1]--;
if(parchment->value[1] == 0) 
{
	act("$p vanishes.", ch, parchment, NULL, TO_CHAR);
	extract_obj(parchment);

}


    //the possible range
    maxRoll = possibilities * maxLevel * skillLvMax * maxIng;
    minRoll = 1;

    //the characters range
    chMaxRoll = possibilities * level * chSkill * ing;
    chMinRoll = chSkill * ing;
    chRange = chMaxRoll - chMinRoll;

    //the bellCurve
    result = chRange / 4;
    quad = number_range(1, 100);

    if (quad <= 20)		//poor result

	result = 0;
    else if (quad <= 70)	//average result

	result = chRange * 1 / 4;
    else if (quad <= 90)	//great result

	result = chRange * 1 / 2;
    else			//incredible result

	result = chRange * 3 / 4;
    result = number_range(result, result + chRange / 4);

    //convert result to match the possibilities;
    // result = x / ( z / y )
    result = result / (maxRoll / possibilities);

if (result < 1)
	result = 1;
    if ( result > possibilities)
	result = 1;

    //make the badboy       
    scroll = create_object(get_obj_index(OBJ_VNUM_SCROLL), 0);
    scroll->value[0] = level / selection[result][0];
    scroll->value[1] = slot_lookup(selection[result][1]);
    scroll->value[2] = slot_lookup(selection[result][2]);
    scroll->value[3] = slot_lookup(selection[result][3]);
    scroll->value[4] = slot_lookup(selection[result][4]);

    obj_to_char(scroll, ch);
    free_string(scroll->short_descr);
    free_string(scroll->name);


if(!selection[result][5])
{
    sprintf(buf, "a scroll of %s", skill_table[scroll->value[1]].name);
    sprintf(buf2, "scroll %s", skill_table[scroll->value[1]].name);
}
else
{
    sprintf(buf, "a scroll of %s", unique_name[selection[result][5]]);
    sprintf(buf2, "scroll %s", unique_name[selection[result][5]]);
}

    scroll->short_descr = str_dup(buf);
    scroll->name = str_dup(buf2);

    //all that other stuff
    act("$n has created a scroll.", ch, scroll, NULL, TO_ROOM);
    send_to_char("A scroll has appeared in your hands.\n\r", ch);
    return;

}

/*
 * Drain MANA
 * 
 */
void spell_energy_drain(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
/*
   if (victim != ch)
   ch->alignment = UMAX(-1000, ch->alignment - 50);
 */

    if(ch == victim)
	return;
    if (saves_spell(level, victim, DAM_NEGATIVE, SAVE_NORMAL)) {
	send_to_char("You feel a momentary chill.\n\r", victim);
	return;
    }
    if (victim->level <= 2) {
	dam = ch->hit + 1;
    } else {
	/*gain_exp( victim, 0 - number_range( level/2, 3 * level / 2 ) );
	 */
	victim->mana /= 2;
	victim->move -= 65;
	dam = dice(1, level);
	ch->hit += dam;
    }

    send_to_char("You feel your life slipping away!\n\r", victim);
    send_to_char("Wow....what a rush!\n\r", ch);
    damage_old(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);

    return;
}

void spell_fireball(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_FIREBALL);

    if (saves_spell(level, victim, DAM_FIRE, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_FIRE, TRUE);
    return;
}

void spell_fireproof(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA af;

    if (IS_OBJ_STAT(obj, ITEM_BURN_PROOF)) {
	act("$p is already protected from burning.", ch, obj, NULL, TO_CHAR);
	return;
    }
    af.where = TO_OBJECT;
    af.type = sn;
    af.level = level;
    af.duration = -1;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = ITEM_BURN_PROOF;

    affect_to_obj(obj, &af);

    act("You protect $p from fire.", ch, obj, NULL, TO_CHAR);
    act("$p is surrounded by a protective aura.", ch, obj, NULL, TO_ROOM);
}

void spell_flamestrike(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_FLAMESTRIKE);
    if (saves_spell(level, victim, DAM_FIRE, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_FIRE, TRUE);
    return;
}

void spell_faerie_fire(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
	return;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = 2 * level;
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char(victim, &af);
    send_to_char("You are surrounded by a pink outline.\n\r", victim);
    act("$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_faerie_fog(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *ich;

    act("$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You conjure a cloud of purple smoke.\n\r", ch);

    for (ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room) {
	if (ich->invis_level > 0)
	    continue;

	if (ich == ch || saves_spell(level, ich, DAM_OTHER, 0))
	    continue;

	affect_strip(ich, gsn_invis);
	affect_strip(ich, gsn_mass_invis);
	affect_strip(ich, gsn_sneak);
	REMOVE_BIT(ich->affected_by, AFF_HIDE);
	REMOVE_BIT(ich->affected_by, AFF_INVISIBLE);
	REMOVE_BIT(ich->affected_by, AFF_SNEAK);
	act("$n is revealed!", ich, NULL, NULL, TO_ROOM);
	send_to_char("You are revealed!\n\r", ich);
    }

    return;
}

void spell_floating_disc(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *disc, *floating;

    floating = get_eq_char(ch, WEAR_FLOAT);
    if (floating != NULL && IS_OBJ_STAT(floating, ITEM_NOREMOVE)) {
	act("You can't remove $p.", ch, floating, NULL, TO_CHAR);
	return;
    }
    disc = create_object(get_obj_index(OBJ_VNUM_DISC), 0);
    disc->value[0] = ch->level * 10;	/* 10 pounds per level capacity */
    disc->value[3] = ch->level * 5;	/* 5 pounds per level max per item */
    disc->timer = ch->level * 2 - number_range(0, level / 2);

    act("$n has created a floating black disc.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You create a floating disc.\n\r", ch);
    obj_to_char(disc, ch);
    wear_obj(ch, disc, TRUE);
    return;
}

void spell_fly(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_FLYING)) {
	if (victim == ch)
	    send_to_char("You are already airborne.\n\r", ch);
	else
	    act("$N doesn't need your help to fly.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level + 3;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char(victim, &af);
    send_to_char("Your feet rise off the ground.\n\r", victim);
    act("$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM);
    return;
}

/* RT clerical berserking spell */

void spell_frenzy(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn) || IS_AFFECTED(victim, AFF_BERSERK)) {
	if (victim == ch)
	    send_to_char("You are already in a frenzy.\n\r", ch);
	else
	    act("$N is already in a frenzy.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (is_affected(victim, skill_lookup("calm"))) {
	if (victim == ch)
	    send_to_char("Why don't you just relax for a while?\n\r", ch);
	else
	    act("$N doesn't look like $e wants to fight anymore.",
		ch, NULL, victim, TO_CHAR);
	return;
    }

	if (!IS_IMMORTAL(ch)) { if (ch->side != victim->side ) 
		{ act("Your god	doesn't seem to like $N", ch, NULL, victim, TO_CHAR); 
	return; } }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 3;
    af.modifier = level / 3;
    af.bitvector = 0;

    af.location = APPLY_HITROLL;
    affect_to_char(victim, &af);

    af.location = APPLY_DAMROLL;
    affect_to_char(victim, &af);

    af.modifier = 10 * (level / 12);
    af.location = APPLY_AC;
    affect_to_char(victim, &af);

    send_to_char("You are filled with holy wrath!\n\r", victim);
    act("$n gets a wild look in $s eyes!", victim, NULL, NULL, TO_ROOM);
}

/* RT ROM-style gate */

void spell_gate(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    bool gate_pet;

    if ((victim = get_char_world(ch, target_name)) == NULL
	|| victim == ch
	|| victim->in_room == NULL
	|| !can_see_room(ch, victim->in_room)
	|| IS_AFFECTED(victim, AFF_NOQUIT)
	|| IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	|| IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	|| IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(ch->in_room->room_flags, ROOM_NEWBIES_ONLY)
	|| victim->level >= level + 5
	|| (!IS_NPC(victim) && victim->level >= LEVEL_HERO)	/* NOT trust */
	||(IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	|| (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOSUMMON))
	|| (IS_NPC(victim) && saves_spell(level, victim, DAM_OTHER, 0))
	|| (!IS_NPC(victim) && (GET_SIDE(ch) != GET_SIDE(victim)))
	) {
	send_to_char("You failed.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_NOQUIT)) {
	send_to_char("Can't do that now!", ch);
	return;
    }
    if (IS_NPC(victim)) {
	send_to_char("You can only gate to players in your kingdom.\n\r", ch);
	return;
    }
    if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
	gate_pet = TRUE;
    else
	gate_pet = FALSE;

    act("$n steps through a gate and vanishes.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You step through a gate and vanish.\n\r", ch);
    char_from_room(ch);
    char_to_room(ch, victim->in_room);

    act("$n has arrived through a gate.", ch, NULL, NULL, TO_ROOM);
    do_look(ch, "auto");

    if (gate_pet) {
	act("$n steps through a gate and vanishes.", ch->pet, NULL, NULL, TO_ROOM);
	send_to_char("You step through a gate and vanish.\n\r", ch->pet);
	char_from_room(ch->pet);
	char_to_room(ch->pet, victim->in_room);
	act("$n has arrived through a gate.", ch->pet, NULL, NULL, TO_ROOM);
	do_look(ch->pet, "auto");
    }
    if (MOUNTED(ch)) {
	act("$n steps through a gate and vanishes.", MOUNTED(ch), NULL, NULL, TO_ROOM);
	send_to_char("You step through a gate and vanish.\n\r", MOUNTED(ch));
	char_from_room(MOUNTED(ch));
	char_to_room(MOUNTED(ch), victim->in_room);
	act("$n has arrived through a gate.", MOUNTED(ch), NULL, NULL, TO_ROOM);
	do_look(MOUNTED(ch), "auto");
    }
}

void spell_giant_strength(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn)) {
	if (victim == ch)
	    send_to_char("You are already as strong as you can get!\n\r", ch);
	else
	    act("$N can't get any stronger.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_STR;
    af.modifier = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("Your muscles surge with heightened power!\n\r", victim);
    act("$n's muscles surge with heightened power.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_harm(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int antiheal;
    int dam;
    antiheal = dice(level, DAM_HARM);
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) 
	{
		victim->hit = UMIN(victim->hit + antiheal, victim->max_hit);
		act("$n grins!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You feel better!!\n\r", victim);
	} 
	else 
	{
	dam = dice(level, DAM_HARMSPELL);
      if (saves_spell(level, victim, DAM_HARM, 0))
		dam = dam / 2;
       damage_old(ch, victim, dam, sn, DAM_HARM, TRUE);
	}

    return;
	}


/* RT haste spell */

void spell_haste(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn) || IS_AFFECTED(victim, AFF_HASTE)
	|| IS_SET(victim->off_flags, OFF_FAST)) {
	if (victim == ch)
	    send_to_char("You can't move any faster!\n\r", ch);
	else
	    act("$N is already moving as fast as $E can.",
		ch, NULL, victim, TO_CHAR);
	return;
    }
    if (IS_AFFECTED(victim, AFF_SLOW)) {
	if (!check_dispel(level, victim, skill_lookup("slow"))) {
	    if (victim != ch)
		send_to_char("Spell failed.\n\r", ch);
	    send_to_char("You feel momentarily faster.\n\r", victim);
	    return;
	}
	act("$n is moving less slowly.", victim, NULL, NULL, TO_ROOM);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    if (victim == ch)
	af.duration = level / 2;
    else
	af.duration = level / 4;
    af.location = APPLY_DEX;
    af.modifier = 1 + (level >= 18) + (level >= 25) + (level >= 32);
    af.bitvector = AFF_HASTE;
    affect_to_char(victim, &af);
    send_to_char("You feel yourself moving more quickly.\n\r", victim);
    act("$n is moving more quickly.", victim, NULL, NULL, TO_ROOM);
    if (ch != victim)
	send_to_char("Ok.\n\r", ch);
    return;
}

void spell_heal(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;


    heal = dice(level, DAM_HEAL);
    
    if (!IS_AFFECTED(victim, AFF_NOQUIT))
	heal *= 2;
    
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) {
	damage_old(ch, victim, heal/2, sn, DAM_OTHER, TRUE);
	act("$n shrieks!", victim, NULL, NULL, TO_ROOM);
	send_to_char("You shriek in pain!!\n\r", victim);
    } else {
	victim->hit = UMIN(victim->hit + heal, victim->max_hit);
	update_pos(victim);
	send_to_char("A warm feeling fills your body.\n\r", victim);
	if (ch != victim)
	    send_to_char("A warm feeling fills their body.\n\r", ch);
    }

    return;
}
void spell_prism(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;

//    heal = (victim->max_hit * 35) / 100;
    heal = (victim->max_hit - 50);
    if (IS_AFFECTED2(victim, AFF_UNDEAD)) 
	{
		damage_old(ch, victim, 666, sn, DAM_OTHER, TRUE);
		act("$n shrieks!", victim, NULL, NULL, TO_ROOM);
		send_to_char("You shriek in pain!!\n\r", victim);
	} 
	else 
	{
		victim->hit = UMIN(victim->hit + heal, victim->max_hit);
		update_pos(victim);
		send_to_char("Your surrounded by a powerful prism which binds and penetrates your wounds.\n\r", victim);
		if (ch != victim)
		    send_to_char("They are surrounded by your prism.\n\r", ch);
	}

    return;
	}

void spell_heat_metal(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj_lose, *obj_next;
    int dam = 0;
    bool fail = TRUE;

    if (!saves_spell(level + 2, victim, DAM_FIRE, 0)
	&& !IS_SET(victim->imm_flags, IMM_FIRE)) {
	for (obj_lose = victim->carrying;
	     obj_lose != NULL;
	     obj_lose = obj_next) {
	    obj_next = obj_lose->next_content;
	    if (number_range(1, 2 * level) > obj_lose->level
		&& !saves_spell(level, victim, DAM_FIRE, 0)
		&& !IS_OBJ_STAT(obj_lose, ITEM_NONMETAL)
		&& !IS_OBJ_STAT(obj_lose, ITEM_BURN_PROOF)) {
		switch (obj_lose->item_type) {
		case ITEM_ARMOR:
		    if (obj_lose->wear_loc != -1) {	/* remove the item */
			if (can_drop_obj(victim, obj_lose)
			    && (obj_lose->weight / 10) <
			    number_range(1, 2 * get_curr_stat(victim, STAT_DEX))
			    && remove_obj(victim, obj_lose->wear_loc, TRUE)) {
			    act("$n yelps and throws $p to the ground!",
				victim, obj_lose, NULL, TO_ROOM);
			    act("You remove and drop $p before it burns you.",
				victim, obj_lose, NULL, TO_CHAR);
			    dam += (number_range(1, obj_lose->level) / 3);
			    obj_from_char(obj_lose);
			    obj_to_room(obj_lose, victim->in_room);
			    fail = FALSE;
			} else {	/* stuck on the body! ouch! */
			    act("Your skin is seared by $p!",
				victim, obj_lose, NULL, TO_CHAR);
			    dam += (number_range(1, obj_lose->level));
			    fail = FALSE;
			}

		    } else {	/* drop it if we can */
			if (can_drop_obj(victim, obj_lose)) {
			    act("$n yelps and throws $p to the ground!",
				victim, obj_lose, NULL, TO_ROOM);
			    act("You and drop $p before it burns you.",
				victim, obj_lose, NULL, TO_CHAR);
			    dam += (number_range(1, obj_lose->level) / 6);
			    obj_from_char(obj_lose);
			    obj_to_room(obj_lose, victim->in_room);
			    fail = FALSE;
			} else {	/* cannot drop */
			    act("Your skin is seared by $p!",
				victim, obj_lose, NULL, TO_CHAR);
			    dam += (number_range(1, obj_lose->level) / 2);
			    fail = FALSE;
			}
		    }
		    break;
		case ITEM_WEAPON:
		    if (obj_lose->wear_loc != -1) {	/* try to drop it */
			if (IS_WEAPON_STAT(obj_lose, WEAPON_FLAMING))
			    continue;

			if (can_drop_obj(victim, obj_lose)
			    && remove_obj(victim, obj_lose->wear_loc, TRUE)) {
			    act("$n is burned by $p, and throws it to the ground.",
				victim, obj_lose, NULL, TO_ROOM);
			    send_to_char(
					    "You throw your red-hot weapon to the ground!\n\r",
					    victim);
			    dam += 1;
			    obj_from_char(obj_lose);
			    obj_to_room(obj_lose, victim->in_room);
			    fail = FALSE;
			} else {	/* YOWCH! */
			    send_to_char("Your weapon sears your flesh!\n\r",
					 victim);
			    dam += number_range(1, obj_lose->level);
			    fail = FALSE;
			}
		    } else {	/* drop it if we can */
			if (can_drop_obj(victim, obj_lose)) {
			    act("$n throws a burning hot $p to the ground!",
				victim, obj_lose, NULL, TO_ROOM);
			    act("You and drop $p before it burns you.",
				victim, obj_lose, NULL, TO_CHAR);
			    dam += (number_range(1, obj_lose->level) / 6);
			    obj_from_char(obj_lose);
			    obj_to_room(obj_lose, victim->in_room);
			    fail = FALSE;
			} else {	/* cannot drop */
			    act("Your skin is seared by $p!",
				victim, obj_lose, NULL, TO_CHAR);
			    dam += (number_range(1, obj_lose->level) / 2);
			    fail = FALSE;
			}
		    }
		    break;
		}
	    }
	}
    }
    if (fail) {
	send_to_char("Your spell had no effect.\n\r", ch);
	send_to_char("You feel momentarily warmer.\n\r", victim);
    } else {			/* damage! */
	if (saves_spell(level, victim, DAM_FIRE, 0))
	    dam = 2 * dam / 3;
	damage_old(ch, victim, dam, sn, DAM_FIRE, TRUE);
    }
}

/* RT really nasty high-level attack spell */
void spell_holy_word(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
    int bless_num, curse_num, frenzy_num;

    bless_num = skill_lookup("bless");
    curse_num = skill_lookup("curse");
    frenzy_num = skill_lookup("frenzy");
/* Lame --Tasydra
    purify_num = skill_lookup("purify");
*/
    act("$n utters a word of divine power!", ch, NULL, NULL, TO_ROOM);
    send_to_char("You utter a word of divine power.\n\r", ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (ch->side == vch->side) {
	    send_to_char("You feel full more powerful.\n\r", vch);
	    spell_frenzy(frenzy_num, level, ch, (void *) vch, TARGET_CHAR);
/* Lame --Tasydra
	    spell_purify(purify_num, level - 10, ch, (void *) vch, TARGET_CHAR);
*/
	    spell_bless(bless_num, level, ch, (void *) vch, TARGET_CHAR);
	} else
/*
   else if ((IS_GOOD(ch) && IS_EVIL(vch)) ||
   (IS_EVIL(ch) && IS_GOOD(vch)) )
 */
	{
	    if (!is_safe_spell(ch, vch, TRUE)) 
		{
		spell_curse(curse_num, level, ch, (void *) vch, TARGET_CHAR);
		send_to_char("You are struck down!\n\r", vch);
		dam = dice(level, DAM_HOLYWORD);

	    if (saves_spell(level, vch, DAM_HOLY, SAVE_ATTACK)) 
			damage_old(ch, vch, dam / 2, sn, DAM_HOLY, TRUE);
	    else 
			damage_old(ch, vch, dam, sn, DAM_HOLY, TRUE);

	    }
	}
/*
   else if (IS_NEUTRAL(ch))
   {
   if (!is_safe_spell(ch,vch,TRUE))
   {
   spell_curse(curse_num,level/2,ch,(void *) vch,TARGET_CHAR);
   send_to_char("You are struck down!\n\r",vch);
   dam = dice(level,DAM_HOLYWORD);
   damage_old(ch,vch,dam,sn,DAM_ENERGY,TRUE);
   }
   }
 */
    }

    send_to_char("You feel drained.\n\r", ch);
    //ch->move = 0;
    //ch->hit /= 2;
}

void spell_identify(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    sprintf(buf,
	    "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",

	    obj->name,
	    item_name(obj->item_type),
	    extra_bit_name(obj->extra_flags),
	    obj->weight / 10,
	    obj->cost,
	    obj->level
	);
    send_to_char(buf, ch);

    switch (obj->item_type) {
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
	sprintf(buf, "Level %d spells of:", obj->value[0]);
	send_to_char(buf, ch);

	if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[1]].name, ch);
	    send_to_char("'", ch);
	}
	if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[2]].name, ch);
	    send_to_char("'", ch);
	}
	if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[3]].name, ch);
	    send_to_char("'", ch);
	}
	if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[4]].name, ch);
	    send_to_char("'", ch);
	}
	send_to_char(".\n\r", ch);
	break;

    case ITEM_WAND:
    case ITEM_STAFF:
	sprintf(buf, "Has %d charges of level %d",
		obj->value[2], obj->value[0]);
	send_to_char(buf, ch);

	if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL) {
	    send_to_char(" '", ch);
	    send_to_char(skill_table[obj->value[3]].name, ch);
	    send_to_char("'", ch);
	}
	send_to_char(".\n\r", ch);
	break;

    case ITEM_DRINK_CON:
	sprintf(buf, "It holds %s-colored %s.\n\r",
		liq_table[obj->value[2]].liq_color,
		liq_table[obj->value[2]].liq_name);
	send_to_char(buf, ch);
	break;

    case ITEM_CONTAINER:
	sprintf(buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
		obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
	send_to_char(buf, ch);
	if (obj->value[4] != 100) {
	    sprintf(buf, "Weight multiplier: %d%%\n\r",
		    obj->value[4]);
	    send_to_char(buf, ch);
	}
	break;

    case ITEM_WEAPON:
	send_to_char("Weapon type is ", ch);
	switch (obj->value[0]) {
	case (WEAPON_EXOTIC):
	    send_to_char("exotic.\n\r", ch);
	    break;
	case (WEAPON_SWORD):
	    send_to_char("sword.\n\r", ch);
	    break;
	case (WEAPON_DAGGER):
	    send_to_char("dagger.\n\r", ch);
	    break;
	case (WEAPON_SPEAR):
	    send_to_char("spear/staff.\n\r", ch);
	    break;
	case (WEAPON_MACE):
	    send_to_char("mace/club.\n\r", ch);
	    break;
	case (WEAPON_AXE):
	    send_to_char("axe.\n\r", ch);
	    break;
	case (WEAPON_FLAIL):
	    send_to_char("flail.\n\r", ch);
	    break;
	case (WEAPON_WHIP):
	    send_to_char("whip.\n\r", ch);
	    break;
	case (WEAPON_POLEARM):
	    send_to_char("polearm.\n\r", ch);
	    break;
	default:
	    send_to_char("unknown.\n\r", ch);
	    break;
	}
	if (obj->pIndexData->new_format)
	    sprintf(buf, "Damage is %dd%d (average %d).\n\r",
		    obj->value[1], obj->value[2],
		    (1 + obj->value[2]) * obj->value[1] / 2);
	else
	    sprintf(buf, "Damage is %d to %d (average %d).\n\r",
		    obj->value[1], obj->value[2],
		    (obj->value[1] + obj->value[2]) / 2);
	send_to_char(buf, ch);
	if (obj->value[4]) {	/* weapon flags */
	    sprintf(buf, "Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]));
	    send_to_char(buf, ch);
	}
	break;

case ITEM_ARMOR:
	sprintf(buf,
		"Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
		obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
	send_to_char(buf, ch);
	break;
case ITEM_INGREDIENT:
case ITEM_PARCHMENT:
	sprintf(buf,
		"Level of %d with %d uses left.\n\r",
		obj->value[0], obj->value[1]);
	send_to_char(buf, ch);
	break;
    }

    if (!obj->enchanted)
	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {
	    if (paf->location != APPLY_NONE && paf->modifier != 0) {
		sprintf(buf, "Affects %s by %d.\n\r",
			affect_loc_name(paf->location), paf->modifier);
		send_to_char(buf, ch);
		if (paf->bitvector) {
		    switch (paf->where) {
		    case TO_AFFECTS:
			sprintf(buf, "Adds %s affect.\n",
				affect_bit_name(paf->bitvector));
			break;
		    case TO_OBJECT:
			sprintf(buf, "Adds %s object flag.\n",
				extra_bit_name(paf->bitvector));
			break;
		    case TO_IMMUNE:
			sprintf(buf, "Adds immunity to %s.\n",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_RESIST:
			sprintf(buf, "Adds resistance to %s.\n\r",
				imm_bit_name(paf->bitvector));
			break;
		    case TO_VULN:
			sprintf(buf, "Adds vulnerability to %s.\n\r",
				imm_bit_name(paf->bitvector));
			break;
		    default:
			sprintf(buf, "Unknown bit %d: %d\n\r",
				paf->where, paf->bitvector);
			break;
		    }
		    send_to_char(buf, ch);
		}
	    }
	}
    for (paf = obj->affected; paf != NULL; paf = paf->next) {
	if (paf->location != APPLY_NONE && paf->modifier != 0) {
	    sprintf(buf, "Affects %s by %d",
		    affect_loc_name(paf->location), paf->modifier);
	    send_to_char(buf, ch);
	    if (paf->duration > -1)
		sprintf(buf, ", %d hours.\n\r", paf->duration);
	    else
		sprintf(buf, ".\n\r");
	    send_to_char(buf, ch);
	    if (paf->bitvector) {
		switch (paf->where) {
		case TO_AFFECTS:
		    sprintf(buf, "Adds %s affect.\n",
			    affect_bit_name(paf->bitvector));
		    break;
		case TO_OBJECT:
		    sprintf(buf, "Adds %s object flag.\n",
			    extra_bit_name(paf->bitvector));
		    break;
		case TO_WEAPON:
		    sprintf(buf, "Adds %s weapon flags.\n",
			    weapon_bit_name(paf->bitvector));
		    break;
		case TO_IMMUNE:
		    sprintf(buf, "Adds immunity to %s.\n",
			    imm_bit_name(paf->bitvector));
		    break;
		case TO_RESIST:
		    sprintf(buf, "Adds resistance to %s.\n\r",
			    imm_bit_name(paf->bitvector));
		    break;
		case TO_VULN:
		    sprintf(buf, "Adds vulnerability to %s.\n\r",
			    imm_bit_name(paf->bitvector));
		    break;
		default:
		    sprintf(buf, "Unknown bit %d: %d\n\r",
			    paf->where, paf->bitvector);
		    break;
		}
		send_to_char(buf, ch);
	    }
	}
    }

    return;
}

void spell_infravision(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_INFRARED)) {
	if (victim == ch)
	    send_to_char("You can already see in the dark.\n\r", ch);
	else
	    act("$N already has infravision.\n\r", ch, NULL, victim, TO_CHAR);
	return;
    }
    act("$n's eyes glow red.\n\r", ch, NULL, NULL, TO_ROOM);

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 2 * level;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char(victim, &af);
    send_to_char("Your eyes glow red.\n\r", victim);
    return;
}

void spell_invis(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    /* object invisibility */
    if (target == TARGET_OBJ) {
	obj = (OBJ_DATA *) vo;

	if (IS_OBJ_STAT(obj, ITEM_INVIS)) {
	    act("$p is already invisible.", ch, obj, NULL, TO_CHAR);
	    return;
	}
	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = level + 12;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = ITEM_INVIS;
	affect_to_obj(obj, &af);

	act("$p fades out of sight.", ch, obj, NULL, TO_ALL);
	return;
    }
    /* character invisibility */
    victim = (CHAR_DATA *) vo;

    if (IS_AFFECTED(victim, AFF_INVISIBLE))
	return;

    act("$n fades out of existence.", victim, NULL, NULL, TO_ROOM);

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level + 12;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char(victim, &af);
    send_to_char("You fade out of existence.\n\r", victim);
    return;
}

void spell_know_level(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char buf[MAX_STRING_LENGTH];

    sprintf(buf, "$N is about level %d.\n\r", victim->level);

    act(buf, ch, NULL, victim, TO_CHAR);
    return;
}

void spell_lightning_bolt(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_LIGHTNINGBOLT);
    if (saves_spell(level, victim, DAM_LIGHTNING, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_LIGHTNING, TRUE);
    return;
}

void spell_locate_object(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    char buf[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;
    char pers_buf[MAX_STRING_LENGTH];

    found = FALSE;
    number = 0;
    max_found = IS_IMMORTAL(ch) ? 200 : 2 * level;

    buffer = new_buf();

    for (obj = object_list; obj != NULL; obj = obj->next) {
	if (!can_see_obj(ch, obj) || !is_name(target_name, obj->name)
	    || IS_OBJ_STAT(obj, ITEM_NOLOCATE) || number_percent() > 2 * level
	    || ch->level < obj->level)
	    continue;

	found = TRUE;
	number++;

	for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);

	if (in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by)) {
	    PERS(in_obj->carried_by, ch, pers_buf);
	    sprintf(buf, "%s is carried by %s\n\r",
		    in_obj->short_descr,
		    pers_buf);
	} else {
	    if (IS_IMMORTAL(ch) && in_obj->in_room != NULL)
		sprintf(buf, "%s is in %s [Room %d]\n\r",
			in_obj->short_descr,
			in_obj->in_room->name, in_obj->in_room->vnum);
	    else
		sprintf(buf, "%s is in %s\n\r",
			in_obj->short_descr,
			in_obj->in_room == NULL
			? "somewhere" : in_obj->in_room->name);
	}

	buf[0] = UPPER(buf[0]);
	add_buf(buffer, buf);

	if (number >= max_found)
	    break;
    }

    if (!found)
	send_to_char("Nothing like that in heaven or earth.\n\r", ch);
    else
	page_to_char(buf_string(buffer), ch);

    free_buf(buffer);

    return;
}

void spell_magic_missile(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_MAGICMISSILE);
    if (saves_spell(level, victim, DAM_ENERGY, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_ENERGY, TRUE);
    return;
}

void spell_mass_healing(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *gch;
    int heal_num, refresh_num;

    heal_num = skill_lookup("heal");
    refresh_num = skill_lookup("refresh");

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (!is_same_group(gch, ch))
	    continue;
	if ((IS_NPC(ch) && IS_NPC(gch)) || (!IS_NPC(ch) && !IS_NPC(gch))) {
	    spell_heal(heal_num, level, ch, (void *) gch, TARGET_CHAR);
	    spell_refresh(refresh_num, level, ch, (void *) gch, TARGET_CHAR);
	}
    }
}

void spell_mass_invis(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (!is_same_group(gch, ch) || IS_AFFECTED(gch, AFF_INVISIBLE))
	    continue;
	act("$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM);
	send_to_char("You slowly fade out of existence.\n\r", gch);

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level / 2;
	af.duration = 24;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char(gch, &af);
    }
    send_to_char("Ok.\n\r", ch);

    return;
}

void spell_null(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    send_to_char("That's not a spell!\n\r", ch);
    return;
}

void spell_pass_door(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_PASS_DOOR)) {
	if (victim == ch)
	    send_to_char("You are already out of phase.\n\r", ch);
	else
	    act("$N is already shifted out of phase.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = number_fuzzy(level / 4);
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char(victim, &af);
    act("$n turns translucent.", victim, NULL, NULL, TO_ROOM);
    send_to_char("You turn translucent.\n\r", victim);
    return;
}

/* RT plague spell, very nasty */

void spell_plague(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    
    if (ch->side == victim->side)
    {
	    send_to_char("You could be arrested for that!\n\r", ch);
	    return;
    }
    

    if (saves_spell(level, victim, DAM_DISEASE, SAVE_NORMAL) ||
	(IS_NPC(victim) && IS_SET(victim->act, ACT_UNDEAD))) {
	if (ch == victim)
	    send_to_char("You feel momentarily ill, but it passes.\n\r", ch);
	else
	    act("$N seems to be unaffected.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level * 3 / 4;
    af.duration = level;
    af.location = APPLY_STR;
    af.modifier = -1;
    af.bitvector = AFF_PLAGUE;
    affect_join(victim, &af);

    send_to_char
	("You scream in agony as plague sores erupt from your skin.\n\r", victim);
    act("$n screams in agony as plague sores erupt from $s skin.",
	victim, NULL, NULL, TO_ROOM);
}

void spell_poison(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    if (target == TARGET_OBJ) {
	obj = (OBJ_DATA *) vo;

	if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON) {
	    if (IS_OBJ_STAT(obj, ITEM_BLESS) || IS_OBJ_STAT(obj, ITEM_BURN_PROOF)) {
		act("Your spell fails to corrupt $p.", ch, obj, NULL, TO_CHAR);
		return;
	    }
	    obj->value[3] = 1;
	    act("$p is infused with poisonous vapors.", ch, obj, NULL, TO_ALL);
	    return;
	}
	if (obj->item_type == ITEM_WEAPON) {
	    if (IS_WEAPON_STAT(obj, WEAPON_FLAMING)
		|| IS_WEAPON_STAT(obj, WEAPON_FROST)
		|| IS_WEAPON_STAT(obj, WEAPON_VAMPIRIC)
		|| IS_WEAPON_STAT(obj, WEAPON_SHARP)
		|| IS_WEAPON_STAT(obj, WEAPON_VORPAL)
		|| IS_WEAPON_STAT(obj, WEAPON_SHOCKING)
		|| IS_OBJ_STAT(obj, ITEM_BLESS) || IS_OBJ_STAT(obj, ITEM_BURN_PROOF)) {
		act("You can't seem to envenom $p.", ch, obj, NULL, TO_CHAR);
		return;
	    }
	    if (IS_WEAPON_STAT(obj, WEAPON_POISON)) {
		act("$p is already envenomed.", ch, obj, NULL, TO_CHAR);
		return;
	    }
	    af.where = TO_WEAPON;
	    af.type = sn;
	    af.level = level / 2;
	    af.duration = level / 8;
	    af.location = 0;
	    af.modifier = 0;
	    af.bitvector = WEAPON_POISON;
	    affect_to_obj(obj, &af);

	    act("$p is coated with deadly venom.", ch, obj, NULL, TO_ALL);
	    return;
	}
	act("You can't poison $p.", ch, obj, NULL, TO_CHAR);
	return;
    }
    victim = (CHAR_DATA *) vo;
    
    if (ch->side == victim->side)
    {
	    send_to_char("You could be arrested for that!\n\r", ch);
	    return;
    }

    if (saves_spell(level, victim, DAM_POISON, SAVE_RARELY)) {
	act("$n turns slightly green, but it passes.", victim, NULL, NULL, TO_ROOM);
	send_to_char("You feel momentarily ill, but it passes.\n\r", victim);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_STR;
    af.modifier = -1;
    af.bitvector = AFF_POISON;
    affect_join(victim, &af);
    send_to_char("You feel very sick.\n\r", victim);
    act("$n looks very ill.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_protection_evil(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_PROTECT_EVIL)
	|| IS_AFFECTED(victim, AFF_PROTECT_GOOD)) {
	if (victim == ch)
	    send_to_char("You are already protected.\n\r", ch);
	else
	    act("$N is already protected.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 24;
    af.location = APPLY_SAVING_SPELL;
    af.modifier = -20;
    af.bitvector = AFF_PROTECT_EVIL;
    affect_to_char(victim, &af);
    send_to_char("You feel holy and pure.\n\r", victim);
    if (ch != victim)
	act("$N is protected from evil.", ch, NULL, victim, TO_CHAR);
    return;
}

void spell_protection_good(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_PROTECT_GOOD)
	|| IS_AFFECTED(victim, AFF_PROTECT_EVIL)) {
	if (victim == ch)
	    send_to_char("You are already protected.\n\r", ch);
	else
	    act("$N is already protected.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 24;
    af.location = APPLY_SAVING_SPELL;
    af.modifier = -20;
    af.bitvector = AFF_PROTECT_GOOD;
    affect_to_char(victim, &af);
    send_to_char("You feel aligned with darkness.\n\r", victim);
    if (ch != victim)
	act("$N is protected from good.", ch, NULL, victim, TO_CHAR);
    return;
}

void spell_ray_of_truth(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    if (victim != ch) {
	act("$n raises $s hand, and a blinding ray of light shoots forth!",
	    ch, NULL, NULL, TO_ROOM);
	send_to_char(
			"You raise your hand and a blinding ray of light shoots forth!\n\r",
			ch);
    }

//    if(ch == victim)
//	dam = 0;
//    else
	dam = dice(level, DAM_RAYOFTRUTH);
	
	//extra dam vs sanc for knights and clerics.
	if (is_affected(victim, skill_lookup("sanctuary")) 
		&& (get_skill(ch, skill_lookup("ray of truth")) > 0))
	//&& (!strcmp(class_table[ch->class].name, "cleric")
	//|| !strcmp(class_table[ch->class].name, "knight")))
	dam *=1.5;

    if (saves_spell(level, victim, DAM_HOLY, SAVE_ATTACK))
	dam /= 2;

    spell_blindness(gsn_blindness,
		    3 * level / 4, ch, (void *) victim, TARGET_CHAR);
damage_old(ch, victim, dam, sn, DAM_HOLY, TRUE);
}

void spell_smite(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

if (victim != ch) {
	act("$n conjures the power of the gods!",
	    ch, NULL, NULL, TO_ROOM);
	send_to_char(
			"You conjure the power of the gods!\n\r",
			ch);
    }



//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_SMITE);
    
    if (is_affected(victim, skill_lookup("sanctuary")))
	dam *=1.5;

    if (saves_spell(level, victim, DAM_HOLY, 0))
	dam /= 2;

    spell_blindness(gsn_blindness, level, ch, (void *) victim, TARGET_CHAR);
    damage_old(ch, victim, dam, sn, DAM_HOLY, TRUE);
}

void spell_recharge(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int chance, percent;

    if (obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF) {
	send_to_char("That item does not carry charges.\n\r", ch);
	return;
    }
    if (obj->value[3] >= 3 * level / 2) {
	send_to_char("Your skills are not great enough for that.\n\r", ch);
	return;
    }
    chance = 40 + 2 * level;

    chance -= obj->value[3];	/* harder to do high-level spells */
    chance -= (obj->value[1] - obj->value[2]) *
	(obj->value[1] - obj->value[2]);

    chance = UMAX(level / 2, chance);

    percent = number_percent();

    if (percent < chance / 2) {
	act("$p glows softly.", ch, obj, NULL, TO_CHAR);
	act("$p glows softly.", ch, obj, NULL, TO_ROOM);
	obj->value[2] = UMAX(obj->value[1], obj->value[2]);
	obj->value[1] = 0;
	return;
    } else if (percent <= chance) {
	int chargeback, chargemax;

	act("$p glows softly.", ch, obj, NULL, TO_CHAR);
	act("$p glows softly.", ch, obj, NULL, TO_CHAR);

	chargemax = obj->value[1] - obj->value[2];

	if (chargemax > 0)
	    chargeback = UMAX(1, chargemax * percent / 100);
	else
	    chargeback = 0;

	obj->value[2] += chargeback;
	obj->value[1] = 0;
	return;
    } else if (percent <= UMIN(95, 3 * chance / 2)) {
	send_to_char("Nothing seems to happen.\n\r", ch);
	if (obj->value[1] > 1)
	    obj->value[1]--;
	return;
    } else {			/* whoops! */
	act("$p glows brightly and explodes!", ch, obj, NULL, TO_CHAR);
	act("$p glows brightly and explodes!", ch, obj, NULL, TO_ROOM);
	extract_obj(obj);
    }
}

void spell_refresh(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    victim->move = UMIN(victim->move + level, victim->max_move);
    if (victim->max_move == victim->move)
	send_to_char("You feel fully refreshed!\n\r", victim);
    else
	send_to_char("You feel less tired.\n\r", victim);
    if (ch != victim)
	send_to_char("They feel less tired.\n\r", ch);
    return;
}

void spell_remove_curse(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    bool found = FALSE;

    /* do object cases first */
    if (target == TARGET_OBJ) {
	obj = (OBJ_DATA *) vo;

	if (IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_NOREMOVE)) {
	    if (!IS_OBJ_STAT(obj, ITEM_NOUNCURSE)
		&& !saves_dispel(level + 2, obj->level, 0)) {
		REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
		REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
		act("$p glows blue.", ch, obj, NULL, TO_ALL);
		return;
	    }
	    act("The curse on $p is beyond your power.", ch, obj, NULL, TO_CHAR);
	    return;
	}
	act("There doesn't seem to be a curse on $p.", ch, obj, NULL, TO_CHAR);
	return;
    }
    /* characters */
    victim = (CHAR_DATA *) vo;

    if (check_dispel(level, victim, gsn_curse)) {
	send_to_char("You feel better.\n\r", victim);
	act("$n looks more relaxed.", victim, NULL, NULL, TO_ROOM);
    }
    for (obj = victim->carrying; (obj != NULL && !found); obj = obj->next_content) {
	if ((IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_NOREMOVE))
	    && !IS_OBJ_STAT(obj, ITEM_NOUNCURSE)) {	/* attempt to remove curse */
	    if (!saves_dispel(level, obj->level, 0)) {
		found = TRUE;
		REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
		REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
		act("Your $p glows blue.", victim, obj, NULL, TO_CHAR);
		act("$n's $p glows blue.", victim, obj, NULL, TO_ROOM);
	    }
	}
    }
}

void spell_sanctuary(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_SANCTUARY)) {
	if (victim == ch)
	    send_to_char("You are already in sanctuary.\n\r", ch);
	else
	    act("$N is already in sanctuary.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 6;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char(victim, &af);
    act("$n is surrounded by a white aura.", victim, NULL, NULL, TO_ROOM);
    send_to_char("You are surrounded by a white aura.\n\r", victim);
    return;
}

void spell_reflect(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_REFLECT)) {
	if (victim == ch)
	    send_to_char("You are already reflective.\n\r", ch);
	else
	    act("$N is already reflective.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 6;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_REFLECT;
    affect_to_char(victim, &af);
    act("$n is surrounded by a reflective barrier.", victim, NULL, NULL,
	TO_ROOM);
    send_to_char("You are surrounded by a reflective barrier.\n\r", victim);
    return;
}

void spell_shield(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn)) {
	if (victim == ch)
	    send_to_char("You are already shielded from harm.\n\r", ch);
	else
	    act("$N is already protected by a shield.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 8 + level;
    af.location = APPLY_AC;
    af.modifier = -20;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    act("$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM);
    send_to_char("You are surrounded by a force shield.\n\r", victim);
    return;
}

void spell_shocking_grasp(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;


//    if(ch == victim)
//	dam = 0;
//    else
    dam = dice(level, DAM_SHOCKINGGRASP);
    if (saves_spell(level, victim, DAM_LIGHTNING, SAVE_ATTACK))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_LIGHTNING, TRUE);
    return;
}

void spell_sleep(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA *paf;

    if (IS_AFFECTED(victim, AFF_SLEEP)
	|| (IS_NPC(victim) && IS_SET(victim->act, ACT_UNDEAD))
	|| (level + 2) < victim->level
	|| saves_spell(level, victim, DAM_CHARM, SAVE_OFTEN))
	return;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level/15;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_SLEEP;
    affect_join(victim, &af);

    if (!IS_NPC(victim)) {
	paf = affect_find(ch->affected, skill_lookup("noquit"));

	if (paf != NULL)
	    affect_remove(ch, paf);

	af.where = TO_AFFECTS;
	af.type = skill_lookup("noquit");
	af.level = 1;
	af.duration = 3;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_NOQUIT;
	affect_to_char(ch, &af);
    }
    paf = affect_find(victim->affected, skill_lookup("noquit"));

    if (paf != NULL)
	affect_remove(victim, paf);

    af.where = TO_AFFECTS;
    af.type = skill_lookup("noquit");
    af.level = 1;
    af.duration = 3;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_NOQUIT;
    affect_to_char(victim, &af);

    if (IS_AWAKE(victim)) {
	send_to_char("You feel very sleepy ..... zzzzzz.\n\r", victim);
	act("$n goes to sleep.", victim, NULL, NULL, TO_ROOM);
	victim->position = POS_SLEEPING;
    }
    return;
}

void spell_slow(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn) || IS_AFFECTED(victim, AFF_SLOW)) {
	if (victim == ch)
	    send_to_char("You can't move any slower!\n\r", ch);
	else
	    act("$N can't get any slower than that.",
		ch, NULL, victim, TO_CHAR);
	return;
    }
    if (saves_spell(level, victim, DAM_OTHER, SAVE_NORMAL)
	|| IS_SET(victim->imm_flags, IMM_MAGIC)) {
	if (victim != ch)
	    send_to_char("Nothing seemed to happen.\n\r", ch);
	send_to_char("You feel momentarily lethargic.\n\r", victim);
	return;
    }
    if (IS_AFFECTED(victim, AFF_HASTE)) {
	if (!check_dispel(level, victim, skill_lookup("haste"))) {
	    if (victim != ch)
		send_to_char("Spell failed.\n\r", ch);
	    send_to_char("You feel momentarily slower.\n\r", victim);
	    return;
	}
	act("$n is moving less quickly.", victim, NULL, NULL, TO_ROOM);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 2;
    af.location = APPLY_DEX;
    af.modifier = -1 - (level >= 18) - (level >= 25) - (level >= 32);
    af.bitvector = AFF_SLOW;
    affect_to_char(victim, &af);
    send_to_char("You feel yourself slowing d o w n...\n\r", victim);
    act("$n starts to move in slow motion.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_stone_skin(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(ch, sn)) {
	if (victim == ch)
	    send_to_char("Your skin is already as hard as a rock.\n\r", ch);
	else
	    act("$N is already as hard as can be.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = -40;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    act("$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM);
    send_to_char("Your skin turns to stone.\n\r", victim);
    return;
}

void spell_summon(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;

    if ((victim = get_char_world(ch, target_name)) == NULL
	|| victim == ch
	|| victim->in_room == NULL
	|| IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	|| IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	|| IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	|| IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	|| (IS_NPC(victim) && IS_SET(victim->act, ACT_AGGRESSIVE))
	|| victim->level >= level + 3
	|| (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL)
	|| victim->fighting != NULL
	|| (IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	|| (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
	|| (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOSUMMON))
	|| (IS_NPC(victim) && saves_spell(level, victim, DAM_OTHER, 0))
	|| (!IS_NPC(victim) && (GET_SIDE(victim) != GET_SIDE(ch)))
	|| (IS_AFFECTED (victim, AFF_NOQUIT))
	|| (IS_AFFECTED (ch, AFF_NOQUIT))
	) {
	send_to_char("You failed.\n\r", ch);
	return;
    }
    if (IS_NPC(victim) && victim->level > ((ch->level / 3) * 2)) {
	send_to_char("You failed.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(victim, AFF_NOQUIT)) {
	send_to_char("That player cannot be summoned right now.\n\r", ch);
	return;
    }
/*    if (IS_AFFECTED(ch, AFF_NOQUIT))
    {
    ch->exp -= number_range(35,75);
    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
    } */
    act("$n disappears suddenly.", victim, NULL, NULL, TO_ROOM);
    char_from_room(victim);
    char_to_room(victim, ch->in_room);
    act("$n arrives suddenly.", victim, NULL, NULL, TO_ROOM);
    act("$n has summoned you!", ch, NULL, victim, TO_VICT);

    if (IS_NPC(victim))	
	SET_BIT(victim->act, ACT_NOXP);

    do_look(victim, "auto");
    return;
}

void spell_teleport(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;

    if (victim->in_room == NULL
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(victim->in_room->room_flags, ROOM_NEWBIES_ONLY)
	|| (victim != ch && IS_SET(victim->imm_flags, IMM_SUMMON))
	|| (!IS_NPC(ch) && victim->fighting != NULL)
	|| (victim != ch
	    && (saves_spell(level - 5, victim, DAM_OTHER, 0)))) {
	send_to_char("You failed.\n\r", ch);
	return;
    }
	 if (IS_AFFECTED(ch, AFF_NOQUIT)) 
   {
	send_to_char("Can't do that now!", ch);
	return;
   }
    pRoomIndex = get_random_room_in_area(victim);

    if (victim != ch)
	send_to_char("You have been teleported!\n\r", victim);

    act("$n vanishes!", victim, NULL, NULL, TO_ROOM);
    char_from_room(victim);
    char_to_room(victim, pRoomIndex);
    act("$n slowly fades into existence.", victim, NULL, NULL, TO_ROOM);
    do_look(victim, "auto");
    return;
}

void spell_ventriloquate(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char speaker[MAX_INPUT_LENGTH];
    CHAR_DATA *vch;

    target_name = one_argument(target_name, speaker);

    sprintf(buf1, "%s says '{y%s{x'\n\r", speaker, target_name);
    sprintf(buf2, "Someone makes %s say '{y%s{x'\n\r", speaker,
target_name);
    buf1[0] = UPPER(buf1[0]);
/*
    baron
    count
    duke
    master
    lord
    legendary
*/   
    if (buf1[0] == 'B' || buf1[0] == 'C')
     if(buf1[6] != '\0')
    buf1[6] = UPPER(buf1[6]);
    
    if (buf1[0] == 'D' || ( buf1[0] == 'L' && buf1[1] != 'e') )
     if(buf1[5] != '\0')
    buf1[5] = UPPER(buf1[5]);
    
    if (buf1[0] == 'M')
     if(buf1[7] != '\0')
    buf1[7] = UPPER(buf1[7]);
    
    if (buf1[0] == 'L' && buf1[1] == 'e')
     if(buf1[10] != '\0')
    buf1[10] = UPPER(buf1[10]);
    
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
	if (!is_exact_name(speaker, vch->name) && IS_AWAKE(vch))
	    send_to_char(saves_spell(level, vch, DAM_OTHER, 0) ? buf2 : buf1, vch);
    }

    return;
}

void spell_weaken(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn)) 
	return;

    if (saves_spell(level, victim, DAM_OTHER, SAVE_NORMAL))
	return;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 2;
    af.location = APPLY_STR;
    af.modifier = -1 * (level / 12);
    af.bitvector = AFF_WEAKEN;
    affect_to_char(victim, &af);
//affect_join(victim, &af);

    send_to_char("You feel your strength slip away.\n\r", victim);
    act("$n looks tired and weak.", victim, NULL, NULL, TO_ROOM);
    return;
}

/* RT recall spell is back */

void spell_word_of_recall(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *location;

    if (IS_NPC(victim))
	return;

    if ((location =
	 get_room_index(kingdom_table[victim->side].recall_room_num)) == NULL) {
	send_to_char("You are completely lost.\n\r", victim);
	return;
    }
    if (IS_AFFECTED(victim, AFF_CURSE)) {
	send_to_char("Spell failed.\n\r", victim);
	return;
    }
    if (victim->fighting != NULL) {
	send_to_char("You can't recall in a fight spell failed.\n\r", victim);
	return;
    }    if(IS_AFFECTED(ch, AFF_NOQUIT))
        {
        gain_exp(ch, 0 - number_range(level/2, 100));
        send_to_char("Even though your courage is pathetic, you do have a will to live.\n\r", victim);
        ch->move /= 2;
        act("$n disappears.", victim, NULL, NULL, TO_ROOM);
        char_from_room(victim);
        char_to_room(victim, location);
        act("$n appears in the room.", victim, NULL, NULL, TO_ROOM);
        do_look(victim, "auto");
        return;
        }
//just added negative exp gain to word of recall.... it will allow it to go above 2500, or max_tnl because i$
    else
        {
            ch->move /= 2;
            act("$n disappears.", victim, NULL, NULL, TO_ROOM);
            char_from_room(victim);
            char_to_room(victim, location);
            act("$n appears in the room.", victim, NULL, NULL, TO_ROOM);
            do_look(victim, "auto");
        }

}

/*
 * NPC spells.
 */
void spell_acid_breath(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    act("$n spits acid at $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n spits a stream of corrosive acid at you.", ch, NULL, victim, TO_VICT);
    act("You spit acid at $N.", ch, NULL, victim, TO_CHAR);

//    if(ch == victim)
//	dam = 0;
//    else
	dam = dice(level, DAM_ACIDBREATH);

    if (saves_spell(level, victim, DAM_ACID, SAVE_ATTACK)) {
	acid_effect(victim, level*4/ 5, dam / 4, TARGET_CHAR);
	damage_old(ch, victim, dam *4/ 5, sn, DAM_ACID, TRUE);
    } else {
	acid_effect(victim, level, dam, TARGET_CHAR);
	damage_old(ch, victim, dam, sn, DAM_ACID, TRUE);
    }
}

void spell_fire_breath(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch, *vch_next;
    int dam;

    act("$n breathes forth a cone of fire.", ch, NULL, victim, TO_NOTVICT);
    act("$n breathes a cone of hot fire over you!", ch, NULL, victim, TO_VICT);
    act("You breath forth a cone of fire.", ch, NULL, NULL, TO_CHAR);

dam = dice(level, DAM_FIREBREATH);

fire_effect(victim->in_room, level, dam / 2, TARGET_ROOM);

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch, vch, TRUE)
	    || (IS_NPC(vch) && IS_NPC(ch)
		&& (ch->fighting != vch || vch->fighting != ch)))
	    continue;

	if (vch == victim) {	/* full damage */
	    if (saves_spell(level, vch, DAM_FIRE, SAVE_ATTACK)) {
		fire_effect(vch, level / 2, dam / 4, TARGET_CHAR);
		damage_old(ch, vch, dam *4/ 5, sn, DAM_FIRE, TRUE);
	    } else {
		fire_effect(vch, level, dam, TARGET_CHAR);
		damage_old(ch, vch, dam, sn, DAM_FIRE, TRUE);
	    }
	} else {		/* partial damage */
	    if (saves_spell(level, vch, DAM_FIRE, SAVE_ATTACK)) {
		fire_effect(vch, level / 4, dam / 8, TARGET_CHAR);
		damage_old(ch, vch, dam *4/ 5, sn, DAM_FIRE, TRUE);
	    } else {
		fire_effect(vch, level / 2, dam / 4, TARGET_CHAR);
		damage_old(ch, vch, dam, sn, DAM_FIRE, TRUE);
	    }
	}
    }
}
void spell_tremor(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    if (IS_OUTSIDE(ch)) {
	send_to_char("This spell must be cast inside.\n\r", ch);
	return;
    }
    send_to_char("You conjure up a mighty TREMOR!\n\r", ch);
    act("$n forces the ground to SHAKE VIOLENTLY!", ch, NULL, NULL, TO_ROOM);

    for (vch = char_list; vch != NULL; vch = vch_next) {
	vch_next = vch->next;
	if (vch->in_room == NULL)
	    continue;
	if (vch->in_room == ch->in_room) {
	    if (vch != ch && !is_safe_spell(ch, vch, TRUE))
		damage_old(ch, vch, dice(level, DAM_TREMOR), sn, DAM_BASH, TRUE);

	    continue;
	}
    }

    return;
}

void spell_firestorm(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch, *vch_next;
    int dam;

    if (!IS_OUTSIDE(ch)) {
	send_to_char("You must be outside to make this work.\n\r", ch);
	return;
    }
    act("$n calls forth hail of pure {rFIRE{x from the SKY!", ch, NULL, victim, TO_NOTVICT);
    act("$n's firestorm {rBURNS YOU TO A CRISP{x!", ch, NULL, victim, TO_VICT);
    act("You conjure up a mighty {rFIRESTORM{x!", ch, NULL, NULL, TO_CHAR);

    dam = dice(level, DAM_FIRESTORM);

    fire_effect(victim->in_room, level / 2, dam / 2, TARGET_ROOM);

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch, vch, TRUE)
	    || (IS_NPC(vch) && IS_NPC(ch)
		&& (ch->fighting != vch || vch->fighting != ch)))
	    continue;

	if (vch == victim) {	/* full damage */
	    if (saves_spell(level - 1, vch, DAM_FIRE, SAVE_NORMAL)) {
		fire_effect(vch, level / 2, dam / 4, TARGET_CHAR);
		damage_old(ch, vch, dam / 2, sn, DAM_FIRE, TRUE);
	    } else {
		fire_effect(vch, level, dam, TARGET_CHAR);
		damage_old(ch, vch, dam, sn, DAM_FIRE, TRUE);
	    }
	} else {		/* partial damage */
	    if (saves_spell(level - 4, vch, DAM_FIRE, SAVE_NORMAL)) {
		fire_effect(vch, level / 4, 0, TARGET_CHAR);
		damage_old(ch, vch, dam / 4, sn, DAM_FIRE, TRUE);
	    } else {
		fire_effect(vch, level / 2, dam / 4, TARGET_CHAR);
		damage_old(ch, vch, dam / 2, sn, DAM_FIRE, TRUE);
	    }
	}
    }
}

void spell_frost_breath(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch, *vch_next;
    int dam;

    act("$n breathes out a freezing cone of frost!", ch, NULL, victim, TO_NOTVICT);
    act("$n breathes a freezing cone of frost over you!",
	ch, NULL, victim, TO_VICT);
    act("You breath out a cone of frost.", ch, NULL, NULL, TO_CHAR);

    dam = dice(level, DAM_FROSTBREATH);

cold_effect(victim->in_room, level, dam / 2, TARGET_ROOM);

    for (vch = victim->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch, vch, TRUE)
	    || (IS_NPC(vch) && IS_NPC(ch)
		&& (ch->fighting != vch || vch->fighting != ch)))
	    continue;

	if (vch == victim) {	/* full damage */
	    if (saves_spell(level, vch, DAM_COLD, SAVE_ATTACK)) {
		cold_effect(vch, level / 2, dam / 4, TARGET_CHAR);
		damage_old(ch, vch, dam *4/ 5, sn, DAM_COLD, TRUE);
	    } else {
		cold_effect(vch, level, dam, TARGET_CHAR);
		damage_old(ch, vch, dam, sn, DAM_COLD, TRUE);
	    }
	} else {
	    if (saves_spell(level - 2, vch, DAM_COLD, SAVE_ATTACK)) {
		cold_effect(vch, level / 4, 0, TARGET_CHAR);
		damage_old(ch, vch, dam *4/ 5, sn, DAM_COLD, TRUE);
	    } else {
		cold_effect(vch, level / 2, dam / 4, TARGET_CHAR);
		damage_old(ch, vch, dam, sn, DAM_COLD, TRUE);
	    }
	}
    }
}

void spell_gas_breath(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;

    act("$n breathes out a cloud of poisonous gas!", ch, NULL, NULL, TO_ROOM);
    act("You breath out a cloud of poisonous gas.", ch, NULL, NULL, TO_CHAR);

dam = dice(level, DAM_GASBREATH);

poison_effect(ch->in_room, level, dam, TARGET_ROOM);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (is_safe_spell(ch, vch, TRUE)
	    || (IS_NPC(ch) && IS_NPC(vch)
		&& (ch->fighting == vch || vch->fighting == ch)))
	    continue;

	if (saves_spell(level, vch, DAM_POISON, 0)) {
	    poison_effect(vch, level / 2, dam / 4, TARGET_CHAR);
	    damage_old(ch, vch, dam *4/ 5, sn, DAM_POISON, TRUE);
	} else {
	    poison_effect(vch, level, dam, TARGET_CHAR);
	    damage_old(ch, vch, dam, sn, DAM_POISON, TRUE);
	}
    }
}

void spell_lightning_breath(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    act("$n breathes a bolt of lightning at $N.", ch, NULL, victim, TO_NOTVICT);
    act("$n breathes a bolt of lightning at you!", ch, NULL, victim, TO_VICT);
    act("You breathe a bolt of lightning at $N.", ch, NULL, victim, TO_CHAR);


//    if(ch == victim)
//	dam = 0;
//    else
dam = dice(level, DAM_LIGHTNINGBREATH);

    if (saves_spell(level, victim, DAM_LIGHTNING, SAVE_ATTACK)) {
	shock_effect(victim, level / 2, dam / 4, TARGET_CHAR);
	damage_old(ch, victim, dam *4/ 5, sn, DAM_LIGHTNING, TRUE);
    } else {
	shock_effect(victim, level, dam, TARGET_CHAR);
	damage_old(ch, victim, dam, sn, DAM_LIGHTNING, TRUE);
    }
}

/*
 * Spells for mega1.are from Glop/Erkenbrand.
 */
void spell_general_purpose(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice(level, 25);
    if (saves_spell(level, victim, DAM_PIERCE, 0))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_PIERCE, TRUE);
    return;
}

void spell_high_explosive(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice(level, 25);
    if (saves_spell(level, victim, DAM_PIERCE, 0))
	dam /= 2;
    damage_old(ch, victim, dam, sn, DAM_PIERCE, TRUE);
    return;
}

void spell_death_coil(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;

    dam = dice(level, DAM_DEATHCOIL);

    if (saves_spell(level, victim, DAM_OTHER, SAVE_NORMAL))
	dam /= 2;

    if (ch->hit >= ch->max_hit)
	{
	dam = 0;
	damage_old(ch, victim, dam, sn, DAM_OTHER, TRUE);
	send_to_char("Coils leap out and sink themselves into you, to no avail!\n\r", victim);
	act("Coils leap out and sink themselves, to no avail, into $n.", victim, NULL, NULL, TO_ROOM);
	return;
	}
    if((ch->hit + dam) > ch->max_hit)
	{
	dam = ch->max_hit - ch->hit;
	damage_old(ch, victim, dam, sn, DAM_OTHER, TRUE);
	ch->hit += dam/2;
	send_to_char("Coils sink deep into you, but do less then expected!\n\r", victim);
  	act("Coils dig into $n but do less then would be expected.",victim, NULL, NULL, TO_ROOM);
	return;
	}
    damage_old(ch, victim, dam, sn, DAM_OTHER, TRUE);
    ch->hit += dam / 2;
    send_to_char("Coils leap out and sink themselves into you!!\n\r", victim);
    act("Coils leap out and sink themselves into $n.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_haven(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    //no fs
    AFFECT_DATA af;
    OBJ_DATA *spring;
    OBJ_DATA *bed;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 3;
    af.bitvector = AFF_FS_PROOF;
    affect_to_room(ch->in_room, &af);

//bed
    bed = create_object(get_obj_index(OBJ_VNUM_BED), 0);
    bed->timer = 4;
    obj_to_room(bed, ch->in_room);

//spring

    spring = create_object(get_obj_index(OBJ_VNUM_SPRING), 0);
    spring->timer = 4;
    obj_to_room(spring, ch->in_room);
    act("A safe haven appears before you.", ch, spring, NULL, TO_ROOM);
    act("A safe haven appears before you.", ch, spring, NULL, TO_CHAR);
    return;

}

void spell_unholy_word(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
    int curse_num, plague_num, poison_num, weaken_num;

    plague_num = skill_lookup("plague");
    curse_num = skill_lookup("curse");
    poison_num = skill_lookup("poison");
    weaken_num = skill_lookup("weaken");

    act("$n utters a word of dark power!", ch, NULL, NULL, TO_ROOM);
    send_to_char("You utter a word of dark power.\n\r", ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (ch->side == vch->side) {
	} else {
	    if (!is_safe_spell(ch, vch, TRUE)) {
		spell_curse(curse_num, level, ch, (void *) vch, TARGET_CHAR);
		spell_plague(plague_num, level, ch, (void *) vch, TARGET_CHAR);
		spell_poison(poison_num, level, ch, (void *) vch, TARGET_CHAR);
		spell_weaken(weaken_num, level, ch, (void *) vch, TARGET_CHAR);
		send_to_char("You are struck down!\n\r", vch);
		dam = dice(level, DAM_UNHOLYWORD);

	    if (saves_spell(level, vch, DAM_NEGATIVE, SAVE_ATTACK)) 
			damage_old(ch, vch, dam / 2, sn, DAM_NEGATIVE, TRUE);
	    else 
			damage_old(ch, vch, dam, sn, DAM_NEGATIVE, TRUE);



	    }
	}
    }

    send_to_char("You feel drained.\n\r", ch);
}

void spell_death_touch(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED2(victim, AFF_UNDEAD)) {
	send_to_char("They are already affected!\n\r", ch);
	return;
    }
    if (ch->side != victim->side && saves_spell(level, victim, DAM_OTHER, SAVE_OFTEN)) {
	send_to_char("Spell failed!\n\r", ch);
	return;
    }
    af.where = TO_AFFECTS2;
    af.type = sn;
    af.level = level;
    af.duration = level / 15;
    af.bitvector = AFF_UNDEAD;
    affect_to_char(victim, &af);
    send_to_char("Your soul is overridden with darkness!\n\r", victim);
    act("$n turns deathly white.", victim, NULL, NULL, TO_ROOM);
    return;
}
extern void start_hating args((CHAR_DATA * ch, CHAR_DATA * victim));

void spell_minion(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    CHAR_DATA *mob;
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    mob = create_mobile(get_mob_index(MOB_MINION));

    send_to_char("You conjure a minion.\n\r", ch);
    char_to_room(mob, ch->in_room);
//        SET_BIT(mob->affected_by, AFF_CHARM);
    //      add_follower( mob, ch );
    //       mob->leader = ch;
    //mob->hate->name = victim->name;
    //mob->hate->who = str_dup(victim->name);
    start_hating(mob, victim);
    mob->side = ch->side;

}

void spell_soul_coil(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int manadam;

    manadam = 100;

//Vashna 07-13-01:Modified to allow random effects on saves vs soil coil.
    if (saves_spell(level, victim, DAM_OTHER, -10)){
        if(!IS_NPC(ch))
	   manadam /= number_range(2,3);
        else
           manadam /= number_range(2,6);
    }

//manadam = UMAX( victim->mana - manadam , 0);

    if (victim->mana - manadam < 0)
	manadam = 0;

//Vashna 07-13-01:Modified to wake folks up when attacked.
//    multi_hit( victim, ch, TYPE_UNDEFINED );
//Recoded Soul Coil so opponents didn't get an extra attack on you. -Zyloch
	if (IS_AFFECTED(victim, AFF_SLEEP)) {
			victim->position = POS_STANDING;
		}		

    ch->mana += manadam;
    victim->mana -= manadam;
//Vashna 07-13-01:Modified so folks will not soil coil multiple times per second.
// What a bad idea. -- Archan
//if (!IS_IMMORTAL(ch))
//{    WAIT_STATE(ch,15);
//}

    send_to_char("Coils burst at you and draw from your soul!!\n\r", victim);
    act("Coils burst at $n and draw from their soul!!", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_wall_of_force(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *mob1;
    CHAR_DATA *mob2;
    CHAR_DATA *rch;

    char *tempStr1;
    char *tempStr2;
    int dir;
    int oppDir;

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {
        if (IS_NPC(rch) && rch->spec_fun == spec_lookup("spec_wall_force")) {
	    send_to_char("\n\rThere is already a wall here.\n\r", ch);
	    return;
        }
    }

    if (target_name[0] == 'n') {
	tempStr1 = "{YA wall of force blocks the north exit.{N\n\r";
	tempStr2 = "{YA wall of force blocks the south exit.{N\n\r";
	dir = 0;
	oppDir = 2;
    } else if (target_name[0] == 'e') {
	tempStr1 = "{YA wall of force blocks the east exit.{N\n\r";
	tempStr2 = "{YA wall of force blocks the west exit.{N\n\r";
	dir = 1;
	oppDir = 3;
    } else if (target_name[0] == 's') {
	tempStr1 = "{YA wall of force blocks the south exit.{N\n\r";
	tempStr2 = "{YA wall of force blocks the north exit.{N\n\r";
	dir = 2;
	oppDir = 0;
    } else if (target_name[0] == 'w') {
	tempStr1 = "{YA wall of force blocks the west exit.{N\n\r";
	tempStr2 = "{YA wall of force blocks the east exit.{N\n\r";
	dir = 3;
	oppDir = 1;
    } else if (target_name[0] == 'u') {
	tempStr1 = "{YA wall of force blocks the up exit.{N\n\r";
	tempStr2 = "{YA wall of force blocks the down exit.{N\n\r";
	dir = 4;
	oppDir = 5;
    } else if (target_name[0] == 'd') {
	tempStr1 = "{YA wall of force blocks the down exit.{N\n\r";
	tempStr2 = "{YA wall of force blocks the up exit.{N\n\r";
	dir = 5;
	oppDir = 4;
    } else {
	send_to_char("Bad direction.\n\r", ch);
	return;
    }

    if (ch->in_room->exit[dir] == NULL) {
	send_to_char("There's no exit in that direction.\n\r", ch);
	return;
    }
    mob1 = create_mobile(get_mob_index(MOB_WALL_OF_FORCE));
    free_string(mob1->long_descr);
    mob1->long_descr = str_dup(tempStr1);
    send_to_char("You conjure up a wall.\n\r", ch);
    char_to_room(mob1, ch->in_room);

    mob2 = create_mobile(get_mob_index(MOB_WALL_OF_FORCE));
    free_string(mob2->long_descr);
    mob2->long_descr = str_dup(tempStr2);
    char_to_room(mob2, ch->in_room->exit[dir]->u1.to_room);

}

void spell_screaming_wall(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    AFFECT_DATA af;
    AFFECT_DATA opp_ap;
    int oppDir;
    int dir;

    if (IS_SET(ch->in_room->affected_by, AFF_SCREAMING_WALL)) {
	send_to_char("There is already a screaming wall here.\n\r", ch);
	return;
    }

    if (target_name[0] == 'n') {
	dir = 0;
	oppDir = 2;
    } else if (target_name[0] == 'e') {
	dir = 1;
	oppDir = 3;
    } else if (target_name[0] == 's') {
	dir = 2;
	oppDir = 0;
    } else if (target_name[0] == 'w') {
	dir = 3;
	oppDir = 1;
    } else if (target_name[0] == 'u') {
	dir = 4;
	oppDir = 5;
    } else if (target_name[0] == 'd') {
	dir = 5;
	oppDir = 4;
    } else {
	send_to_char("Bad direction.\n\r", ch);
	return;
    }

    if (ch->in_room->exit[dir] == NULL) {
	send_to_char("There's no exit in that direction.\n\r", ch);
	return;
    }
    send_to_char("You conjure a screaming wall.\n\r", ch);

//      ch->in_room->exit[dir]->u1.to_room ;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.modifier = dir;
    af.level = level;
    af.duration = level;
    af.bitvector = AFF_SCREAMING_WALL;
    affect_to_room(ch->in_room, &af);

    opp_ap.where = TO_AFFECTS;
    opp_ap.type = sn;
    opp_ap.modifier = oppDir;
    opp_ap.level = level;
    opp_ap.duration = level;
    opp_ap.bitvector = AFF_SCREAMING_WALL;
    affect_to_room(ch->in_room->exit[dir]->u1.to_room, &opp_ap);
}

void spell_flaming_wall(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    AFFECT_DATA af;
    AFFECT_DATA opp_ap;
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

    int oppDir;
    int dir;

    if (IS_SET(ch->in_room->affected_by, AFF_FLAMING_WALL)) {
	send_to_char("There is already a flaming wall here.\n\r", ch);
	return;
    }

    obj1 = create_object(get_obj_index(OBJ_VNUM_WALL_FLAMING), 0);
    obj2 = create_object(get_obj_index(OBJ_VNUM_WALL_FLAMING), 0);

    if (target_name[0] == 'n') {
	dir = 0;
	oppDir = 2;
    } else if (target_name[0] == 'e') {
	dir = 1;
	oppDir = 3;
    } else if (target_name[0] == 's') {
	dir = 2;
	oppDir = 0;
    } else if (target_name[0] == 'w') {
	dir = 3;
	oppDir = 1;
    } else if (target_name[0] == 'u') {
	dir = 4;
	oppDir = 5;
    } else if (target_name[0] == 'd') {
	dir = 5;
	oppDir = 4;
    } else {
	send_to_char("Bad direction.\n\r", ch);
	return;
    }

    sprintf(buf1, obj1->description , dir_name[dir]);
    sprintf(buf2, obj2->description, dir_name[oppDir]);


    if (ch->in_room->exit[dir] == NULL) {
	send_to_char("There's no exit in that direction.\n\r", ch);
	return;
    }

    send_to_char("You conjure a flaming wall.\n\r", ch);

//      ch->in_room->exit[dir]->u1.to_room ;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.modifier = dir;
    af.level = level;
    af.duration = level / 2;
    af.bitvector = AFF_FLAMING_WALL;
    affect_to_room(ch->in_room, &af);
obj1->timer = level / 2;
free_string(obj1-> description);
obj1-> description = str_dup(buf1);
obj_to_room(obj1, ch->in_room);

    opp_ap.where = TO_AFFECTS;
    opp_ap.type = sn;
    opp_ap.modifier = oppDir;
    opp_ap.level = level;
    opp_ap.duration = level / 2;
    opp_ap.bitvector = AFF_FLAMING_WALL;
    affect_to_room(ch->in_room->exit[dir]->u1.to_room, &opp_ap);
obj2->timer = level / 2;

free_string(obj2->description);
obj2->description = str_dup(buf2);
obj_to_room(obj2, ch->in_room->exit[dir]->u1.to_room);
}

void spell_living_wall(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
CHAR_DATA *mob;
    CHAR_DATA *och;
    int num = 0;

send_to_char("A wall appears, then changes its mind and disappears.\n\r", ch);
return;


    for (och = char_list; och != NULL; och = och->next) 
    {
	if (IS_NPC(och))
	    if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch)
		num++;
    }
    if (num > 0) 
    {
	send_to_char("You fail.\n\r", ch);
	return;
    }
    mob = create_mobile(get_mob_index(MOB_LIVING_WALL));

    if (mob == NULL) 
    {
	send_to_char("Nothing happens...\n\r", ch);
	return;
    }
    send_to_char("You conjure up a wall.\n\r", ch);
    char_to_room(mob, ch->in_room);
    SET_BIT(mob->affected_by, AFF_CHARM);
    add_follower(mob, ch);
    mob->leader = ch;

}

void spell_purify(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    AFFECT_DATA *paf;
    paf = victim->affected;

    /* begin running through the spells */

    if (check_dispel(level, victim, skill_lookup("weaken")))
	act("$n looks stronger.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("blindness")))
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("fire breath")))
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, gsn_dirt))
	act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("calm")))
	act("$n no longer looks so peaceful...", victim, NULL, NULL, TO_ROOM);
	
	else if (check_dispel(level, victim, skill_lookup("mana burn")))
	act("$n no longer is spraying mana everywhere...", victim, NULL, NULL, TO_ROOM);
	
	else if (check_dispel(level, victim, skill_lookup("life drain")))
	act("$n no longer is being drained of life.", victim, NULL, NULL, TO_ROOM);	

    else if (check_dispel(level, victim, skill_lookup("chill touch")))
	act("$n looks warmer.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("curse")))
	act("$n is no longer cursed.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("faerie fire")))
	act("$n's outline fades.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("sleep")))
	act("$n can awaken.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("death touch")))
	act("$n no longer looks so white.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("slow")))
	act("$n is no longer moving so slowly.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("entangle")))
	act("$n breaks free!", victim, NULL, NULL, TO_ROOM);

//    else if (check_dispel(level, victim, gsn_trap))
//	act("$n breaks free!", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("youth")))
	act("$n looks older.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("age")))
	act("$n looks younger.", victim, NULL, NULL, TO_ROOM);

	else if (check_dispel(level, victim, skill_lookup("decripify")))
	act("$n look relieved.", victim, NULL, NULL, TO_ROOM);
	
    else if (check_dispel(level, victim, skill_lookup("web")))
	act("$n breaks free!", victim, NULL, NULL, TO_ROOM);

//net
    else if (IS_AFFECTED(victim, AFF_WEB)) 
    {
	    while (paf->bitvector != AFF_WEB && paf->next != NULL)
		paf = paf->next;

	    if (paf->bitvector != AFF_WEB) 
		return;

		    if (paf->duration > 0) 
		    {
			paf->duration--;
			send_to_char("The net loosens!\n\r", ch);
			send_to_char("The net loosens!\n\r", victim);
		    } else {
			affect_remove(ch, paf);
			send_to_char("You are free!\n\r", victim);
			send_to_char("they are free!\n\r", ch);
		    }
	    }//end net

    send_to_char("Ok.\n\r", ch);

}

void spell_nullify(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    CHAR_DATA *victim = (CHAR_DATA *) vo;
//int num_passes;    //spell will kill 2 things at most

//for(num_passes = 1; num_passes <= 2; num_passes++)


//nullify hack to make it hit less
	if (number_range(1,10) == 10)
	{
		send_to_char("You failed.\n\r",ch);
		return;
	}

    if (check_dispel(level, victim, skill_lookup("armor")))
	act("$n no longer looks so protected.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("bless")))
	act("$n no longer looks so holy.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("calm")))
	act("$n no longer looks so peaceful...", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("detect hidden")))
	act("$n is less insightful.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("detect invis")))
	act("$n is less insightful.", victim, NULL, NULL, TO_ROOM);
	
	else if (check_dispel(level, victim, skill_lookup("enhance aura")))
	act("$n no longer has increased attack power.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("frenzy")))
	act("$n no longer looks so wild.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("giant strength")))
	act("$n no longer looks so mighty.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("haste")))
	act("$n is no longer moving so quickly.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("infravision")))
	act("$n's eyes stop glowing..", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("invis")))
	act("$n fades into existance.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("mass invis")))
	act("$n fades into existance.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("pass door")))
	act("$n is no longer so translucent.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("protection evil")))
	act("$n is no longer so protected.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("protection good")))
	act("$n is no longer so protected.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("decay aura")))
	act("$n decay aura fades.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("fly")))
        act("$n falls to the ground!", victim, NULL, NULL, TO_ROOM);
	
    else if (check_dispel(level, victim, skill_lookup("sanctuary")))
	act("The white aura around $n's body vanishes.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("reflect")))
    {
		act("The silver aura around $n's body vanishes.", victim, NULL, NULL, TO_ROOM);
	    if (IS_AFFECTED(victim, AFF_REFLECT)) 
	    {
			REMOVE_BIT(victim->affected_by, AFF_REFLECT);
			act("The silver aura around $n's body vanishes.",
	    	victim, NULL, NULL, TO_ROOM);
    	}
    	if (IS_NPC(victim)
    	&& IS_SET(victim->affected_by, AFF_REFLECT)) 
    	{							//prz work izlude reflect
   	    	REMOVE_BIT(victim->affected_by, AFF_REFLECT);
    		act("The silver aura around $n's body vanishes.", victim, NULL, NULL, TO_ROOM);
		}
	}


    else if (check_dispel(level, victim, skill_lookup("shield")))
	act("The shield protecting $n vanishes.", victim, NULL, NULL, TO_ROOM);

    else if (check_dispel(level, victim, skill_lookup("stone skin")))
	act("$n's skin regains its normal texture.", victim, NULL, NULL, TO_ROOM);

}

void spell_detect_alignment(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (IS_AFFECTED(victim, AFF_DETECT_ALIGNMENT)) {
	if (victim == ch)
	    send_to_char("You can already sense alignments.\n\r", ch);
	else
	    act("$N can already detect alignments.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_DETECT_ALIGNMENT;
    affect_to_char(victim, &af);
    send_to_char("Your eyes tingle.\n\r", victim);
    if (ch != victim)
	send_to_char("Ok.\n\r", ch);
    return;
}

void spell_silence(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 10;
    af.bitvector = AFF_SILENT_ROOM;
    affect_to_room(ch->in_room, &af);

}

void spell_strengthen_ground(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    //incomplete
    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 5;
    af.bitvector = AFF_CANT_TRAP;
    affect_to_room(ch->in_room, &af);

}

void spell_age(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA af2;
    int dam;

     dam = dice(level, DAM_AGE);


    if(IS_AFFECTED2(victim, AFF_AGE))
    {
    if (saves_spell(level, victim, DAM_ENERGY, SAVE_ATTACK)) 
			damage_old(ch, victim, dam / 2, sn, DAM_ENERGY, TRUE);
	    else 
			damage_old(ch, victim, dam, sn, DAM_ENERGY, TRUE);
    return;
    }

	act("$n seems to age before your eyes!", victim, NULL, NULL, TO_ROOM);
	af.where = TO_AFFECTS2;
	af.type = sn;
	af.level = level;
	af.duration = level / 4;
	af.location = APPLY_STR;
	af.modifier = -3;
	af.bitvector = AFF_AGE;
	affect_to_char(victim, &af);

	af2.where = TO_AFFECTS2;
	af2.type = sn;
	af2.level = level;
	af2.duration = level / 4;
	af2.location = APPLY_DEX;
	af2.modifier = -3;
	af2.bitvector = AFF_AGE;
	affect_to_char(victim, &af2);

     if (!IS_SET(victim->affected2_by, AFF_AGE) )
       SET_BIT(victim->affected2_by, AFF_AGE);

    if (saves_spell(level, victim, DAM_ENERGY, SAVE_ATTACK)) 
			damage_old(ch, victim, dam / 2, sn, DAM_ENERGY, TRUE);
	    else 
			damage_old(ch, victim, dam, sn, DAM_ENERGY, TRUE);
    return;
}

void spell_youth(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    AFFECT_DATA af2;
    int dam;

     dam = dice(level, DAM_YOUTH);

    if(IS_AFFECTED2(victim, AFF_YOUTH))
     {
    if (saves_spell(level, victim, DAM_ENERGY, SAVE_ATTACK)) 
			damage_old(ch, victim, dam / 2, sn, DAM_ENERGY, TRUE);
	    else 
			damage_old(ch, victim, dam, sn, DAM_ENERGY, TRUE);
    return;
    }

	act("$n looks younger!", victim, NULL, NULL, TO_ROOM);
	af.where = TO_AFFECTS2;
	af.type = sn;
	af.level = level;
	af.duration = level / 4;
	af.location = APPLY_WIS;
	af.modifier = -3;
	af.bitvector = AFF_YOUTH;
	affect_to_char(victim, &af);

	af2.where = TO_AFFECTS2;
	af2.type = sn;
	af2.level = level;
	af2.duration = level / 4;
	af2.location = APPLY_INT;
	af2.modifier = -3;
	af2.bitvector = AFF_YOUTH;
	affect_to_char(victim, &af2);
    if(!IS_SET(ch->affected2_by, AFF_YOUTH))
      SET_BIT(ch->affected2_by, AFF_YOUTH);

    if (saves_spell(level, victim, DAM_ENERGY, SAVE_ATTACK)) 
			damage_old(ch, victim, dam / 2, sn, DAM_ENERGY, TRUE);
	    else 
			damage_old(ch, victim, dam, sn, DAM_ENERGY, TRUE);
    return;
    return;
}


void spell_decay(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    AFFECT_DATA af2;

     victim = (CHAR_DATA *) vo;

    if (saves_spell(level, victim, DAM_POISON, SAVE_NORMAL)) {
	act("$n looks ill, but recovers.", victim, NULL, NULL, TO_ROOM);
	send_to_char("Your skin is clammy, but feels normal again quickly.\n\r", victim);
	return;
    }
    af2.where = TO_AFFECTS2;
    af2.type = sn;
    af2.level = level;
    af2.duration = level;
    af2.location = APPLY_STR;
    af2.modifier = -1;
    af2.bitvector = AFF_DECAY;
    affect_to_char(victim, &af2);
    send_to_char("Your flesh begins to rot.\n\r", victim);
    act("$n is decaying!", victim, NULL, NULL, TO_ROOM);
    return;
}
//New auras for classes not yet added - 4-10-03
//Chaotic - Magical barrier
void spell_chaotic(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af2;

    if (IS_AFFECTED2(victim, AFF_CHAOTIC)) {
	if (victim == ch)
	    send_to_char("You are already surrounded by chaos.\n\r", ch);
	else
	    act("$N is already in chaos.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af2.where = TO_AFFECTS2;
    af2.type = sn;
    af2.level = level;
    af2.duration = level / 6;
    af2.location = APPLY_NONE;
    af2.modifier = 0;
    af2.bitvector = AFF_CHAOTIC;
    affect_to_char(victim, &af2);
    act("$n is surrounded by a chaotic aura.", victim, NULL, NULL,
	TO_ROOM);
    send_to_char("You are surrounded by a chaotic aura.\n\r", victim);
    return;
}
//Deflective - Melee barrier
void spell_deflective(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af2;

    if (IS_AFFECTED2(victim, AFF_DEFLECT)) {
	if (victim == ch)
	    send_to_char("You are already surrounded by a deflective aura.\n\r", ch);
	else
	    act("$N is already surrounded by a deflective aura.", ch, NULL, victim, TO_CHAR);
	return;
    }
    af2.where = TO_AFFECTS2;
    af2.type = sn;
    af2.level = level;
    af2.duration = level / 6;
    af2.location = APPLY_NONE;
    af2.modifier = 0;
    af2.bitvector = AFF_DEFLECT;
    affect_to_char(victim, &af2);
    act("$n is surrounded by a deflective aura.", victim, NULL, NULL,
	TO_ROOM);
    send_to_char("You are surrounded by a deflective aura.\n\r", victim);
    return;
}

void spell_life_drain(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int chance;

    if (is_affected(victim, skill_lookup("life drain") ))
	return;
	
	chance = number_percent();
	if (get_skill(ch,skill_lookup("life drain")) - 15 > chance) {

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.location = APPLY_DAMROLL;
    af.modifier = -10;
    af.duration = 1;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("You feel as if your life is draining away...\n\r", victim);
    act("$n looks drained and weak.", victim, NULL, NULL, TO_ROOM);
}
    return;
}

void spell_mana_burn(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
	int chance;
    
    if (is_affected(victim, skill_lookup("mana burn")) )
	return;
	
	chance = number_percent();
	if (get_skill(ch,skill_lookup("mana burn")) - 5 > chance) {

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.location = APPLY_HITROLL;
    af.modifier = -10;
    af.duration = 1;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("You are struck with a pain in your chest!\n\r", victim);
    act("$n is stuck with sudden pain.", victim, NULL, NULL, TO_ROOM);
}
    return;
}

void spell_hex(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, skill_lookup("hex") ) || saves_spell(level, victim, DAM_OTHER, SAVE_RARELY))
	return;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.location = APPLY_CON;
    af.modifier = -3;
    af.duration = 1;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    send_to_char("You are cursed with black magic!\n\r", victim);
    act("$n is cursed with black magic!", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_spirit_rain(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    act("$n calls forth a spiral of spirit energies!", ch, NULL, NULL, TO_ROOM);
    send_to_char("You call forth the spirits.\n\r", ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (ch->side != vch->side) {
	
		send_to_char("You are being torn apart!\n\r", vch);	 
		spell_soul_coil(skill_lookup("soul coil"), level-15, ch, (void *) vch, TARGET_CHAR);
		spell_death_coil(skill_lookup("death coil"), level, ch, (void *) vch, TARGET_CHAR);
			}
	else if (ch->side == vch->side && ch != vch) {
		vch->hit += 50;
		vch->mana += 50;
    }
}
    send_to_char("You feel drained.\n\r", ch);
}

void spell_tracer_cloud(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int chance;
    
    chance = number_percent();
    if (chance < get_skill(ch,skill_lookup("tracer cloud")-5)) { 

    send_to_char("You summon a tracer cloud.\n\r", ch);

	af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 3;
    af.location = APPLY_SAVING_SPELL;
    af.modifier = 2;
    af.bitvector = 0;
    affect_join(victim, &af);
}
}

void spell_enhance_aura(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, skill_lookup("enhance aura"))) {
	    send_to_char("Already affected!!\n\r", ch);
	return;
    }
    
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = number_range(11,12);
    af.modifier = number_fuzzy(4);
    af.location = APPLY_CON;
    af.bitvector = 0;
    affect_to_char(victim, &af); 
    send_to_char("You reach your potential attack power!\n\r", victim);
    act("$n's reaches $s full attack power!", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_spirit_blow(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int chance;
    
    chance = number_percent();
    if (chance < 75 && !is_affected(victim,skill_lookup("spirit blow"))) { 

    act("You hit $n's spirit with a powerful blow!", victim, NULL, victim, TO_CHAR);
    act("Your spirit is shaken, and you stumble!", victim, NULL, victim, TO_VICT);
    
    WAIT_STATE(victim,PULSE_VIOLENCE*2);

	af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 0;
    af.location = APPLY_DEX;
    af.modifier = -1;
    af.bitvector = 0;
    affect_join(victim, &af);
	}
	return;
}

void spell_decripify(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, sn) )
	return;
	
	if (saves_spell(level, victim, DAM_OTHER, SAVE_RARELY))
	return;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.location = APPLY_HITROLL;
    af.modifier = -100;
    af.duration = 0;
    af.bitvector = AFF_WEAKEN;
    affect_to_char(victim, &af);
    send_to_char("You feel clumsy and weak...\n\r", victim);
    act("$n looks clumsy and weak.", victim, NULL, NULL, TO_ROOM);
    return;
}

void spell_decay_aura(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if (is_affected(victim, skill_lookup("decay aura"))) {
	if (victim == ch)
	    send_to_char("You are already surrounded by a decay aura.\n\r", ch);
	else
	    act("$N is already surrounded by a decay aura", ch, NULL, victim, TO_CHAR);
	return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 6;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = 0;
    affect_to_char(victim, &af);
    act("$n is surrounded by a decay aura", victim, NULL, NULL,
	TO_ROOM);
    send_to_char("You are surrounded by a decay aura.\n\r", victim);
    return;
}
