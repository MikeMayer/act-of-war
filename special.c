
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

/* command procedures needed */
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_open);
DECLARE_DO_FUN(do_close);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_murder);

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(spec_breath_any);
DECLARE_SPEC_FUN(spec_breath_acid);
DECLARE_SPEC_FUN(spec_breath_fire);
DECLARE_SPEC_FUN(spec_breath_frost);
DECLARE_SPEC_FUN(spec_breath_gas);
DECLARE_SPEC_FUN(spec_breath_lightning);
DECLARE_SPEC_FUN(spec_cast_adept);
DECLARE_SPEC_FUN(spec_cast_cleric);
DECLARE_SPEC_FUN(spec_cast_judge);
DECLARE_SPEC_FUN(spec_cast_mage);
DECLARE_SPEC_FUN(spec_cast_undead);
DECLARE_SPEC_FUN(spec_executioner);
DECLARE_SPEC_FUN(spec_fido);
DECLARE_SPEC_FUN(spec_guard);
DECLARE_SPEC_FUN(spec_king);
DECLARE_SPEC_FUN(spec_guard_tier);
DECLARE_SPEC_FUN(spec_guard_tier2);
DECLARE_SPEC_FUN(spec_guard_evil);
DECLARE_SPEC_FUN(spec_guard_evil2);
DECLARE_SPEC_FUN(spec_guard_kael);
DECLARE_SPEC_FUN(spec_guard_kael2);
DECLARE_SPEC_FUN(spec_guard_cairn); //Cairn guards are disabled - Akuji
DECLARE_SPEC_FUN(spec_guard_cairn2);//Here too
DECLARE_SPEC_FUN(spec_janitor);
DECLARE_SPEC_FUN(spec_mayor);
DECLARE_SPEC_FUN(spec_poison);
DECLARE_SPEC_FUN(spec_survey);
DECLARE_SPEC_FUN(spec_thief);
DECLARE_SPEC_FUN(spec_nasty);
DECLARE_SPEC_FUN(spec_troll_member);
DECLARE_SPEC_FUN(spec_ogre_member);
DECLARE_SPEC_FUN(spec_patrolman);
DECLARE_SPEC_FUN(spec_questmaster);	/* Vassago */
DECLARE_SPEC_FUN(spec_guildWarriorLow);
DECLARE_SPEC_FUN(spec_guildWarriorHigh);
DECLARE_SPEC_FUN(spec_guildBarbarianLow);
DECLARE_SPEC_FUN(spec_guildBarbarianHigh);
DECLARE_SPEC_FUN(spec_guildKnightLow);
DECLARE_SPEC_FUN(spec_guildKnightHigh);
DECLARE_SPEC_FUN(spec_guildDeathKnightLow);
DECLARE_SPEC_FUN(spec_guildDeathKnightHigh);
DECLARE_SPEC_FUN(spec_guildMageLow);
DECLARE_SPEC_FUN(spec_guildMageHigh);
DECLARE_SPEC_FUN(spec_guildWarlockLow);
DECLARE_SPEC_FUN(spec_guildWarlockHigh);
DECLARE_SPEC_FUN(spec_guildDefilerLow);
DECLARE_SPEC_FUN(spec_guildDefilerHigh);
DECLARE_SPEC_FUN(spec_guildWitchLow);
DECLARE_SPEC_FUN(spec_guildWitchHigh);
DECLARE_SPEC_FUN(spec_guildIllusionistLow);
DECLARE_SPEC_FUN(spec_guildIllusionistHigh);
DECLARE_SPEC_FUN(spec_guildDruidLow);
DECLARE_SPEC_FUN(spec_guildDruidHigh);
DECLARE_SPEC_FUN(spec_guildMonkLow);
DECLARE_SPEC_FUN(spec_guildMonkHigh);
DECLARE_SPEC_FUN(spec_guildClericLow);
DECLARE_SPEC_FUN(spec_guildClericHigh);
DECLARE_SPEC_FUN(spec_guildAssassinLow);
DECLARE_SPEC_FUN(spec_guildAssassinHigh);
DECLARE_SPEC_FUN(spec_guildShedaklahLow);
DECLARE_SPEC_FUN(spec_guildShedaklahHigh);
DECLARE_SPEC_FUN(spec_guildBardLow);
DECLARE_SPEC_FUN(spec_guildBardHigh);
DECLARE_SPEC_FUN(spec_guildRangerLow);
DECLARE_SPEC_FUN(spec_guildRangerHigh);
DECLARE_SPEC_FUN(spec_guildNecromancerLow);
DECLARE_SPEC_FUN(spec_guildNecromancerHigh);
DECLARE_SPEC_FUN(spec_wall_force);

