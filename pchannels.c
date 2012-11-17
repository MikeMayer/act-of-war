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
 **************************************************************************/
/***************************************************************************
*       ROM 2.4 is copyright 1993-1996 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@efn.org)                                  *
*           Gabrielle Taylor                                               *
*           Brian Moore (zump@rom.org)                                     *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

/***************************************************************************
*                                                                          *
*     Private channels by Coelcoeth					   *
*                                                                          *
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "merc.h"

#define PCLOSED		0
#define POPENED		1
#define PADDED		2
#define PJOINED		3

#define GET_IDNUM(ch)           (IS_NPC(ch) ? -1 : (ch)->id)
#define GET_PRIVATE(ch)         ((ch)->pcdata->private_channel)
#define GET_PMODE(ch)           ((ch)->pcdata->private_mode)
#define GET_PTOPIC(ch)          ((ch)->pcdata->private_topic)

/* command procedures needed */
DECLARE_DO_FUN(do_psay);
DECLARE_DO_FUN(do_pclose);
DECLARE_DO_FUN(do_pcreate);
DECLARE_DO_FUN(do_pdestroy);
DECLARE_DO_FUN(do_pinfo);
DECLARE_DO_FUN(do_pjoin);
DECLARE_DO_FUN(do_poff);
DECLARE_DO_FUN(do_popen);
DECLARE_DO_FUN(do_premove);
DECLARE_DO_FUN(do_pwho);
DECLARE_DO_FUN(do_pcheck);
DECLARE_DO_FUN(do_pchannels);
DECLARE_DO_FUN(do_padd);

static void close_private(CHAR_DATA * ch)
{
    if (IS_NPC(ch))
    return;
    
    GET_PRIVATE(ch) = 0;
    GET_PMODE(ch) = 0;
    if (GET_PTOPIC(ch))
	free_string(GET_PTOPIC(ch));
    GET_PTOPIC(ch) = str_dup("");
}

static char *channel_mode(CHAR_DATA * ch)
{    
    switch (GET_PMODE(ch)) {
    case PCLOSED:
    case POPENED:
	return "(Owner )";
    case PADDED:
	return "(Added )";
    case PJOINED:
	return "(Joined)";
    }
    return "(??????)";
}

CHAR_DATA *get_char_by_idnum(int idnum)
{
    register DESCRIPTOR_DATA *i;
    for (i = descriptor_list; i; i = i->next)
	if (!i->connected && i->character && GET_IDNUM(i->character) == idnum)
	    return i->character;
    return NULL;
}

