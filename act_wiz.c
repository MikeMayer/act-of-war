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
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sqlite3.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

/* command procedures needed */
DECLARE_DO_FUN(do_rstat);
DECLARE_DO_FUN(do_mstat);
DECLARE_DO_FUN(do_ostat);
DECLARE_DO_FUN(do_rset);
DECLARE_DO_FUN(do_mset);
DECLARE_DO_FUN(do_oset);
DECLARE_DO_FUN(do_sset);
DECLARE_DO_FUN(do_sideSet);
DECLARE_DO_FUN(do_mfind);
DECLARE_DO_FUN(do_ofind);
DECLARE_DO_FUN(do_slookup);
DECLARE_DO_FUN(do_mload);
DECLARE_DO_FUN(do_oload);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_stand);
DECLARE_DO_FUN(do_remove_all);
DECLARE_DO_FUN(do_addlag);
DECLARE_DO_FUN(do_cast);

DECLARE_DO_FUN(do_asave);

void gnews args((char *argument));
void parse_penalty args((CHAR_DATA *ch, char *subject, char *body));
time_t ctime args((time_t* timer));


int				copyover_engage;             /* Used for copyover */
CHAR_DATA	*	copyover_commander;          /* Used for copyover */
int	ftick;

/*
 * Local functions.
 */
ROOM_INDEX_DATA *find_location args((CHAR_DATA * ch, char *arg));

void do_wiznet(CHAR_DATA * ch, char *argument)
{
    int flag;
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0') {
	if (IS_SET(ch->wiznet, WIZ_ON)) {
	    send_to_char("Signing off of Wiznet.\n\r", ch);
	    REMOVE_BIT(ch->wiznet, WIZ_ON);
	} else {
	    send_to_char("Welcome to Wiznet!\n\r", ch);
	    SET_BIT(ch->wiznet, WIZ_ON);
	}
	return;
    }
    if (!str_prefix(argument, "on")) {
	send_to_char("Welcome to Wiznet!\n\r", ch);
	SET_BIT(ch->wiznet, WIZ_ON);
	return;
    }
    if (!str_prefix(argument, "off")) {
	send_to_char("Signing off of Wiznet.\n\r", ch);
	REMOVE_BIT(ch->wiznet, WIZ_ON);
	return;
    }
    /* show wiznet status */
    if (!str_prefix(argument, "status")) {
	buf[0] = '\0';

	if (!IS_SET(ch->wiznet, WIZ_ON))
	    strcat(buf, "off ");

	for (flag = 0; wiznet_table[flag].name != NULL; flag++)
	    if (IS_SET(ch->wiznet, wiznet_table[flag].flag)) {
		strcat(buf, wiznet_table[flag].name);
		strcat(buf, " ");
	    }
	strcat(buf, "\n\r");

	send_to_char("Wiznet status:\n\r", ch);
	send_to_char(buf, ch);
	return;
    }
    if (!str_prefix(argument, "show"))
	/* list of all wiznet options */
    {
	buf[0] = '\0';

	for (flag = 0; wiznet_table[flag].name != NULL; flag++) {
	    if (wiznet_table[flag].level <= get_trust(ch)) {
		strcat(buf, wiznet_table[flag].name);
		strcat(buf, " ");
	    }
	}

	strcat(buf, "\n\r");

	send_to_char("Wiznet options available to you are:\n\r", ch);
	send_to_char(buf, ch);
	return;
    }
    flag = wiznet_lookup(argument);

    if (flag == -1 || get_trust(ch) < wiznet_table[flag].level) {
	send_to_char("No such option.\n\r", ch);
	return;
    }
    if (IS_SET(ch->wiznet, wiznet_table[flag].flag)) {
	sprintf(buf, "You will no longer see %s on wiznet.\n\r",
		wiznet_table[flag].name);
	send_to_char(buf, ch);
	REMOVE_BIT(ch->wiznet, wiznet_table[flag].flag);
	return;
    } else {
	sprintf(buf, "You will now see %s on wiznet.\n\r",
		wiznet_table[flag].name);
	send_to_char(buf, ch);
	SET_BIT(ch->wiznet, wiznet_table[flag].flag);
	return;
    }

}

void wiznet(char *string, CHAR_DATA * ch, OBJ_DATA * obj, long flag, long flag_skip, int min_level)
{
    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING
	    && IS_IMMORTAL(d->character)
	    && IS_SET(d->character->wiznet, WIZ_ON)
	    && (!flag || IS_SET(d->character->wiznet, flag))
	    && (!flag_skip || !IS_SET(d->character->wiznet, flag_skip))
	    && get_trust(d->character) >= min_level
	    && d->character != ch) {
	    if (IS_SET(d->character->wiznet, WIZ_PREFIX))
		send_to_char("--> ", d->character);
	    act_new(string, d->character, obj, ch, TO_CHAR, POS_DEAD);
	}
    }

    return;
}
void do_banis(CHAR_DATA * ch, char *argument)
{
	send_to_char("Huh?\n\r", ch);
	return;
}

void do_banish(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
	char subject[SmSL];
	char body[SmSL];

    CHAR_DATA *victim;
    char buf[100];
    char strsave[100];

    if (IS_NPC(ch)) return;

    if (!IS_IMMORTAL(ch))
	if (ch->pcdata->judge != 2) 
	{
	    send_to_char("Huh?\n\r", ch);
	    return;
	}
	argument = one_argument(argument, arg1);



if ((victim = get_player_world(ch, arg1)) == NULL) 
    {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }
if (IS_NPC(victim)) 
    {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }
if (!IS_IMMORTAL(ch))
if( ch->side != victim->side)
    {
	send_to_char("They aren't playing.\n\r", ch);
	return;
    }

	//require argument for banishing - Akuji
	if (argument[0] == '\0')
	{
		sprintf(buf, "You must provide a reason to banish %s\n\r", victim->name);
		stc(buf, ch);
		return;
	}

	sprintf(subject, "{R[AUTO]{c[Banish]{x %s has been banished from %s by %s", victim->name, kingdom_table[victim->side].cname, ch->name);
	sprintf(body, "%s has banished %s from %s with the following reason:\n\r----\n\r%s\n\r\n\r\n\r----\n\r%s's host: %s\n\r%s's host: %s\n\r%s\n\r",
		ch->name, victim->name, kingdom_table[victim->side].cname, argument, ch->name, ch->host, victim->name, victim->host, (char *) ctime(&current_time));


	parse_penalty(ch, subject, body);
	sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(victim->name));
	sprintf(buf, "%s has been BANISHED!!!\n\r", victim->name);
	news(buf, victim->side);
	news(buf, SIDE_NONE);

	victim->side = SIDE_NONE;

return;

}
void do_ic(CHAR_DATA * ch, char *argument)
{
	send_to_char("Huh?\n\r", ch);
	return;
}

void do_ice(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char arg2[MAX_INPUT_LENGTH];
	int ice_duration = 0;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if (!IS_IMMORTAL(ch))
	if (ch->pcdata->judge == 0 || ch->pcdata->judge ==3) 
	{
	    send_to_char("Huh?\n\r", ch);
	    return;
	}
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (!is_number(arg2) )
    {
		send_to_char("Syntax: ice <player> <1-10 ticks>.\n\r", ch);
		return;
    }
    ice_duration = atoi(arg2);

	if(ice_duration < 1 || ice_duration > 10)

	{
		send_to_char("Ice duration must be 1 - 10.\n\r", ch);
		return;
	}

	if ((victim = get_player_world(ch, arg1)) == NULL) 
    {
		send_to_char("They aren't playing.\n\r", ch);
		return;
    }

	if (IS_NPC(victim)) 
    {
		send_to_char("They aren't playing.\n\r", ch);
		return;
    }
      if(IS_IMMORTAL(victim))
	{
		send_to_char("You can not ice Immortals.\n\r",ch);
		return;
	}

	if (!IS_IMMORTAL(ch))

	if( ch->side != victim->side)
    {
		send_to_char("They aren't playing.\n\r", ch);
		return;
    }

    af.where = TO_AFFECTS2;
    af.level = 50;
    af.duration = ice_duration;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_IMM_FROZEN;
    af.type = skill_lookup("noquit");
    affect_to_char(victim, &af);


	return;

}


void do_rename(CHAR_DATA * ch, char *argument)

{

    FILE *fp;

    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char temp[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char notearg[MIL];
	char notearg2[MIL];

    CHAR_DATA *victim;

    DESCRIPTOR_DATA *d;



    if (IS_NPC(ch)) {

	send_to_char("You can't rename that.\n\r", ch);

	return;

    }

    if (!IS_IMMORTAL(ch))

	if (ch->pcdata->judge == 0 || ch->pcdata->judge == 3) {

	    send_to_char("You can't use this command.\n\r", ch);

	    return;

	}

    fp = NULL;



    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);



    if (arg1[0] == '\0' || arg2[0] == '\0') {

	send_to_char("Syntax: rename <player> <newname>\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("They aren't playing.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("You can't rename that.\n\r", ch);

	return;

    }

    if (IS_IMMORTAL(victim))

	return;



    if (!IS_IMMORTAL(ch)) {

	if (victim->pcdata->rename > 0) {

	    send_to_char("That person has been renamed once already.\n\r", ch);

	    return;

	}

	if (ch->side != victim->side) {

	    send_to_char("They aren't playing.\n\r", ch);

	    return;

	}

    }

    if (!check_parse_name(arg2)) {

	send_to_char("That name is invalid try another.\n\r", ch);

	return;

    }

    sprintf(buf, "%s%s", PLAYER_DIR, capitalize(arg2));

    fp = fopen(buf, "r");



    if (fp != NULL) {

	send_to_char("That name is taken already.\n\r", ch);

	return;

    }

    for (d = descriptor_list; d != NULL; d = d->next) {

	if (d->character != NULL) {

	    if (d->character != victim) {

		if (!strcmp(d->character->name, victim->name)) {

		    send_to_char("This player has more then one login of itself. They must log out all extra chars.\n\r", ch);

		    return;

		}

	    }

	}

    }

    arg2[0] = UPPER(arg2[0]);
    sprintf(temp, "%s", victim->name);
    strcpy(victim->name, arg2);
    send_to_char("Name changed.\n\r", ch);
    victim->pcdata->rename = 1;
    sprintf(
	       buf1, "Your name was changed to %s.\n\r", arg2);
    send_to_char(buf1, victim);
    save_char_obj(victim);

    sprintf(buf, "%s%s", PLAYER_DIR, capitalize(temp));
    unlink(buf);


	if (IS_IMMORTAL(ch))
	{
	sprintf(notearg, "%s was renamed to %s", temp, victim->name);
	sprintf(notearg2, "{R[Auto]{W Rename %s -> %s", temp, victim->name);
	parse_penalty(ch, notearg2, notearg);
	}
    return;

}



void do_leader(CHAR_DATA * ch, char *argument)

{

    CHAR_DATA *victim;



    if (IS_NPC(ch))

	return;



    if (ch->level <= 102) {

	send_to_char("Nope you can't do that.\n\r", ch);

	return;

    }

    if (argument[0] == '\0') {

	send_to_char("Who would you like to be made a leader.\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, argument)) == NULL) {

	send_to_char("They aren't playing.\n\r", ch);

	return;

    }

    if (IS_SET(victim->act, PLR_CLANLEADER)) {

	send_to_char("They are no longer a leader.\n\r", ch);

	send_to_char("You are no longer a clan leader.\n\r", victim);

	REMOVE_BIT(victim->act, PLR_CLANLEADER);

    } else if (!IS_SET(victim->act, PLR_CLANLEADER)) {

	send_to_char("They are now a clan leader.\n\r", ch);

	send_to_char("You are now a clan leader. Please do not abuse your authority.\n\r", victim);

	SET_BIT(victim->act, PLR_CLANLEADER);

    }

}

void do_guild(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];

    char buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;

    int clan;



    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);



    if (arg1[0] == '\0' || arg2[0] == '\0') {

	send_to_char("Syntax: guild <char> <cln name>\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("They aren't playing.\n\r", ch);

	return;

    }

    if (!str_prefix(arg2, "none")) {

	send_to_char("They are now clanless.\n\r", ch);

	send_to_char("You are now a member of no clan!\n\r", victim);

	victim->clan = 0;

	return;

    }

    if ((clan = clan_lookup(arg2)) == 0) {

	send_to_char("No such clan exists.\n\r", ch);

	return;

    }

    if (clan_table[clan].independent) {

	sprintf(buf, "They are now a %s.\n\r", clan_table[clan].name);

	send_to_char(buf, ch);

	sprintf(buf, "You are now a %s.\n\r", clan_table[clan].name);

	send_to_char(buf, victim);

    } else {

	sprintf(buf, "They are now a member of clan %s.\n\r",

		capitalize(clan_table[clan].name));

	send_to_char(buf, ch);

	sprintf(buf, "You are now a member of clan %s.\n\r",

		capitalize(clan_table[clan].name));

    }



    victim->clan = clan;

}



/* equips a character */

void do_outfit(CHAR_DATA * ch, char *argument)

{

    OBJ_DATA *obj;

    int i, sn, vnum;



    if (ch->level > 5 || IS_NPC(ch)) {

	send_to_char("Find it yourself!\n\r", ch);

	return;

    }

    if (GET_SIDE(ch) != SIDE_UM)

	if ((obj = get_eq_char(ch, WEAR_LIGHT)) == NULL) {

	    obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0);

	    obj->cost = 0;

	    obj_to_char(obj, ch);

	    equip_char(ch, obj, WEAR_LIGHT);

	}

    if ((obj = get_eq_char(ch, WEAR_BODY)) == NULL) {

	obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0);

	obj->cost = 0;

	obj_to_char(obj, ch);

	equip_char(ch, obj, WEAR_BODY);

    }

    /* do the weapon thing */

    if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL) {

	sn = 0;

	vnum = OBJ_VNUM_SCHOOL_SWORD;	/* just in case! */



	for (i = 0; weapon_table[i].name != NULL; i++) {

	    if (ch->pcdata->learned[sn] <

		ch->pcdata->learned[*weapon_table[i].gsn]) {

		sn = *weapon_table[i].gsn;

		vnum = weapon_table[i].vnum;

	    }

	}



	obj = create_object(get_obj_index(vnum), 0);

	obj_to_char(obj, ch);

	equip_char(ch, obj, WEAR_WIELD);

    }

    if (((obj = get_eq_char(ch, WEAR_WIELD)) == NULL

	 || !IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS))

	&& (obj = get_eq_char(ch, WEAR_SHIELD)) == NULL) {

	obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0);

	obj->cost = 0;

	obj_to_char(obj, ch);

	equip_char(ch, obj, WEAR_SHIELD);

    }

/*

   send_to_char("You have been equipped by Act of War.\n\r",ch);

 */

}