/* the function table */
const struct spec_type spec_table[] =
{
    {"spec_breath_any", spec_breath_any},
    {"spec_breath_acid", spec_breath_acid},
    {"spec_breath_fire", spec_breath_fire},
    {"spec_breath_frost", spec_breath_frost},
    {"spec_breath_gas", spec_breath_gas},
    {"spec_breath_lightning", spec_breath_lightning},
    {"spec_cast_adept", spec_cast_adept},
    {"spec_cast_cleric", spec_cast_cleric},
    {"spec_cast_judge", spec_cast_judge},
    {"spec_cast_mage", spec_cast_mage},
    {"spec_cast_undead", spec_cast_undead},
    {"spec_executioner", spec_executioner},
    {"spec_fido", spec_fido},
    {"spec_guard", spec_guard},
    {"spec_king", spec_king},
    {"spec_guard_tier", spec_guard_tier},
    {"spec_guard_tier2", spec_guard_tier2},
    {"spec_guard_evil", spec_guard_evil},
    {"spec_guard_evil", spec_guard_evil2},
    {"spec_guard_kael", spec_guard_kael},
    {"spec_guard_kael", spec_guard_kael2},
    {"spec_guard_cairn", spec_guard_cairn},
    {"spec_guard_cairn", spec_guard_cairn2},
    {"spec_janitor", spec_janitor},
    {"spec_mayor", spec_mayor},
    {"spec_poison", spec_poison},
    {"spec_survey", spec_survey},
    {"spec_thief", spec_thief},
    {"spec_nasty", spec_nasty},
    {"spec_troll_member", spec_troll_member},
    {"spec_ogre_member", spec_ogre_member},
    {"spec_patrolman", spec_patrolman},
    {"spec_questmaster", spec_questmaster},
    {"spec_guildWarriorLow", spec_guildWarriorLow},
    {"spec_guildWarriorHigh", spec_guildWarriorHigh},
    {"spec_guildBarbarianLow", spec_guildBarbarianLow},
    {"spec_guildBarbarianHigh", spec_guildBarbarianHigh},
    {"spec_guildKnightLow", spec_guildKnightLow},
    {"spec_guildKnightHigh", spec_guildKnightHigh},
    {"spec_guildDeathKnightLow", spec_guildDeathKnightLow},
    {"spec_guildDeathKnightHigh", spec_guildDeathKnightHigh},
    {"spec_guildMageLow", spec_guildMageLow},
    {"spec_guildMageHigh", spec_guildMageHigh},
    {"spec_guildWarlockLow", spec_guildWarlockLow},
    {"spec_guildWarlockHigh", spec_guildWarlockHigh},
    {"spec_guildDefilerLow", spec_guildDefilerLow},
    {"spec_guildDefilerHigh", spec_guildDefilerHigh},
    {"spec_guildWitchLow", spec_guildWitchLow},
    {"spec_guildWitchHigh", spec_guildWitchHigh},
    {"spec_guildIllusionistLow", spec_guildIllusionistLow},
    {"spec_guildIllusionistHigh", spec_guildIllusionistHigh},
    {"spec_guildDruidLow", spec_guildDruidLow},
    {"spec_guildDruidHigh", spec_guildDruidHigh},
    {"spec_guildMonkLow", spec_guildMonkLow},
    {"spec_guildMonkHigh", spec_guildMonkHigh},
    {"spec_guildClericLow", spec_guildClericLow},
    {"spec_guildClericHigh", spec_guildClericHigh},
    {"spec_guildAssassinLow", spec_guildAssassinLow},
    {"spec_guildAssassinHigh", spec_guildAssassinHigh},
    {"spec_guildShedaklahLow", spec_guildShedaklahLow},
    {"spec_guildShedaklahHigh", spec_guildShedaklahHigh},
    {"spec_guildRangerLow", spec_guildRangerLow},
    {"spec_guildRangerHigh", spec_guildRangerHigh},
    {"spec_guildBardLow", spec_guildBardLow},
    {"spec_guildBardHigh", spec_guildBardHigh},
    {"spec_guildNecromancerLow", spec_guildNecromancerLow},
    {"spec_guildNecromancerHigh", spec_guildNecromancerHigh},
    {"spec_wall_force", spec_wall_force},
    {NULL, NULL}
};

/*
 * Given a name, return the appropriate spec fun.
 */