void do_pchannels(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
    return;
    
    if (!IS_SET(ch->comm, COMM_PRIVATE)) {
	send_to_char("Private channels are now {gON{x.\n\r", ch);
	SET_BIT(ch->comm, COMM_PRIVATE);
    } else {
	send_to_char("Private channels are now {rOFF{x.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_PRIVATE);
    }
}

void do_psay(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    
    if (IS_NPC(ch))
    return;
    
    if (!GET_PRIVATE(ch)) {
	send_to_char("You are not on a private channel!\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Psay what?\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (!IS_SET(ch->comm, COMM_PRIVATE)) {
	send_to_char("You must turn on private channels first.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel priviliges have been revoked.\n\r", ch);
	return;
    }
    argument = makedrunk(argument, ch);

    sprintf(buf, "You private-say '{W%s{x'\n\r", argument);
    send_to_char(buf, ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (victim == NULL)
	    continue;

	if (GET_PRIVATE(ch) != GET_PRIVATE(victim))
	    continue;

	if (d->connected == CON_PLAYING && victim != ch &&
	    !IS_SET(victim->comm, COMM_QUIET) &&
	    IS_SET(victim->comm, COMM_PRIVATE))
	    act_new("$n private says '{W$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
    }
}

void do_pclose(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
    return;
    
    if (GET_PRIVATE(ch) != GET_IDNUM(ch))
	send_to_char("You must open your own private channel first.\n\r", ch);
    else if (GET_PMODE(ch) != POPENED)
	send_to_char("Your private channel is not open.\n\r", ch);
    else {
	GET_PMODE(ch) = PCLOSED;
	if (GET_PTOPIC(ch)) {
	    free_string(GET_PTOPIC(ch));
	    GET_PTOPIC(ch) = str_dup("");
	}
	send_to_char("You close your private channel.  No one can join now.\n\r", ch);
    }
}

void do_pcreate(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *current;
    
    if (IS_NPC(ch))
    return;
    
    if (!IS_SET(ch->comm, COMM_PRIVATE)) {
	send_to_char("You must turn on private channels first.\n\r", ch);
	return;
    }
    if (!GET_PRIVATE(ch) || !(current = get_char_by_idnum(GET_PRIVATE(ch))))
	current = NULL;

    if (current && current != ch) {
	act_new("$n has left your private channel.", ch, 0, current, TO_VICT, POS_DEAD);
	act_new("You have left $N's private channel.", ch, 0, current, TO_CHAR, POS_DEAD);
    }
    GET_PRIVATE(ch) = GET_IDNUM(ch);
    GET_PMODE(ch) = PCLOSED;

    send_to_char("You open your own private channel.\n\r", ch);
}

void do_pdestroy(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;
    
    if (IS_NPC(ch))
    return;
    
    close_private(ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (victim == NULL)
	    continue;

	if (d->connected == CON_PLAYING && GET_PRIVATE(victim) == GET_IDNUM(ch) && victim != ch) {
	    close_private(victim);
	    act_new("$n has closed $s private channel.", ch, 0, victim, TO_VICT, POS_DEAD);
	}
    }

    send_to_char("You close your private channel.\n\r", ch);
}

void do_pinfo(CHAR_DATA * ch, char *argument)
{
    char pers_buf[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    
    if (IS_NPC(ch))
    return;
    
    send_to_char("{W{4   Open Private Channels    {x\n\r\n\r", ch);
    send_to_char("Name\n\r   Topic\n\r", ch);
    send_to_char("--------------------------------------------------------------------------\n\r", ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (victim == NULL)
	    continue;

	if (d->connected == CON_PLAYING && GET_PMODE(victim) == POPENED && GET_PRIVATE(victim) == GET_IDNUM(victim)) {
	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "{c%s\n\r   {y%s{x\n\r", pers_buf, GET_PTOPIC(victim));
	    send_to_char(buf, ch);
	    send_to_char("--------------------------------------------------------------------------\n\r\n\r", ch);
	}
    }
}

void do_pjoin(CHAR_DATA * ch, char *argument)
{

    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    
    if (IS_NPC(ch))
    return;
    
	return;
    if (!IS_SET(ch->comm, COMM_PRIVATE)) {
	send_to_char("You must turn on private channels first.\n\r", ch);
	return;
    }
    if (GET_PRIVATE(ch)) {
	send_to_char("You are already on a private channel.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Who's private channel did you want to join?\n\r", ch);
	return;
    }
    one_argument(argument, arg);

    victim = get_char_world(ch, arg);

    if (victim == NULL || GET_PRIVATE(victim) != GET_IDNUM(victim) || GET_PMODE(victim) != POPENED) {
	send_to_char("Sorry, no such private channel in existence.\n\r", ch);
	return;
    }
    GET_PRIVATE(ch) = GET_IDNUM(victim);
    GET_PMODE(ch) = PJOINED;
    act_new("$n has joined your private channel.", ch, 0, victim, TO_VICT, POS_DEAD);
    act_new("You join $N's private channel.", ch, 0, victim, TO_CHAR, POS_DEAD);
}

void do_poff(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *current;
    
    if (IS_NPC(ch))
    return;
    
    if (!GET_PRIVATE(ch)) {
	send_to_char("You are not on a private channel!\n\r", ch);
	return;
    }
    if (GET_PRIVATE(ch) == GET_IDNUM(ch)) {
	do_pdestroy(ch, "");
	return;
    }
    if (!(current = get_char_by_idnum(GET_PRIVATE(ch))))
	current = NULL;

    if (current && current != ch) {
	act_new("$n has left your private channel.", ch, 0, current, TO_VICT, POS_DEAD);
	act_new("You have left $N's private channel.", ch, 0, current, TO_CHAR, POS_DEAD);
    } else
	send_to_char("You stop using any private channel.\n\r", ch);

    close_private(ch);
}

void do_popen(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
    return;
    
    if (!IS_SET(ch->comm, COMM_PRIVATE)) {
	send_to_char("You must turn on private channels first.\n\r", ch);
	return;
    }
    if (GET_PRIVATE(ch) != GET_IDNUM(ch))
	send_to_char("You must open your own private channel first.\n\r", ch);
    else if (GET_PMODE(ch) != PCLOSED)
	send_to_char("Your private channel is not closed.\n\r", ch);
    else if (argument[0] == '\0')
	send_to_char("You must state a topic for your channel.\n\r", ch);
    else {
	GET_PMODE(ch) = POPENED;
	GET_PTOPIC(ch) = str_dup(argument);
	send_to_char("You open up your private channel.  Anyone can join now.\n\r", ch);
    }
}

void do_premove(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    
    if (IS_NPC(ch))
    return;
    
    if (!GET_PRIVATE(ch) || GET_PRIVATE(ch) != GET_IDNUM(ch)) {
	send_to_char("You do not own a private channel at this moment.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Who did you want to remove from your private channel?\n\r", ch);
	return;
    }
    one_argument(argument, arg);

    victim = get_char_world(ch, arg);

    if (victim == NULL || IS_NPC(victim)) {
	send_to_char("That is not possible.\n\r", ch);
	return;
    }
    if (GET_PRIVATE(victim) != GET_IDNUM(ch)) {
	send_to_char("Are you sure they are on your private channel?\n\r", ch);
	return;
    }
    close_private(victim);
    act_new("$n has removed you from $s private channel.", ch, 0, victim, TO_VICT, POS_DEAD);
    act_new("You remove $N from your private channel.", ch, 0, victim, TO_CHAR, POS_DEAD);
}

void do_pwho(CHAR_DATA * ch, char *argument)
{
    char pers_buf[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    
    if (IS_NPC(ch))
    return;
    

    if (!GET_PRIVATE(ch)) {
	send_to_char("You are not on a private channel!\n\r", ch);
	return;
    }
    send_to_char("{W{4   Private Channel Members    {x\n\r\n\r", ch);
    send_to_char("--------------------------------------------------------------------------\n\r", ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (victim == NULL)
	    continue;

	if (d->connected == CON_PLAYING && GET_PRIVATE(ch) == GET_PRIVATE(victim)) {
	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "   {y%s   {c%s{x\n\r", channel_mode(victim), pers_buf);
	    send_to_char(buf, ch);
	}
    }
}

void do_pcheck(CHAR_DATA * ch, char *argument)
{
    char pers_buf[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (IS_NPC(ch))
    return;
    
    send_to_char("{W{4   Private Channel Listing    {x\n\r\n\r", ch);
    send_to_char("--------------------------------------------------------------------------\n\r", ch);

    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;

	victim = d->original ? d->original : d->character;

	if (victim == NULL)
	    continue;

	if (d->connected == CON_PLAYING && GET_PRIVATE(victim)) {
	    PERS(victim, ch, pers_buf);
	    sprintf(buf, "{r%d   {y%s   {c%s{x\n\r", GET_PRIVATE(ch), channel_mode(victim), pers_buf);
	    send_to_char(buf, ch);
	}
    }
}

void do_padd(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (IS_NPC(ch))
    return;
    
    if (GET_PRIVATE(ch) != GET_IDNUM(ch)) {
	send_to_char("You must open your own private channel first.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Who did you wish to add?\n\r", ch);
	return;
    }
    one_argument(argument, arg);

    victim = get_char_world(ch, arg);

    if (victim == NULL || IS_NPC(victim) || GET_PRIVATE(victim) || !IS_SET(victim->comm, COMM_PRIVATE) ||
	(GET_PMODE(ch) == PCLOSED && victim->side != ch->side)) {
	send_to_char("That is not possible.\n\r", ch);
	return;
    }
    if (ch == victim) {
	send_to_char("Ummm...Ok", ch);
	return;
    }
    GET_PRIVATE(victim) = GET_IDNUM(ch);
    GET_PMODE(victim) = PADDED;
    act_new("$n has added you to $s private channel.", ch, 0, victim, TO_VICT, POS_DEAD);
    act_new("You add $N to your private channel.", ch, 0, victim, TO_CHAR, POS_DEAD);
}