void do_nonote(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);



    /*if (!IS_IMMORTAL(ch))

	if (ch->pcdata->judge == 0) {

	    send_to_char("You cant use this command.\n\r", ch);

	    return;

	}*/

    if (arg[0] == '\0') {

	send_to_char("Revoke who's notes?", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(ch))

	return;



    if (IS_NPC(victim))

	return;



    /*

       if ( get_trust( victim ) >= get_trust( ch ) )

       {

       send_to_char( "You failed.\n\r", ch );

       return;

       }

     */



    if (IS_IMMORTAL(victim))

	return;



    if (!IS_IMMORTAL(ch)) {

	if (ch->pcdata->judge == 0) {

	    send_to_char("Huh?\n\r", ch);

	    return;

	}

	if (victim->side != ch->side) {

	    send_to_char("They aren't here.\n\r", ch);

	    return;

	}

    }

    if (IS_SET(victim->comm, COMM_NONOTE)) {

	REMOVE_BIT(victim->comm, COMM_NONOTE);

	send_to_char("You have restored their note writing priviliges.", ch);
	send_to_char("An immortal has restored your note writing priviliges.\n\r",

		     victim);

	send_to_char("NONOTE removed.\n\r", ch);

	sprintf(buf, "$N restores notes to %s", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    } else {

	SET_BIT(victim->comm, COMM_NONOTE);
	
	send_to_char("You have have revoked their note writing privilges.", ch);
	send_to_char("An immortal has revoked your note writing priviliges.\n\r",

		     victim);

	send_to_char("NONOTE set.\n\r", ch);

	sprintf(buf, "$N revokes %s's note writing.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    }



    return;

}

void do_nochannels(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);



    if (!IS_IMMORTAL(ch))

	if (ch->pcdata->judge == 0 || ch->pcdata->judge == 3) {

	    send_to_char("You cant use this command.\n\r", ch);

	    return;

	}

    if (arg[0] == '\0') {

	send_to_char("Nochannel whom?", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(ch))

	return;



    if (IS_NPC(victim))

	return;



    /*

       if ( get_trust( victim ) >= get_trust( ch ) )

       {

       send_to_char( "You failed.\n\r", ch );

       return;

       }

     */



    if (IS_IMMORTAL(victim))

	return;



    if (!IS_IMMORTAL(ch)) {

	if (ch->pcdata->judge == 0) {

	    send_to_char("You cant use this command your not a constable.\n\r", ch);

	    return;

	}

	if (victim->side != ch->side) {

	    send_to_char("They aren't here.\n\r", ch);

	    return;

	}

    }

    if (IS_SET(victim->comm, COMM_NOCHANNELS)) {

	REMOVE_BIT(victim->comm, COMM_NOCHANNELS);

	send_to_char("A constable has restored your channel priviliges.\n\r",

		     victim);

	send_to_char("NOCHANNELS removed.\n\r", ch);

	sprintf(buf, "$N restores channels to %s", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    } else {

	SET_BIT(victim->comm, COMM_NOCHANNELS);

	send_to_char("A constable has revoked your channel priviliges.\n\r",

		     victim);

	send_to_char("NOCHANNELS set.\n\r", ch);

	sprintf(buf, "$N revokes %s's channels.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    }



    return;

}



void do_smote(CHAR_DATA * ch, char *argument)

{

    CHAR_DATA *vch;

    char *letter, *name;

    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];

    int matches = 0;



    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE)) {

	send_to_char("You can't show your emotions.\n\r", ch);

	return;

    }

    if (argument[0] == '\0') {

	send_to_char("Emote what?\n\r", ch);

	return;

    }

    if (strstr(argument, ch->name) == NULL) {

	send_to_char("You must include your name in an smote.\n\r", ch);

	return;

    }

    send_to_char(argument, ch);

    send_to_char("\n\r", ch);



    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {

	if (vch->desc == NULL || vch == ch)

	    continue;



	if ((letter = strstr(argument, vch->name)) == NULL) {

	    send_to_char(argument, vch);

	    send_to_char("\n\r", vch);

	    continue;

	}

	strcpy(temp, argument);

	temp[strlen(argument) - strlen(letter)] = '\0';

	last[0] = '\0';

	name = vch->name;



	for (; *letter != '\0'; letter++) {

	    if (*letter == '\'' && matches == strlen(vch->name)) {

		strcat(temp, "r");

		continue;

	    }

	    if (*letter == 's' && matches == strlen(vch->name)) {

		matches = 0;

		continue;

	    }

	    if (matches == strlen(vch->name)) {

		matches = 0;

	    }

	    if (*letter == *name) {

		matches++;

		name++;

		if (matches == strlen(vch->name)) {

		    strcat(temp, "you");

		    last[0] = '\0';

		    name = vch->name;

		    continue;

		}

		strncat(last, letter, 1);

		continue;

	    }

	    matches = 0;

	    strcat(temp, last);

	    strncat(temp, letter, 1);

	    last[0] = '\0';

	    name = vch->name;

	}



	send_to_char(temp, vch);

	send_to_char("\n\r", vch);

    }



    return;

}



void do_bamfin(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];



    if (!IS_NPC(ch)) {

	smash_tilde(argument);



	if (argument[0] == '\0') {

	    sprintf(buf, "Your poofin is %s\n\r", ch->pcdata->bamfin);

	    send_to_char(buf, ch);

	    return;

	}

	if (strstr(argument, ch->name) == NULL) {

	    send_to_char("You must include your name.\n\r", ch);

	    return;

	}

	free_string(ch->pcdata->bamfin);

	ch->pcdata->bamfin = str_dup(argument);



	sprintf(buf, "Your poofin is now %s\n\r", ch->pcdata->bamfin);

	send_to_char(buf, ch);

    }

    return;

}



void do_bamfout(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];



    if (!IS_NPC(ch)) {

	smash_tilde(argument);



	if (argument[0] == '\0') {

	    sprintf(buf, "Your poofout is %s\n\r", ch->pcdata->bamfout);

	    send_to_char(buf, ch);

	    return;

	}

	if (strstr(argument, ch->name) == NULL) {

	    send_to_char("You must include your name.\n\r", ch);

	    return;

	}

	free_string(ch->pcdata->bamfout);

	ch->pcdata->bamfout = str_dup(argument);



	sprintf(buf, "Your poofout is now %s\n\r", ch->pcdata->bamfout);

	send_to_char(buf, ch);

    }

    return;

}



void do_deny(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);

    if (arg[0] == '\0') {

	send_to_char("Deny whom?\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("Not on NPC's.\n\r", ch);

	return;

    }

    if (get_trust(victim) >= get_trust(ch)) {

	send_to_char("You failed.\n\r", ch);

	return;

    }

    SET_BIT(victim->act, PLR_DENY);

    send_to_char("You are denied access!\n\r", victim);

    sprintf(buf, "$N denies access to %s", victim->name);

    wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    send_to_char("You deny them access to AoW.\n\r", ch);

    if (victim->level > 1)

	save_char_obj(victim);

    stop_fighting(victim, TRUE);

    victim->pcdata->confirm_delete = TRUE;

    do_quit(victim, "");



    return;

}



void do_disconnect(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    DESCRIPTOR_DATA *d;

    CHAR_DATA *victim;



    one_argument(argument, arg);

    if (arg[0] == '\0') {

	send_to_char("Disconnect whom?\n\r", ch);

	return;

    }

    if (is_number(arg)) {

	int desc;



	desc = atoi(arg);

	for (d = descriptor_list; d != NULL; d = d->next) {

	    if (d->descriptor == desc) {

		close_socket(d);

		send_to_char("They have been disconnected.\n\r", ch);

		return;

	    }

	}

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (victim->desc == NULL) {

	act("$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR);

	return;

    }

    for (d = descriptor_list; d != NULL; d = d->next) {

	if (d == victim->desc) {

	    close_socket(d);

	    send_to_char("They have been disconnected.\n\r", ch);

	    return;

	}

    }



    bug("Do_disconnect: desc not found.", 0);

    send_to_char("Descriptor not found!\n\r", ch);

    return;

}


void do_constable(CHAR_DATA * ch, char *argument)
{
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
//   char arg[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *d;

    if (IS_NPC(ch))
    return;
    
  if (ch->pcdata->judge == 0 && !IS_IMMORTAL(ch))
  {
  send_to_char("Huh?\n\r", ch);
  return;
  }
  
        sprintf(buf, "{DYou constable {R\'{W%s{R\'{x\n\r", argument);
        send_to_char(buf, ch);

 	for (d = descriptor_list; d != NULL; d = d->next) {
 	    victim = d->original ? d->original : d->character;

 	    if (victim == NULL) 
 		continue;
 		
 		if (victim == ch)
 		continue;
 		
        if (victim->pcdata->judge != 1 && victim->pcdata->judge != 2)
        if (!IS_IMMORTAL(victim) )
        continue;
        if (!IS_IMMORTAL(ch))
	    if (ch->pcdata->judge == 1) {	
 		sprintf(buf, "{R<{D%s{R> {D%s Constables{R:{W %s{x\n\r",kingdom_table[ch->side].name,
 		 ch->name, argument);
	    }
	    else {
 		sprintf(buf, "{R<{D%s{R> {DHead Constable %s Constables{R:{W %s{x\n\r",kingdom_table[ch->side].name,
		 ch->name, argument);
	    }
 	else
 	    sprintf(buf, "{R<{D%s{R> {DImmortal %s Constables{R:{W %s{x\n\r",kingdom_table[ch->side].name,
 	     ch->name, argument); 		 
 		send_to_char( buf, victim);

 	}
 		
	return;

}

void do_mhconstable(CHAR_DATA * ch, char *argument)
{
   CHAR_DATA *victim;
   
   if (IS_NPC(ch) )
   return;
	
   if (IS_IMMORTAL(ch))
   {
	if ((victim = get_player_world(ch, argument)) == NULL)
  	{
    	send_to_char("They aren't here.\n\r", ch);
	    return;
	    }
	    if (IS_NPC(victim))
	    {
	    send_to_char("You can't make NPC's constables!\n\r", ch);
	    return;
	    }
	   if ( victim->pcdata->judge == 2 )
	   {
	     send_to_char ( "You have been DEMOTED from head constable.\n\r", victim);
	     send_to_char ( "You have demoted them from head constable.\n\r", ch);
	     victim->pcdata->judge = 0;
	     return;
	   } else if ( victim->pcdata->judge == 0 || victim->pcdata->judge == 1)
	     {
	     send_to_char ( "You have been PROMOTED to Head constable.\n\r", victim);
	     send_to_char ( "Use the \"constable\" command to talk to\n\r", victim);
	     send_to_char ( "all other constables and Imm's currentlly online.\n\r", victim);
	     send_to_char ( "{RABUSE OF THIS COMMAND WILL MAKE YOU WISH YOU WERE NOT BORN.{x\n\r", victim);
	     send_to_char ( "You have made them a constable.\n\r", ch);
	     victim->pcdata->judge = 2;
	     return;
	     }
	}
	return;
}	
	     	

	
void do_mconstable(CHAR_DATA * ch, char *argument)
{
   CHAR_DATA *victim;
//   char buf[MAX_STRING_LENGTH];
//   char arg[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    return;
    
  if (IS_IMMORTAL(ch) )
  {
        if ((victim = get_player_world(ch, argument)) == NULL) 
        {
    	send_to_char("They aren't here.\n\r", ch);
	    return;
	    }
	    if (IS_NPC(victim))
	     {
	    send_to_char("You can't make NPC's constables!\n\r", ch);
	    return;
	     }

	   if ( victim->pcdata->judge == 1 )
	   {
	     send_to_char ( "You have been DEMOTED from constable.\n\r", victim);
	     send_to_char ( "You have demoted them from constable.\n\r", ch);
	     victim->pcdata->judge = 0;
	     return;
	   } else if ( victim->pcdata->judge == 0 )
	   {
	     send_to_char ( "You have been PROMOTED to constable.\n\r", victim);
	     send_to_char ( "Use the \"constable\" command to talk to\n\r", victim);
	     send_to_char ( "all other constables and Imm's currentlly online.\n\r", victim);
	     send_to_char ( "{RABUSE OF THIS COMMAND WILL MAKE YOU WISH YOU WERE NOT BORN.{x\n\r", victim);
	     send_to_char ( "You have made them a constable.\n\r", ch);
	     victim->pcdata->judge = 1;
	     return;
	   }
  }
   return;
}

void do_sellsword(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
    return;
    
  if (ch->level >= 50)
 
		{

	if (IS_AFFECTED(ch, AFF_NOQUIT)) 
	    {
		send_to_char("You are currently {RNOQUIT{x. Please wait until later.\n\r", ch);
	  	return;
	    }


	   		if ( ch->pcdata->judge == 3 )
	   			{
	    			send_to_char ( "It's far too late to try to rebuild those bridges now.\n\r", ch);
	     			return;
				}
	   		else if (ch->pcdata->judge == 1)
				{
	    			send_to_char ("You are a constable, how dare you think such things.\n\r", ch);
	    			return;
				}
	   		else if (ch->pcdata->judge == 2)
				{
	    			send_to_char ("You are the head constable of this fine town, how dare you!\n\r", ch);
	    			return;
				} 
			else if (ch->pcdata->judge == 0 )
	   			{
	     			send_to_char ( "You have decided to sell your sword to the highest bidding.\n\r", ch);
	     			send_to_char ( "There is no way to reverse this command.  You can switch sides\n\r", ch);
	     			send_to_char ( "any time you are not noquit.  Remember not to burn too many\n\r", ch);
	     			send_to_char ( "bridges on your way out. To see current fares, type {Rfare{x.\n\r", ch);
	     			send_to_char ( "For further information, consult {Rhelp mercenary{x.\n\r", ch);
	     			ch->pcdata->judge = 3;
	     			return;
	   			}
  		}
	else 
		{
			send_to_char ( "You can only speak for yourself on such matters.\n\r", ch);
			return;
		}
   return;

}
void do_fares(CHAR_DATA * ch, char *argument)
{
if (ch->pcdata->judge != 3)
	{
	send_to_char ("So interested in the financial needs of the sellswords?\n\r", ch);
	send_to_char ("Why don't you start a kingdom instead.\n\r", ch);
	return;
	}
else
//fees will actually need to be calculated.  The following table will work... for now.

	{
	send_to_char ("{r* * * {RMercenary Fares{r * * *{x\n\r",ch);
	send_to_char ("{RTier           :{x 0{ygp{x\n\r",ch);
	send_to_char ("{RKael           :{x 0{ygp{x\n\r",ch);
	send_to_char ("{RUndermountain  :{x 0{ygp{x\n\r",ch);
	send_to_char ("{RCairn          :{x0{ygp{x\n\r",ch);
	send_to_char ("{RNo Alliance    :{x 0{ygp{x\n\r",ch);
	send_to_char ("{r* * * {RMercenary Fares{x * * *{x\n\r",ch);
	return;
	}
return;
}


void do_align(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
//    int value;

smash_tilde(argument);
argument = one_argument(argument, arg1);

if (ch->pcdata->judge != 3)
	{
	send_to_char ("You're a turncloak? Not amazing.\n\r", ch);
	return;
	}

if (IS_AFFECTED(ch, AFF_NOQUIT)) 
	{
	send_to_char("{RBetray the men you fight with now?{x Never!!\n\r", ch);
	return;
	}
else
	{
	/* arg1 value */

 	if (arg1[0] == '\0') 
		{

		send_to_char("Syntax: align <side>\n\r", ch);
		send_to_char("Side is None, Tier, Kael, Undermountain, UM, or Cairn.\n\r", ch);

	return;

   	}

if (!is_number(arg1) && is_name(arg1, "tier kael undermountain um cairn"))
{

	if (is_name(arg1, "none"))

	    ch->side = SIDE_NONE;

	if (is_name(arg1, "tier"))

	    ch->side = SIDE_TIER;

	if (is_name(arg1, "kael"))

	    ch->side = SIDE_KAEL;

	if (is_name(arg1, "um"))

	    ch->side = SIDE_UM;

	if (is_name(arg1, "undermountain"))

	    ch->side = SIDE_UM;

	if (is_name(arg1, "cairn"))

	    ch->side = SIDE_CAIRN;

	send_to_char("You welcome the challenge of a new fight.\n\r", ch);

	return;

    }
return;
}
}


void do_pardon(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;



    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);



    if (IS_NPC(ch))

	return;



    if (!IS_IMMORTAL(ch))

	if (ch->pcdata->judge == 0 || ch->pcdata->judge == 3) {

	    send_to_char("You can't use this command.\n\r", ch);

	    return;

	}

    if (arg1[0] == '\0' || arg2[0] == '\0') {

	send_to_char("Syntax: pardon <character> <killer|thief>.\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("Not on NPC's.\n\r", ch);

	return;

    }

    if (!IS_IMMORTAL(ch))

	if (victim->side != ch->side) {

	    send_to_char("You cant pardon an enemy.\n\r", ch);

	    return;

	}

    if (!str_cmp(arg2, "traitor")) {

	if (IS_SET(victim->act, PLR_KILLER)) {

	    REMOVE_BIT(victim->act, PLR_KILLER);

	    send_to_char("Traitor flag removed.\n\r", ch);

	    send_to_char("You are no longer a TRAITOR.\n\r", victim);

	}

	return;

    }

    if (!str_cmp(arg2, "criminal")) {

	if (IS_SET(victim->act, PLR_THIEF)) {

	    REMOVE_BIT(victim->act, PLR_THIEF);

	    send_to_char("Criminal flag removed.\n\r", ch);

	    send_to_char("You are no longer a CRIMINAL.\n\r", victim);

	}

	return;

    }

    send_to_char("Syntax: pardon <character> <traitor|criminal>.\n\r", ch);

    return;

}



void do_echo(CHAR_DATA * ch, char *argument)

{

    DESCRIPTOR_DATA *d;



    if (argument[0] == '\0') {

	send_to_char("Global echo what?\n\r", ch);

	return;

    }

    for (d = descriptor_list; d; d = d->next) {

	if (d->connected == CON_PLAYING) {

	    if (get_trust(d->character) >= get_trust(ch))

		send_to_char("global> ", d->character);

	    send_to_char(argument, d->character);

	    send_to_char("\n\r", d->character);

	}

    }



    return;

}



void do_recho(CHAR_DATA * ch, char *argument)

{

    DESCRIPTOR_DATA *d;



    if (argument[0] == '\0') {

	send_to_char("Local echo what?\n\r", ch);



	return;

    }

    for (d = descriptor_list; d; d = d->next) {

	if (d->connected == CON_PLAYING

	    && d->character->in_room == ch->in_room) {

	    if (get_trust(d->character) >= get_trust(ch))

		send_to_char("local> ", d->character);

	    send_to_char(argument, d->character);

	    send_to_char("\n\r", d->character);

	}

    }



    return;

}



void do_zecho(CHAR_DATA * ch, char *argument)

{

    DESCRIPTOR_DATA *d;



    if (argument[0] == '\0') {

	send_to_char("Zone echo what?\n\r", ch);

	return;

    }

    for (d = descriptor_list; d; d = d->next) {

	if (d->connected == CON_PLAYING

	    && d->character->in_room != NULL && ch->in_room != NULL

	    && d->character->in_room->area == ch->in_room->area) {

	    if (get_trust(d->character) >= get_trust(ch))

		send_to_char("zone> ", d->character);

	    send_to_char(argument, d->character);

	    send_to_char("\n\r", d->character);

	}

    }

}



void do_pecho(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;



    argument = one_argument(argument, arg);



    if (argument[0] == '\0' || arg[0] == '\0') {

	send_to_char("Personal echo what?\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("Target not found.\n\r", ch);

	return;

    }

    if (get_trust(victim) >= get_trust(ch) && get_trust(ch) != MAX_LEVEL)

	send_to_char("personal> ", victim);



    send_to_char(argument, victim);

    send_to_char("\n\r", victim);

    send_to_char("personal> ", ch);

    send_to_char(argument, ch);

    send_to_char("\n\r", ch);

}



ROOM_INDEX_DATA *find_location(CHAR_DATA * ch, char *arg)

{

    CHAR_DATA *victim;

    OBJ_DATA *obj;



    if (is_number(arg))

	return get_room_index(atoi(arg));



    if ((victim = get_player_world(ch, arg)) != NULL)

	return victim->in_room;



    if ((obj = get_obj_world(ch, arg)) != NULL)

	return obj->in_room;



    return NULL;

}



void do_transfer(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    ROOM_INDEX_DATA *location;

    DESCRIPTOR_DATA *d;

    CHAR_DATA *victim;



    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);



    if (arg1[0] == '\0') {

	send_to_char("Transfer whom (and where)?\n\r", ch);

	return;

    }

    if (!str_cmp(arg1, "all")) {

	for (d = descriptor_list; d != NULL; d = d->next) {

	    if (d->connected == CON_PLAYING

		&& d->character != ch

		&& d->character->in_room != NULL

		&& can_see(ch, d->character)) {

		char buf[MAX_STRING_LENGTH];

		sprintf(buf, "%s %s", d->character->name, arg2);

		do_transfer(ch, buf);

	    }

	}

	return;

    }

    /*

     * Thanks to Grodyn for the optional location parameter.

     */

    if (arg2[0] == '\0') {

	location = ch->in_room;

    } else {

	if ((location = find_location(ch, arg2)) == NULL) {

	    send_to_char("No such location.\n\r", ch);

	    return;

	}

	if (!is_room_owner(ch, location) && room_is_private(location)

	    && get_trust(ch) < MAX_LEVEL) {

	    send_to_char("That room is private right now.\n\r", ch);

	    return;

	}

    }



    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (victim->in_room == NULL) {

	send_to_char("They are in limbo.\n\r", ch);

	return;

    }

    if (victim->fighting != NULL)

	stop_fighting(victim, TRUE);

    act("$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM);

    char_from_room(victim);

    char_to_room(victim, location);

    act("$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM);



    if (MOUNTED(victim)) {

	char_from_room(MOUNTED(victim));

	char_to_room(MOUNTED(victim), location);

	send_to_char("Your rider is being transferred, and so are you.\n\r", MOUNTED(victim));

    }

    if (ch->pet != NULL) {

	char_from_room(ch->pet);

	char_to_room(ch->pet, location);

    }

    if (ch != victim)

	act("$n has transferred you.", ch, NULL, victim, TO_VICT);

    do_look(victim, "auto");

    send_to_char("You have transfered them.\n\r", ch);

}



void do_at(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    ROOM_INDEX_DATA *location;

    ROOM_INDEX_DATA *original;

    OBJ_DATA *on;

    CHAR_DATA *wch;



    argument = one_argument(argument, arg);



    if (arg[0] == '\0' || argument[0] == '\0') {

	send_to_char("At where what?\n\r", ch);

	return;

    }

    if ((location = find_location(ch, arg)) == NULL) {

	send_to_char("No such location.\n\r", ch);

	return;

    }

    if (!is_room_owner(ch, location) && room_is_private(location)

	&& get_trust(ch) < MAX_LEVEL) {

	send_to_char("That room is private right now.\n\r", ch);

	return;

    }

    original = ch->in_room;

    on = ch->on;

    char_from_room(ch);

    char_to_room(ch, location);

    interpret(ch, argument);



    /*

     * See if 'ch' still exists before continuing!

     * Handles 'at XXXX quit' case.

     */

    for (wch = char_list; wch != NULL; wch = wch->next) {

	if (wch == ch) {

	    char_from_room(ch);

	    char_to_room(ch, original);

	    ch->on = on;

	    break;

	}

    }



    return;

}



void do_goto(CHAR_DATA * ch, char *argument)

{

    ROOM_INDEX_DATA *location;

    CHAR_DATA *rch;

    int count = 0;



    if (argument[0] == '\0') {

	send_to_char("Goto where?\n\r", ch);

	return;

    }

    if ((location = find_location(ch, argument)) == NULL) {

	send_to_char("No such location.\n\r", ch);

	return;

    }

    count = 0;

    for (rch = location->people; rch != NULL; rch = rch->next_in_room)

	count++;



    if (!is_room_owner(ch, location) && room_is_private(location)

	&& (count > 1 || get_trust(ch) < MAX_LEVEL)) {

	send_to_char("That room is private right now.\n\r", ch);

	return;

    }

    if (ch->fighting != NULL)

	stop_fighting(ch, TRUE);



    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	if (get_trust(rch) >= ch->invis_level) {

	    if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')

		act("$t", ch, ch->pcdata->bamfout, rch, TO_VICT);

	    else

		act("$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT);

	}

    }



    char_from_room(ch);

    char_to_room(ch, location);



    if (ch->pet != NULL) {

	char_from_room(ch->pet);

	char_to_room(ch->pet, location);

    }

    if (MOUNTED(ch)) {

	char_from_room(MOUNTED(ch));

	char_to_room(MOUNTED(ch), location);

	send_to_char("Your rider is a god, and did a goto.  You went along for the ride.\n\r", MOUNTED(ch));

    }

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	if (get_trust(rch) >= ch->invis_level) {

	    if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')

		act("$t", ch, ch->pcdata->bamfin, rch, TO_VICT);

	    else

		act("$n appears in a swirling mist.", ch, NULL, rch, TO_VICT);

	}

    }



    do_look(ch, "auto");

    return;

}



void do_violate(CHAR_DATA * ch, char *argument)

{

    ROOM_INDEX_DATA *location;

    CHAR_DATA *rch;



    if (argument[0] == '\0') {

	send_to_char("Goto where?\n\r", ch);

	return;

    }

    if ((location = find_location(ch, argument)) == NULL) {

	send_to_char("No such location.\n\r", ch);

	return;

    }

    if (!room_is_private(location)) {

	send_to_char("That room isn't private, use goto.\n\r", ch);

	return;

    }

    if (ch->fighting != NULL)

	stop_fighting(ch, TRUE);



    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	if (get_trust(rch) >= ch->invis_level) {

	    if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')

		act("$t", ch, ch->pcdata->bamfout, rch, TO_VICT);

	    else

		act("$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT);

	}

    }



    char_from_room(ch);

    char_to_room(ch, location);



    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	if (get_trust(rch) >= ch->invis_level) {

	    if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')

		act("$t", ch, ch->pcdata->bamfin, rch, TO_VICT);

	    else

		act("$n appears in a swirling mist.", ch, NULL, rch, TO_VICT);

	}

    }



    do_look(ch, "auto");

    return;

}



/* RT to replace the 3 stat commands */

void do_stat(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    char *string;

    OBJ_DATA *obj;

    ROOM_INDEX_DATA *location;

    CHAR_DATA *victim;



    string = one_argument(argument, arg);

    if (arg[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  stat <name>\n\r", ch);

	send_to_char("  stat obj <name>\n\r", ch);

	send_to_char("  stat mob <name>\n\r", ch);

	send_to_char("  stat room <number>\n\r", ch);

	return;

    }

    if (!str_cmp(arg, "room")) {

	do_rstat(ch, string);

	return;

    }

    if (!str_cmp(arg, "obj")) {

	do_ostat(ch, string);

	return;

    }

    if (!str_cmp(arg, "char") || !str_cmp(arg, "mob")) {

	do_mstat(ch, string);

	return;

    }

    /* do it the old way */



    obj = get_obj_world(ch, argument);

    if (obj != NULL) {

	do_ostat(ch, argument);

	return;

    }

    victim = get_char_world(ch, argument);

    if (victim != NULL) {

	do_mstat(ch, argument);

	return;

    }

    location = find_location(ch, argument);

    if (location != NULL) {

	do_rstat(ch, argument);

	return;

    }

    send_to_char("Nothing by that name found anywhere.\n\r", ch);

}



void do_rstat(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    char arg[MAX_INPUT_LENGTH];

    ROOM_INDEX_DATA *location;

    OBJ_DATA *obj;

    CHAR_DATA *rch;

    int door;



    one_argument(argument, arg);

    location = (arg[0] == '\0') ? ch->in_room : find_location(ch, arg);

    if (location == NULL) {

	send_to_char("No such location.\n\r", ch);

	return;

    }

    if (!is_room_owner(ch, location) && ch->in_room != location

	&& room_is_private(location) && !IS_TRUSTED(ch, IMPLEMENTOR)) {

	send_to_char("That room is private right now.\n\r", ch);

	return;

    }

    sprintf(buf, "Name: '%s'\n\rArea: '%s'\n\r",

	    location->name,

	    location->area->name);

    send_to_char(buf, ch);



    sprintf(buf,

	    "Vnum: %d  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",

	    location->vnum,

	    location->sector_type,

	    location->light,

	    location->heal_rate,

	    location->mana_rate);

    send_to_char(buf, ch);



    sprintf(buf,

	    "Room flags: %d.\n\rDescription:\n\r%s",

	    location->room_flags,

	    location->description);

    send_to_char(buf, ch);



    if (location->extra_descr != NULL) {

	EXTRA_DESCR_DATA *ed;



	send_to_char("Extra description keywords: '", ch);

	for (ed = location->extra_descr; ed; ed = ed->next) {

	    send_to_char(ed->keyword, ch);

	    if (ed->next != NULL)

		send_to_char(" ", ch);

	}

	send_to_char("'.\n\r", ch);

    }

    send_to_char("Characters:", ch);

    for (rch = location->people; rch; rch = rch->next_in_room) {

	if (can_see(ch, rch)) {

	    send_to_char(" ", ch);

	    one_argument(rch->name, buf);

	    send_to_char(buf, ch);

	}

    }



    send_to_char(".\n\rObjects:   ", ch);

    for (obj = location->contents; obj; obj = obj->next_content) {

	send_to_char(" ", ch);

	one_argument(obj->name, buf);

	send_to_char(buf, ch);

    }

    send_to_char(".\n\r", ch);



    for (door = 0; door <= 5; door++) {

	EXIT_DATA *pexit;



	if ((pexit = location->exit[door]) != NULL) {

	    sprintf(buf,

		    "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",



		    door,

		    (pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum),

		    pexit->key,

		    pexit->exit_info,

		    pexit->keyword,

		    pexit->description[0] != '\0'

		    ? pexit->description : "(none).\n\r");

	    send_to_char(buf, ch);

	}

    }



    return;

}



void do_ostat(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    char arg[MAX_INPUT_LENGTH];

    AFFECT_DATA *paf;

    OBJ_DATA *obj;



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Stat what?\n\r", ch);

	return;

    }

    if ((obj = get_obj_world(ch, argument)) == NULL) {

	send_to_char("Nothing like that in hell, earth, or heaven.\n\r", ch);

	return;

    }

    sprintf(buf, "Name(s): %s\n\r",

	    obj->name);

    send_to_char(buf, ch);



    sprintf(buf, "Vnum: %d  Format: %s  Type: %s  Resets: %d\n\r",

	    obj->pIndexData->vnum, obj->pIndexData->new_format ? "new" : "old",

	    item_name(obj->item_type), obj->pIndexData->reset_num);

    send_to_char(buf, ch);



    sprintf(buf, "Short description: %s\n\rLong description: %s\n\r",

	    obj->short_descr, obj->description);

    send_to_char(buf, ch);



    sprintf(buf, "Wear bits: %s\n\rExtra bits: %s\n\r",

	    wear_bit_name(obj->wear_flags), extra_bit_name(obj->extra_flags));

    send_to_char(buf, ch);



    sprintf(buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",

	    1, get_obj_number(obj),

	    obj->weight, get_obj_weight(obj), get_true_weight(obj));

    send_to_char(buf, ch);



    sprintf(buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",

	    obj->level, obj->cost, obj->condition, obj->timer);

    send_to_char(buf, ch);



    sprintf(buf,

	    "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",

	    obj->in_room == NULL ? 0 : obj->in_room->vnum,

	    obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr,

	    obj->carried_by == NULL ? "(none)" :

	    can_see(ch, obj->carried_by) ? obj->carried_by->name

	    : "someone",

	    obj->wear_loc);

    send_to_char(buf, ch);



    sprintf(buf, "Values: %d %d %d %d %d\n\r",

	    obj->value[0], obj->value[1], obj->value[2], obj->value[3],

	    obj->value[4]);

    send_to_char(buf, ch);



    /* now give out vital statistics as per identify */



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

	sprintf(buf, "Has %d(%d) charges of level %d",

		obj->value[1], obj->value[2], obj->value[0]);

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

	case ITEM_SHIELD:
	
	send_to_char("Is a shield.\n\r",ch);
	
	break;


    case ITEM_WEAPON:

	send_to_char("Weapon type is ", ch);

	switch (obj->value[0]) {

	case (WEAPON_EXOTIC):

	    send_to_char("exotic\n\r", ch);

	    break;

	case (WEAPON_SWORD):

	    send_to_char("sword\n\r", ch);

	    break;

	case (WEAPON_DAGGER):

	    send_to_char("dagger\n\r", ch);

	    break;

	case (WEAPON_SPEAR):

	    send_to_char("spear/staff\n\r", ch);

	    break;

	case (WEAPON_MACE):

	    send_to_char("mace/club\n\r", ch);

	    break;

	case (WEAPON_AXE):

	    send_to_char("axe\n\r", ch);

	    break;

	case (WEAPON_FLAIL):

	    send_to_char("flail\n\r", ch);

	    break;

	case (WEAPON_WHIP):

	    send_to_char("whip\n\r", ch);

	    break;

	case (WEAPON_POLEARM):

	    send_to_char("polearm\n\r", ch);

	    break;

	default:

	    send_to_char("unknown\n\r", ch);

	    break;

	}

	if (obj->pIndexData->new_format)

	    sprintf(buf, "Damage is %dd%d (average %d)\n\r",

		    obj->value[1], obj->value[2],

		    (1 + obj->value[2]) * obj->value[1] / 2);

	else

	    sprintf(buf, "Damage is %d to %d (average %d)\n\r",

		    obj->value[1], obj->value[2],

		    (obj->value[1] + obj->value[2]) / 2);

	send_to_char(buf, ch);



	sprintf(buf, "Damage noun is %s.\n\r",

		(obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ?

		attack_table[obj->value[3]].noun : "undefined");

	send_to_char(buf, ch);



	if (obj->value[4]) {	/* weapon flags */

	    sprintf(buf, "Weapons flags: %s\n\r",

		    weapon_bit_name(obj->value[4]));

	    send_to_char(buf, ch);

	}

	break;



    case ITEM_ARMOR:

	sprintf(buf,

		"Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",

		obj->value[0], obj->value[1], obj->value[2], obj->value[3]);

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

    }



    if (obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL) {

	EXTRA_DESCR_DATA *ed;



	send_to_char("Extra description keywords: '", ch);



	for (ed = obj->extra_descr; ed != NULL; ed = ed->next) {

	    send_to_char(ed->keyword, ch);

	    if (ed->next != NULL)

		send_to_char(" ", ch);

	}



	for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next) {

	    send_to_char(ed->keyword, ch);

	    if (ed->next != NULL)

		send_to_char(" ", ch);

	}



	send_to_char("'\n\r", ch);

    }

    for (paf = obj->affected; paf != NULL; paf = paf->next) {

	sprintf(buf, "Affects %s by %d, level %d",

		affect_loc_name(paf->location), paf->modifier, paf->level);

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

	    case TO_WEAPON:

		sprintf(buf, "Adds %s weapon flags.\n",

			weapon_bit_name(paf->bitvector));

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



    if (!obj->enchanted)

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) {

	    sprintf(buf, "Affects %s by %d, level %d.\n\r",

		    affect_loc_name(paf->location), paf->modifier, paf->level);

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

    return;

}



void do_mstat(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    char arg[MAX_INPUT_LENGTH];

    AFFECT_DATA *paf;

    CHAR_DATA *victim;



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Stat whom?\n\r", ch);

	return;

    }

    victim = get_char_room(ch, argument);



    if (victim == NULL) {

	if ((victim = get_player_world(ch, argument)) == NULL) {

	    send_to_char("They aren't here.\n\r", ch);

	    return;

	}

    }

    sprintf(buf, "Name: %s   Side: %d\n\r",

	    victim->name, victim->side);

    send_to_char(buf, ch);



    sprintf(buf,

	    "Vnum: %d  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %d\n\r",

	    IS_NPC(victim) ? victim->pIndexData->vnum : 0,

	    IS_NPC(victim) ? victim->pIndexData->new_format ? "new" : "old" : "pc",

	    race_table[victim->race].name,

	    IS_NPC(victim) ? victim->group : 0, sex_table[victim->sex].name,

	    victim->in_room == NULL ? 0 : victim->in_room->vnum

	);

    send_to_char(buf, ch);



    if (IS_NPC(victim)) {

	sprintf(buf, "Count: %d  Killed: %d\n\r",

		victim->pIndexData->count, victim->pIndexData->killed);

	send_to_char(buf, ch);

    }

    sprintf(buf,

	    "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",

	    victim->perm_stat[STAT_STR],

	    get_curr_stat(victim, STAT_STR),

	    victim->perm_stat[STAT_INT],

	    get_curr_stat(victim, STAT_INT),

	    victim->perm_stat[STAT_WIS],

	    get_curr_stat(victim, STAT_WIS),

	    victim->perm_stat[STAT_DEX],

	    get_curr_stat(victim, STAT_DEX),

	    victim->perm_stat[STAT_CON],

	    get_curr_stat(victim, STAT_CON));

    send_to_char(buf, ch);



    sprintf(buf, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Practices: %d\n\r",

	    victim->hit, victim->max_hit,

	    victim->mana, victim->max_mana,

	    victim->move, victim->max_move,

	    IS_NPC(ch) ? 0 : victim->practice);

    send_to_char(buf, ch);



    sprintf(buf,

	    "Lv: %d  Class: %s  Align: %d  Gold: %ld  Silver: %ld  Exp: %d\n\r",

	    victim->level,

	    IS_NPC(victim) ? "mobile" : class_table[victim->class].name,

	    victim->alignment,

	    victim->gold, victim->silver, victim->exp);

    send_to_char(buf, ch);



    sprintf(buf, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",

	    GET_AC(victim, AC_PIERCE), GET_AC(victim, AC_BASH),

	    GET_AC(victim, AC_SLASH), GET_AC(victim, AC_EXOTIC));

    send_to_char(buf, ch);



    sprintf(buf,

	    "Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",

	    GET_HITROLL(victim), GET_DAMROLL(victim), victim->saving_throw,

	    size_table[victim->size].name, position_table[victim->position].name,

	    victim->wimpy);

    send_to_char(buf, ch);



    if (IS_NPC(victim) && victim->pIndexData->new_format) {

	sprintf(buf, "Damage: %dd%d  Message:  %s\n\r",

		victim->damage[DICE_NUMBER], victim->damage[DICE_TYPE],

		attack_table[victim->dam_type].noun);

	send_to_char(buf, ch);

    }

    sprintf(buf, "Fighting: %s\n\r",

	    victim->fighting ? victim->fighting->name : "(none)");

    send_to_char(buf, ch);



    if (!IS_NPC(victim)) {

	sprintf(buf,

		"Thirst: %d  Hunger: %d  Full: %d  Drunk: %d\n\r",

		victim->pcdata->condition[COND_THIRST],

		victim->pcdata->condition[COND_HUNGER],

		victim->pcdata->condition[COND_FULL],

		victim->pcdata->condition[COND_DRUNK]);

	send_to_char(buf, ch);

    }

    sprintf(buf, "Carry number: %d  Carry weight: %ld\n\r",

	    victim->carry_number, get_carry_weight(victim) / 10);

    send_to_char(buf, ch);



    if (!IS_NPC(victim)) {

	sprintf(buf,

		"Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",

		get_age(victim),

		(int) (victim->played + current_time - victim->logon) / 3600,

		victim->pcdata->last_level,

		victim->timer);

	send_to_char(buf, ch);

    }

    sprintf(buf, "Act: %s\n\r", act_bit_name(victim->act));

    send_to_char(buf, ch);



    if (victim->comm) {

	sprintf(buf, "Comm: %s\n\r", comm_bit_name(victim->comm));

	send_to_char(buf, ch);

    }

    if (IS_NPC(victim) && victim->off_flags) {

	sprintf(buf, "Offense: %s\n\r", off_bit_name(victim->off_flags));

	send_to_char(buf, ch);

    }

    if (victim->imm_flags) {

	sprintf(buf, "Immune: %s\n\r", imm_bit_name(victim->imm_flags));

	send_to_char(buf, ch);

    }

    if (victim->res_flags) {

	sprintf(buf, "Resist: %s\n\r", imm_bit_name(victim->res_flags));

	send_to_char(buf, ch);

    }

    if (victim->vuln_flags) {

	sprintf(buf, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));

	send_to_char(buf, ch);

    }

    sprintf(buf, "Form: %s\n\rParts: %s\n\r",

	    form_bit_name(victim->form), part_bit_name(victim->parts));

    send_to_char(buf, ch);



    if (victim->affected_by) {

	sprintf(buf, "Affected by %s\n\r",

		affect_bit_name(victim->affected_by));

	send_to_char(buf, ch);

    }

    if (victim->affected2_by) {

	sprintf(buf, "Also affected by %s\n\r",

		affect2_bit_name(victim->affected2_by));

	send_to_char(buf, ch);

    }

    sprintf(buf, "Master: %s  Leader: %s Following: %s Pet: %s  Horse: %s\n\r",

	    victim->master ? victim->master->name : "(none)",

	    victim->leader ? victim->leader->name : "(none)",

	    victim->following ? victim->following->name : "(none)",

	    victim->pet ? victim->pet->name : "(none)",

	    victim->mount ? victim->mount->name : "(none)");

    send_to_char(buf, ch);



    if (!IS_NPC(victim)) {

	sprintf(buf, "Security: %d.\n\r", victim->pcdata->security);	/* OLC */

	send_to_char(buf, ch);	/* OLC */

    }

    sprintf(buf, "Short description: %s\n\rLong  description: %s",

	    victim->short_descr,

	    victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r");

    send_to_char(buf, ch);



    sprintf(buf, "Hating: %s   Hunting: %s\n\r",

	    victim->hate ? victim->hate->who->name : "(none)",

	    victim->hunting ? victim->hunting->name : "nobody");

    send_to_char(buf, ch);



    if (IS_NPC(victim) && victim->spec_fun != 0) {

	sprintf(buf, "Mobile has special procedure %s.\n\r",

		spec_name(victim->spec_fun));

	send_to_char(buf, ch);

    }

    for (paf = victim->affected; paf != NULL; paf = paf->next) {

	sprintf(buf,

		"Spell: '%s' modifies %s by %d for %d hours with bits %s, level %d.\n\r",

		skill_table[(int) paf->type].name,

		affect_loc_name(paf->location),

		paf->modifier,

		paf->duration,

		affect_bit_name(paf->bitvector),

		paf->level

	    );

	send_to_char(buf, ch);

    }



    sprintf(buf, "\n\r");

    send_to_char(buf, ch);



    return;

}



/* ofind and mfind replaced with vnum, vnum skill also added */

void do_vnum(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    char *string;



    string = one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  vnum obj <name>\n\r", ch);

	send_to_char("  vnum mob <name>\n\r", ch);

	send_to_char("  vnum skill <skill or spell>\n\r", ch);

	return;

    }

    if (!str_cmp(arg, "obj")) {

	do_ofind(ch, string);

	return;

    }

    if (!str_cmp(arg, "mob") || !str_cmp(arg, "char")) {

	do_mfind(ch, string);

	return;

    }

    if (!str_cmp(arg, "skill") || !str_cmp(arg, "spell")) {

	do_slookup(ch, string);

	return;

    }

    /* do both */

    do_mfind(ch, argument);

    do_ofind(ch, argument);

}



void do_mfind(CHAR_DATA * ch, char *argument)

{

    extern int top_mob_index;

    char buf[MAX_STRING_LENGTH];

    char arg[MAX_INPUT_LENGTH];

    MOB_INDEX_DATA *pMobIndex;

    int vnum;

    int nMatch;

    bool fAll;

    bool found;



    one_argument(argument, arg);

    if (arg[0] == '\0') {

	send_to_char("Find whom?\n\r", ch);

	return;

    }

    fAll = FALSE;		/* !str_cmp( arg, "all" ); */

    found = FALSE;

    nMatch = 0;



    /*

     * Yeah, so iterating over all vnum's takes 10,000 loops.

     * Get_mob_index is fast, and I don't feel like threading another link.

     * Do you?

     * -- Furey

     */

    for (vnum = 0; nMatch < top_mob_index; vnum++) {

	if ((pMobIndex = get_mob_index(vnum)) != NULL) {

	    nMatch++;

	    if (fAll || is_name(argument, pMobIndex->player_name)) {

		found = TRUE;

		sprintf(buf, "[%5d] %s\n\r",

			pMobIndex->vnum, pMobIndex->short_descr);

		send_to_char(buf, ch);

	    }

	}

    }



    if (!found)

	send_to_char("No mobiles by that name.\n\r", ch);



    return;

}



void do_ofind(CHAR_DATA * ch, char *argument)

{

    extern int top_obj_index;

    char buf[MAX_STRING_LENGTH];

    char arg[MAX_INPUT_LENGTH];

    OBJ_INDEX_DATA *pObjIndex;

    int vnum;

    int nMatch;

    bool fAll;

    bool found;



    one_argument(argument, arg);

    if (arg[0] == '\0') {

	send_to_char("Find what?\n\r", ch);

	return;

    }

    fAll = FALSE;		/* !str_cmp( arg, "all" ); */

    found = FALSE;

    nMatch = 0;



    /*

     * Yeah, so iterating over all vnum's takes 10,000 loops.

     * Get_obj_index is fast, and I don't feel like threading another link.

     * Do you?

     * -- Furey

     */

    for (vnum = 0; nMatch < top_obj_index; vnum++) {

	if ((pObjIndex = get_obj_index(vnum)) != NULL) {

	    nMatch++;

	    if (fAll || is_name(argument, pObjIndex->name)) {

		found = TRUE;

		sprintf(buf, "[%5d] %s\n\r",

			pObjIndex->vnum, pObjIndex->short_descr);

		send_to_char(buf, ch);

	    }

	}

    }



    if (!found)

	send_to_char("No objects by that name.\n\r", ch);



    return;

}



void do_owhere(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_INPUT_LENGTH];

    char pers_buf[MAX_STRING_LENGTH];

    BUFFER *buffer;

    OBJ_DATA *obj;

    OBJ_DATA *in_obj;

    bool found;

    int number = 0, max_found;



    found = FALSE;

    number = 0;

    max_found = 200;



    buffer = new_buf();



    if (argument[0] == '\0') {

	send_to_char("Find what?\n\r", ch);

	return;

    }

    for (obj = object_list; obj != NULL; obj = obj->next) {

	if (!can_see_obj(ch, obj) || !is_name(argument, obj->name)

	    || ch->level < obj->level)

	    continue;



	found = TRUE;

	number++;



	for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);



	if (in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by)

	    && in_obj->carried_by->in_room != NULL) {

	    PERS(in_obj->carried_by, ch, pers_buf);

	    sprintf(buf, "%3d) %s is carried by %s [Room %d]\n\r",

		    number, obj->short_descr, pers_buf,

		    in_obj->carried_by->in_room->vnum);

	} else if (in_obj->in_room != NULL && can_see_room(ch, in_obj->in_room))

	    sprintf(buf, "%3d) %s is in %s [Room %d]\n\r",

		    number, obj->short_descr, in_obj->in_room->name,

		    in_obj->in_room->vnum);

	else

	    sprintf(buf, "%3d) %s is somewhere\n\r", number, obj->short_descr);



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

}