SPEC_FUN *spec_lookup(const char *name)
{
    int i;

    for (i = 0; spec_table[i].name != NULL; i++) {
	if (LOWER(name[0]) == LOWER(spec_table[i].name[0])
	    && !str_prefix(name, spec_table[i].name))
	    return spec_table[i].function;
    }

    return 0;
}

char *spec_name(SPEC_FUN * function)
{
    int i;

    for (i = 0; spec_table[i].function != NULL; i++) {
	if (function == spec_table[i].function)
	    return spec_table[i].name;
    }

    return NULL;
}

bool spec_troll_member(CHAR_DATA * ch)
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;

    if (!IS_AWAKE(ch) || ch->in_room == NULL
	|| IS_AFFECTED(ch, AFF_CHARM) || ch->fighting != NULL)
	return FALSE;

    /* find an ogre to beat up */
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
	if (!IS_NPC(vch) || ch == vch)
	    continue;

	if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
	    return FALSE;

	if (vch->pIndexData->group == GROUP_VNUM_OGRES
	    && ch->level > vch->level - 2 && !is_safe(ch, vch)) {
	    if (number_range(0, count) == 0)
		victim = vch;

	    count++;
	}
    }

    if (victim == NULL)
	return FALSE;

    /* say something, then raise hell */
    switch (number_range(0, 6)) {
    default:
	message = NULL;
	break;
    case 0:
	message = "$n yells 'I've been looking for you, punk!'";
	break;
    case 1:
	message = "With a scream of rage, $n attacks $N.";
	break;
    case 2:
	message =
	    "$n says 'What's slimy Ogre trash like you doing around here?'";
	break;
    case 3:
	message = "$n cracks his knuckles and says 'Do ya feel lucky?'";
	break;
    case 4:
	message = "$n says 'There's no cops to save you this time!'";
	break;
    case 5:
	message = "$n says 'Time to join your brother, spud.'";
	break;
    case 6:
	message = "$n says 'Let's rock.'";
	break;
    }

    if (message != NULL)
	act(message, ch, NULL, victim, TO_ALL);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return TRUE;
}

bool spec_ogre_member(CHAR_DATA * ch)
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;

    if (!IS_AWAKE(ch) || ch->in_room == NULL
	|| IS_AFFECTED(ch, AFF_CHARM) || ch->fighting != NULL)
	return FALSE;

    /* find an troll to beat up */
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
	if (!IS_NPC(vch) || ch == vch)
	    continue;

	if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
	    return FALSE;

	if (vch->pIndexData->group == GROUP_VNUM_TROLLS
	    && ch->level > vch->level - 2 && !is_safe(ch, vch)) {
	    if (number_range(0, count) == 0)
		victim = vch;

	    count++;
	}
    }

    if (victim == NULL)
	return FALSE;

    /* say something, then raise hell */
    switch (number_range(0, 6)) {
    default:
	message = NULL;
	break;
    case 0:
	message = "$n yells 'I've been looking for you, punk!'";
	break;
    case 1:
	message = "With a scream of rage, $n attacks $N.'";
	break;
    case 2:
	message =
	    "$n says 'What's Troll filth like you doing around here?'";
	break;
    case 3:
	message = "$n cracks his knuckles and says 'Do ya feel lucky?'";
	break;
    case 4:
	message = "$n says 'There's no cops to save you this time!'";
	break;
    case 5:
	message = "$n says 'Time to join your brother, spud.'";
	break;
    case 6:
	message = "$n says 'Let's rock.'";
	break;
    }

    if (message != NULL)
	act(message, ch, NULL, victim, TO_ALL);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return TRUE;
}

bool spec_questmaster(CHAR_DATA * ch)
{
    if (ch->fighting != NULL)
	return spec_cast_mage(ch);
    return FALSE;
}