void do_mwhere(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    BUFFER *buffer;

    CHAR_DATA *victim;

    bool found;

    int count = 0;



    if (argument[0] == '\0') {

	DESCRIPTOR_DATA *d;



	/* show characters logged */



	buffer = new_buf();

	for (d = descriptor_list; d != NULL; d = d->next) {

	    if (d->character != NULL && d->connected == CON_PLAYING

		&& d->character->in_room != NULL && can_see(ch, d->character)

		&& can_see_room(ch, d->character->in_room)

		&& d->character->level < 110) {

		victim = d->character;

		count++;

		if (d->original != NULL)

		    sprintf(buf, "%3d) %s (in the body of %s) is in %s [%d]\n\r",

			    count, d->original->name, victim->short_descr,

			    victim->in_room->name, victim->in_room->vnum);

		else

		    sprintf(buf, "%3d) %s is in %s [%d]\n\r",

			    count, victim->name, victim->in_room->name,

			    victim->in_room->vnum);

		add_buf(buffer, buf);

	    }

	}



	page_to_char(buf_string(buffer), ch);

	free_buf(buffer);

	return;

    }

    found = FALSE;

    buffer = new_buf();

    for (victim = char_list; victim != NULL; victim = victim->next) {

	if (victim->in_room != NULL

	    && is_name(argument, victim->name)

	    && victim->level < 110) {

	    found = TRUE;

	    count++;

	    sprintf(buf, "%3d) [%5d] %-28s [%5d] %s\n\r", count,

		    IS_NPC(victim) ? victim->pIndexData->vnum : 0,

		    IS_NPC(victim) ? victim->short_descr : victim->name,

		    victim->in_room->vnum,

		    victim->in_room->name);

	    add_buf(buffer, buf);

	}

    }



    if (!found)

	act("You didn't find any $T.", ch, NULL, argument, TO_CHAR);

    else

	page_to_char(buf_string(buffer), ch);



    free_buf(buffer);



    return;

}



void do_reboo(CHAR_DATA * ch, char *argument)