bool spec_patrolman(CHAR_DATA * ch)
{
    CHAR_DATA *vch, *victim = NULL;
    OBJ_DATA *obj;
    char *message;
    int count = 0;

    if (!IS_AWAKE(ch) || ch->in_room == NULL
	|| IS_AFFECTED(ch, AFF_CHARM) || ch->fighting != NULL)
	return FALSE;

    /* look for a fight in the room */
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
	if (vch == ch)
	    continue;

	if (vch->fighting != NULL) {	/* break it up! */
	    if (number_range(0, count) == 0)
		victim = (vch->level > vch->fighting->level)
		    ? vch : vch->fighting;
	    count++;
	}
    }

    if (victim == NULL || (IS_NPC(victim) && victim->spec_fun == ch->spec_fun))
	return FALSE;

    if (((obj = get_eq_char(ch, WEAR_NECK_1)) != NULL
	 && obj->pIndexData->vnum == OBJ_VNUM_WHISTLE)
	|| ((obj = get_eq_char(ch, WEAR_NECK_2)) != NULL
	    && obj->pIndexData->vnum == OBJ_VNUM_WHISTLE)) {
	act("You blow down hard on $p.", ch, obj, NULL, TO_CHAR);
	act("$n blows on $p, ***WHEEEEEEEEEEEET***", ch, obj, NULL, TO_ROOM);

	for (vch = char_list; vch != NULL; vch = vch->next) {
	    if (vch->in_room == NULL)
		continue;

	    if (vch->in_room != ch->in_room
		&& vch->in_room->area == ch->in_room->area)
		send_to_char("You hear a shrill whistling sound.\n\r", vch);
	}
    }
    switch (number_range(0, 6)) {
    default:
	message = NULL;
	break;
    case 0:
	message = "$n yells 'All roit! All roit! break it up!'";
	break;
    case 1:
	message =
	    "$n says 'Society's to blame, but what's a bloke to do?'";
	break;
    case 2:
	message =
	    "$n mumbles 'bloody kids will be the death of us all.'";
	break;
    case 3:
	message = "$n shouts 'Stop that! Stop that!' and attacks.";
	break;
    case 4:
	message = "$n pulls out his billy and goes to work.";
	break;
    case 5:
	message =
	    "$n sighs in resignation and proceeds to break up the fight.";
	break;
    case 6:
	message = "$n says 'Settle down, you hooligans!'";
	break;
    }

    if (message != NULL)
	act(message, ch, NULL, NULL, TO_ALL);

    multi_hit(ch, victim, TYPE_UNDEFINED);

    return TRUE;
}

bool spec_nasty(CHAR_DATA * ch)
{
    CHAR_DATA *victim, *v_next;
    long gold;

    if (!IS_AWAKE(ch)) {
	return FALSE;
    }
    if (ch->position != POS_FIGHTING) {
	for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	    v_next = victim->next_in_room;
	    if (!IS_NPC(victim)
		&& (victim->level > ch->level)
		&& (victim->level < ch->level + 10)) {
		do_backstab(ch, victim->name);
		if (ch->position != POS_FIGHTING)
		    do_murder(ch, victim->name);
		/* should steal some coins right away? :) */
		return TRUE;
	    }
	}
	return FALSE;		/*  No one to attack */
    }
    /* okay, we must be fighting.... steal some coins and flee */
    if ((victim = ch->fighting) == NULL)
	return FALSE;		/* let's be paranoid.... */

    switch (number_bits(2)) {
    case 0:
	act("$n rips apart your coin purse, spilling your gold!",
	    ch, NULL, victim, TO_VICT);
	act("You slash apart $N's coin purse and gather his gold.",
	    ch, NULL, victim, TO_CHAR);
	act("$N's coin purse is ripped apart!",
	    ch, NULL, victim, TO_NOTVICT);
	gold = victim->gold / 10;	/* steal 10% of his gold */
	victim->gold -= gold;
	ch->gold += gold;
	return TRUE;

    case 1:
	do_flee(ch, "");
	return TRUE;

    default:
	return FALSE;
    }
}

/*
 * Core procedure for dragons.
 */

bool dragon(CHAR_DATA * ch, char *spell_name)
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int sn;

    if (ch->position != POS_FIGHTING)
	return FALSE;

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;
	if (victim->fighting == ch && number_bits(3) == 0)
	    break;
    }

    if (victim == NULL)
	return FALSE;

    if ((sn = skill_lookup(spell_name)) < 0)
	return FALSE;
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

/*
 * Special procedures for mobiles.
 */

bool spec_breath_any(CHAR_DATA * ch)
{
    if (ch->position != POS_FIGHTING)
	return FALSE;

    switch (number_bits(3)) {
    case 0:
	return spec_breath_fire(ch);
    case 1:
    case 2:
	return spec_breath_lightning(ch);
    case 3:
	return spec_breath_gas(ch);
    case 4:
	return spec_breath_acid(ch);
    case 5:
    case 6:
    case 7:
	return spec_breath_frost(ch);
    }

    return FALSE;
}

bool spec_breath_acid(CHAR_DATA * ch)
{
    return dragon(ch, "acid breath");
}

bool spec_breath_fire(CHAR_DATA * ch)
{
    return dragon(ch, "fire breath");
}

bool spec_breath_frost(CHAR_DATA * ch)
{
    return dragon(ch, "frost breath");
}