{

    send_to_char("If you want to REBOOT, spell it out.\n\r", ch);

    return;

}



void do_reboot(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    extern bool merc_down;

    DESCRIPTOR_DATA *d, *d_next;

    CHAR_DATA *vch;



    if (ch->invis_level < LEVEL_HERO) {

	sprintf(buf, "Reboot by %s.", ch->name);

	do_echo(ch, buf);

    }

    merc_down = TRUE;

    for (d = descriptor_list; d != NULL; d = d_next) {

	d_next = d->next;

	vch = d->original ? d->original : d->character;

	if ((vch != NULL) && (vch->level > 1))

	    save_char_obj(vch);

	close_socket(d);

    }



    return;

}



void do_shutdow(CHAR_DATA * ch, char *argument)

{

    send_to_char("If you want to SHUTDOWN, spell it out.\n\r", ch);

    return;

}



void do_shutdown(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    extern bool merc_down;

/*

    DESCRIPTOR_DATA *d, *d_next;

    CHAR_DATA *vch;

*/



    if (ch->invis_level < LEVEL_HERO)

	sprintf(buf, "Shutdown by %s.", ch->name);

    append_file(ch, SHUTDOWN_FILE, buf);

    strcat(buf, "\n\r");

    if (ch->invis_level < LEVEL_HERO)

	do_echo(ch, buf);

    merc_down = TRUE;

    do_auto_shutdown(FALSE);

/*

    for (d = descriptor_list; d != NULL; d = d_next) {

	d_next = d->next;

	vch = d->original ? d->original : d->character;

	if ((vch != NULL) && (vch->level > 1))

	    save_char_obj(vch);

	close_socket(d);

    }

*/

    return;

}



void do_protect(CHAR_DATA * ch, char *argument)

{

    CHAR_DATA *victim;



    if (argument[0] == '\0') {

	send_to_char("Protect whom from snooping?\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, argument)) == NULL) {

	send_to_char("You can't find them.\n\r", ch);

	return;

    }

    if (IS_SET(victim->comm, COMM_SNOOP_PROOF)) {

	act_new("$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD);

//	send_to_char("Your snoop-proofing was just removed.\n\r", victim);

	REMOVE_BIT(victim->comm, COMM_SNOOP_PROOF);

    } else {

	act_new("$N is now snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD);

//	send_to_char("You are now immune to snooping.\n\r", victim);

	SET_BIT(victim->comm, COMM_SNOOP_PROOF);

    }

}



void do_snoop(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    DESCRIPTOR_DATA *d;

    CHAR_DATA *victim;

    char buf[MAX_STRING_LENGTH];



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Snoop whom?\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (victim->desc == NULL) {

	send_to_char("No descriptor to snoop.\n\r", ch);

	return;

    }

    if (victim == ch) {

	send_to_char("Cancelling all snoops.\n\r", ch);

	wiznet("$N stops being such a snoop.",

	       ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust(ch));

	for (d = descriptor_list; d != NULL; d = d->next) {

	    if (d->snoop_by == ch->desc)

		d->snoop_by = NULL;

	}

	return;

    }

    if (victim->desc->snoop_by != NULL) {

	send_to_char("Busy already.\n\r", ch);

	return;

    }

    if (!is_room_owner(ch, victim->in_room) && ch->in_room != victim->in_room

	&& room_is_private(victim->in_room) && !IS_TRUSTED(ch, IMPLEMENTOR)) {

	send_to_char("That character is in a private room.\n\r", ch);

	return;

    }

    if (get_trust(victim) >= get_trust(ch)

	|| IS_SET(victim->comm, COMM_SNOOP_PROOF)) {

	send_to_char("You failed.\n\r", ch);

	return;

    }

    if (ch->desc != NULL) {

	for (d = ch->desc->snoop_by; d != NULL; d = d->snoop_by) {

	    if (d->character == victim || d->original == victim) {

		send_to_char("No snoop loops.\n\r", ch);

		return;

	    }

	}

    }

    victim->desc->snoop_by = ch->desc;

    sprintf(buf, "$N starts snooping on %s",

	    (IS_NPC(ch) ? victim->short_descr : victim->name));

    wiznet(buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust(ch));

    send_to_char("You sure are nosy.\n\r", ch);

    return;

}



void do_switch(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Switch into whom?\n\r", ch);

	return;

    }

    if (ch->desc == NULL)

	return;



    if (ch->desc->original != NULL) {

	send_to_char("You are already switched.\n\r", ch);

	return;

    }

    if ((victim = get_char_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (victim == ch) {

	send_to_char("You take possesion of a new form.\n\r", ch);

	return;

    }

    if (!IS_NPC(victim)) {

	send_to_char("You can only switch into mobiles.\n\r", ch);

	return;

    }

    if (!is_room_owner(ch, victim->in_room) && ch->in_room != victim->in_room

	&& room_is_private(victim->in_room) && !IS_TRUSTED(ch, IMPLEMENTOR)) {

	send_to_char("That character is in a private room.\n\r", ch);

	return;

    }

    if (victim->desc != NULL) {

	send_to_char("Character in use.\n\r", ch);

	return;

    }

    sprintf(buf, "$N switches into %s", victim->short_descr);

    wiznet(buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust(ch));



    ch->desc->character = victim;

    ch->desc->original = ch;

    victim->desc = ch->desc;

    ch->desc = NULL;

    /* change communications to match */

    if (ch->prompt != NULL)

	victim->prompt = str_dup(ch->prompt);

    victim->comm = ch->comm;

    victim->lines = ch->lines;

    send_to_char("You take possession of a new form.\n\r", victim);

    return;

}



void do_return(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];



    if (ch->desc == NULL)

	return;



    if (ch->desc->original == NULL) {

	send_to_char("You aren't switched.\n\r", ch);

	return;

    }

    send_to_char(

		    "You return to your original body. Type replay to see any missed tells.\n\r",

		    ch);

    if (ch->prompt != NULL) {

	free_string(ch->prompt);

	ch->prompt = NULL;

    }

    sprintf(buf, "$N returns from %s.", ch->short_descr);

    wiznet(buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE, get_trust(ch));

    ch->desc->character = ch->desc->original;

    ch->desc->original = NULL;

    ch->desc->character->desc = ch->desc;

    ch->desc = NULL;

    return;

}



/* trust levels for load and clone */

bool obj_check(CHAR_DATA * ch, OBJ_DATA * obj)

{

    if (IS_TRUSTED(ch, GOD)

	|| (IS_TRUSTED(ch, IMMORTAL) && obj->level <= 20 && obj->cost <= 1000)

	|| (IS_TRUSTED(ch, DEMI) && obj->level <= 10 && obj->cost <= 500)

	|| (IS_TRUSTED(ch, ANGEL) && obj->level <= 5 && obj->cost <= 250)

	|| (IS_TRUSTED(ch, AVATAR) && obj->level == 0 && obj->cost <= 100))

	return TRUE;

    else

	return FALSE;

}



/* for clone, to insure that cloning goes many levels deep */

void recursive_clone(CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * clone)

{

    OBJ_DATA *c_obj, *t_obj;



    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content) {

	if (obj_check(ch, c_obj)) {

	    t_obj = create_object(c_obj->pIndexData, 0);

	    clone_object(c_obj, t_obj);

	    obj_to_obj(t_obj, clone);

	    recursive_clone(ch, c_obj, t_obj);

	}

    }

}



/* command that is similar to load */

void do_clone(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    char *rest;

    CHAR_DATA *mob;

    OBJ_DATA *obj;



    rest = one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Clone what?\n\r", ch);

	return;

    }

    if (!str_prefix(arg, "object")) {

	mob = NULL;

	obj = get_obj_here(ch, rest);

	if (obj == NULL) {

	    send_to_char("You don't see that here.\n\r", ch);

	    return;

	}

    } else if (!str_prefix(arg, "mobile") || !str_prefix(arg, "character")) {

	obj = NULL;

	mob = get_char_room(ch, rest);

	if (mob == NULL) {

	    send_to_char("You don't see that here.\n\r", ch);

	    return;

	}

    } else {			/* find both */

	mob = get_char_room(ch, argument);

	obj = get_obj_here(ch, argument);

	if (mob == NULL && obj == NULL) {

	    send_to_char("You don't see that here.\n\r", ch);

	    return;

	}

    }



    /* clone an object */

    if (obj != NULL) {

	OBJ_DATA *clone;



	if (!obj_check(ch, obj)) {

	    send_to_char(

			    "Your powers are not great enough for such a task.\n\r", ch);

	    return;

	}

	clone = create_object(obj->pIndexData, 0);

	clone_object(obj, clone);

	if (obj->carried_by != NULL)

	    obj_to_char(clone, ch);

	else

	    obj_to_room(clone, ch->in_room);

	recursive_clone(ch, obj, clone);



	act("$n has created $p.", ch, clone, NULL, TO_ROOM);

	act("You clone $p.", ch, clone, NULL, TO_CHAR);

	wiznet("$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE, get_trust(ch));

	return;

    } else if (mob != NULL) {

	CHAR_DATA *clone;

	OBJ_DATA *new_obj;

	char buf[MAX_STRING_LENGTH];



	if (!IS_NPC(mob)) {

	    send_to_char("You can only clone mobiles.\n\r", ch);

	    return;

	}

	if ((mob->level > 20 && !IS_TRUSTED(ch, GOD))

	    || (mob->level > 10 && !IS_TRUSTED(ch, IMMORTAL))

	    || (mob->level > 5 && !IS_TRUSTED(ch, DEMI))

	    || (mob->level > 0 && !IS_TRUSTED(ch, ANGEL))

	    || !IS_TRUSTED(ch, AVATAR)) {

	    send_to_char(

			    "Your powers are not great enough for such a task.\n\r", ch);

	    return;

	}

	clone = create_mobile(mob->pIndexData);

	clone_mobile(mob, clone);



	for (obj = mob->carrying; obj != NULL; obj = obj->next_content) {

	    if (obj_check(ch, obj)) {

		new_obj = create_object(obj->pIndexData, 0);

		clone_object(obj, new_obj);

		recursive_clone(ch, obj, new_obj);

		obj_to_char(new_obj, clone);

		new_obj->wear_loc = obj->wear_loc;

	    }

	}

	char_to_room(clone, ch->in_room);

	act("$n has created $N.", ch, NULL, clone, TO_ROOM);

	act("You clone $N.", ch, NULL, clone, TO_CHAR);

	sprintf(buf, "$N clones %s.", clone->short_descr);

	wiznet(buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust(ch));

	return;

    }

}



/* RT to replace the two load commands */

void do_load(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];



    argument = one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  load mob <vnum>\n\r", ch);

	send_to_char("  load obj <vnum> <level>\n\r", ch);

	return;

    }

    if (!str_cmp(arg, "mob") || !str_cmp(arg, "char")) {

	do_mload(ch, argument);

	return;

    }

    if (!str_cmp(arg, "obj")) {

	do_oload(ch, argument);

	return;

    }

    /* echo syntax */

    do_load(ch, "");

}



void do_mload(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    MOB_INDEX_DATA *pMobIndex;

    CHAR_DATA *victim;

    char buf[MAX_STRING_LENGTH];



    one_argument(argument, arg);



    if (arg[0] == '\0' || !is_number(arg)) {

	send_to_char("Syntax: load mob <vnum>.\n\r", ch);

	return;

    }

    if ((pMobIndex = get_mob_index(atoi(arg))) == NULL) {

	send_to_char("No mob has that vnum.\n\r", ch);

	return;

    }

    victim = create_mobile(pMobIndex);

    char_to_room(victim, ch->in_room);

    act("$n has created $N!\n\r", ch, NULL, victim, TO_ROOM);

    sprintf(buf, "$N loads %s.", victim->name);

    wiznet(buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust(ch));

    sprintf(buf, "You create %s.\n\r", victim->short_descr);
    send_to_char(buf,ch);

    return;

}



void do_oload(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    OBJ_INDEX_DATA *pObjIndex;

    OBJ_DATA *obj;

    int level;



    argument = one_argument(argument, arg1);

    one_argument(argument, arg2);



    if (arg1[0] == '\0' || !is_number(arg1)) {

	send_to_char("Syntax: load obj <vnum> <level>.\n\r", ch);

	return;

    }

    level = get_trust(ch);	/* default */



    if (arg2[0] != '\0') {	/* load with a level */

	if (!is_number(arg2)) {

	    send_to_char("Syntax: oload <vnum> <level>.\n\r", ch);

	    return;

	}

	level = atoi(arg2);

	if (level < 0 || level > get_trust(ch)) {

	    send_to_char("Level must be be between 0 and your level.\n\r", ch);

	    return;

	}

    }

    if ((pObjIndex = get_obj_index(atoi(arg1))) == NULL) {

	send_to_char("No object has that vnum.\n\r", ch);

	return;

    }

    obj = create_object(pObjIndex, level);

    if (CAN_WEAR(obj, ITEM_TAKE))

	obj_to_char(obj, ch);

    else

	obj_to_room(obj, ch->in_room);

    act("$n has created $p! <-- Hobbie fixed Izludes Mistake!!\n\r", ch, obj, NULL, TO_ROOM);

    wiznet("$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust(ch));

    sprintf(buf, "You create %s.\n\r", obj->short_descr);
    send_to_char(buf,ch);
    
    return;

}



void do_purge(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];
	char arg2[MIL];
	char notearg[MIL];
    char buf[100];
    char strsave[100];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    argument = one_argument(argument, arg);
	one_argument(argument, arg2);

    if (arg[0] == '\0') {
	/* 'purge' */
	CHAR_DATA *vnext;
	OBJ_DATA *obj_next;

	for (victim = ch->in_room->people; victim != NULL; victim = vnext) {
	    vnext = victim->next_in_room;
	    if (IS_NPC(victim) && !IS_SET(victim->act, ACT_NOPURGE)
		&& victim != ch /* safety precaution */ )
		extract_char(victim, TRUE);
	}

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next) {
	    obj_next = obj->next_content;
	    if (!IS_OBJ_STAT(obj, ITEM_NOPURGE))
		extract_obj(obj);
	}



	act("$n purges the room!", ch, NULL, NULL, TO_ROOM);
	send_to_char("You clean house.\n\r", ch);
	return;
    }

    if ((victim = get_player_world(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (!IS_NPC(victim)) {

	if (ch == victim) {
	    send_to_char("Ho ho ho.\n\r", ch);
	    return;
	}

	if (get_trust(ch) <= get_trust(victim)) {
	    send_to_char("Maybe that wasn't a good idea...\n\r", ch);
	    sprintf(buf, "%s tried to purge you!\n\r", ch->name);
	    send_to_char(buf, victim);
	    return;

	}

	if (arg2[0] == '\0')
	{
		send_to_char("Purge command updated!\n\r", ch);
		send_to_char("Synax: Purge <victim> <reason>\n\r", ch);
		return;
	}
	sprintf(notearg, "{R[Auto]{R PURGE:{W %s", victim->name);
	act("$n disintegrates $N.", ch, 0, victim, TO_NOTVICT);


	sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(victim->name));
	sprintf(buf, "%s has been PURGED from AoW!!\n\r", victim->name);
	stop_fighting(victim, TRUE);
	news(buf, victim->side);
	victim->pcdata->confirm_delete = TRUE;
	do_quit(victim, "");
	unlink(strsave);

	parse_penalty(ch, notearg, arg2);


	/*

	   if (victim->level > 1)

	   save_char_obj( victim );

	   d = victim->desc;

	   extract_char( victim, TRUE );

	   if ( d != NULL )

	   close_socket( d );

	 */

	return;
    }

    act("$n purges $N.", ch, NULL, victim, TO_NOTVICT);
    extract_char(victim, TRUE);
    return;

}



void do_advance(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;

    int level;

    int iLevel;



    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);



    if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2)) {

	send_to_char("Syntax: advance <char> <level>.\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("That player is not here.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("Not on NPC's.\n\r", ch);

	return;

    }
    
    if ((ch->trust == victim->trust) && (ch->level == victim->level)) 
    
    {

	send_to_char("You cannot advance/demote an equal. The word is 'demote', not devance.\n\r", ch);

	return;

    }

    if ((level = atoi(arg2)) < 1 || level > 110) {

	send_to_char("Level must be 1 to 110.\n\r", ch);

	return;

    }

    if (level > get_trust(ch)) {

	send_to_char("Limited to your trust level.\n\r", ch);

	return;

   }
	if(victim->level == 110)
	{
	send_to_char("I'm sorry we are unable to process your request.\n\r",ch);
	return;
	}

    /*

     * Lower level:

     *   Reset to level 1.

     *   Then raise again.

     *   Currently, an imp can lower another imp.

     *   -- Swiftest

     */

    if (level <= victim->level) {

	int temp_prac;



	send_to_char("Lowering a player's level!\n\r", ch);

	send_to_char("**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim);

	temp_prac = victim->practice;

	victim->level = 1;

	victim->exp = exp_per_level(victim, victim->pcdata->points);

	victim->max_hit = 10;

	victim->max_mana = 100;

	victim->max_move = 100;

	victim->practice = 0;

	victim->hit = victim->max_hit;

	victim->mana = victim->max_mana;

	victim->move = victim->max_move;

	advance_level(victim, TRUE);

	victim->practice = temp_prac;

    } else {

	send_to_char("Raising a player's level!\n\r", ch);

	send_to_char("**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim);

    }



    for (iLevel = victim->level; iLevel < level; iLevel++) {

	victim->level += 1;

	advance_level(victim, TRUE);

    }

    sprintf(buf, "You are now level %d.\n\r", victim->level);

    send_to_char(buf, victim);

    victim->exp = exp_per_level(victim, victim->pcdata->points)

	* UMAX(1, victim->level);

    victim->trust = 0;

    save_char_obj(victim);

    return;

}



void do_trust(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;

    int level;



    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);



    if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2)) {

	send_to_char("Syntax: trust <char> <level>.\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("That player is not here.\n\r", ch);

	return;

    }

    if ((level = atoi(arg2)) < 0 || level > 110) {

	send_to_char("Level must be 0 (reset) or 1 to 110.\n\r", ch);

	return;

    }

    if (level > get_trust(ch)) {

	send_to_char("Limited to your trust.\n\r", ch);

	return;

    }

    victim->trust = level;

    return;

}