bool spec_breath_gas(CHAR_DATA * ch)
{
    int sn;

    if (ch->position != POS_FIGHTING)
	return FALSE;

    if ((sn = skill_lookup("gas breath")) < 0)
	return FALSE;
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, NULL, TARGET_CHAR);
    return TRUE;
}

bool spec_breath_lightning(CHAR_DATA * ch)
{
    return dragon(ch, "lightning breath");
}

bool spec_cast_adept(CHAR_DATA * ch)
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if (!IS_AWAKE(ch))
	return FALSE;

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;
	if (victim != ch && can_see(ch, victim) && number_bits(1) == 0
	    && !IS_NPC(victim) && victim->level < 11)
	    break;
    }

    if (victim == NULL)
	return FALSE;

    switch (number_bits(4)) {
    case 0:
	act("$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM);
	spell_armor(skill_lookup("armor"), ch->level, ch, victim, TARGET_CHAR);
	return TRUE;

    case 1:
	act("$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM);
	spell_bless(skill_lookup("bless"), ch->level, ch, victim, TARGET_CHAR);
	return TRUE;

    case 2:
	act("$n utters the words 'judicandus noselacri'.", ch, NULL, NULL, TO_ROOM);
	spell_cure_blindness(skill_lookup("cure blindness"),
			     ch->level, ch, victim, TARGET_CHAR);
	return TRUE;

    case 3:
	act("$n utters the words 'judicandus dies'.", ch, NULL, NULL, TO_ROOM);
	spell_cure_light(skill_lookup("cure light"),
			 ch->level, ch, victim, TARGET_CHAR);
	return TRUE;

    case 4:
	act("$n utters the words 'judicandus sausabru'.", ch, NULL, NULL, TO_ROOM);
	spell_cure_poison(skill_lookup("cure poison"),
			  ch->level, ch, victim, TARGET_CHAR);
	return TRUE;

    case 5:
	act("$n utters the word 'candusima'.", ch, NULL, NULL, TO_ROOM);
	spell_refresh(skill_lookup("refresh"), ch->level, ch, victim, TARGET_CHAR);
	return TRUE;

    case 6:
	act("$n utters the words 'judicandus eugzagz'.", ch, NULL, NULL, TO_ROOM);
	spell_cure_disease(skill_lookup("cure disease"),
			   ch->level, ch, victim, TARGET_CHAR);
    }

    return FALSE;
}

bool spec_cast_cleric(CHAR_DATA * ch)
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if (ch->position != POS_FIGHTING)
	return FALSE;

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;
	if (victim->fighting == ch && number_bits(2) == 0)
	    break;
    }

    if (victim == NULL)
	return FALSE;

    for (;;) {
	int min_level;

	switch (number_bits(4)) {
	case 0:
	    min_level = 0;
	    spell = "blindness";
	    break;
	case 1:
	    min_level = 3;
	    spell = "cause serious";
	    break;
	case 2:
	    min_level = 7;
	    spell = "earthquake";
	    break;
	case 3:
	    min_level = 9;
	    spell = "cause critical";
	    break;
	case 4:
	    min_level = 10;
	    spell = "dispel evil";
	    break;
	case 5:
	    min_level = 12;
	    spell = "curse";
	    break;
	case 6:
	    min_level = 12;
	    spell = "change sex";
	    break;
	case 7:
	    min_level = 13;
	    spell = "flamestrike";
	    break;
	case 8:
	case 9:
	case 10:
	    min_level = 15;
	    spell = "harm";
	    break;
	case 11:
	    min_level = 15;
	    spell = "plague";
	    break;
	default:
	    min_level = 16;
	    spell = "dispel magic";
	    break;
	}

	if (ch->level >= min_level)
	    break;
    }

    if ((sn = skill_lookup(spell)) < 0)
	return FALSE;
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

bool spec_cast_judge(CHAR_DATA * ch)
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if (ch->position != POS_FIGHTING)
	return FALSE;

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;
	if (victim->fighting == ch && number_bits(2) == 0)
	    break;
    }

    if (victim == NULL)
	return FALSE;

    spell = "high explosive";
    if ((sn = skill_lookup(spell)) < 0)
	return FALSE;
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

bool spec_cast_mage(CHAR_DATA * ch)
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if (ch->position != POS_FIGHTING)
	return FALSE;

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;
	if (victim->fighting == ch && number_bits(2) == 0)
	    break;
    }

    if (victim == NULL)
	return FALSE;

    for (;;) {
	int min_level;

	switch (number_bits(4)) {
	case 0:
	    min_level = 0;
	    spell = "blindness";
	    break;
	case 1:
	    min_level = 3;
	    spell = "chill touch";
	    break;
	case 2:
	    min_level = 7;
	    spell = "weaken";
	    break;
	case 3:
	    min_level = 8;
	    spell = "blindness";
	    break;
	case 4:
	    min_level = 11;
	    spell = "colour spray";
	    break;
	case 5:
	    min_level = 12;
	    spell = "change sex";
	    break;
	case 6:
	    min_level = 13;
	    spell = "energy drain";
	    break;
	case 7:
	case 8:
	case 9:
	    min_level = 15;
	    spell = "fireball";
	    break;
	case 10:
	    min_level = 20;
	    spell = "plague";
	    break;
	default:
	    min_level = 20;
	    spell = "acid blast";
	    break;
	}

	if (ch->level >= min_level)
	    break;
    }

    if ((sn = skill_lookup(spell)) < 0)
	return FALSE;
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

bool spec_cast_undead(CHAR_DATA * ch)
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if (ch->position != POS_FIGHTING)
	return FALSE;

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;
	if (victim->fighting == ch && number_bits(2) == 0)
	    break;
    }

    if (victim == NULL)
	return FALSE;

    for (;;) {
	int min_level;

	switch (number_bits(4)) {
	case 0:
	    min_level = 0;
	    spell = "curse";
	    break;
	case 1:
	    min_level = 3;
	    spell = "weaken";
	    break;
	case 2:
	    min_level = 6;
	    spell = "chill touch";
	    break;
	case 3:
	    min_level = 9;
	    spell = "blindness";
	    break;
	case 4:
	    min_level = 12;
	    spell = "poison";
	    break;
	case 5:
	    min_level = 15;
	    spell = "energy drain";
	    break;
	case 6:
	    min_level = 18;
	    spell = "harm";
	    break;
	case 7:
	    min_level = 21;
	    spell = "poison";
	    break;
	case 8:
	    min_level = 20;
	    spell = "plague";
	    break;
	default:
	    min_level = 18;
	    spell = "harm";
	    break;
	}

	if (ch->level >= min_level)
	    break;
    }

    if ((sn = skill_lookup(spell)) < 0)
	return FALSE;
    (*skill_table[sn].spell_fun) (sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

bool spec_executioner(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (!IS_AWAKE(ch) || ch->fighting != NULL)
	return FALSE;

    crime = "";
    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER)
	    && can_see(ch, victim)) {
	    crime = "KILLER";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF)
	    && can_see(ch, victim)) {
	    crime = "THIEF";
	    break;
	}
    }

    if (victim == NULL)
	return FALSE;

    sprintf(buf, "%s is a %s!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!",
	    victim->name, crime);
    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
    do_yell(ch, buf);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return TRUE;
}

bool spec_fido(CHAR_DATA * ch)
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if (!IS_AWAKE(ch))
	return FALSE;

    for (corpse = ch->in_room->contents; corpse != NULL; corpse = c_next) {
	c_next = corpse->next_content;
	if (corpse->item_type != ITEM_CORPSE_NPC)
	    continue;

	act("$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM);
	for (obj = corpse->contains; obj; obj = obj_next) {
	    obj_next = obj->next_content;
	    obj_from_obj(obj);
	    obj_to_room(obj, ch->in_room);
	}
	extract_obj(corpse);
	return TRUE;
    }

    return FALSE;
}
bool spec_king(CHAR_DATA * ch)
{
    return TRUE;

}

bool spec_guard(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (!IS_AWAKE(ch))
	return FALSE;

    if (armageddon)
    return FALSE;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_NONE) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_NONE) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "GUARDS GUARDS!! I have %s cornered at %s.", pers_buf, ch->in_room->name);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    ch->hunting = victim;
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_guard_kael(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

   ch->side = SIDE_KAEL;

    if (!IS_AWAKE(ch))
	return FALSE;

    if (armageddon)
    return FALSE;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_KAEL) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_KAEL) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "DEFENDERS OF KAEL!! I have %s cornered at %s.", pers_buf, ch->in_room->name);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    ch->hunting = victim;
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_kael")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_guard_kael2(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (!IS_AWAKE(ch))
	return FALSE;
    if (armageddon)
    return FALSE;

    ch->side = SIDE_KAEL;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_KAEL) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_KAEL) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "DEFENDERS OF KAEL!! I have %s cornered at %s.", pers_buf, ch->in_room->name);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_kael")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_guard_evil(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (!IS_AWAKE(ch))
	return FALSE;

    if (armageddon)
    return FALSE;