void do_restore(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;

    CHAR_DATA *vch;

    DESCRIPTOR_DATA *d;



    one_argument(argument, arg);

    if (arg[0] == '\0' || !str_cmp(arg, "room")) {

	/* cure room */



	for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {

	    affect_strip(vch, gsn_plague);

	    affect_strip(vch, gsn_poison);

	    affect_strip(vch, gsn_blindness);

	    affect_strip(vch, gsn_sleep);

	    affect_strip(vch, gsn_curse);



	    vch->hit = vch->max_hit;

	    vch->mana = vch->max_mana;

	    vch->move = vch->max_move;

	    update_pos(vch);

	    act("$n has restored you.", ch, NULL, vch, TO_VICT);

	}



	sprintf(buf, "$N restored room %d.", ch->in_room->vnum);

	wiznet(buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust(ch));



	send_to_char("Room restored.\n\r", ch);

	return;



    }

    if (get_trust(ch) >= MAX_LEVEL - 1 && !str_cmp(arg, "all")) {

	/* cure all */



	for (d = descriptor_list; d != NULL; d = d->next) {

	    victim = d->character;



	    if (victim == NULL || IS_NPC(victim))

		continue;



	    affect_strip(victim, gsn_plague);

	    affect_strip(victim, gsn_poison);

	    affect_strip(victim, gsn_blindness);

	    affect_strip(victim, gsn_sleep);

	    affect_strip(victim, gsn_curse);



	    victim->hit = victim->max_hit;

	    victim->mana = victim->max_mana;

	    victim->move = victim->max_move;

	    update_pos(victim);

	    if (victim->in_room != NULL)
		
	    
		act("$n has restored you.", ch, NULL, victim, TO_VICT);

	}

	send_to_char("All active players restored.\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    affect_strip(victim, gsn_plague);

    affect_strip(victim, gsn_poison);

    affect_strip(victim, gsn_blindness);

    affect_strip(victim, gsn_sleep);

    affect_strip(victim, gsn_curse);

    victim->hit = victim->max_hit;

    victim->mana = victim->max_mana;

    victim->move = victim->max_move;

    update_pos(victim);

    act("$n has restored you.", ch, NULL, victim, TO_VICT);

    sprintf(buf, "$N restored %s",

    IS_NPC(victim) ? victim->short_descr : victim->name);

   wiznet(buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust(ch));

    send_to_char("You grace them with your divine benefits.\n\r", ch);   

    return;

}



void do_freeze(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
	char arg2[MIL];
	char notearg[MSL];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);
	one_argument(argument, arg2);
	

    if (arg[0] == '\0') {
	send_to_char("Freeze whom?\n\r", ch);
	return;
    }

    if ((victim = get_player_world(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }

    if (IS_NPC(victim)) {
	send_to_char("Not on NPC's.\n\r", ch);
	return;
    }

    if (get_trust(victim) >= get_trust(ch)) {
	send_to_char("You failed.\n\r", ch);
	return;
    }

			
	if (IS_SET(victim->act, PLR_FREEZE)) {
	REMOVE_BIT(victim->act, PLR_FREEZE);
	send_to_char("You can play again.\n\r", victim);
	send_to_char("FREEZE removed.\n\r", ch);
	sprintf(buf, "$N thaws %s.", victim->name);
	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
    } else {
	if (arg2[0] == '\0')
	{
		send_to_char("Freeze command updated!\n\rSyntax: Freeze <player> <reason>", ch);
		send_to_char("Reason is automagiclly posted in a penalty!\n\r", ch);
		return;
	}
	SET_BIT(victim->act, PLR_FREEZE);
	send_to_char("You can't do ANYthing!\n\r", victim);
	send_to_char("FREEZE set.\n\r", ch);
	sprintf(buf, "$N puts %s in the deep freeze.", victim->name);
	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	sprintf(notearg, "{R[Auto]{C Freeze:{R %s", victim->name);
	parse_penalty(ch, notearg, arg2);

    }



    if (victim->level > 1)
	save_char_obj(victim);



    return;

}



void do_log(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Log whom?\n\r", ch);

	return;

    }

    if (!str_cmp(arg, "all")) {

	if (fLogAll) {

	    fLogAll = FALSE;

	    send_to_char("Log ALL off.\n\r", ch);

	} else {

	    fLogAll = TRUE;

	    send_to_char("Log ALL on.\n\r", ch);

	}

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("Not on NPC's.\n\r", ch);

	return;

    }

    /*

     * No level check, gods can log anyone.

     */

    if (IS_SET(victim->act, PLR_LOG)) {

	REMOVE_BIT(victim->act, PLR_LOG);

	send_to_char("LOG removed.\n\r", ch);

    } else {

	SET_BIT(victim->act, PLR_LOG);

	send_to_char("LOG set.\n\r", ch);

    }



    return;

}



void do_noemote(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Noemote whom?\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (get_trust(victim) >= get_trust(ch)) {

	send_to_char("You failed.\n\r", ch);

	return;

    }

    if (IS_SET(victim->comm, COMM_NOEMOTE)) {

	REMOVE_BIT(victim->comm, COMM_NOEMOTE);

	send_to_char("You can emote again.\n\r", victim);

	send_to_char("NOEMOTE removed.\n\r", ch);

	sprintf(buf, "$N restores emotes to %s.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    } else {

	SET_BIT(victim->comm, COMM_NOEMOTE);

	send_to_char("You can't emote!\n\r", victim);

	send_to_char("NOEMOTE set.\n\r", ch);

	sprintf(buf, "$N revokes %s's emotes.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    }



    return;

}



void do_noshout(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Noshout whom?\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("Not on NPC's.\n\r", ch);

	return;

    }

    if (get_trust(victim) >= get_trust(ch)) {

	send_to_char("You failed.\n\r", ch);

	return;

    }

    if (IS_SET(victim->comm, COMM_NOSHOUT)) {

	REMOVE_BIT(victim->comm, COMM_NOSHOUT);

	send_to_char("You can shout again.\n\r", victim);

	send_to_char("NOSHOUT removed.\n\r", ch);

	sprintf(buf, "$N restores shouts to %s.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    } else {

	SET_BIT(victim->comm, COMM_NOSHOUT);

	send_to_char("You can't shout!\n\r", victim);

	send_to_char("NOSHOUT set.\n\r", ch);

	sprintf(buf, "$N revokes %s's shouts.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    }



    return;

}



void do_notell(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];

    CHAR_DATA *victim;



    one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Notell whom?", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (get_trust(victim) >= get_trust(ch)) {

	send_to_char("You failed.\n\r", ch);

	return;

    }

    if (IS_SET(victim->comm, COMM_NOTELL)) {

	REMOVE_BIT(victim->comm, COMM_NOTELL);

	send_to_char("You can tell again.\n\r", victim);

	send_to_char("NOTELL removed.\n\r", ch);

	sprintf(buf, "$N restores tells to %s.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    } else {

	SET_BIT(victim->comm, COMM_NOTELL);

	send_to_char("You can't tell!\n\r", victim);

	send_to_char("NOTELL set.\n\r", ch);

	sprintf(buf, "$N revokes %s's tells.", victim->name);

	wiznet(buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);

    }



    return;

}



void do_peace(CHAR_DATA * ch, char *argument)

{

    CHAR_DATA *rch;



    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	if (rch->fighting != NULL)

	    stop_fighting(rch, TRUE);

	if (IS_NPC(rch) && IS_SET(rch->act, ACT_AGGRESSIVE))

	    REMOVE_BIT(rch->act, ACT_AGGRESSIVE);

	stop_hating(rch);



    }



    send_to_char("Peace has been declared.\n\r", ch);

    return;

}



void do_wizlock(CHAR_DATA * ch, char *argument)

{

    extern bool wizlock;

    wizlock = !wizlock;



    if (wizlock) {

	wiznet("$N has wizlocked the game.", ch, NULL, 0, 0, 0);

	send_to_char("Game wizlocked.\n\r", ch);

    } else {

	wiznet("$N removes wizlock.", ch, NULL, 0, 0, 0);

	send_to_char("Game un-wizlocked.\n\r", ch);

    }



    return;

}



void do_saltrecal(CHAR_DATA * ch, char *argument)

{

	send_to_char("spell it out. this toggle makes ppl recall to salt mines...\n\r", ch);

	send_to_char("for use with armageddon quests.\n\r", ch);

}

void do_saltrecall(CHAR_DATA * ch, char *argument)

{

    extern bool saltrecall;



	saltrecall = !saltrecall;



    if (saltrecall) {

	wiznet("$N sets all recalling to salt mines.", ch, NULL, 0, 0, 0);

	send_to_char("all to saltmines.\n\r", ch);

    } else {

	wiznet("$N resumes normal recalling.", ch, NULL, 0, 0, 0);

	send_to_char("back to normal.\n\r", ch);

    }



    return;

}



void do_infect(CHAR_DATA * ch, char *argument)

{





}



/* RT anti-newbie code */

void do_newlock(CHAR_DATA * ch, char *argument)

{

    extern bool newlock;

    newlock = !newlock;



    if (newlock) {

	wiznet("$N locks out new characters.", ch, NULL, 0, 0, 0);

	send_to_char("New characters have been locked out.\n\r", ch);

    } else {

	wiznet("$N allows new characters back in.", ch, NULL, 0, 0, 0);

	send_to_char("Newlock removed.\n\r", ch);

    }



    return;

}

void do_armageddo(CHAR_DATA * ch, char *argument)

{

    send_to_char("armageddon quest.\n\r", ch);

    send_to_char("mobs stop repopping.\n\r", ch);

    send_to_char("death causes a quit.\n\r", ch);

    send_to_char("dont forget to wizlock.\n\r", ch);

}

void do_armageddon(CHAR_DATA * ch, char *argument)

{

    extern bool armageddon;

    armageddon = !armageddon;



	if(armageddon){

		news("{RThe bloodshed starts now!", SIDE_KAEL);

		news("{RThe bloodshed starts now!", SIDE_TIER);

		news("{RThe bloodshed starts now!", SIDE_UM);

		news("{RThe bloodshed starts now!", SIDE_CAIRN);

		news("{RThe bloodshed starts now!", SIDE_NONE);

	}

    return;

}



void do_slookup(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    char arg[MAX_INPUT_LENGTH];

    int sn;



    one_argument(argument, arg);

    if (arg[0] == '\0') {

	send_to_char("Lookup which skill or spell?\n\r", ch);

	return;

    }

    if (!str_cmp(arg, "all")) {

	for (sn = 0; sn < MAX_SKILL; sn++) {

	    if (skill_table[sn].name == NULL)

		break;

	    sprintf(buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",

		    sn, skill_table[sn].slot, skill_table[sn].name);

	    send_to_char(buf, ch);

	}

    } else {

	if ((sn = skill_lookup(arg)) < 0) {

	    send_to_char("No such skill or spell.\n\r", ch);

	    return;

	}

	sprintf(buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",

		sn, skill_table[sn].slot, skill_table[sn].name);

	send_to_char(buf, ch);

    }



    return;

}



/* RT set replaces sset, mset, oset, and rset */

void do_set(CHAR_DATA * ch, char *argument)

{

    char arg[MAX_INPUT_LENGTH];



    argument = one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  set mob   <name> <field> <value>\n\r", ch);

	send_to_char("  set obj   <name> <field> <value>\n\r", ch);

	send_to_char("  set room  <room> <field> <value>\n\r", ch);

	send_to_char("  set skill <name> <spell or skill> <value>\n\r", ch);

	send_to_char("  set side  <name> <field> \n\r", ch);

	return;

    }

    if (!str_prefix(arg, "mobile") || !str_prefix(arg, "character")) {

	do_mset(ch, argument);

	return;

    }

    if (!str_prefix(arg, "side")) {



	do_sideSet(ch, argument);

	return;

    }

    if (!str_prefix(arg, "skill") || !str_prefix(arg, "spell")) {

	do_sset(ch, argument);

	return;

    }

    if (!str_prefix(arg, "object")) {

	do_oset(ch, argument);

	return;

    }

    if (!str_prefix(arg, "room")) {

	do_rset(ch, argument);

	return;

    }

    /* echo syntax */

    do_set(ch, "");

}



void do_sideSet(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;

    int value;



    smash_tilde(argument);

    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);



    /* arg1 name  , arg2 side value */

    if (arg1[0] == '\0' || arg2[0] == '\0') {

	send_to_char("Syntax set side <name> <side>\n\r", ch);

	send_to_char("Side is none, tier, kael, undermountain, um, or cairn; or 0,1,2,3 or 4.\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("Not on NPC's.\n\r", ch);

	return;

    }

    if (!is_number(arg2) && is_name(arg2, "tier kael undermountain um cairn")) {

	if (is_name(arg2, "none"))

	    victim->side = SIDE_NONE;

	if (is_name(arg2, "tier"))

	    victim->side = SIDE_TIER;

	if (is_name(arg2, "kael"))

	    victim->side = SIDE_KAEL;

	if (is_name(arg2, "um"))

	    victim->side = SIDE_UM;

	if (is_name(arg2, "undermountain"))

	    victim->side = SIDE_UM;

	if (is_name(arg2, "cairn"))

	    victim->side = SIDE_CAIRN;

	send_to_char("Your side has been changed.\n\r", victim);

	send_to_char("Thier side has been changed.\n\r", ch);

	return;

    }

    value = atoi(arg2);



    if (value < 0 || value > 4) {

	send_to_char("Side must be 0,1,2,3 or 4.\n\r", ch);

	return;

    }

    victim->side = value;

    send_to_char("Your side has been changed.\n\r", victim);

    send_to_char("Thier side has been changed.\n\r", ch);

}



void do_sset(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    char arg3[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;

    int value;

    int sn;

    bool fAll;



    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);

    argument = one_argument(argument, arg3);



    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  set skill <name> <spell or skill> <value>\n\r", ch);

	send_to_char("  set skill <name> all <value>\n\r", ch);

	send_to_char("   (use the name of the skill, not the number)\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    if (IS_NPC(victim)) {

	send_to_char("Not on NPC's.\n\r", ch);

	return;

    }

    fAll = !str_cmp(arg2, "all");

    sn = 0;

    if (!fAll && (sn = skill_lookup(arg2)) < 0) {

	send_to_char("No such skill or spell.\n\r", ch);

	return;

    }

    /*

     * Snarf the value.

     */

    if (!is_number(arg3)) {

	send_to_char("Value must be numeric.\n\r", ch);

	return;

    }

    value = atoi(arg3);

    if (value < 0 || value > 100) {

	send_to_char("Value range is 0 to 100.\n\r", ch);

	return;

    }

    if (fAll) {

	for (sn = 0; sn < MAX_SKILL; sn++) {

	    if (skill_table[sn].name != NULL)

		victim->pcdata->learned[sn] = value;

	}

    } else {

	victim->pcdata->learned[sn] = value;

    }



    return;

}



void do_mset(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    char arg3[MAX_INPUT_LENGTH];

    char buf[100];

    CHAR_DATA *victim;

    int value;



    smash_tilde(argument);

    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);

    strcpy(arg3, argument);



    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  set char <name> <field> <value>\n\r", ch);

	send_to_char("  Field being one of:\n\r", ch);

	send_to_char("    str int wis dex con sex class level\n\r", ch);

	send_to_char("    race group gold silver hp mana move prac\n\r", ch);

	send_to_char("    align train thirst hunger drunk full\n\r", ch);

	send_to_char("    security\n\r", ch);

	return;

    }

    if ((victim = get_player_world(ch, arg1)) == NULL) {

	send_to_char("They aren't here.\n\r", ch);

	return;

    }

    /* clear zones for mobs */

    victim->zone = NULL;



    /*

     * Snarf the value (which need not be numeric).

     */

    value = is_number(arg3) ? atoi(arg3) : -1;



    /*

     * Set something.

     */

    if (!str_cmp(arg2, "str")) {

	if (value < 3 || value > get_max_train(victim, STAT_STR)) {

	    sprintf(buf,

		    "Strength range is 3 to %d\n\r.",

		    get_max_train(victim, STAT_STR));

	    send_to_char(buf, ch);

	    return;

	}

	victim->perm_stat[STAT_STR] = value;

	return;

    }

    if (!str_cmp(arg2, "security")) {	/* OLC */

	if (IS_NPC(victim)) {

	    send_to_char("Not on NPC's.\n\r", ch);

	    return;

	}

	if (value > ch->pcdata->security || value < 0) {

	    if (ch->pcdata->security != 0) {

		sprintf(buf, "Valid security is 0-%d.\n\r",

			ch->pcdata->security);

		send_to_char(buf, ch);

	    } else {

		send_to_char("Valid security is 0 only.\n\r", ch);

	    }

	    return;

	}

	victim->pcdata->security = value;

	return;

    }

    if (!str_cmp(arg2, "int")) {

	if (value < 3 || value > get_max_train(victim, STAT_INT)) {

	    sprintf(buf,

		    "Intelligence range is 3 to %d.\n\r",

		    get_max_train(victim, STAT_INT));

	    send_to_char(buf, ch);

	    return;

	}

	victim->perm_stat[STAT_INT] = value;

	return;

    }

    if (!str_cmp(arg2, "wis")) {

	if (value < 3 || value > get_max_train(victim, STAT_WIS)) {

	    sprintf(buf,

		    "Wisdom range is 3 to %d.\n\r", get_max_train(victim, STAT_WIS));

	    send_to_char(buf, ch);

	    return;

	}

	victim->perm_stat[STAT_WIS] = value;

	return;

    }

    if (!str_cmp(arg2, "dex")) {

	if (value < 3 || value > get_max_train(victim, STAT_DEX)) {

	    sprintf(buf,

		    "Dexterity ranges is 3 to %d.\n\r",

		    get_max_train(victim, STAT_DEX));

	    send_to_char(buf, ch);

	    return;

	}

	victim->perm_stat[STAT_DEX] = value;

	return;

    }

    if (!str_cmp(arg2, "con")) {

	if (value < 3 || value > get_max_train(victim, STAT_CON)) {

	    sprintf(buf,

		    "Constitution range is 3 to %d.\n\r",

		    get_max_train(victim, STAT_CON));

	    send_to_char(buf, ch);

	    return;

	}

	victim->perm_stat[STAT_CON] = value;

	return;

    }

    if (!str_prefix(arg2, "sex")) {

	if (value < 0 || value > 2) {

	    send_to_char("Sex range is 0 to 2.\n\r", ch);

	    return;

	}

	victim->sex = value;

	if (!IS_NPC(victim))

	    victim->pcdata->true_sex = value;

	return;

    }

    if (!str_prefix(arg2, "class")) {

	int class;



	if (IS_NPC(victim)) {

	    send_to_char("Mobiles have no class.\n\r", ch);

	    return;

	}

	class = class_lookup(arg3);

	if (class == -1) {

	    char buf[MAX_STRING_LENGTH];



	    strcpy(buf, "Possible classes are: ");

	    for (class = 0; class < MAX_CLASS; class++) {

		if (class > 0)

		    strcat(buf, " ");

		strcat(buf, class_table[class].name);

	    }

	    strcat(buf, ".\n\r");



	    send_to_char(buf, ch);

	    return;

	}

	victim->class = class;

	return;

    }

    if (!str_prefix(arg2, "level")) {

	if (!IS_NPC(victim)) {

	    send_to_char("Not on PC's.\n\r", ch);

	    return;

	}

	if (value < 0 || value > 110) {

	    send_to_char("Level range is 0 to 110.\n\r", ch);

	    return;

	}

	victim->level = value;

	return;

    }

    if (!str_prefix(arg2, "gold")) {

	victim->gold = value;

	return;

    }

    if (!str_prefix(arg2, "qp")) {

	victim->questpoints = value;

	return;

    }

    if (!str_prefix(arg2, "silver")) {

	victim->silver = value;

	return;

    }

    if (!str_prefix(arg2, "raise")) {

	victim->levelpoints = value;

	return;

    }

    if (!str_prefix(arg2, "vassals")) {

	victim->pcdata->vassals = value;

	return;

    }

    if (!str_prefix(arg2, "farms")) {

	victim->pcdata->farms = value;

	return;

    }

    if (!str_prefix(arg2, "mines")) {

	victim->pcdata->mines = value;

	return;

    }

    if (!str_prefix(arg2, "tax")) {

	victim->pcdata->tax = value;

	return;

    }

    if (!str_prefix(arg2, "research")) {

	victim->pcdata->research = value;

	return;

    }

    if (!str_prefix(arg2, "parcels")) {

	victim->pcdata->parcels = value;

	return;

    }

    if (!str_prefix(arg2, "hp")) {

	if (value < -10 || value > 30000) {

	    send_to_char("Hp range is -10 to 30,000 hit points.\n\r", ch);

	    return;

	}

	victim->max_hit = value;

	if (!IS_NPC(victim))

	    victim->pcdata->perm_hit = value;

	return;

    }

    if (!str_prefix(arg2, "mana")) {

	if (value < 0 || value > 30000) {

	    send_to_char("Mana range is 0 to 30,000 mana points.\n\r", ch);

	    return;

	}

	victim->max_mana = value;

	if (!IS_NPC(victim))

	    victim->pcdata->perm_mana = value;

	return;

    }

    if (!str_prefix(arg2, "move")) {

	if (value < 0 || value > 30000) {

	    send_to_char("Move range is 0 to 30,000 move points.\n\r", ch);

	    return;

	}

	victim->max_move = value;

	if (!IS_NPC(victim))

	    victim->pcdata->perm_move = value;

	return;

    }

    if (!str_prefix(arg2, "practice")) {

	if (value < 0 || value > 250) {

	    send_to_char("Practice range is 0 to 250 sessions.\n\r", ch);

	    return;

	}

	victim->practice = value;

	return;

    }

    if (!str_prefix(arg2, "train")) {

	if (value < 0 || value > 50) {

	    send_to_char("Training session range is 0 to 50 sessions.\n\r", ch);

	    return;

	}

	victim->train = value;

	return;

    }

    if (!str_prefix(arg2, "align")) {

	if (value < -1000 || value > 1000) {

	    send_to_char("Alignment range is -1000 to 1000.\n\r", ch);

	    return;

	}

	victim->alignment = value;

	return;

    }

    if (!str_prefix(arg2, "thirst")) {

	if (IS_NPC(victim)) {

	    send_to_char("Not on NPC's.\n\r", ch);

	    return;

	}

	if (value < -1 || value > 100) {

	    send_to_char("Thirst range is -1 to 100.\n\r", ch);

	    return;

	}

	victim->pcdata->condition[COND_THIRST] = value;

	return;

    }

    if (!str_prefix(arg2, "drunk")) {

	if (IS_NPC(victim)) {

	    send_to_char("Not on NPC's.\n\r", ch);

	    return;

	}

	if (value < -1 || value > 100) {

	    send_to_char("Drunk range is -1 to 100.\n\r", ch);

	    return;

	}

	victim->pcdata->condition[COND_DRUNK] = value;

	return;

    }

    if (!str_prefix(arg2, "full")) {

	if (IS_NPC(victim)) {

	    send_to_char("Not on NPC's.\n\r", ch);

	    return;

	}

	if (value < -1 || value > 100) {

	    send_to_char("Full range is -1 to 100.\n\r", ch);

	    return;

	}

	victim->pcdata->condition[COND_FULL] = value;

	return;

    }

    if (!str_prefix(arg2, "hunger")) {

	if (IS_NPC(victim)) {

	    send_to_char("Not on NPC's.\n\r", ch);

	    return;

	}

	if (value < -1 || value > 100) {

	    send_to_char("Full range is -1 to 100.\n\r", ch);

	    return;

	}

	victim->pcdata->condition[COND_HUNGER] = value;

	return;

    }

    if (!str_prefix(arg2, "race")) {

	int race;



	race = race_lookup(arg3);



	if (race == 0) {

	    send_to_char("That is not a valid race.\n\r", ch);

	    return;

	}

//	if (!IS_NPC(victim) && pc_race_table[race].isRemort) {

//	    send_to_char("That is not a valid player race.\n\r", ch);

//	    return;

//	}

	victim->race = race;

	return;

    }

    if (!str_prefix(arg2, "group")) {

	if (!IS_NPC(victim)) {

	    send_to_char("Only on NPCs.\n\r", ch);

	    return;

	}

	victim->group = value;

	return;

    }

    /*

     * Generate usage message.

     */

    do_mset(ch, "");

    return;

}



void do_string(CHAR_DATA * ch, char *argument)

{

    char type[MAX_INPUT_LENGTH];

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    char arg3[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;

    OBJ_DATA *obj;



    smash_tilde(argument);

    argument = one_argument(argument, type);

    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);

    strcpy(arg3, argument);



    if (type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == 
'\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  string char <name> <field> <string>\n\r", ch);

	send_to_char("    fields: name short long desc title spec\n\r", ch);

	send_to_char("  string obj  <name> <field> <string>\n\r", ch);

	send_to_char("    fields: name short long extended\n\r", ch);

	return;

    }

    if (!str_prefix(type, "character") || !str_prefix(type, "mobile")) {

	if ((victim = get_char_world(ch, arg1)) == NULL) {

	    send_to_char("They aren't here.\n\r", ch);

	    return;

	}

	/* clear zone for mobs */

	victim->zone = NULL;



	/* string something */



	if (!str_prefix(arg2, "name")) {

	    if (!IS_NPC(victim)) {

		send_to_char("Not on PC's.\n\r", ch);

		return;

	    }

	    free_string(victim->name);

	    victim->name = str_dup(arg3);

	    return;

	}

	if (!str_prefix(arg2, "description")) {

	    free_string(victim->description);

	    victim->description = str_dup(arg3);

	    return;

	}

	if (!str_prefix(arg2, "short")) {

	    free_string(victim->short_descr);

	    victim->short_descr = str_dup(arg3);

	    return;

	}

	if (!str_prefix(arg2, "long")) {

	    free_string(victim->long_descr);

	    strcat(arg3, "\n\r");

	    victim->long_descr = str_dup(arg3);

	    return;

	}

	if (!str_prefix(arg2, "title")) {

	    if (IS_NPC(victim)) {

		send_to_char("Not on NPC's.\n\r", ch);

		return;

	    }

	    set_title(victim, arg3);

	    return;

	}

	if (!str_prefix(arg2, "spec")) {

	    if (!IS_NPC(victim)) {

		send_to_char("Not on PC's.\n\r", ch);

		return;

	    }

	    if ((victim->spec_fun = spec_lookup(arg3)) == 0) {

		send_to_char("No such spec fun.\n\r", ch);

		return;

	    }

	    return;

	}

    }

    if (!str_prefix(type, "object")) {

	/* string an obj */



	if ((obj = get_obj_world(ch, arg1)) == NULL) {
	    send_to_char("Nothing like that in heaven or earth.\n\r", ch);
	    return;
	}


	if (!str_prefix(arg2, "name")) {
	    free_string(obj->name);
	    obj->name = str_dup(arg3);
	    SET_BIT(obj->extra_flags, ITEM_NOBANK);
	    return;
	}

	if (!str_prefix(arg2, "short")) {
	    free_string(obj->short_descr);
	    obj->short_descr = str_dup(arg3);
	    SET_BIT(obj->extra_flags, ITEM_NOBANK);
	    return;
	}

	if (!str_prefix(arg2, "long")) {
	    free_string(obj->description);
	    obj->description = str_dup(arg3);
	    SET_BIT(obj->extra_flags, ITEM_NOBANK);
	    return;

	}

	if (!str_prefix(arg2, "ed") || !str_prefix(arg2, "extended")) {
	    EXTRA_DESCR_DATA *ed;

	    argument = one_argument(argument, arg3);
	    if (argument == NULL) {
		send_to_char("Syntax: oset <object> ed <keyword> <string>\n\r",
			     ch);
		return;
	    }
	    strcat(argument, "\n\r");

	    ed = new_extra_descr();
	    ed->keyword = str_dup(arg3);
	    ed->description = str_dup(argument);
	    ed->next = obj->extra_descr;
	    obj->extra_descr = ed;
	    SET_BIT(obj->extra_flags, ITEM_NOBANK);
	    return;
	}
    }

    /* echo bad use message */

    do_string(ch, "");

}



void do_oset(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    char arg3[MAX_INPUT_LENGTH];

    OBJ_DATA *obj;

    int value;



    smash_tilde(argument);

    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);

    strcpy(arg3, argument);



    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  set obj <object> <field> <value>\n\r", ch);

	send_to_char("  Field being one of:\n\r", ch);

	send_to_char("    value0 value1 value2 value3 value4 (v1-v4)\n\r", ch);

	send_to_char("    extra wear level weight cost timer\n\r", ch);

	return;

    }

    if ((obj = get_obj_world(ch, arg1)) == NULL) {

	send_to_char("Nothing like that in heaven or earth.\n\r", ch);

	return;

    }

    /*

     * Snarf the value (which need not be numeric).

     */

    value = atoi(arg3);



    /*

     * Set something.

     */

    if (!str_cmp(arg2, "value0") || !str_cmp(arg2, "v0")) {

	obj->value[0] = UMIN(50, value);

	return;

    }

    if (!str_cmp(arg2, "value1") || !str_cmp(arg2, "v1")) {

	obj->value[1] = value;

	return;

    }

    if (!str_cmp(arg2, "value2") || !str_cmp(arg2, "v2")) {

	obj->value[2] = value;

	return;

    }

    if (!str_cmp(arg2, "value3") || !str_cmp(arg2, "v3")) {

	obj->value[3] = value;

	return;

    }

    if (!str_cmp(arg2, "value4") || !str_cmp(arg2, "v4")) {

	obj->value[4] = value;

	return;

    }

    if (!str_prefix(arg2, "extra")) {

	obj->extra_flags = value;

	return;

    }

    if (!str_prefix(arg2, "wear")) {

	obj->wear_flags = value;

	return;

    }

    if (!str_prefix(arg2, "level")) {

	obj->level = value;

	return;

    }

    if (!str_prefix(arg2, "weight")) {

	obj->weight = value;

	return;

    }

    if (!str_prefix(arg2, "cost")) {

	obj->cost = value;

	return;

    }

    if (!str_prefix(arg2, "timer")) {

	obj->timer = value;

	return;

    }

    /*

     * Generate usage message.

     */

    do_oset(ch, "");

    return;

}