ch->side = SIDE_UM;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_UM) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_UM) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "INVASION ARGHH DIE!! %s found, kill them at %s.", pers_buf, ch->in_room->name);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    ch->hunting = victim;
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_evil")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_guard_evil2(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if(armageddon)
    return FALSE;
ch->side = SIDE_UM;

    if (!IS_AWAKE(ch))
	return FALSE;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_UM) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_UM) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "INVASION ARGHH DIE!! %s found, kill them at %s.", pers_buf, ch->in_room->name);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_evil")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_guard_tier(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (armageddon)
    return FALSE;

ch->side = SIDE_TIER;

    if (!IS_AWAKE(ch))
	return FALSE;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_TIER) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_TIER) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "WE ARE UNDER ATTACK!! Soldiers rally at %s to kill %s.", ch->in_room->name, pers_buf);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    ch->hunting = victim;
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_tier")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_guard_tier2(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (armageddon)
    return FALSE;

ch->side = SIDE_TIER;

    if (!IS_AWAKE(ch))
	return FALSE;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_TIER) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_TIER) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "WE ARE UNDER ATTACK!! Soldiers rally at %s to kill %s.", ch->in_room->name, pers_buf);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_tier")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_guard_cairn(CHAR_DATA * ch)
{
	
	//    return FALSE;
	    
	    
	   // ****     Cairn Guards are disabled! ****

    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (armageddon)
    return FALSE;

    if (!IS_AWAKE(ch))
	return FALSE;

ch->side = SIDE_CAIRN;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_CAIRN) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_CAIRN) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "I HAVE FOUND THE ENEMY!!  Soldiers rally at %s to slay %s.", ch->in_room->name, pers_buf);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    ch->hunting = victim;
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_cairn")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    } 
    return FALSE;
}

bool spec_guard_cairn2(CHAR_DATA * ch)
{
	//****     Cairn Guards are disabled! ****
	
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if (armageddon)
    return FALSE;

    if (!IS_AWAKE(ch))
	return FALSE;

   ch->side = SIDE_CAIRN;

    crime = "";

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER) && can_see(ch, victim)) {
	    crime = "TRAITOR";
	    break;
	}
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF) && can_see(ch, victim)) {
	    crime = "CRIMINAL";
	    break;
	}
	if ((!IS_NPC(victim) && GET_SIDE(victim) != SIDE_CAIRN) && can_see(ch, victim) && victim->level < 101) {
	    break;
	}
    }

    if (victim != NULL) {
	if (!IS_NPC(victim) && GET_SIDE(victim) != SIDE_CAIRN) {
	    CHAR_DATA *mob;
	    char pers_buf[MAX_STRING_LENGTH];

	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "I HAVE FOUND THE ENEMY!!  Soldiers rally at %s to slay %s.", ch->in_room->name, pers_buf);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    if (ch->fighting == NULL) {
		do_yell(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
	    }
	    for (mob = char_list; mob != NULL; mob = mob->next) {
		if (!IS_NPC(mob))
		    continue;

		if (mob->spec_fun == NULL)
		    continue;

		if (mob->in_room->area == ch->in_room->area) {
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guard_cairn")) {
			mob->hunting = victim;
		    }
		}
	    }

	    return TRUE;
	} else {
	    sprintf(buf, "%s is a %s!  ASSISTANCE IS NEEDED!!", victim->name, crime);
	    REMOVE_BIT(ch->comm, COMM_NOSHOUT);
	    do_yell(ch, buf);
	    multi_hit(ch, victim, TYPE_UNDEFINED);
	    return TRUE;
	}
    }
    return FALSE;
}

bool spec_janitor(CHAR_DATA * ch)
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if (!IS_AWAKE(ch))
	return FALSE;

    for (trash = ch->in_room->contents; trash != NULL; trash = trash_next) {
	trash_next = trash->next_content;
	if (!IS_SET(trash->wear_flags, ITEM_TAKE) || !can_loot(ch, trash))
	    continue;
	if (trash->item_type == ITEM_DRINK_CON
	    || trash->item_type == ITEM_TRASH
	    || trash->cost < 10) {
	    act("$n picks up some trash.", ch, NULL, NULL, TO_ROOM);
	    obj_from_room(trash);
	    obj_to_char(trash, ch);
	    return TRUE;
	}
    }

    return FALSE;
}