void do_rset(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];

    char arg3[MAX_INPUT_LENGTH];

    ROOM_INDEX_DATA *location;

    int value;



    smash_tilde(argument);

    argument = one_argument(argument, arg1);

    argument = one_argument(argument, arg2);

    strcpy(arg3, argument);



    if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  set room <location> <field> <value>\n\r", ch);

	send_to_char("  Field being one of:\n\r", ch);

	send_to_char("    flags sector\n\r", ch);

	return;

    }

    if ((location = find_location(ch, arg1)) == NULL) {

	send_to_char("No such location.\n\r", ch);

	return;

    }

    if (!is_room_owner(ch, location) && ch->in_room != location

	&& room_is_private(location) && !IS_TRUSTED(ch, IMPLEMENTOR)) {

	send_to_char("That room is private right now.\n\r", ch);

	return;

    }

    /*

     * Snarf the value.

     */

    if (!is_number(arg3)) {

	send_to_char("Value must be numeric.\n\r", ch);

	return;

    }

    value = atoi(arg3);



    /*

     * Set something.

     */

    if (!str_prefix(arg2, "flags")) {

	location->room_flags = value;

	return;

    }

    if (!str_prefix(arg2, "sector")) {

	location->sector_type = value;

	return;

    }

    /*

     * Generate usage message.

     */

    do_rset(ch, "");

    return;

}



void char_time(CHAR_DATA * ch, char *buf)

{

    int i;

    char temp[MAX_STRING_LENGTH];



    if (ch == NULL) {

	buf[0] = '\0';

	strcat(buf, "00:00:00");

	return;

    }

    i = ((int) current_time - ch->logon);



    buf[0] = '\0';



    sprintf(temp, "%s%d:", (i / 3600) > 9 ? "" : "0", (i / 3600) > 0 ? (i / 
3600) : 0);

    strcat(buf, temp);

    i %= 3600;



    sprintf(temp, "%s%d:", (i / 60) > 9 ? "" : "0", (i / 60) > 0 ? (i / 60) 
: 0);

    strcat(buf, temp);



    i %= 60;



    sprintf(temp, "%s%d", i > 9 ? "" : "0", i > 0 ? i : 0);

    strcat(buf, temp);



    return;

}



void do_sockets(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;
    DESCRIPTOR_DATA *cd;
    CHAR_DATA *vch;
    CHAR_DATA *link_dead;
    BUFFER *output;
    char *st;
    char arg[MAX_INPUT_LENGTH];
    char sd[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
	char buf3[MSL];
    char notify = 'c';
    int count = 0;
	bool canSeeImps = FALSE;

	if (ch->level == MAX_LEVEL)
	{
		canSeeImps = TRUE;
	}

    sd[0] = '\0';
    buf[0] = '\0';
    buf2[0] = '\0';
    output = new_buf();

    one_argument(argument, arg);

    strcat(buf2, "\n\r{wNum    Connected_State   Idl      Time        Side       Name           Host{x\n\r");
	sprintf(buf3, "{m--------------------------------------------------------------------------------------------------------------{x\n\r");
    strcat(buf2, buf3);

    add_buf(output, buf2);


   for (link_dead = char_list; link_dead != NULL; link_dead = link_dead->next) {
	if (!IS_NPC(link_dead) && link_dead->desc == NULL && can_see(ch, link_dead)) {
	    for (cd = descriptor_list; cd; cd = cd->next) {



		if (can_see(ch, cd->character)) {
		    if (cd->character != link_dead &&
			!str_cmp(link_dead->host, cd->host))
			notify = 'C';
		}
	    }

	    switch (link_dead->position) {
	    case POS_DEAD:
		st = "{R LD     DEAD!! {x";
		break;
	    case POS_INCAP:
		st = "{r LD      Incap {x";
		break;
	    case POS_STUNNED:
		st = "{r LD    Stunned {x";
		break;
	    case POS_SLEEPING:
		st = "{g LD   Sleeping {x";
		break;
	    case POS_RESTING:
		st = "{g LD    Resting {x";
		break;
	    case POS_SITTING:
		st = "{g LD    Sitting {x";
		break;
	    case POS_FIGHTING:
		st = "{M LD   Fighting {x";
		break;
	    default:
		st = "{y LD    Playing {x";
		break;
	    }

	    char_time(link_dead, sd);

 		//prevent non-imps from seeing them on socket (sneaky sneaky)

		
		if (link_dead->level == 111)
		{
			if (!canSeeImps)
			{
				continue;
			}
		}
		

	    sprintf(buf, " {WLD {w|{x %-20s {w|{x %-3d {w|{x %-10.10s {w|{x %-12s {w|{x %-12s {w|{x %s{%c %s{x\n\r",
		    st, 
		    link_dead->timer > 0 ? link_dead->timer : 0,  
		    sd, 
		    kingdom_table[link_dead->side].cname,
		    link_dead->name, 
			(link_dead->sameip == TRUE) ? "{R[MF]{x" : "",
			notify, 
			link_dead->host); 


    if (arg[0] == '\0' || is_name(arg, buf)) {
		add_buf(output, buf);
		count++;
	    }
	    notify = 'c';
	}
    }

    for (d = descriptor_list; d; d = d->next) {
	if (can_see(ch, d->character)) {
	    for (cd = descriptor_list; cd; cd = cd->next) {



		if (can_see(ch, cd->character)) {
		    if (cd != d && !str_cmp(d->host, cd->host))
			notify = 'C';
		}
	    }

	    switch (d->connected) {

	    case CON_PLAYING:
		switch (d->character->position) {
		case POS_DEAD:
		    st = "{R     DEAD!!    {x";
		    break;
		case POS_INCAP:
		    st = "{r Incapacitated {x";
		    break;
		case POS_STUNNED:
		    st = "{r    Stunned    {x";
		    break;
		case POS_SLEEPING:
		    st = "{g    Sleeping   {x";
		    break;
		case POS_RESTING:
		    st = "{g    Resting    {x";
		    break;
		case POS_SITTING:
		    st = "{g    Sitting    {x";
		    break;
		case POS_FIGHTING:
		    st = "{M    Fighting   {x";
		    break;
		default:
		    st = "{y    Playing    {x";
		    break;
		}
		break;
	    case CON_GET_NAME:
		st = "{w   Get Name    {x";
		break;
	    case CON_GET_OLD_PASSWORD:
		st = "{wGet Old Passwd {x";
		break;
	    case CON_CONFIRM_NEW_NAME:
		st = "{w Confirm Name  {x";
		break;
	    case CON_GET_NEW_PASSWORD:
		st = "{wGet New Passwd {x";
		break;
	    case CON_CONFIRM_NEW_PASSWORD:
		st = "{wConfirm Passwd {x";
		break;
	    case CON_GET_NEW_RACE:
		st = "{w  Get New Race {x";
		break;
	    case CON_GET_NEW_SEX:
		st = "{w  Get New Sex  {x";
		break;
	    case CON_GET_NEW_CLASS:
		st = "{w Get New Class {x";
		break;
	    case CON_GET_ALIGNMENT:
		st = "{w Get New Align {x";
		break;
	    case CON_DEFAULT_CHOICE:
		st = "{w Choosing Cust {x";
		break;
	    case CON_GEN_GROUPS:
		st = "{w Customization {x";
		break;
	    case CON_PICK_WEAPON:
		st = "{w Picking Weapon{x";
		break;
	    case CON_READ_IMOTD:
		st = "{w Reading IMOTD {x";
		break;
	    case CON_BREAK_CONNECT:
		st = "{W   LINKDEAD    {x";
		break;
	    case CON_READ_MOTD:
		st = "{w  Reading MOTD {x";
		break;
	    case CON_GET_NEW_SIDE:
		st = "{w  Get Kingdom  {x";
		break;
	    case CON_COLOR:
		st = "{w   Get Color   {x";
		break;
	
//    case CON_SHOW_RECLASS:
//		st = "{w Show Reclass  {x";
//		break;	
//	
//	case CON_CONFIRM_EMAIL:
//		st = "{w Confirm Email {x";
//		break;
//	
//	case CON_GET_NEW_EMAIL:
//		st = "{w Get Email Add {x";
//		break;	
//		case CON_GET_DIFFICULTY:
//		st = "{w Get Difficult {x";
//		break;
    default:
		st = "{R   !UNKNOWN!   {x";
		break;
	    }

	    vch = d->original ? d->original : d->character;

	    char_time(vch, sd);


		//prevent non-imps from seeing them on socket (sneaky sneaky)
		if (d->original)
		{
			if (d->original->level == IMPLEMENTOR)
			{
				if (!canSeeImps)
					continue;
			}
		}
		if (d->character)
		{
			if (d->character->level == IMPLEMENTOR)
			{
				if (!canSeeImps)
					continue;
			}
		}

	    sprintf(buf, "{W%3d{w |{x %-20s {w|{x %-3d {w|{x %-10.10s {w|{x %-12s{w |{x %-12s {w|{x %s{%c %s{x\n\r",
		    d->descriptor,
		    st,
		    (vch) ? vch->timer > 0 ? vch->timer : 0 : 0,
		    sd,
		    (d->original) ? kingdom_table[d->original->side].cname : (d->character) ? kingdom_table[d->character->side].cname : "{DNone Yet{x",
		    (d->original) ? d->original->name : (d->character) ? d->character->name : "(None yet!)",
			(d->original) ? ( (d->original->sameip == TRUE) ? "{R[MF]{x" : "" ) : (d->character) ? ( (d->character->sameip == TRUE) ? "{R[MF]{x" : "" ) : "",
		    notify,
		    d->host);

	    if (arg[0] == '\0' || !str_infix(arg, buf)) {
		add_buf(output, buf);
		count++;
	    }
	    notify = 'c';
	}
    }

    if (arg[0] == '\0')
	sprintf(buf, "\n\r%d character%s in memory.\n\r", count, count == 1 ? "" : "s");
    else
	sprintf(buf, "\n\r%d match%s for %s.\n\r", count, count == 1 ? "" : "es", arg);

    add_buf(output, buf);

    page_to_char(buf_string(output), ch);
    free_buf(output);

    return;
}


/*

* Thanks to Grodyn for pointing out bugs in this function.

*/

void do_force(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    char arg[MAX_INPUT_LENGTH];

    char arg2[MAX_INPUT_LENGTH];



    argument = one_argument(argument, arg);



    if (arg[0] == '\0' || argument[0] == '\0') {

	send_to_char("Force whom to do what?\n\r", ch);

	return;

    }

    one_argument(argument, arg2);



    if (!str_cmp(arg2, "delete") || !str_prefix(arg2, "mob")) {

	send_to_char("That will NOT be done.\n\r", ch);

	return;

    }

    sprintf(buf, "$n forces you to '%s'.", argument);



    if (!str_cmp(arg, "all")) {

	CHAR_DATA *vch;

	CHAR_DATA *vch_next;



	if (get_trust(ch) < MAX_LEVEL - 3) {

	    send_to_char("Not at your level!\n\r", ch);

	    return;

	}

	for (vch = char_list; vch != NULL; vch = vch_next) {

	    vch_next = vch->next;



	    if (!IS_NPC(vch) && get_trust(vch) < get_trust(ch)) {

		act(buf, ch, NULL, vch, TO_VICT);

		interpret(vch, argument);

	    }

	}

    } else if (!str_cmp(arg, "players")) {

	CHAR_DATA *vch;

	CHAR_DATA *vch_next;



	if (get_trust(ch) < MAX_LEVEL - 2) {

	    send_to_char("Not at your level!\n\r", ch);

	    return;

	}

	for (vch = char_list; vch != NULL; vch = vch_next) {

	    vch_next = vch->next;



	    if (!IS_NPC(vch) && get_trust(vch) < get_trust(ch)

		&& vch->level < LEVEL_HERO) {

		act(buf, ch, NULL, vch, TO_VICT);

		interpret(vch, argument);

	    }

	}

    } else if (!str_cmp(arg, "gods")) {

	CHAR_DATA *vch;

	CHAR_DATA *vch_next;



	if (get_trust(ch) < MAX_LEVEL - 2) {

	    send_to_char("Not at your level!\n\r", ch);

	    return;

	}

	for (vch = char_list; vch != NULL; vch = vch_next) {

	    vch_next = vch->next;



	    if (!IS_NPC(vch) && get_trust(vch) < get_trust(ch)

		&& vch->level >= LEVEL_HERO) {

		act(buf, ch, NULL, vch, TO_VICT);

		interpret(vch, argument);

	    }

	}

    } else {

	CHAR_DATA *victim;



	if ((victim = get_char_world(ch, arg)) == NULL) {

	    send_to_char("They aren't here.\n\r", ch);

	    return;

	}

	if (victim == ch) {

	    send_to_char("Aye aye, right away!\n\r", ch);

	    return;

	}

	if (!is_room_owner(ch, victim->in_room)

	    && ch->in_room != victim->in_room

	    && room_is_private(victim->in_room) && !IS_TRUSTED(ch, IMPLEMENTOR)) {

	    send_to_char("That character is in a private room.\n\r", ch);

	    return;

	}

	if (get_trust(victim) >= get_trust(ch)) {

	    send_to_char("Do it yourself!\n\r", ch);

	    return;

	}

	if (!IS_NPC(victim) && get_trust(ch) < MAX_LEVEL - 3) {

	    send_to_char("Not at your level!\n\r", ch);

	    return;

	}

	act(buf, ch, NULL, victim, TO_VICT);

	interpret(victim, argument);

    }



    send_to_char("They jump at your command.\n\r", ch);

    return;

}



/*

* New routines by Dionysos.

*/

void do_invis(CHAR_DATA * ch, char *argument)

{

    int level;

    char arg[MAX_STRING_LENGTH];



    /* RT code for taking a level argument */

    one_argument(argument, arg);



    if (arg[0] == '\0')

	/* take the default path */



	if (ch->invis_level) {

	    ch->invis_level = 0;

	    act("$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM);

	    send_to_char("You slowly fade back into existence.\n\r", ch);

	} else {

	    ch->invis_level = get_trust(ch);

	    act("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM);

	    send_to_char("You slowly vanish into thin air.\n\r", ch);

    } else

	/* do the level thing */

    {

	level = atoi(arg);

	if (level < 2 || level > get_trust(ch)) {

	    send_to_char("Invis level must be between 2 and your level.\n\r", ch);

	    return;

	} else {

	    ch->reply = NULL;

	    ch->invis_level = level;

	    act("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM);

	    send_to_char("You slowly vanish into thin air.\n\r", ch);

	}

    }



    return;

}



void do_incognito(CHAR_DATA * ch, char *argument)

{

    int level;

    char arg[MAX_STRING_LENGTH];



    /* RT code for taking a level argument */

    one_argument(argument, arg);



    if (arg[0] == '\0')

	/* take the default path */



	if (ch->incog_level) {

	    ch->incog_level = 0;

	    send_to_char("You are no longer cloaked.\n\r", ch);

	} else {

	    ch->incog_level = get_trust(ch);

	    send_to_char("You cloak your presence.\n\r", ch);

    } else

	/* do the level thing */

    {

	level = atoi(arg);

	if (level < 2 || level > get_trust(ch)) {

	    send_to_char("Incog level must be between 2 and your level.\n\r", ch);

	    return;

	} else {

	    ch->reply = NULL;

	    ch->incog_level = level;

	    send_to_char("You cloak your presence.\n\r", ch);

	}

    }



    return;

}



void do_holylight(CHAR_DATA * ch, char *argument)

{

    if (IS_NPC(ch))

	return;



    if (IS_SET(ch->act, PLR_HOLYLIGHT)) {

	REMOVE_BIT(ch->act, PLR_HOLYLIGHT);

	send_to_char("Holy light mode off.\n\r", ch);

    } else {

	SET_BIT(ch->act, PLR_HOLYLIGHT);

	send_to_char("Holy light mode on.\n\r", ch);

    }



    return;

}



/* prefix command: it will put the string typed on each line typed */

void do_prefi(CHAR_DATA * ch, char *argument)

{

    send_to_char("You cannot abbreviate the prefix command.\r\n", ch);

    return;

}



void do_prefix(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_INPUT_LENGTH];



    if (argument[0] == '\0') {

	if (ch->prefix[0] == '\0') {

	    send_to_char("You have no prefix to clear.\r\n", ch);

	    return;

	}

	send_to_char("Prefix removed.\r\n", ch);

	free_string(ch->prefix);

	ch->prefix = str_dup("");

	return;

    }

    if (ch->prefix[0] != '\0') {

	sprintf(buf, "Prefix changed to %s.\r\n", argument);

	free_string(ch->prefix);

    } else {

	sprintf(buf, "Prefix set to %s.\r\n", argument);

    }


    ch->prefix = str_dup(argument);

}
//Begin Copyover Izlude 10-10-05
#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)

/* This file holds the copyover data */
#define COPYOVER_FILE "../src/copyover.dat"

/* This is the executable file */
#define EXE_FILE	  "../src/aow"

bool write_to_descriptor args((int desc, char *txt, int length));

void do_copyover (CHAR_DATA *ch, char * argument)
{
	//DESCRIPTOR_DATA *d;
   char		buf[MAX_STRING_LENGTH];
   char		arg[100];

   argument = one_argument(argument, arg);

   if((!str_cmp(arg, "cancel") ) ||
   	(copyover_engage && arg[0] == '\0'))
   {
      copyover_engage = 0;
      sprintf(buf, "{YCOPYOVER cancelled by {Y%s{x", ch->name);
      	gnews(buf);

      return;
   }
   if (atoi(arg) > 500)
   {
	   stc("Copyover can be no longer than 500 seconds.\n\r", ch);
	   return;
   }


	if(!(copyover_engage = atoi( arg )) )
   		copyover_engage = 10;

/*Crashy...
	// Akuji
	//minimum copyover time is 30 seconds if there is a noquit player
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (IS_AFFECTED(d->character, AFF_NOQUIT))
		{
			if (copyover_engage < 30)
			{
				copyover_engage = 30;
				break;
			}
		}
	}
	*/

   if( !(copyover_engage / 60) )
   	sprintf(buf, "{YCopyover begins in {W%d{Y seconds.{x\n\r",
   					copyover_engage );
   else if( !(copyover_engage % 60) )
   	sprintf(buf, "{YCopyover begins in {W%d{Y minute%s.{x\n\r",
      				copyover_engage / 60,
                  (copyover_engage / 60) > 1 ? "s" : "");
   else
   	sprintf(buf, "{YCopyover begins in {W%d{Y minute%s and {W%d{Y second%s.{x\n\r",
      				copyover_engage / 60,
                  (copyover_engage / 60) > 1 ? "s" : "",
                  copyover_engage % 60,
                  (copyover_engage % 60) > 1 ? "s" : "");
                  	gnews(buf);
   sprintf(buf, "{YReason{y:{= %s{x", argument[0] == '\0' ? "Not Specified" : argument );
	
	gnews(buf);

   copyover_engage *= PULSE_PER_SECOND;

  do_asave(ch , "changed");   /* - autosave changed areas */

  copyover_commander = ch;
  return;
}

void do_copyover2 (void)
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	CHAR_DATA		*ch;
	char buf [100], buf2[100], buf3[100];
	extern int port,control; /* db.c */
	int color;
	ch = copyover_commander;
	
	do_asave( NULL, "changed" );
	
	   sprintf(buf, "\n\n"
	   "<<<<<<<  Copyover by %s - Lvl %d - %s >>>>>>", ch->name, ch->level, ch->host);
	   	log_string(buf);
	
	fp = fopen (COPYOVER_FILE, "w");
	
	if (!fp)
	{
		stc ("Copyover file not writeable, aborted.\n\r",ch);
		log_string("Could not write to copyover file: copyover.dat");
		perror ("do_copyover:fopen");
		return;
	}
	
	
	sprintf(buf, "\n\r *** COPYOVER by %s - please remain seated!\n\r", ch->name);
	gnews(buf);
	
	/* For each playing descriptor, save its state */
	for (d = descriptor_list; d ; d = d_next)
	{
		CHAR_DATA * och = CH (d);
		d_next = d->next; /* We delete from the list , so need to save this */
		
		if (!d->character || d->connected > CON_PLAYING) /* drop those logging on */
		{
			write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
			close_socket (d); /* throw'em out */
		}
		else
		{
		    if(d->color == TRUE)
		        color = 1;
		    else
		        color = 0;
		        
			fprintf (fp, "%d %d %s %s\n", d->descriptor, color, och->name, d->host);

#if 0			 /* This is not necessary for ROM */
			if (och->level == 1)
			{
				write_to_descriptor (d->descriptor, "Since you are level one, and level one characters do not save, you gain a free level!\n\r", 0);
				advance_level (och);
				och->level++; /* Advance_level doesn't do that */
			}
#endif			
			save_char_obj (och);
			
			write_to_descriptor (d->descriptor, buf, 0);
		}
	}
	
	fprintf (fp, "-1\n");
	fclose (fp);
	
	/* Close reserve and other always-open files and release other resources */
	
	fclose (fpReserve);
	
	/* exec - descriptors are inherited */
	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
    strncpy( buf3, "-1", 100 );

	
	execl (EXE_FILE, "aow", buf, "copyover", buf2, buf3, (char *) NULL);

	/* Failed - sucessful exec will not return */
	
	perror ("do_copyover: execl");
	stc ("Copyover FAILED!\n\r",ch);
	
	/* Here you might want to reopen fpReserve */
	fpReserve = fopen (NULL_FILE, "r");
}

void do_copyover3 (void)
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf [100], buf2[100], buf3[100];
	extern int port,control; /* db.c */
	int color;
	
	// I think this is what was bugging the areas! - Akuji 10/9/2004
	//do_asave( NULL, "changed" );
	
	   sprintf(buf, "\n\n" "<<<<<<<  Copyover CRASH!! -  %s >>>>>>", (char*) ctime(&current_time));
	   log_string(buf);
	
	fp = fopen (COPYOVER_FILE, "w");
	
	if (!fp)
	{
		log_string("Could not write to copyover file: copyover.dat");
		perror ("do_copyover:fopen");
		return;
	}
	
	
	sprintf(buf, "\n\r *** COPYOVER CRASH!\n\r");
	gnews(buf);
	
	/* For each playing descriptor, save its state */
	for (d = descriptor_list; d ; d = d_next)
	{
		CHAR_DATA * och = CH (d);
		d_next = d->next; /* We delete from the list , so need to save this */
		
		if (!d->character || d->connected > CON_PLAYING) /* drop those logging on */
		{
			write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
			close_socket (d); /* throw'em out */
		}
		else
		{
		    if(d->color == TRUE)
		        color = 1;
		    else
		        color = 0;
		        
			fprintf (fp, "%d %d %s %s\n", d->descriptor, color, och->name, d->host);

#if 0			 /* This is not necessary for ROM */
			if (och->level == 1)
			{
				write_to_descriptor (d->descriptor, "Since you are level one, and level one characters do not save, you gain a free level!\n\r", 0);
				advance_level (och);
				och->level++; /* Advance_level doesn't do that */
			}
#endif			
			save_char_obj (och);
			
			write_to_descriptor (d->descriptor, buf, 0);
		}
	}
	
	fprintf (fp, "-1\n");
	fclose (fp);
	
	/* Close reserve and other always-open files and release other resources */
	
	fclose (fpReserve);
	
	/* exec - descriptors are inherited */
	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
    strncpy( buf3, "-1", 100 );
	
	execl (EXE_FILE, "aow", buf, "copyover", buf2, buf3, (char *) NULL);

	/* Failed - sucessful exec will not return */
	
	perror ("do_copyover: execl");
	
	/* Here you might want to reopen fpReserve */
	fpReserve = fopen (NULL_FILE, "r");
}

/* Recover from a copyover - load players */
void copyover_recover ()
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name [100];
	char host[MSL];
	int desc;
	int color;
	bool fOld;
	//extern bool crashed_once;
	//crashed_once = TRUE;
	log_string("Copyover recovery initiated");
	
	fp = fopen (COPYOVER_FILE, "r");
	
	if (!fp) /* there are some descriptors open which will hang forever then ? */
	{
		perror ("copyover_recover:fopen");
		log_string("Copyover file not found. Exitting.");
		exit (1);
	}

	unlink (COPYOVER_FILE); /* In case something crashes - doesn't prevent reading	*/
	
	for (;;)
	{
		fscanf (fp, "%d %d %s %s\n", &desc, &color, name, host);
		if (desc == -1)
			break;

		/* Write something, and check if it goes error-free */		
		if (!write_to_descriptor (desc, "\n\rRestoring from copyover...\n\r",0))
		{
			close (desc); /* nope */
			continue;
		}
		
		d = new_descriptor();
		d->descriptor = desc;
		
		if(color == 0)
		    d->color = FALSE;
		else
		    d->color = TRUE;
		    // Izlude, free string, etc. Copyover might be troubled after this, so for refrence
		free_string(d->host);
		
		d->host = str_dup (host);
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_COPYOVER_RECOVER; /* -15, so close_socket frees the char */
		
	
		/* Now, find the pfile */
		
		fOld = load_char_obj (d, name);
		
		if (!fOld) /* Player file not found?! */
		{
			write_to_descriptor (desc, "\n\rSomehow, your character was lost in the copyover. Sorry.\n\r", 0);
			close_socket (d);			
		}
		else /* ok! */
		{
			write_to_descriptor (desc, "\n\rCopyover recovery complete.\n\r",0);
	
			/* Just In Case */
			if (!d->character->in_room)
				d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);

			/* Insert in the char_list */
			d->character->next = char_list;
			char_list = d->character;

			char_to_room (d->character, d->character->in_room);
			do_look (d->character, "auto");
			act ("$n materializes!", d->character, NULL, NULL, TO_ROOM);
			d->connected = CON_PLAYING;

			if (d->character->pet != NULL)
			{
			    char_to_room(d->character->pet,d->character->in_room);
			    act("$n materializes!.",d->character->pet,NULL,NULL,TO_ROOM);
			}
		}
		
	}
   fclose (fp);
	
	
}