bool spec_mayor(CHAR_DATA * ch)
{
    static const char open_path[] =
    "W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

    static const char close_path[] =
    "W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

    static const char *path;
    static int pos;
    static bool move;

    if (!move) {
	if (time_info.hour == 6) {
	    path = open_path;
	    move = TRUE;
	    pos = 0;
	}
	if (time_info.hour == 20) {
	    path = close_path;
	    move = TRUE;
	    pos = 0;
	}
    }
    if (ch->fighting != NULL)
	return spec_cast_mage(ch);
    if (!move || ch->position < POS_SLEEPING)
	return FALSE;

    switch (path[pos]) {
    case '0':
    case '1':
    case '2':
    case '3':
	move_char(ch, path[pos] - '0', FALSE);
	break;

    case 'W':
	ch->position = POS_STANDING;
	act("$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM);
	break;

    case 'S':
	ch->position = POS_SLEEPING;
	act("$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM);
	break;

    case 'a':
	act("$n says 'Hello Honey!'", ch, NULL, NULL, TO_ROOM);
	break;

    case 'b':
	act("$n says 'What a view!  I must do something about that dump!'",
	    ch, NULL, NULL, TO_ROOM);
	break;

    case 'c':
	act("$n says 'Vandals!  Youngsters have no respect for anything!'",
	    ch, NULL, NULL, TO_ROOM);
	break;

    case 'd':
	act("$n says 'Good day, citizens!'", ch, NULL, NULL, TO_ROOM);
	break;

    case 'e':
	act("$n says 'I hereby declare the city of Midgaard open!'",
	    ch, NULL, NULL, TO_ROOM);
	break;

    case 'E':
	act("$n says 'I hereby declare the city of Midgaard closed!'",
	    ch, NULL, NULL, TO_ROOM);
	break;

    case 'O':
/*      do_unlock( ch, "gate" ); */
	do_open(ch, "gate");
	break;

    case 'C':
	do_close(ch, "gate");
/*      do_lock( ch, "gate" ); */
	break;

    case '.':
	move = FALSE;
	break;
    }

    pos++;
    return FALSE;
}

bool spec_poison(CHAR_DATA * ch)
{
    CHAR_DATA *victim;

    if (ch->position != POS_FIGHTING
	|| (victim = ch->fighting) == NULL
	|| number_percent() > 2 * ch->level)
	return FALSE;

    act("You bite $N!", ch, NULL, victim, TO_CHAR);
    act("$n bites $N!", ch, NULL, victim, TO_NOTVICT);
    act("$n bites you!", ch, NULL, victim, TO_VICT);
    spell_poison(gsn_poison, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}

bool spec_survey(CHAR_DATA * ch)
{

    return TRUE;
}

bool spec_guildWarriorLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildWarriorHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildBarbarianLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildBarbarianHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildKnightLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildKnightHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildDeathKnightLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildDeathKnightHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildMageLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildMageHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildWarlockLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildWarlockHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildDefilerLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildDefilerHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildWitchLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildWitchHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildIllusionistLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildIllusionistHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildDruidLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildDruidHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildClericLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildClericHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildShedaklahLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildShedaklahHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildRangerLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildRangerHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildBardLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildBardHigh(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildMonkLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildMonkHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildAssassinLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildAssassinHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_guildNecromancerLow(CHAR_DATA * ch)
{
    return TRUE;
}
bool spec_guildNecromancerHigh(CHAR_DATA * ch)
{
    return TRUE;
}

bool spec_wall_force(CHAR_DATA * ch)
{
    return TRUE;

}

bool spec_thief(CHAR_DATA * ch)
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    long gold, silver;

    if (ch->position != POS_STANDING)
	return FALSE;

    for (victim = ch->in_room->people; victim != NULL; victim = v_next) {
	v_next = victim->next_in_room;

	if (IS_NPC(victim)
	    || victim->level >= LEVEL_IMMORTAL
	    || number_bits(5) != 0
	    || !can_see(ch, victim))
	    continue;

	if (IS_AWAKE(victim) && number_range(0, ch->level) == 0) {
	    act("You discover $n's hands in your wallet!",
		ch, NULL, victim, TO_VICT);
	    act("$N discovers $n's hands in $S wallet!",
		ch, NULL, victim, TO_NOTVICT);
	    return TRUE;
	} else {
	    gold = victim->gold * UMIN(number_range(1, 20), ch->level / 2) / 100;
	    gold = UMIN(gold, ch->level * ch->level * 10);
	    ch->gold += gold;
	    victim->gold -= gold;
	    silver = victim->silver * UMIN(number_range(1, 20), ch->level / 2) / 100;
	    silver = UMIN(silver, ch->level * ch->level * 25);
	    ch->silver += silver;
	    victim->silver -= silver;
	    return TRUE;
	}
    }

    return FALSE;
}



//endline