//End Copyover Izlude 10/10/05

void do_sameip(CHAR_DATA * ch, char *argument)
{
    char buf[SmSL];
	char subject[SmSL];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
	send_to_char("Syntax: sameip <name> <yes|no>\n\r", ch);
	return;
    }
    one_argument(argument, arg2);

    if (str_cmp(arg2, "yes") && str_prefix(arg2, "no")) {
	send_to_char("Syntax: sameip <name> <yes|no>\n\r", ch);
	return;
    }
    if (!str_cmp(arg2, "yes")) {

	if ((victim = get_char_world(ch, arg)) == NULL) {
	    send_to_char("They aren't here.\n\r", ch);
	    return;
	}
	if (get_trust(victim) >= get_trust(ch)) {
	    send_to_char("You do not have enough security to do that.\n\r", ch);
	    return;
	}
	if (!IS_NPC(victim) && get_trust(ch) < 109) {
	    send_to_char("Not at your level!\n\r", ch);
	    return;
	}

	sprintf(buf, "%s allows you to use the same IP as long as you only play one character at        a time.\n\r", ch->name);
	send_to_char(buf, victim);
	victim->sameip = TRUE;
	sprintf(buf, "You allow %s to use the same IP number.\n\r", victim->name);
	send_to_char(buf, ch);

	sprintf(buf, "%s IS allowed to multiplay.\n\r%s's current host: %s\n\r", victim->name, victim->name, victim->host);
	sprintf(subject, "{R[Auto]{B SameIP Allowed:{W %s %s", victim->name, victim->host);
	parse_penalty(ch, subject, buf);
	return;
    }
    if (!str_cmp(arg2, "no")) {

	if ((victim = get_char_world(ch, arg)) == NULL) {
	    send_to_char("They aren't here.\n\r", ch);
	    return;
	}
	if (get_trust(victim) >= get_trust(ch)) {
	    send_to_char("You do not have enough security to do that.\n\r", ch);
	    return;
	}
	if (!IS_NPC(victim) && get_trust(ch) < MAX_LEVEL) {
	    send_to_char("Not at your level!\n\r", ch);
	    return;
	}
	sprintf(buf, "%s disallows you to use the same IP.\n\r", ch->name);
	send_to_char(buf, victim);
	victim->sameip = FALSE;
	sprintf(buf, "You disallow %s to use the same IP number.\n\r", 
	victim->name);

	sprintf(buf, "%s is {RNOT{x allowed to multiplay.\n\r%s's current host: %s\n\r", victim->name, victim->name, victim->host);
	sprintf(subject, "{R[Auto]{B SameIP Revoked:{W %s %s", victim->name, victim->host);
	parse_penalty(ch, subject, buf);

	send_to_char(buf, ch);
	return;
    }
}



/* show a list of all used AreaVNUMS */

/* By The Mage */

void do_fvlist(CHAR_DATA * ch, char *argument)

{

    int i, j;

    char buffer[MAX_STRING_LENGTH];	/* should be plenty */

    char arg[MAX_INPUT_LENGTH];

    char *string;

    int COLUMNS = 8;



    string = one_argument(argument, arg);



    if (arg[0] == '\0') {

	send_to_char("Syntax:\n\r", ch);

	send_to_char("  fvlist obj\n\r", ch);

	send_to_char("  fvlist mob\n\r", ch);

	send_to_char("  fvlist room\n\r", ch);

	return;

    }

    j = 1;

    if (!str_cmp(arg, "obj")) {

	sprintf(buffer, "{WFree {C%s{W vnum listing for area {C%s{x\n\r", arg, 
ch->in_room->area->name);

	send_to_char(buffer, ch);

	send_to_char("{Y=============================================================================={C\n\r",

ch);

	for (i = ch->in_room->area->min_vnum; i <= ch->in_room->area->max_vnum; 
i++) {

	    if (get_obj_index(i) == NULL) {

		sprintf(buffer, "%8d, ", i);

		send_to_char(buffer, ch);

		if (j == COLUMNS) {

		    send_to_char("\n\r", ch);

		    j = 0;

		}

		j++;

	    }

	}

	send_to_char("{x\n\r", ch);

	return;

    }

    if (!str_cmp(arg, "mob")) {

	sprintf(buffer, "{WFree {C%s {Wvnum listing for area {C%s{x\n\r", arg, 
ch->in_room->area->name);

	send_to_char(buffer, ch);

	send_to_char("{Y=============================================================================={C\n\r",

ch);

	for (i = ch->in_room->area->min_vnum; i <= ch->in_room->area->max_vnum; 
i++) {

	    if (get_mob_index(i) == NULL) {

		sprintf(buffer, "%8d, ", i);

		send_to_char(buffer, ch);

		if (j == COLUMNS) {

		    send_to_char("\n\r", ch);

		    j = 0;

		} else

		    j++;

	    }

	}

	send_to_char("{x\n\r", ch);

	return;

    }

    if (!str_cmp(arg, "room")) {

	sprintf(buffer, "{WFree {C%s {Wvnum listing for area {C%s{x\n\r", arg, 
ch->in_room->area->name);

	send_to_char(buffer, ch);

	send_to_char("{Y=============================================================================={C\n\r",

ch);

	for (i = ch->in_room->area->min_vnum; i <= ch->in_room->area->max_vnum; 
i++) {

	    if (get_room_index(i) == NULL) {

		sprintf(buffer, "%8d, ", i);

		send_to_char(buffer, ch);

		if (j == COLUMNS) {

		    send_to_char("\n\r", ch);

		    j = 0;

		} else

		    j++;

	    }

	}

	send_to_char("{x\n\r", ch);

	return;

    }

    send_to_char("WHAT??? \n\r", ch);

    send_to_char("Syntax:\n\r", ch);

    send_to_char("  fvlist obj\n\r", ch);

    send_to_char("  fvlist mob\n\r", ch);

    send_to_char("  fvlist room\n\r", ch);

}



void do_remove_all(CHAR_DATA * ch, char *argument)

{

    char arg1[MAX_INPUT_LENGTH];

    CHAR_DATA *victim;

    OBJ_DATA *obj_next;

    OBJ_DATA *obj;



    argument = one_argument(argument, arg1);



    if (arg1[0] == '\0') {

	send_to_char("Syntax: fremove <char>\n\r", ch);

	return;

    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {

	send_to_char("That player is not here.\n\r", ch);

	return;

    }

    if (victim->level > get_trust(ch)) {

	send_to_char("Limited to your trust level.\n\r", ch);

	return;

    }

    for (obj = victim->carrying; obj; obj = obj_next) {

	obj_next = obj->next_content;



	if (obj->wear_loc != WEAR_NONE)

	    remove_obj(victim, obj->wear_loc, TRUE);

    }



    save_char_obj(victim);

    return;

}



void do_addlag(CHAR_DATA * ch, char *argument)

{



    CHAR_DATA *victim;

    char arg1[MAX_STRING_LENGTH];

    int x;



    argument = one_argument(argument, arg1);



    if (arg1[0] == '\0') {

	send_to_char("addlag to who?", ch);

	return;

    }

    if ((victim = get_char_world(ch, arg1)) == NULL) {

	send_to_char("They're not here.", ch);

	return;

    }

    if ((x = atoi(argument)) <= 0) {

	send_to_char("That makes a LOT of sense.", ch);

	return;

    }

    if (x > 100) {

	send_to_char("There's a limit to cruel and unusual punishment", ch);

	return;

    }

    WAIT_STATE(victim, x);

    send_to_char("Adding lag now...", ch);

    return;

}



void do_xlist(CHAR_DATA * ch, char *argument)

{

    ROOM_INDEX_DATA *pRoom;

    RESET_DATA *pReset;

    AREA_DATA *pArea;

    char arg[MAX_INPUT_LENGTH];

    char outbuf[MAX_STRING_LENGTH * 8];

    bool found = FALSE;

    bool fAll = FALSE;

    bool fMob = FALSE;

    bool fObj = FALSE;

    int rvnum;

    int vnum = 0;



    pArea = ch->in_room->area;



    if (argument[0] == '\0')

	fAll = TRUE;

    else {

	argument = one_argument(argument, arg);

	fMob = !str_cmp(arg, "mob");

	fObj = !str_cmp(arg, "obj");

	if (!fMob && !fObj) {

	    send_to_char("Syntax: xlist\n\r"

			 "        xlist mob <vnum>\n\r"

			 "        xlist obj <vnum>\n\r", ch);

	    return;

	} else if (is_number(argument)) {

	    vnum = atoi(argument);

	    if ((vnum < pArea->min_vnum) || (vnum > pArea->max_vnum)) {

		send_to_char("Invalid vnum for this area!\n\r", ch);

		return;

	    }

	}

    }



    strcpy(outbuf, "  Vnum         Description       Location Room World/Room\n\r");

    strcat(outbuf, "======== ======================= ======== ==== ==========\n\r");



    for (rvnum = pArea->min_vnum; rvnum <= pArea->max_vnum; rvnum++) {

	if ((pRoom = get_room_index(rvnum)) != NULL) {

	    for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {

		MOB_INDEX_DATA *pMob;

		OBJ_INDEX_DATA *pObj;

		char buf[MAX_STRING_LENGTH];



		switch (pReset->command) {

		case 'M':

		    if (fAll || fMob) {

			if (fMob && (pReset->arg1 != vnum) && (vnum != 0))

			    break;

			found = TRUE;

			pMob = get_mob_index(pReset->arg1);

			sprintf(buf, "M[%5d] %23.23s in room %5d [ %3d/%3d ]\n\r",

				pReset->arg1, pMob->short_descr,

				pReset->arg3, pReset->arg2, pReset->arg4);

			strcat(outbuf, buf);

		    }

		    break;



		case 'O':

		    if (fAll || fObj) {

			if (fObj && (pReset->arg1 != vnum) && (vnum != 0))

			    break;

			found = TRUE;

			pObj = get_obj_index(pReset->arg1);

			sprintf(buf, "O[%5d] %23.23s in room %5d [ %3d/    ]\n\r",

				pReset->arg1, pObj->short_descr,

				pRoom->vnum, pReset->arg2);

			strcat(outbuf, buf);

		    }

		    break;



		case 'P':

		    if (fAll || fObj) {

			if (fObj && (pReset->arg1 != vnum) && (vnum != 0))

			    break;

			found = TRUE;

			pObj = get_obj_index(pReset->arg1);

			sprintf(buf, "O[%5d] %23.23s in obj  %5d [ %3d/%3d ]\n\r",

				pReset->arg1, pObj->short_descr,

				pRoom->vnum, pReset->arg2, pReset->arg4);

			strcat(outbuf, buf);

		    }

		    break;



		case 'G':

		case 'E':

		    if (fAll || fObj) {

			if (fObj && (pReset->arg1 != vnum) && (vnum != 0))

			    break;

			found = TRUE;

			pObj = get_obj_index(pReset->arg1);

			sprintf(buf, "O[%5d] %23.23s mob inv %5d [ %3d/    ]\n\r",

				pReset->arg1, pObj->short_descr,

				pRoom->vnum, pReset->arg2);

			strcat(outbuf, buf);

		    }

		    break;



		default:

		    break;	/* ignore all other resets ( D, R ) */

		}

	    }

	}

    }



    if (!found) {

	send_to_char("No reset(s) found.\n\r", ch);

	return;

    }

    page_to_char(outbuf, ch);

    return;

}



void do_showclan(CHAR_DATA *ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    int  i;



    for (i=0 ; i < MAX_CLAN; i++)

        if (!clan_table[i].independent) {

	    sprintf(buf, "{r%s{x -> %s\n", clan_table[i].who_name, 
kingdom_table[clan_table[i].side].name);

	    send_to_char(buf, ch);

        }

}

//Marcus: System netstat to look for hackers lags and buffer overflows
void do_netstat(CHAR_DATA *ch, char *argument)

{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char tmpfp[MAX_STRING_LENGTH];
    FILE *fp;
    int c;
    int PID;
    BUFFER *output;


   sprintf(tmpfp, "%s%sns.tmp", TEMP_DIR, ch->name);

    if (argument[0] != '\0') {
   fp = fopen(tmpfp, "r");
   if (fp) { send_to_char("Bug", ch); fclose(fp); return; }
	sprintf(buf, "netstat | grep \"%s\" > %s", tmpfp, ch->name);
	PID = fork();

	if (PID == 0) {
	   system(buf);
	   exit(0); } }
    else if (strcmp(argument,"read")) {
   fp = fopen(tmpfp, "r");
   if (fp) { send_to_char("Bug", ch); fclose(fp); return; }
	sprintf(buf, "netstat > %s", tmpfp);
	PID = fork();

   	if (PID == 0) {
	   system(buf);
	   exit(0); } }


    else   {
   fp = fopen(tmpfp, "r");
   if (!fp) { send_to_char("Bugr", ch); return; }

   output = new_buf();

   while ((c = getc(fp)) != EOF) {
      if ((char)c != '\n') {
         sprintf(buf2, "%c", (char)c);
	add_buf(output, buf2); }
      else {
	add_buf(output, "\n");
      } }
    page_to_char(buf_string(output), ch);
    free_buf(output);

   fclose(fp);
   sprintf(buf, "rm -f %s", tmpfp);
   system(buf);
   }

}
//Marcus: System ps ux to check memory, lags and buffer overflows
void do_psux(CHAR_DATA *ch, char *argument)

{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char tmpfp[MAX_STRING_LENGTH];
    FILE *fp;
    int c;
    int PID;
    BUFFER *output;


   sprintf(tmpfp, "%s%sps.tmp", TEMP_DIR, ch->name);

   if (strcmp(argument,"read")) {
   fp = fopen(tmpfp, "r");
   if (fp) { send_to_char("Bug", ch); fclose(fp); return; }
	sprintf(buf, "psux > %s", tmpfp);
	PID = fork();

	if (PID == 0) {
	   send_to_char("test", ch);
	   system(buf);
	   exit(0); }
   }
   else {
   fp = fopen(tmpfp, "r");
   if (!fp) { send_to_char("Bugr", ch); return; }

   output = new_buf();

   while ((c = getc(fp)) != EOF) {
      if ((char)c != '\n') {
         sprintf(buf2, "%c", (char)c);
	add_buf(output, buf2); }
      else {
	add_buf(output, "\n");
      } }
    page_to_char(buf_string(output), ch);
    free_buf(output);

   fclose(fp);
   sprintf(buf, "rm -f %s", tmpfp);
   system(buf);
   }

}
// Pretend you never fucking saw this, understand?
// -Tasy 6/11/02

void do_slog(CHAR_DATA *ch, char *argument)
{
    return;
}

/*
* Dynamic adding of helps
*
* -Tasydra
*/
/* Commented out, per Hobbie

void do_addhelp( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char arg[MAX_STRING_LENGTH];
    int level;
	extern HELP_DATA *help_last;
    argument = one_argument( argument, arg );

    if ( !is_number( arg ) || arg[0] == '\0' )
    {
       send_to_char( "Usage: ADDHELP <level> <keyword(s)>.\n\r", ch );
       return;
    }

    level = atoi( arg );

    if ( level < -1 || level > 110 )
    {
       send_to_char( "Level must be between -1 and 110.\n\r", ch );
       return;
    }

    pHelp = new_help_data();
	pHelp->level    = level;
	pHelp->keyword  = str_dup( arg );
	pHelp->text     = str_dup( "NEW HELP.  DELETE THIS LINE FIRST!" );
    help_last->next = pHelp;
    help_last = pHelp;
    top_help++;
    send_to_char( "Help added.  Use \"hedit <keyword>\" to edit it.\n\r", ch 
);
    all_help_save();
    return;
}

*/

/*
* Dynamic help editing.
*
* -Tasydra
*/

/* Commented out per Hobbie

void do_hedit(CHAR_DATA * ch, char *argument)
{
    HELP_DATA *pHelp;
    bool found = FALSE;

    pHelp = NULL;

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    {
		if (pHelp->keyword[0] == '{')
			pHelp->keyword += 2;

	 if (!strcmp(argument, pHelp->keyword))
      {
     found = TRUE;
     break;
      }
     }
     if (!found)
      {
       send_to_char("No such help word found. Use 'addhelp' to create a 
help.\n\r", ch);
       return;
      }
    string_append(ch, &pHelp->text);
    all_help_save();
    return;
}
*/ 

void do_scatter( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  ROOM_INDEX_DATA *pRoomIndex;

  for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next ) {
    obj_next = obj->next_content;
    for ( ; ; )
    {
        pRoomIndex = get_room_index( number_range( 0, 99999 ) );
        if ( pRoomIndex != NULL )
            break;
    }
    act("$p vanishes in a puff of {ws{Wm{wo{wk{We{x!",ch,obj,NULL,TO_ROOM);
	act("$p vanishes in a puff of {ws{Wm{wo{wk{We{x!",ch,obj,NULL,TO_CHAR);
    obj_from_room(obj);
    obj_to_room(obj, pRoomIndex);
  }
  return;
}


void do_spup( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA       *wch;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    char            arg1[MAX_INPUT_LENGTH];
    char            buf[MAX_STRING_LENGTH];
    char            *arg2;


    one_argument( argument, arg1 );



    original = ch->in_room;

    if( arg1[0]== '\0' )
     {
      send_to_char( "Which spell would you like to cast on the mud?", ch
);
      return;
     }

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch; wch = wch->next )
    {
       if ( wch == ch )
         continue;

       if( IS_IMMORTAL( wch ) )
	 continue;

       if (!IS_NPC ( wch ) )
        {
         arg2 = wch->name;
         location = find_location( ch, wch->name );
         sprintf( buf, "'%s' %s", arg1, wch->name );
         char_from_room( ch );
         char_to_room( ch, location );
         do_cast( ch, buf );

        }
    }

    char_from_room( ch );
    char_to_room( ch, original );

    return;
}

void dump_core();

void do_coredump(CHAR_DATA * ch, char * argument )
{
  dump_core();
  send_to_char("Core dumped.",ch);
}   

//advice editor so I can get a little more familiar with sqlite workflow I guess? - Akuji
typedef struct 
{
  BUFFER * buffer;
} ADVICE_DB_DATA;

int advice_callback(void *data, int argc, char **argv, char **azColName)
{
  int i;
  char buf[SmSL];
  BUFFER * buffer = ((ADVICE_DB_DATA*)data)->buffer;

  for(i=0; i<argc; i++){
    sprintf(buf, "{W%s{x){x %s\n", argv[0], argv[i] ? argv[i] : "NULL");
  }
  add_buf(buffer, buf);
  return 0;
}

void do_advice_editor(CHAR_DATA* ch, char* argument)
{
	BUFFER* output = new_buf();
	char* query;
	sqlite3* db;
	int rc = sqlite3_open(ADVICE_DB,&db);
	char* error = NULL;
	char buf[SmSL];
	char mode[SmSL], num[SmSL];
	int adviceId = -1;

	argument = one_argument(argument, mode);
	argument = one_argument(argument, num);

	//create the db if not exists?
	query = "CREATE TABLE IF NOT EXISTS advice( id INTEGER PRIMARY KEY, text TEXT);";
	rc = sqlite3_exec(db, query, NULL, NULL, &error);

	if(rc)
	{
		bug("Couldn't open ADVICE DB. Reason:",0);
		bug(sqlite3_errmsg(db),0);
		sqlite3_close(db);
		send_to_char("Something went wrong. We'll try to fix it.\n\r",ch);
		return;
	}

	
	//no arguments - display advice list
	if (mode[0] == '\0')
	{
		add_buf(output, "Advice Editor:\n\r");

		query = "select * from advice;";

		rc = sqlite3_exec(db, query, advice_callback, &output, &error);

		if(rc != SQLITE_OK)
		{ 
			bug("Couldn't execute command. Reason:",0);
			bug(sqlite3_errmsg(db),0);
			send_to_char("Something went wrong. We'll try to fix it.\n\r",ch);
		}
			else
		{
			page_to_char(buf_string(output), ch);
			sqlite3_close(db);
			free_buf(output);

			stc("\n\rAdviceedit syntax: adviceedit (mode) [number] \"Text you want to edit.\"\n\r\t\t   adviceedit (new) \"Text you wanted to add.\"\n\rModes: update, delete, new\n\r", ch);
			return;
		}


	}//end advice list

	//updating
	if (!strcmp(mode, "update"))
	{
		if (num[0] == '\0')
		{
			stc("You must provide the advice number you'd like to update.\n\r", ch);
			sqlite3_close(db);
			return;
		}
		if (argument[0] == '\0')
		{
			stc("Well, what text did you want the advice to have?\n\r", ch);
			sqlite3_close(db);
			return;
		}
		if (!is_number(num))
		{
			sprintf(buf, "%s is a number?\n\r", num);
			stc(buf, ch);
			free_buf(output);
			sqlite3_close(db);
			return;
		}

		adviceId = atoi(num);

		if (adviceId != -1)
		{
			sprintf(buf, "update advice set text==\"%s\" where id==%d", argument, adviceId);
			query = buf;

			rc = sqlite3_exec(db, query, NULL, NULL, &error);

			if(rc)
			{
				bug("Couldn't open ADVICE DB. Reason:",0);
				bug(sqlite3_errmsg(db),0);
				sqlite3_close(db);
				send_to_char("Something went wrong. We'll try to fix it.\n\r",ch);
				free_buf(output);
				return;
			}
			else
			{
				sprintf(buf, "Advice #:%-3d updated to:\n\r\"%s\"\n\r", adviceId, argument);
				stc(buf, ch);
				free_buf(output);
				sqlite3_close(db);
				return;
			}
		}
	}//end updating

	//deleting
	if (!strcmp(mode, "delete"))
	{
		if (num[0] == '\0')
		{
			stc("You must provide the advice number you'd like to delete.\n\r", ch);
			sqlite3_close(db);
			return;
		}
		if (!is_number(num))
		{
			sprintf(buf, "%s is a number?\n\r", num);
			stc(buf, ch);
			free_buf(output);
			sqlite3_close(db);
			return;
		}

		adviceId = atoi(num);

		if (adviceId != -1)
		{
			sprintf(buf, "delete from advice where id==%d", adviceId);
			query = buf;

			rc = sqlite3_exec(db, query, NULL, NULL, &error);

			if(rc)
			{
				bug("Couldn't open ADVICE DB. Reason:",0);
				bug(sqlite3_errmsg(db),0);
				sqlite3_close(db);
				send_to_char("Something went wrong. We'll try to fix it.\n\r",ch);
				free_buf(output);
				return;
			}
			else
			{
				sprintf(buf, "Advice #:%-3d Deleted!\n\r", adviceId);
				stc(buf, ch);
				free_buf(output);
				sqlite3_close(db);
				return;
			}
		}
	}//end updating

	stc("Adviceedit syntax: adviceedit (mode) [number / new] \"Text you want to edit.\"\n\rModes: update, delete\n\r", ch);

	free_buf(output);
	sqlite3_close(db);
	return;
}

void do_arealinks(CHAR_DATA *ch, char *argument)
{
    FILE *fp;
    BUFFER *buffer = new_buf();
    AREA_DATA *parea;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int vnum = 0;
    int iHash, door;
    bool found = FALSE;

    /* To provide a convenient way to translate door numbers to words */
    static char * const dir_name[] = {"north","east","south","west","up","down"};

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    /* First, the 'all' option */
    if (!str_cmp(arg1,"all"))
    {
	/*
	 * If a filename was provided, try to open it for writing
	 * If that fails, just spit output to the screen.
	 */
	if (arg2[0] != '\0')
	{
	    fclose(fpReserve);
	    if( (fp = fopen(arg2, "w")) == NULL)
	    {
		send_to_char("Error opening file, printing to screen.\n\r",ch);
		fclose(fp);
		fpReserve = fopen(NULL_FILE, "r");
		fp = NULL;
	    }
	}
	else
	    fp = NULL;

	/* Open a buffer if it's to be output to the screen */
	if (!fp)
	    buffer = new_buf();

	/* Loop through all the areas */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    /* First things, add area name  and vnums to the buffer */
	    sprintf(buf, "*** %s (%d to %d) ***\n\r",
			 parea->name, parea->min_vnum, parea->max_vnum);
	    fp ? fprintf(fp, buf) : add_buf(buffer, buf);

	    /* Now let's start looping through all the rooms. */
	    found = FALSE;
	    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	    {
		for( from_room = room_index_hash[iHash];
		     from_room != NULL;
		     from_room = from_room->next )
		{
		    /*
		     * If the room isn't in the current area,
		     * then skip it, not interested.
		     */
		    if ( from_room->vnum < parea->min_vnum
		    ||   from_room->vnum > parea->max_vnum )
			continue;

		    /* Aha, room is in the area, lets check all directions */
		    for (door = 0; door < 5; door++)
		    {
			/* Does an exit exist in this direction? */
			if( (pexit = from_room->exit[door]) != NULL )
			{
			    to_room = pexit->u1.to_room;

			    /*
			     * If the exit links to a different area
			     * then add it to the buffer/file
			     */
			    if( to_room != NULL
			    &&  (to_room->vnum < parea->min_vnum
			    ||   to_room->vnum > parea->max_vnum) )
			    {
				found = TRUE;
				sprintf(buf, "    (%d) links %s to %s (%d)\n\r",
				    from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

				/* Add to either buffer or file */
				if(fp == NULL)
				    add_buf(buffer, buf);
				else
				    fprintf(fp, buf);
			    }
			}
		    }
		}
	    }

	    /* Informative message for areas with no external links */
	    if (!found)
		add_buf(buffer, "    No links to other areas found.\n\r");
	}

	/* Send the buffer to the player */
	if (!fp)
	{
	    page_to_char(buf_string(buffer), ch);
	}
	/* Or just clean up file stuff */
	else
	{
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	}

	free_buf(buffer);
	return;
    }

    /* No argument, let's grab the char's current area */
    if(arg1[0] == '\0')
    {
	parea = ch->in_room ? ch->in_room->area : NULL;

	/* In case something wierd is going on, bail */
	if (parea == NULL)
	{
	    send_to_char("You aren't in an area right now, funky.\n\r",ch);
	    return;
	}
    }
    /* Room vnum provided, so lets go find the area it belongs to */
    else if(is_number(arg1))
    {
	vnum = atoi(arg1);

	/* Hah! No funny vnums! I saw you trying to break it... */
	if (vnum <= 0 || vnum > 250000)
	{
	    send_to_char("The vnum must be between 1 and 250000.\n\r",ch);
	    return;
	}

	/* Search the areas for the appropriate vnum range */
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(vnum >= parea->min_vnum && vnum <= parea->max_vnum)
		break;
	}

	/* Whoops, vnum not contained in any area */
	if (parea == NULL)
	{
	    send_to_char("There is no area containing that vnum.\n\r",ch);
	    return;
	}
    }
    /* Non-number argument, must be trying for an area name */
    else
    {
	/* Loop the areas, compare the name to argument */
	for(parea = area_first; parea != NULL; parea = parea->next)
	{
	    if(!str_prefix(arg1, parea->name))
		break;
	}

	/* Sorry chum, you picked a goofy name */
	if (parea == NULL)
	{
	    send_to_char("There is no such area.\n\r",ch);
	    return;
	}
    }

    /* Just like in all, trying to fix up the file if provided */
    if (arg2[0] != '\0')
    {
	fclose(fpReserve);
	if( (fp = fopen(arg2, "w")) == NULL)
	{
	    send_to_char("Error opening file, printing to screen.\n\r",ch);
	    fclose(fp);
	    fpReserve = fopen(NULL_FILE, "r");
	    fp = NULL;
	}
    }
    else
	fp = NULL;

    /* And we loop the rooms */
    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
    {
	for( from_room = room_index_hash[iHash];
	     from_room != NULL;
	     from_room = from_room->next )
	{
	    /* Gotta make sure the room belongs to the desired area */
	    if ( from_room->vnum < parea->min_vnum
	    ||   from_room->vnum > parea->max_vnum )
		continue;

	    /* Room's good, let's check all the directions for exits */
	    for (door = 0; door < 6; door++)
	    {
		if( (pexit = from_room->exit[door]) != NULL )
		{
		    to_room = pexit->u1.to_room;

		    /* Found an exit, does it lead to a different area? */
		    if( to_room != NULL
		    &&  (to_room->vnum < parea->min_vnum
		    ||   to_room->vnum > parea->max_vnum) )
		    {
			found = TRUE;
			sprintf(buf, "{W%s{x (%d) links %s to {W%s{x (%d)\n\r",
				    parea->name, from_room->vnum, dir_name[door],
				    to_room->area->name, to_room->vnum);

			/* File or buffer output? */
			if(fp == NULL)
			    send_to_char(buf, ch);
			else
			    fprintf(fp, buf);
		    }
		}
	    }
	}
    }

    /* Informative message telling you it's not externally linked */
    if(!found)
    {
	send_to_char("No links to other areas found.\n\r",ch);
	/* Let's just delete the file if no links found */
	if (fp)
	    unlink(arg2);
	return;
    }

    /* Close up and clean up file stuff */
    if(fp)
    {
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
    }

}

//give this function an area's vnum and it'll tell you if any other areas link to it! - Akuji
bool area_is_linked(int area_vnum)
{
    AREA_DATA *parea;
    EXIT_DATA *pexit;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    int iHash, door;
    bool found = FALSE;
   

	//set parea to the correct area
	for (parea = area_first; parea != NULL; parea = parea->next)
	{
		if (parea->vnum == area_vnum)
			break;
	}

	//we didn't find it! bail out
	if (parea == NULL)
	{
	    return FALSE;
	}

    /* And we loop the rooms */
    for(iHash = 0; iHash < MAX_KEY_HASH; iHash++)
    {
	for( from_room = room_index_hash[iHash];
	     from_room != NULL;
	     from_room = from_room->next )
	{
	    /* Gotta make sure the room belongs to the desired area */
	    if ( from_room->vnum < parea->min_vnum
	    ||   from_room->vnum > parea->max_vnum )
		continue;

	    /* Room's good, let's check all the directions for exits */
	    for (door = 0; door < 6; door++)
	    {
		if( (pexit = from_room->exit[door]) != NULL )
		{
		    to_room = pexit->u1.to_room;

		    /* Found an exit, does it lead to a different area? */
		    if( to_room != NULL
		    &&  (to_room->vnum < parea->min_vnum
		    ||   to_room->vnum > parea->max_vnum) )
		    {
				found = TRUE;
		    }
		}
	    }
	}
    }

	return found;

}

void implementorAdvance(CHAR_DATA* ch, char* argument)
{
	CHAR_DATA* victim = NULL;
	char buf[SmSL];

	if (ch->level < 111)
		return;

	if (argument[0] == '\0')
	{
		stc("Give who implementor status?\n\r", ch);
		return;
	}
	
	victim = get_char_world(ch, argument);

	if (victim == NULL)
	{
		stc("Give who implementor status?\n\r", ch);
		return;
	}
	else
	{

		if (IS_NPC(victim))
		{
			stc("Mobs don't make good implementors!\n\r", ch);
			return;
		}


		if (!IS_IMMORTAL(victim))
		{
			stc("You cannot make non-immortals have implementor status.\n\r", ch);
			return;
		}
		else
		{
			if (victim->level == 111 || victim->trust == 111)
			{
				victim->level = 110;
				victim->trust = 110;
				stc("You have been revoked implementor status.\n\r", victim);
				stc("You have revoked their implementor status.\n\r", ch);

				sprintf(buf, "$N remove %s's implementor status", victim->name);
				wiznet(buf, ch, NULL, WIZ_SECURE, WIZ_SECURE, 0);
				return;
			}
			else
			{
				victim->level = 111;
				victim->trust = 111;
				stc("You are now implementor level.\n\r", victim);
				stc("You have given them implementor status.\n\r", ch);

				sprintf(buf, "$N sets %s to implementor status", victim->name);
				wiznet(buf, ch, NULL, WIZ_SECURE, WIZ_SECURE, 0);
				return;
			}
		}
	}
}

void implementorTalk (CHAR_DATA* ch, char* argument)
{
    char buf[MSL];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOWIZ)) {
	    stc("Immortal channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOWIZ);
	} else {
	    stc("Immortal channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOWIZ);
	}
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOWIZ);

   for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING &&
		(d->character->level == IMPLEMENTOR) &&
	    !IS_SET(d->character->comm, COMM_NOWIZ)) {
	
 	    sprintf(buf, "{c[{D111{c]{x-{{c[{x%s{c]:{C %s{x\n\r",
 	    ch->name, 
 	    argument);
 		stc( buf, d->character);
		}
	}

	return;
}
