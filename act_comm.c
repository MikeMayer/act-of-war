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
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* command procedures needed */
DECLARE_DO_FUN(do_quit);

/* function prototypes */
char *makedrunk(char *string, CHAR_DATA * ch);

void do_delet(CHAR_DATA * ch, char *argument)
{
    send_to_char("You must type the full command to delete yourself.\n\r", ch);
}

void do_delete(CHAR_DATA * ch, char *argument)
{
    char strsave[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
 
     if (IS_NPC(ch))
 	return;
 	
 	if (IS_AFFECTED(ch, AFF_NOQUIT))
 	{
	 	send_to_char("You can't delete while noquit, cheater.\n\r",ch);
	 	return;
 	}
 
     if (ch->pcdata->confirm_delete) {
 	 if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd) || argument[0] == '\0') {
 	    send_to_char("Delete status removed. Password Incorrect.\n\r", ch);
 	    ch->pcdata->confirm_delete = FALSE;
 	    return;
 	} else {
 	    sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
 	    wiznet("$N turns $Mself into line noise.", ch, NULL, 0, 0, 0);
 	    sprintf(buf, "AoW just lost a player. %s has self deleted.\n\r", ch->name);
 	    news(buf, ch->side);
 	    stop_fighting(ch, TRUE);
 	    do_quit(ch, "");
 	    unlink(strsave);
 	    return;
 	}
     }
      if (argument[0] == '\0')
        {
          send_to_char("Delete command updated. Type delete <password> to delete.\n\r", ch);
          return;
        }
      if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd)) {
 	send_to_char("Incorrect password. Type delete <password> to delete.\n\r", ch);
 	return;
     }
     send_to_char("Type delete <password> again to confirm this command.\n\r", ch);
     send_to_char("WARNING: this command is {Rirreversible{x.\n\r", ch);
     send_to_char("Typing delete with an argument, or any other command, will undo delete status.\n\r", ch);
     ch->pcdata->confirm_delete = TRUE;
     wiznet("$N is contemplating deletion.", ch, NULL, 0, 0, get_trust(ch));
 }



void do_channels(CHAR_DATA * ch, char *argument)
{
    //char buf[MSL];
	char colA[SmSL], colB[SmSL];

	stc("{w C O M M U N I C A T I O N{m  - - -{w  S T A T U S {x\n\r", ch);
	stc("{m================================================{x\n\r", ch);


	// Main Channel
    if (!IS_SET(ch->comm, COMM_NOGOSSIP))
	sprintf(colA, "{G"); else sprintf(colA, "{r");
	//Advice Channel
    if (!IS_SET(ch->comm, COMM_NOADVICE))
	sprintf(colB, "{G"); else sprintf(colB, "{r");
	ptc(ch, "{D|%s    Kingdom Channel{D    |%s    Advice Channel{D    |{x\n\r", colA, colB);
	//News Channel
    if (!IS_SET(ch->comm, COMM_NONEWS))
	sprintf(colA, "{G"); else sprintf(colA, "{r");
	//Trivia Channel
    if (!IS_SET(ch->comm, COMM_NOTRIVIA))
	sprintf(colB, "{G"); else sprintf(colB, "{r");
	ptc(ch, "{D|%s       News Channel{D    |%s    Trivia Channel{D    |{x\n\r", colA, colB);
	//Auction Channel
    if (!IS_SET(ch->comm, COMM_NOAUCTION))
	sprintf(colA, "{G"); else sprintf(colA, "{r");
	//Music Channel
    if (!IS_SET(ch->comm, COMM_NOMUSIC))
	sprintf(colB, "{G"); else sprintf(colB, "{r");
	ptc(ch, "{D|%s    Auction Channel{D    |%s    Music  Channel{D    |{x\n\r", colA, colB);
	//Question Channel
    if (!IS_SET(ch->comm, COMM_NOQUESTION))
	sprintf(colA, "{G"); else sprintf(colA, "{r");
	//Quote Channel
    if (!IS_SET(ch->comm, COMM_NOQUOTE))
	sprintf(colB, "{G"); else sprintf(colB, "{r");
	ptc(ch, "{D|%s   Question Channel{D    |%s    Quote  Channel{D    |{x\n\r", colA, colB);
	//Grat Channel
    if (!IS_SET(ch->comm, COMM_NOGRATS))
	sprintf(colA, "{G"); else sprintf(colA, "{r");
	//Shout Channel
    if (!IS_SET(ch->comm, COMM_SHOUTSOFF))
	sprintf(colB, "{G"); else sprintf(colB, "{r");
	ptc(ch, "{D|%s      Grats Channel{D    |%s    Shout  Channel{D    |{x\n\r", colA, colB);
	//Private Channels
    if (IS_SET(ch->comm, COMM_PRIVATE))
	sprintf(colA, "{G"); else sprintf(colA, "{x{r");
	//Crusade Channel
    if (!IS_SET(ch->comm, COMM_CRUSADE))
	sprintf(colB, "{G"); else sprintf(colB, "{x{r");
    //Tell Channel
    if (!IS_SET(ch->comm, COMM_DEAF))
	sprintf(colA, "{G"); else sprintf(colA, "{x{r");
	//Quiet Mode
    if (IS_SET(ch->comm, COMM_QUIET))
	sprintf(colB, "{G"); else sprintf(colB, "{x{r");
	ptc(ch, "{D|%s          Deaf Mode{D    |%s    Quiet Mode    {D    |{x\n\r", colA, colB);
	if (IS_SET(ch->comm,COMM_PENTER))
	sprintf(colA, "{G"); else sprintf(colA, "{r");

    if (IS_IMMORTAL(ch))
     {

	if (!IS_SET(ch->comm, COMM_NOWIZ))
	sprintf(colB, "{G"); else sprintf(colB, "{r");

	ptc(ch, "{D|%s       Player Entry{D    |%s    Immtalk       {D    |{x\n\r", colA, colB);
    } else {
	ptc(ch, "{D|%s       Player Entry{D    |%s                  {D    |{x\n\r", colA, colB);
	}

	stc("{m================================================{x\n\r", ch);

    if (IS_SET(ch->comm, COMM_AFK))
	stc("{$You are {g[AFK]{$.{x\n\r", ch);
	if (IS_SET(ch->comm, COMM_SNOOP_PROOF))
	stc("{$You are immune to snooping.{x\n\r", ch);
	if (ch->prompt != NULL) {
	ptc(ch, "{$Your current prompt is{@:{x %s\n\r", ch->prompt);
    } 
//    if (ch->pcdata->email_addy != NULL) {
//	ptc(ch, "{$Your current Email Address is{@:{x %s\n\r", ch->pcdata->email_addy);
//    }
    if (IS_SET(ch->comm, COMM_NOSHOUT))
	stc("{rYou cannot shout.{x\n\r", ch);
    if (IS_SET(ch->comm, COMM_NOTELL))
	stc("{rYou cannot use tell.{x\n\r", ch);
    if (IS_SET(ch->comm, COMM_NOCHANNELS))
	stc("{rYou cannot communicate at all.{x\n\r", ch);
    if (IS_SET(ch->comm, COMM_NOEMOTE))
	stc("{rYou cannot show emotions.{x\n\r", ch);
	if (IS_SET(ch->comm, COMM_NONOTE))
	stc("{rYou cannot send or read notes.{x\n\r", ch);
}


void do_channels2(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    send_to_char("\n\rChannel      Status\n\r", ch);
    send_to_char("{m---------------------{x\n\r", ch);

    send_to_char("advice         ", ch);
    if (!IS_SET(ch->comm, COMM_NOADVICE))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("news           ", ch);
    if (!IS_SET(ch->comm, COMM_NONEWS))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("trivia         ", ch);
    if (!IS_SET(ch->comm, COMM_NOTRIVIA))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("gossip         ", ch);
    if (!IS_SET(ch->comm, COMM_NOGOSSIP))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("auction        ", ch);
    if (!IS_SET(ch->comm, COMM_NOAUCTION))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("music          ", ch);
    if (!IS_SET(ch->comm, COMM_NOMUSIC))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("Q/A            ", ch);
    if (!IS_SET(ch->comm, COMM_NOQUESTION))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("Quote          ", ch);
    if (!IS_SET(ch->comm, COMM_NOQUOTE))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("grats          ", ch);
    if (!IS_SET(ch->comm, COMM_NOGRATS))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    if (IS_IMMORTAL(ch)) {
	send_to_char("god channel    ", ch);
	if (!IS_SET(ch->comm, COMM_NOWIZ))
	    send_to_char("{gON{x\n\r", ch);
	else
	    send_to_char("{rOFF{x\n\r", ch);
    }
    send_to_char("shouts         ", ch);
    if (!IS_SET(ch->comm, COMM_SHOUTSOFF))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("tells          ", ch);
    if (!IS_SET(ch->comm, COMM_DEAF))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("quiet mode     ", ch);
    if (IS_SET(ch->comm, COMM_QUIET))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("Pchannels      ", ch);
    if (IS_SET(ch->comm, COMM_PRIVATE))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);
    send_to_char("Crusade        ", ch);
    if (!IS_SET(ch->comm, COMM_CRUSADE))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);
	
	send_to_char("Player Entry   ",ch);
	if (IS_SET(ch->comm,COMM_PENTER))
	send_to_char("{gON{c\n\r",ch);
    else
    send_to_char("{rOFF{c\n\r",ch);

    if (IS_SET(ch->comm, COMM_AFK))
	send_to_char("You are {g[AFK]{x.\n\r", ch);

    if (IS_SET(ch->comm, COMM_SNOOP_PROOF))
	send_to_char("You are immune to snooping.\n\r", ch);

    if (ch->lines != PAGELEN) {
	if (ch->lines) {
	    sprintf(buf, "You display %d lines of scroll.\n\r", ch->lines + 2);
	    send_to_char(buf, ch);
	} else
	    send_to_char("Scroll buffering is off.\n\r", ch);
    }
    if (ch->prompt != NULL) {
	sprintf(buf, "Your current prompt is: %s\n\r", ch->prompt);
	send_to_char(buf, ch);
    }
    if (IS_SET(ch->comm, COMM_NOSHOUT))
	send_to_char("{rYou cannot shout.{x\n\r", ch);

    if (IS_SET(ch->comm, COMM_NOTELL))
	send_to_char("{rYou cannot use tell.{x\n\r", ch);

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
	send_to_char("{rYou cannot communicate at all.{x\n\r", ch);

    if (IS_SET(ch->comm, COMM_NOEMOTE))
	send_to_char("{rYou cannot show emotions.{x\n\r", ch);
}

void do_penter ( CHAR_DATA *ch, char * argument)
{
   if (IS_SET(ch->comm,COMM_PENTER))
   {
      send_to_char("{MPlayer Entry message removed.{x\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_PENTER);
   }
   else
   {
     send_to_char("{MFrom now on, you will see Player Entries.{x\n\r",ch);
     SET_BIT(ch->comm,COMM_PENTER);
   }
}

void do_deaf(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_DEAF)) {
	send_to_char("You can now hear tells again.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_DEAF);
    } else {
	send_to_char("From now on, you won't hear tells.\n\r", ch);
	SET_BIT(ch->comm, COMM_DEAF);
    }
}

void do_quiet(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("Quiet mode removed.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_QUIET);
    } else {
	send_to_char("From now on, you will only hear says and emotes.\n\r", ch);
	SET_BIT(ch->comm, COMM_QUIET);
    }
}

void do_afk(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_AFK)) {
	send_to_char("{g[AFK]{x mode removed.  Type 'replay' to see tells.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_AFK);
    } else {
	send_to_char("You are now in {g[AFK]{x mode.\n\r", ch);
	SET_BIT(ch->comm, COMM_AFK);
    }
}

void do_replay(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch)) {
	send_to_char("You can't replay.\n\r", ch);
	return;
    }
    if (buf_string(ch->pcdata->buffer)[0] == '\0') {
	send_to_char("You have no tells to replay.\n\r", ch);
	return;
    }
   
//Hardcore crash protection for replay bug if needed - Akuji    
//     
//     if (strlen(ch->pcdata->buffer) >= 2048) {
// 	    send_to_char("Replay buffer too large, tells are lost.\n\r", ch);
// 	    clear_buf(ch->pcdata->buffer);
// 	    return;
// 	}

    page_to_char(buf_string(ch->pcdata->buffer), ch);

    clear_buf(ch->pcdata->buffer);

}

void do_auction(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOAUCTION)) {
	    send_to_char("Auction channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOAUCTION);
	} else {
	    send_to_char("Auction channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOAUCTION);
	}
    } else {			/* auction message sent, turn auction on if it is off */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOAUCTION);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You auction '{y%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOAUCTION) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == GET_SIDE(ch))
		    act_new("$n auctions '{y$t{x'", ch, argument, d->character, TO_VICT, POS_DEAD);
	    }
	}
    }
}

void news(char *message, int side)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (message == NULL)
	return;

    sprintf(buf, "{y[NEWS]:{x %s\n\r", message);

    for (d = descriptor_list; d; d = d->next) {
	if (d->character == NULL)
	    continue;

	if IS_IMMORTAL (d->character)
	{
		send_to_char(buf, d->character);
		continue;
	}

if ((d->connected == CON_PLAYING &&
	    side == d->character->side &&
	    !IS_NPC(d->character) &&
	    !IS_SET(d->character->comm, COMM_NONEWS))
           || (side == 9 && !IS_SET(d->character->comm, COMM_NONEWS)))
	    send_to_char(buf, d->character);
    }
}



void do_info(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_NONEWS)) {
	send_to_char("News channel is now {gON{x.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_NONEWS);
    } else {
	send_to_char("News channel is now {rOFF{x.\n\r", ch);
	SET_BIT(ch->comm, COMM_NONEWS);
    }
}

void do_gossip(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (ch->level < 5) {
	send_to_char("You must at least be level 5 to use global communication.\n\r", ch);
	return;
    }
    if ((GET_SIDE(ch) != SIDE_TIER) && (!IS_IMMORTAL(ch))) {
	send_to_char("Gossip is for Tier only.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOGOSSIP)) {
	    send_to_char("Main channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
	} else {
	    send_to_char("Main channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOGOSSIP);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOGOSSIP);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You gossip '{M%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGOSSIP) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == SIDE_TIER || IS_IMMORTAL(victim)) {
		    if (IS_IMMORTAL(victim) && !IS_NPC(ch)) {
			sprintf(buf, "[%s] %s gossips '{M%s{x'\n\r",
kingdom_table[ch->side].cname, ch->name, argument);
			send_to_char(buf, victim);
		    } else
			act_new("$n gossips '{M$t{x'", ch, argument,
d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
}
/*
void do_whine(CHAR_DATA * ch, char *argument)

{

    char buf[MAX_STRING_LENGTH];

    DESCRIPTOR_DATA *d;

    if (ch->level < 5) {

	send_to_char("You must at least be level 5 to use global communication.\n\r", ch);

	return;

    }

    if ( ch->level < 50 ) {

	send_to_char("Only level 50 characters or Tierians can whine!\n\r", ch);

	return;

    }

    if (argument[0] == '\0') {

	if (IS_SET(ch->comm, COMM_NOGOSSIP)) {

	    send_to_char("Main channel is now {gON{x.\n\r", ch);

	    REMOVE_BIT(ch->comm, COMM_NOGOSSIP);

	} else {

	    send_to_char("Main channel is now {rOFF{x.\n\r", ch);

	    SET_BIT(ch->comm, COMM_NOGOSSIP);

	}

    } else {			// gossip message sent, turn gossip on if it isn't already 

	if (IS_SET(ch->comm, COMM_QUIET)) {

	    send_to_char("You must turn off quiet mode first.\n\r", ch);

	    return;

	}

	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {

	    send_to_char("Your channel privileges have been revoked.\n\r", ch);

	    return;

	}

	REMOVE_BIT(ch->comm, COMM_NOGOSSIP);



	argument = makedrunk(argument, ch);

	if (ch->side == 1)
	sprintf(buf, "You whine '{c%s{x'\n\r", argument);
	else if (ch->side == 2)
	sprintf(buf, "You whine '{g%s{x'\n\r", argument);
	else if (ch->side == 3)
	sprintf(buf, "You whine '{r%s{x'\n\r", argument);
	else
	sprintf(buf, "You whine '{W%s{x'\n\r", argument);

	send_to_char(buf, ch);

	for (d = descriptor_list; d != NULL; d = d->next) {

	    CHAR_DATA *victim;

	    victim = d->original ? d->original : d->character;



	    if (victim == NULL)

		continue;



	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGOSSIP) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (victim->level >= 50 || IS_IMMORTAL(victim) ) {

		    if (IS_IMMORTAL(victim) && !IS_NPC(ch)) {

			sprintf(buf, "[%s] %s whines '{M%s{x'\n\r",kingdom_table[ch->side].cname, ch->name, argument);

			send_to_char(buf, victim);

		    } 
		    else {
		    
		    if (ch->side == 1)
		    act_new("$n whines '{c$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
		    
		    else if (ch->side == 2)
		    act_new("$n whines '{g$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
		    
		    else if (ch->side == 3)
		    act_new("$n whines '{r$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
		    
		    else
			act_new("$n whines '{W$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
			}
		}

	    }

	}

    }

}
*/

///

void do_crufun(CHAR_DATA * ch, char *argument)
{

	do_crusade(ch, argument, GET_SIDE(ch));
		return;
}

void do_crusade(CHAR_DATA * ch, char *argument, int side)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0' && !IS_NPC(ch)) {
	if (IS_SET(ch->comm, COMM_CRUSADE)) {
	    send_to_char("Crusade channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_CRUSADE);
	} else {
	    send_to_char("Crusade channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_CRUSADE);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS) && !IS_NPC(ch)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	if (IS_IMMORTAL(ch))
		side = 9;

    if (!IS_NPC(ch))
	REMOVE_BIT(ch->comm, COMM_CRUSADE);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You {BC{brusad{Be{x '{M%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_CRUSADE) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == side || IS_IMMORTAL(victim)) {
		    if (IS_IMMORTAL(victim) ) {
				if (!IS_NPC(ch))
			sprintf(buf, "[%s] %s {BC{brusade{Bs{x '{M%s{x'\n\r", kingdom_table[ch->side].cname, ch->name, argument);
            else
            sprintf(buf, "[%s] %s {BC{brusade{Bs{x '{M%s{x'\n\r", kingdom_table[ch->side].cname,
			ch->pIndexData->short_descr, argument);

			send_to_char(buf, victim);

		    } else
			act_new("$n {BC{brusade{Bs{x '{M$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
		} 		else if (side == 9 && !IS_IMMORTAL(victim))
             act_new("$n {BC{brusade{Bs{x '{M$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);


	    }

	}

	if (  ( !str_cmp( argument, "what mob?")			)
       || ( !str_cmp( argument, "who was the thief?" )	)
       || ( !str_cmp( argument, "who is the thief?"  )	)
	   || ( !str_cmp( argument, "what item?" )			)
       || ( !str_cmp( argument, "where are you?" )		)
       || ( !str_cmp( argument, "who stole the item?" )	)
       || ( !str_cmp( argument, "where is the thief?" )	)  )
	{
       ask_quest_question(ch,argument);
       return;
	}

    }

  }

void do_snarl(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (ch->level < 5) {
	send_to_char("You must at least be level 5 to use global communication.\n\r", ch);
	return;
    }
    if ((GET_SIDE(ch) != SIDE_UM) && (!IS_IMMORTAL(ch))) {
	send_to_char("Snarl is used by undermountain.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOGOSSIP)) {
	    send_to_char("Main channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
	} else {
	    send_to_char("Main channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOGOSSIP);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOGOSSIP);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You snarl '{M%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGOSSIP) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == SIDE_UM || IS_IMMORTAL(victim)) {
		    if (IS_IMMORTAL(victim) && !IS_NPC(ch)) {
			sprintf(buf, "[%s] %s snarls '{M%s{x'\n\r",
kingdom_table[ch->side].cname, ch->name, argument);
			send_to_char(buf, victim);
		    } else
			act_new("$n snarls '{M$t{x'",ch, argument,
d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
}

void do_bellow(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (ch->level < 5) {
	send_to_char("You must at least be level 5 to use global communication.\n\r", ch);
	return;
    }
    if ((GET_SIDE(ch) != SIDE_KAEL) && (!IS_IMMORTAL(ch))) {
	send_to_char("Bellow is for Kael only.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOGOSSIP)) {
	    send_to_char("Main channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
	} else {
	    send_to_char("Main channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOGOSSIP);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOGOSSIP);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You bellow '{M%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGOSSIP) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == SIDE_KAEL || IS_IMMORTAL(victim)) {
		    if (IS_IMMORTAL(victim) && !IS_NPC(ch)) {
			sprintf(buf, "[%s] %s bellows '{M%s{x'\n\r",
kingdom_table[ch->side].cname, ch->name, argument);
			send_to_char(buf, victim);
		    } else
			act_new("$n bellows '{M$t{x'", ch, argument,
d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
}

void do_inform(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (ch->level < 5) {
	send_to_char("You must at least be level 5 to use global communication.\n\r", ch);
	return;
    }
    if ((GET_SIDE(ch) != SIDE_CAIRN) && (!IS_IMMORTAL(ch))) {
	send_to_char("Inform is for Cairn only.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOGOSSIP)) {
	    send_to_char("Main channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
	} else {
	    send_to_char("Main channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOGOSSIP);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;

	}
	REMOVE_BIT(ch->comm, COMM_NOGOSSIP);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You inform '{M%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGOSSIP) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == SIDE_CAIRN || IS_IMMORTAL(victim)) {
		    if (IS_IMMORTAL(victim) && !IS_NPC(ch)) {
			sprintf(buf, "[%s] %s informs '{M%s{x'\n\r",
kingdom_table[ch->side].cname, ch->name, argument);
			send_to_char(buf, victim);
		    } else
			act_new("$n informs '{M$t{x'",ch, argument,
d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
}

void do_trivia(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOTRIVIA)) {
	    send_to_char("Trivia channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOTRIVIA);
	} else {
	    send_to_char("Trivia channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOTRIVIA);
	}
    } else {			/* gossip message sent, turn gossip on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;

	}
	REMOVE_BIT(ch->comm, COMM_NOTRIVIA);

	argument = makedrunk(argument, ch);
	sprintf(buf, "{C[{YTrIvIa{C]:{W %s{x\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOTRIVIA) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == GET_SIDE(ch) || IS_IMMORTAL(victim) || IS_IMMORTAL(ch)) {
		    if (IS_IMMORTAL(victim) && !IS_NPC(ch)) {
			sprintf(buf, "{W[%s{W][%s]{C[{YTrIvIa{C]:{W %s{x\n\r",
kingdom_table[ch->side].cname, ch->name, argument);
			send_to_char(buf, victim);
		    } else
			act_new("{C[{YTrIvIa{C][{Y$n{C]:{W $t{x", ch, argument, d->character, TO_VICT, POS_SLEEPING);
		}
	    }
	}
    }
}

void do_grats(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOGRATS)) {
	    send_to_char("Grats channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOGRATS);
	} else {
	    send_to_char("Grats channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOGRATS);
	}
    } else {			/* grats message sent, turn grats on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOGRATS);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You grat '{c%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOGRATS) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == GET_SIDE(ch))
		    act_new("$n grats '{c$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

void do_quote(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOQUOTE)) {
	    send_to_char("Quote channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOQUOTE);
	} else {
	    send_to_char("Quote channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOQUOTE);
	}
    } else {			/* quote message sent, turn quote on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOQUOTE);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You quote '{y%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOQUOTE) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == GET_SIDE(ch))
		    act_new("$n quotes '{y$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

/* RT question channel */
void do_question(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOQUESTION)) {
	    send_to_char("Q/A channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOQUESTION);
	} else {
	    send_to_char("Q/A channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOQUESTION);
	}
    } else {			/* question sent, turn Q/A on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOQUESTION);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You question '{y%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (victim == NULL)
		continue;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOQUESTION) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == GET_SIDE(ch) && !IS_IMMORTAL(victim))
		    act_new("{y$n questions '{y$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
           if (IS_IMMORTAL(victim))
                 {
               sprintf(buf2, "[%s] {y$n questions '{y$t{x'",
kingdom_table[ch->side].cname);
                act_new(buf2, ch, argument, d->character, TO_VICT, POS_SLEEPING);
                  }
	    }
	}
    }
}

/* RT answer channel - uses same line as questions */
void do_answer(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOQUESTION)) {
	    send_to_char("Q/A channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOQUESTION);
	} else {
	    send_to_char("Q/A channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOQUESTION);
	}
    } else {			/* answer sent, turn Q/A on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOQUESTION);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You answer '{y%s{x'\n\r", argument);
	send_to_char(buf, ch);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOQUESTION) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == GET_SIDE(ch))
		    act_new("$n answers '{y$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

/* RT music channel */
void do_music(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOMUSIC)) {
	    send_to_char("Music channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOMUSIC);
	} else {
	    send_to_char("Music channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOMUSIC);
	}
    } else {			/* music sent, turn music on if it isn't already */
	if (IS_SET(ch->comm, COMM_QUIET)) {
	    send_to_char("You must turn off quiet mode first.\n\r", ch);
	    return;
	}
	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    send_to_char("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(ch->comm, COMM_NOMUSIC);

	argument = makedrunk(argument, ch);
	sprintf(buf, "You MUSIC: '{y%s{x'\n\r", argument);
	send_to_char(buf, ch);
	sprintf(buf, "$n MUSIC: '{y%s{x'\n\r", argument);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    CHAR_DATA *victim;
	    victim = d->original ? d->original : d->character;

	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		!IS_SET(victim->comm, COMM_NOMUSIC) &&
		!IS_SET(victim->comm, COMM_QUIET)) {
		if (GET_SIDE(victim) == GET_SIDE(ch))
		    act_new("$n MUSIC: '{y$t{x'", ch, argument, d->character, TO_VICT, POS_SLEEPING);
	    }
	}
    }
}

/* clan channels */
void do_clantalk(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    if (!is_clan(ch) || clan_table[ch->clan].independent) {
	send_to_char("You aren't in a clan.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOCLAN)) {
	    send_to_char("Clan channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOCLAN);
	} else {
	    send_to_char("Clan channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOCLAN);
	}
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOCLAN);

    argument = makedrunk(argument, ch);
    sprintf(buf, "You clan '{W%s{x'\n\r", argument);
    send_to_char(buf, ch);
    sprintf(buf, "$n clans '{W%s{x'\n\r", argument);
    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    is_same_clan(ch, d->character) &&
	    !IS_SET(d->character->comm, COMM_NOCLAN) &&
	    !IS_SET(d->character->comm, COMM_QUIET)) {
	    if (GET_SIDE(d->character) == GET_SIDE(ch))
		act_new("$n clans '{W$t{x'", ch, argument, d->character, TO_VICT, POS_DEAD);
	}
    }
}

void do_immtalk(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_NOWIZ)) {
	    send_to_char("Immortal channel is now {gON{x.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_NOWIZ);
	} else {
	    send_to_char("Immortal channel is now {rOFF{x.\n\r", ch);
	    SET_BIT(ch->comm, COMM_NOWIZ);
	}
	return;
    }
    REMOVE_BIT(ch->comm, COMM_NOWIZ);

    argument = makedrunk(argument, ch);
    sprintf(buf, "{c[{y$n{c]: %s{x", argument);
    act_new("{c[{y$n{c]: $t{x", ch, argument, NULL, TO_CHAR, POS_DEAD);
    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING &&
	    IS_IMMORTAL(d->character) &&
	    !IS_SET(d->character->comm, COMM_NOWIZ)) {
	    act_new("{c[{y$n{c]: $t{x", ch, argument, d->character, TO_VICT, POS_DEAD);
	}
    }
}

char *makedrunk(char *string, CHAR_DATA * ch)
{
    struct drunk_struct {
	int min_drunk_level;
	int number_of_rep;
	char *replacement[11];
    };

    struct drunk_struct drunk[] =
    {
	{3, 10, {"a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "oa", "ahhhh"}},
	{8, 5,  {"b", "b", "b", "B", "B", "vb"}},
	{3, 5,  {"c", "c", "C", "cj", "sj", "zj"}},
	{5, 2,  {"d", "d", "D"}},
	{3, 3,  {"e", "e", "eh", "E"}},
	{4, 5,  {"f", "f", "ff", "fff", "fFf", "F"}},
	{8, 2,  {"g", "g", "G"}},
	{9, 6,  {"h", "h", "hh", "hhh", "Hhh", "HhH", "H"}},
	{7, 6,  {"i", "i", "Iii", "ii", "iI", "Ii", "I"}},
	{9, 5,  {"j", "j", "jj", "Jj", "jJ", "J"}},
	{7, 2,  {"k", "k", "K"}},
	{3, 2,  {"l", "l", "L"}},
	{5, 8,  {"m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M"}},
	{6, 6,  {"n", "n", "nn", "Nn", "nnn", "nNn", "N"}},
	{3, 6,  {"o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo"}},
	{3, 2,  {"p", "p", "P"}},
	{5, 5,  {"q", "q", "Q", "ku", "ququ", "kukeleku"}},
	{4, 2,  {"r", "r", "R"}},
	{2, 5,  {"s", "ss", "zzZzssZ", "ZSssS", "sSzzsss", "sSss"}},
	{5, 2,  {"t", "t", "T"}},
	{3, 6,  {"u", "u", "uh", "Uh", "Uhuhhuh", "uhU", "uhhu"}},
	{4, 2,  {"v", "v", "V"}},
	{4, 2,  {"w", "w", "W"}},
	{5, 6,  {"x", "x", "X", "ks", "iks", "kz", "xz"}},
	{3, 2,  {"y", "y", "Y"}},
	{2, 9,  {"z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ", "Z"}}
    };

    char buf[1024];		/* this should be enough (?) */
    char temp;
    int pos = 0;
    int randomnum;

    if (IS_NPC(ch))
	return (string);

    if (ch->pcdata->condition[COND_DRUNK] > 0) {	/* character is drunk */
	do {
	    temp = toupper(*string);
	    if ((temp >= 'A') && (temp <= 'Z')) {
		if (ch->pcdata->condition[COND_DRUNK] > drunk[(temp - 'A')].min_drunk_level) {
		    randomnum = number_range(0, (drunk[(temp - 'A')].number_of_rep));
		    strcpy(&buf[pos], drunk[(temp - 'A')].replacement[randomnum]);
		    pos += strlen(drunk[(temp - 'A')].replacement[randomnum]);
		} else
		    buf[pos++] = *string;
	    } else {
		if ((temp >= '0') && (temp <= '9')) {
		    temp = '0' + number_range(0, 9);
		    buf[pos++] = temp;
		} else
		    buf[pos++] = *string;
	    }
	}
	while (*string++);

	buf[pos] = '\0';	/* Mark end of the string... */
	strcpy(string, buf);
	return (string);
    }
    return (string);		/* character is not drunk, just return the string */
}

void do_say(CHAR_DATA * ch, char *argument)
{

    char say[16], buf[MAX_STRING_LENGTH], *argp;
    const char *last, *point;

    if (argument[0] == '\0') {
	send_to_char("Say what?\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    argp = strcpy(buf, argument);

    for (last = point = argp; *point; point++)
	if (!isspace(*point))
	    last = point;

    if (*(last - 1) == '{')
	last = last - 2;

    switch (*last) {
    case '?':
	strcpy(say, "ask");
	break;
    case '!':
	strcpy(say, "exclaim");
	break;
    case '.':
	strcpy(say, "state");
	break;
    case '"':
	strcpy(say, "quote");
	break;
    default:
	strcpy(say, "say");
	break;
    }

    argument = makedrunk(argument, ch);

    sprintf(buf, "You %s '{y$T{x'", say);
    act(buf, ch, NULL, argument, TO_CHAR);

    sprintf(buf, "$n %ss '{y$T{x'", say);
    act(buf, ch, NULL, argument, TO_ROOM);

    if (!IS_NPC(ch)) {
	CHAR_DATA *mob, *mob_next;
	for (mob = ch->in_room->people; mob != NULL; mob = mob_next) {
	    mob_next = mob->next_in_room;
	    if (IS_NPC(mob) && HAS_TRIGGER(mob, TRIG_SPEECH) &&
		mob->position == mob->pIndexData->default_pos)
		mp_act_trigger(argument, mob, ch, NULL, NULL, TRIG_SPEECH);
	}
    }
    return;
}

void do_shout(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_SHOUTSOFF)) {
	    send_to_char("You can hear shouts again.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_SHOUTSOFF);
	} else {
	    send_to_char("You will no longer hear shouts.\n\r", ch);
	    SET_BIT(ch->comm, COMM_SHOUTSOFF);
	}
	return;
    }
    if (IS_SET(ch->comm, COMM_NOSHOUT)) {
	send_to_char("You can't shout.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    REMOVE_BIT(ch->comm, COMM_SHOUTSOFF);

    WAIT_STATE(ch, 12);

    argument = makedrunk(argument, ch);
    act("You shout '{r$T{x'", ch, NULL, argument, TO_CHAR);
    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *victim;
	victim = d->original ? d->original : d->character;

	if (victim == NULL)
	    continue;

	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    d->character->in_room->area == ch->in_room->area &&
	    !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
	    !IS_SET(victim->comm, COMM_QUIET)) {
	    act("$n shouts '{r$t{x'", ch, argument, d->character, TO_VICT);
	}
    }
}

void do_tell(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], pers_buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm, COMM_DEAF)) {
	send_to_char("Your message didn't get through.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_QUIET)) {
	send_to_char("You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_DEAF) && !IS_IMMORTAL(ch)) {
	send_to_char("You must turn off deaf mode first.\n\r", ch);
	return;
    }
    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0') {
	send_to_char("Tell whom what?\n\r", ch);
	return;
    }
    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */

    argument = makedrunk(argument, ch);
    if ((victim = get_char_world(ch, arg)) == NULL || (IS_NPC(victim) && victim->in_room != ch->in_room)) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if ((GET_SIDE(victim) != GET_SIDE(ch)) && (victim->level < LEVEL_IMMORTAL && ch->level < LEVEL_IMMORTAL)) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim->desc == NULL && !IS_NPC(victim)) {
	act("$N seems to have misplaced $S link...try again later.", ch, NULL, victim, TO_CHAR);
	PERS(ch, victim, pers_buf);
	sprintf(buf, "%s tells you '{B%s{x'\n\r", pers_buf, argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer, buf);
	return;
    }
   
if (victim == ch){
	act("You can't tell yourself!\n\r", ch, 0, victim, TO_CHAR);
	return;
    }
   
   
   if ((IS_SET(victim->comm, COMM_QUIET) || IS_SET(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch)) {
	act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
	return;
    }
    if (IS_SET(victim->comm, COMM_AFK)) {
	if (IS_NPC(victim)) {
	    act("$E is {g[AFK]{x, and not receiving tells.", ch, NULL, victim, TO_CHAR);
	    return;
	}
	act("$E is {g[AFK]{x, but your tell will go through when $E returns.", ch, NULL, victim, TO_CHAR);
	PERS(ch, victim, pers_buf);
	sprintf(buf, "%s tells you '{B%s{x'\n\r", pers_buf, argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer, buf);
	return;
    }
    act_new("You tell $N '{B$t{x'", ch, argument, victim, TO_CHAR, POS_DEAD);
    act_new("$n tells you '{B$t{x'", ch, argument, victim, TO_VICT, POS_DEAD);
    victim->reply = ch;

    if (!IS_NPC(ch) && IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_SPEECH))
	mp_act_trigger(argument, victim, ch, NULL, NULL, TRIG_SPEECH);

    return;
}

void do_reply(CHAR_DATA * ch, char *argument)
{
   CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH], pers_buf[MAX_STRING_LENGTH];
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOTELL)) {
	send_to_char("Your message didn't get through.\n\r", ch);
	return;
    }
    if ((victim = ch->reply) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    argument = makedrunk(argument, ch);
    if (victim->desc == NULL && !IS_NPC(victim)) {
	act("$N seems to have misplaced $S link...try again later.", ch, NULL, victim, TO_CHAR);
	PERS(ch, victim, pers_buf);
	sprintf(buf, "%s tells you '{B%s{x'\n\r", pers_buf, argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer, buf);
	return;
    }
    if ((IS_SET(victim->comm, COMM_QUIET) || IS_SET(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim)) {
	act_new("$E is not receiving tells.", ch, 0, victim, TO_CHAR, POS_DEAD);
	return;
    }
    if (IS_SET(victim->comm, COMM_AFK)) {
	if (IS_NPC(victim)) {
	    act_new("$E is {g[AFK]{x, and not receiving tells.", ch, NULL, victim, TO_CHAR, POS_DEAD);
	    return;
	}
	act_new("$E is {g[AFK]{x, but your tell will go through when $E returns.", ch, NULL, victim, TO_CHAR, POS_DEAD);
	PERS(ch, victim, pers_buf);
	sprintf(buf, "%s tells you '{B%s{x'\n\r", pers_buf, argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer, buf);
	return;
    }
    act_new("You reply $N '{B$t{x'", ch, argument, victim, TO_CHAR, POS_DEAD);
    act_new("$n replies '{B$t{x'", ch, argument, victim, TO_VICT, POS_DEAD);
    victim->reply = ch;

    return;
}

void do_yell(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOSHOUT)) {
	send_to_char("You can't yell.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Yell what?\n\r", ch);
	return;
    }
    argument = makedrunk(argument, ch);
    act("You yell '{R$t{x'", ch, argument, NULL, TO_CHAR);
    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING &&
	    d->character != ch &&
	    d->character->in_room != NULL &&
	    d->character->in_room->area == ch->in_room->area &&
	    !IS_SET(d->character->comm, COMM_QUIET)) {
	    act("$n yells '{R $t{x'", ch, argument, d->character, TO_VICT);
	}
    }
    return;
}

void do_noadvice(CHAR_DATA * ch)
{
    if (IS_SET(ch->comm, COMM_NOADVICE)) {
	send_to_char("Advice channel is now {gON{x.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_NOADVICE);
    } else {
	send_to_char("Advice channel is now {rOFF{x.\n\r", ch);
	SET_BIT(ch->comm, COMM_NOADVICE);
    }
}

void do_advice(char *argument)
{
    char buf[MAX_STRING_LENGTH];

    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d != NULL; d = d->next) {
	if (d->connected == CON_PLAYING && d->character->in_room != NULL && !IS_SET(d->character->comm, COMM_NOADVICE)) {
	    sprintf(buf, "{m(Advice: {w%s {m){x\n\r", argument);
	    send_to_char(buf, d->character);
	}
    }
    return;
}

void do_emote(CHAR_DATA * ch, char *argument)
{
    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE)) {
	send_to_char("You can't show your emotions.\n\r", ch);
	return;
    }
    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Emote what?\n\r", ch);
	return;
    }
    MOBtrigger = FALSE;
    act("$n $T", ch, NULL, argument, TO_ROOM);
    act("$n $T", ch, NULL, argument, TO_CHAR);
    MOBtrigger = TRUE;
    return;
}

void do_pmote(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *vch;
    char *letter, *name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE)) {
	send_to_char("You can't show your emotions.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	send_to_char("Your channel privileges have been revoked.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	send_to_char("Emote what?\n\r", ch);
	return;
    }
    act("$n $t", ch, argument, NULL, TO_CHAR);

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
	if (vch->desc == NULL || vch == ch)
	    continue;

	if ((letter = strstr(argument, vch->name)) == NULL) {
	    MOBtrigger = FALSE;
	    act("$N $t", vch, argument, ch, TO_CHAR);
	    MOBtrigger = TRUE;
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

	MOBtrigger = FALSE;
	act("$N $t", vch, temp, ch, TO_CHAR);
	MOBtrigger = TRUE;
    }

    return;
}

/*
 * All the posing stuff.
 */

struct pose_table_type {
    char *message[2 * MAX_CLASS];
};

const struct pose_table_type pose_table[] =
{
    {
	{
	    "You sizzle with energy.",
	    "$n sizzles with energy.",
	    "You feel very holy.",
	    "$n looks very holy.",
	    "You perform a small card trick.",
	    "$n performs a small card trick.",
	    "You show your bulging muscles.",
	    "$n shows $s bulging muscles."
	}
    },

    {
	{
	    "You turn into a butterfly, then return to your normal shape.",
	    "$n turns into a butterfly, then returns to $s normal shape.",
	    "You nonchalantly turn wine into water.",
	    "$n nonchalantly turns wine into water.",
	    "You wiggle your ears alternately.",
	    "$n wiggles $s ears alternately.",
	    "You crack nuts between your fingers.",
	    "$n cracks nuts between $s fingers."
	}
    },

    {
	{
	    "Blue sparks fly from your fingers.",
	    "Blue sparks fly from $n's fingers.",
	    "A halo appears over your head.",
	    "A halo appears over $n's head.",
	    "You nimbly tie yourself into a knot.",
	    "$n nimbly ties $mself into a knot.",
	    "You grizzle your teeth and look mean.",
	    "$n grizzles $s teeth and looks mean."
	}
    },

    {
	{
	    "Little red lights dance in your eyes.",
	    "Little red lights dance in $n's eyes.",
	    "You recite words of wisdom.",
	    "$n recites words of wisdom.",
	    "You juggle with daggers, apples, and eyeballs.",
	    "$n juggles with daggers, apples, and eyeballs.",
	    "You hit your head, and your eyes roll.",
	    "$n hits $s head, and $s eyes roll."
	}
    },

    {
	{
	    "A slimy green monster appears before you and bows.",
	    "A slimy green monster appears before $n and bows.",
	    "Deep in prayer, you levitate.",
	    "Deep in prayer, $n levitates.",
	    "You steal the underwear off every person in the room.",
	    "Your underwear is gone!  $n stole it!",
	    "Crunch, crunch -- you munch a bottle.",
	    "Crunch, crunch -- $n munches a bottle."
	}
    },

    {
	{
	    "You turn everybody into a little pink elephant.",
	    "You are turned into a little pink elephant by $n.",
	    "An angel consults you.",
	    "An angel consults $n.",
	    "The dice roll ... and you win again.",
	    "The dice roll ... and $n wins again.",
	    "... 98, 99, 100 ... you do pushups.",
	    "... 98, 99, 100 ... $n does pushups."
	}
    },

    {
	{
	    "A small ball of light dances on your fingertips.",
	    "A small ball of light dances on $n's fingertips.",
	    "Your body glows with an unearthly light.",
	    "$n's body glows with an unearthly light.",
	    "You count the money in everyone's pockets.",
	    "Check your money, $n is counting it.",
	    "Arnold Schwarzenegger admires your physique.",
	    "Arnold Schwarzenegger admires $n's physique."
	}
    },

    {
	{
	    "Smoke and fumes leak from your nostrils.",
	    "Smoke and fumes leak from $n's nostrils.",
	    "A spot light hits you.",
	    "A spot light hits $n.",
	    "You balance a pocket knife on your tongue.",
	    "$n balances a pocket knife on your tongue.",
	    "Watch your feet, you are juggling granite boulders.",
	    "Watch your feet, $n is juggling granite boulders."
	}
    },

    {
	{
	    "The light flickers as you rap in magical languages.",
	    "The light flickers as $n raps in magical languages.",
	    "Everyone levitates as you pray.",
	    "You levitate as $n prays.",
	    "You produce a coin from everyone's ear.",
	    "$n produces a coin from your ear.",
	    "Oomph!  You squeeze water out of a granite boulder.",
	    "Oomph!  $n squeezes water out of a granite boulder."
	}
    },

    {
	{
	    "Your head disappears.",
	    "$n's head disappears.",
	    "A cool breeze refreshes you.",
	    "A cool breeze refreshes $n.",
	    "You step behind your shadow.",
	    "$n steps behind $s shadow.",
	    "You pick your teeth with a spear.",
	    "$n picks $s teeth with a spear."
	}
    },

    {
	{
	    "A fire elemental singes your hair.",
	    "A fire elemental singes $n's hair.",
	    "The sun pierces through the clouds to illuminate you.",
	    "The sun pierces through the clouds to illuminate $n.",
	    "Your eyes dance with greed.",
	    "$n's eyes dance with greed.",
	    "Everyone is swept off their foot by your hug.",
	    "You are swept off your feet by $n's hug."
	}
    },

    {
	{
	    "The sky changes color to match your eyes.",
	    "The sky changes color to match $n's eyes.",
	    "The ocean parts before you.",
	    "The ocean parts before $n.",
	    "You deftly steal everyone's weapon.",
	    "$n deftly steals your weapon.",
	    "Your karate chop splits a tree.",
	    "$n's karate chop splits a tree."
	}
    },

    {
	{
	    "The stones dance to your command.",
	    "The stones dance to $n's command.",
	    "A thunder cloud kneels to you.",
	    "A thunder cloud kneels to $n.",
	    "The Grey Mouser buys you a beer.",
	    "The Grey Mouser buys $n a beer.",
	    "A strap of your armor breaks over your mighty thews.",
	    "A strap of $n's armor breaks over $s mighty thews."
	}
    },

    {
	{
	    "The heavens and grass change colour as you smile.",
	    "The heavens and grass change colour as $n smiles.",
	    "The Burning Man speaks to you.",
	    "The Burning Man speaks to $n.",
	    "Everyone's pocket explodes with your fireworks.",
	    "Your pocket explodes with $n's fireworks.",
	    "A boulder cracks at your frown.",
	    "A boulder cracks at $n's frown."
	}
    },

    {
	{
	    "Everyone's clothes are transparent, and you are laughing.",
	    "Your clothes are transparent, and $n is laughing.",
	    "An eye in a pyramid winks at you.",
	    "An eye in a pyramid winks at $n.",
	    "Everyone discovers your dagger a centimeter from their eye.",
	    "You discover $n's dagger a centimeter from your eye.",
	    "Mercenaries arrive to do your bidding.",
	    "Mercenaries arrive to do $n's bidding."
	}
    },

    {
	{
	    "A black hole swallows you.",
	    "A black hole swallows $n.",
	    "Valentine Michael Smith offers you a glass of water.",
	    "Valentine Michael Smith offers $n a glass of water.",
	    "Where did you go?",
	    "Where did $n go?",
	    "Four matched Percherons bring in your chariot.",
	    "Four matched Percherons bring in $n's chariot."
	}
    },

    {
	{
	    "The world shimmers in time with your whistling.",
	    "The world shimmers in time with $n's whistling.",
	    "The great god Mota gives you a staff.",
	    "The great god Mota gives $n a staff.",
	    "Click.",
	    "Click.",
	    "Atlas asks you to relieve him.",
	    "Atlas asks $n to relieve him."
	}
    }
};

void do_pose(CHAR_DATA * ch, char *argument)
{
    int level;
    int pose;

    if (IS_NPC(ch))
	return;

    level = UMIN(ch->level, sizeof(pose_table) / sizeof(pose_table[0]) - 1);
    pose = number_range(0, level);

    act(pose_table[pose].message[2 * ch->class + 0], ch, NULL, NULL, TO_CHAR);
    act(pose_table[pose].message[2 * ch->class + 1], ch, NULL, NULL, TO_ROOM);

    return;
}

void do_bug(CHAR_DATA * ch, char *argument)
{
    append_file(ch, BUG_FILE, argument);
    send_to_char("{WBug logged.  Thank you!{x\n\r", ch);
    return;
}

void do_typo(CHAR_DATA * ch, char *argument)
{
    append_file(ch, TYPO_FILE, argument);
    send_to_char("{WTypo logged.  Thank you!{x\n\r", ch);
    return;
}

void do_rent(CHAR_DATA * ch, char *argument)
{
    send_to_char("There is no rent here.  Just save and quit.\n\r", ch);
    return;
}

void do_qui(CHAR_DATA * ch, char *argument)
{
    send_to_char("If you want to QUIT, you have to spell it out.\n\r", ch);
    return;
}

void do_quit(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d, *d_next;
    CHAR_DATA *och, *och_next;
    int id;

    if (ch == NULL)
	return;

    if (IS_NPC(ch))
	return;

    /*
     *  Caught some people using different flags and situations to 
     *  use the delete command to thier advantage.  Now, delete delete 
     *  should actually delete the char no matter what :)
     */

    if (!ch->pcdata->confirm_delete) {
	if ((ch->position == POS_FIGHTING || ch->fighting != NULL) && !(armageddon)) {
	    send_to_char("No way! You are fighting.\n\r", ch);
	    return;
	}
	if ((ch->pet != NULL) && (ch->pet->position == POS_FIGHTING) && !(armageddon)) {
	    send_to_char("No way!  Your pet is in danger!\n\r", ch);
	    return;
	}
if (!IS_IMMORTAL(ch))

if (IS_AFFECTED(ch, AFF_NOQUIT) && !(armageddon)) {
	    send_to_char("You have recently engaged in combat with players.\n\rYou cannot quit yet.\n\r", ch);
	    return;
	} 


/*      {
	    OBJ_DATA *obj;
	    char buf[MAX_STRING_LENGTH];

	    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
		if (obj == NULL)
		    continue;

		if (IS_OBJ_STAT(obj, ITEM_NOBANK)) {
		    sprintf(buf, "The item %s is an ancient relic. You can't quit while carrying it. Drop it first!\n\r",
			    obj->short_descr);
		    send_to_char(buf, ch);
		    return;
		}
	    }
	}
*/

	if (ch->position < POS_STUNNED && !(armageddon)) {
	    send_to_char("You're not DEAD yet.\n\r", ch);
	    return;
	}
    }
   stop_follower(ch);
    send_to_char("\n\r{WThanks for playing Act of War, Cya again soon!!.{x\n\r", ch);
    act("$n has left the game.", ch, NULL, NULL, TO_ROOM);
    sprintf(log_buf, "%s has quit.", ch->name);
    log_string(log_buf);
    wiznet("$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0, get_trust(ch));

    ch->pcdata->confirm_delete = FALSE;
    if (ch->level > 1)
	save_char_obj(ch);

    for (och = ch->in_room->people; och != NULL; och = och_next) {
        och_next = och->next_in_room;
        if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch) {
            stop_follower(och);
        }
    }

    /*
     * After extract_char the ch is no longer valid!
     */

    /*
     * Added new position (POS_QUITTING) to take care of some bugs.  Since char 
     * is already saaved, this shouldn't hurt anything.
     */

    ch->position = POS_QUITTING;

    id = ch->id;
    d = ch->desc;
    extract_char(ch, TRUE);
    if (d != NULL)
	close_socket(d);

    /* toast evil cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next) {
	CHAR_DATA *tch;

	d_next = d->next;
	tch = d->original ? d->original : d->character;
	if (tch && tch->id == id) {
	    extract_char(tch, TRUE);
	    close_socket(d);
	}
    }
    return;
}

void do_save(CHAR_DATA * ch, char *argument)
{
   if (armageddon)
   return;
    if (IS_NPC(ch))
	return;

   if (ch->level <= 1) {
	send_to_char("You must be at least 2 level to save.\n\r", ch);
	return;
    } else
	save_char_obj(ch);

    send_to_char("Saving.  Remember that we have automatic saving now.\n\r", ch);
    return;
}

void do_follow(CHAR_DATA * ch, char *argument)
{
    /* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Follow whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
//    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL) {
//	act("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
//	return;
//    }

   if (victim == ch) 
	{
	if (ch->following == NULL) 
	{
	    send_to_char("You already follow yourself.\n\r", ch);
	    return;
	}

/*	if (IS_AFFECTED(ch, AFF_NOQUIT)) { // Izlude, setting up new group system
	
	send_to_char("You cannot follow yourself while noquit.\n\r",ch);
	return;
}*/ 
	stop_follower(ch);
	return;
    }
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOFOLLOW) && !IS_IMMORTAL(ch)) {
	act("$N doesn't seem to want any followers.\n\r",
	    ch, NULL, victim, TO_CHAR);
	return;
    }
    REMOVE_BIT(ch->act, PLR_NOFOLLOW);

    if (ch->following != NULL)
//	stop_follower(ch);

    if ((!IS_NPC(ch) && !IS_NPC(victim)) && (GET_SIDE(ch) != GET_SIDE(victim))) {
	send_to_char("You cannot follow the enemy!\n\r", ch);
	return;
    }
    add_follower(ch, victim);
    return;

}

void add_follower(CHAR_DATA * ch, CHAR_DATA * master)
{
//    if (ch->master != NULL) {
//	bug("Add_follower: non-null master.", 0);
//	return;
//    }
    ch->following = master;
//    ch->leader = master;

    if (can_see(master, ch))
	act("$n now follows you.", ch, NULL, master, TO_VICT);

    if(IS_NPC(ch))
        ch->side = master->side;

    act("You now follow $N.", ch, NULL, master, TO_CHAR);

    return;
}

void stop_follower(CHAR_DATA * ch)
{
    if (ch->following == NULL) 
	{
	bug("Stop_follower: null master.", 0);
//	return;
    }

    if (IS_AFFECTED(ch, AFF_CHARM)) 
	{
	REMOVE_BIT(ch->affected_by, AFF_CHARM);
	affect_strip(ch, gsn_charm_person);
    }
	if (ch->following)
    if (can_see(ch->following, ch) && ch->in_room != NULL) 
	{
	act("$n stops following you.", ch, NULL, ch->following, TO_VICT);
	act("You stop following $N.", ch, NULL, ch->following, TO_CHAR);
    }
	if (ch->master)
	    if (ch->master->pet == ch)
			ch->master->pet = NULL;

    ch->master = NULL;
    ch->following = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void nuke_pets(CHAR_DATA * ch)
{
    CHAR_DATA *pet;

    if ((pet = ch->pet) != NULL) {
	stop_follower(pet);
	if (pet->in_room != NULL)
	    act("$N slowly fades away.", ch, NULL, pet, TO_NOTVICT);
	extract_char(pet, TRUE);
    }
    ch->pet = NULL;

    if (ch->mount && (ch->mount->in_room == ch->in_room || ch->mount->in_room == NULL)) {
	pet = ch->mount;
	do_dismount(ch, "");
	if (pet->in_room != NULL)
	    act("$N slowly fades away.", ch, NULL, pet, TO_NOTVICT);
	else
	    log_string("void nuke_pets: Extracting null pet");
	ch->mount = NULL;
	ch->riding = FALSE;
	extract_char(pet, TRUE);
    } else if (ch->mount) {
	ch->mount->mount = NULL;
	ch->mount->riding = FALSE;
    }
    ch->mount = NULL;

    return;
}

void die_follower(CHAR_DATA * ch)
{
   CHAR_DATA *fch;

    if (ch->master != NULL) {
	if (ch->master->pet == ch)
	    ch->master->pet = NULL;
	stop_follower(ch);
    }

    for (fch = char_list; fch != NULL; fch = fch->next) {
	if (fch->master == ch)
	  stop_follower(fch);
	if (fch->leader == ch)
	    fch->leader = fch;
if (fch->following == ch)
	fch->following = fch;
    }

    ch->master = NULL;
    ch->leader = ch;
   ch->following = NULL;

    return;
}

void do_order(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument(argument, arg);
    one_argument(argument, arg2);

    if (!str_cmp(arg2, "delete")   || 
       !str_cmp(arg2, "mob")      ||
       !str_cmp(arg2, "shutdown") ||
        !str_cmp(arg2, "reboot")   ||
        !str_cmp(arg2, "purge")) {

	send_to_char("That will {RNOT{x be done.\n\r", ch);
	return;
    }
    if (arg[0] == '\0' || argument[0] == '\0') {
	send_to_char("Order whom to do what?\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CHARM)) {
	send_to_char("You feel like taking, not giving, orders.\n\r", ch);
	return;
    }
    if (!str_cmp(arg, "all")) {
	fAll = TRUE;
	victim = NULL;
    } else {
	fAll = FALSE;

	if (!strcmp(arg, "mount")) {
	    if (!ch->mount) {
		send_to_char("You don't have a mount.\n\r", ch);
		return;
	    }
	    if (ch->mount->in_room != ch->in_room) {
		send_to_char("Your mount isn't here!\n\r", ch);
		return;
	    } else {
		victim = ch->mount;
	    }
	} else if ((victim = get_char_room(ch, arg)) == NULL) {
	    send_to_char("They aren't here.\n\r", ch);
	    return;
	}
	if (victim == ch) {
	    send_to_char("Aye aye, right away!\n\r", ch);
	    return;
	}
	if (victim->mount == ch) {
	    /*  if ( !mount_success(ch, victim, FALSE) )
	       { */
	    act("$N ignores your orders.", ch, NULL, victim, TO_CHAR);
	    return;
	    /*
	       else
	       {
	       sprintf(buf,"%s orders you to \'%s\'.", ch->name, argument);
	       send_to_char(buf, victim);
	       interpret( victim, argument );
	       return;
	       } 
	     */
	} else if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch
		   || (IS_IMMORTAL(victim) && victim->trust >= ch->trust)) {
	    send_to_char("Do it yourself!\n\r", ch);
	    return;
	}
    }

    found = FALSE;
    for (och = ch->in_room->people; och != NULL; och = och_next) {
	och_next = och->next_in_room;

	if (IS_AFFECTED(och, AFF_CHARM)
	    && och->master == ch
	    && (fAll || och == victim)) {
	    found = TRUE;
	    sprintf(buf, "$n orders you to '%s'.", argument);
	    act(buf, ch, NULL, och, TO_VICT);
	    interpret(och, argument);
	}
    }

    if (found) {
if (!IS_IMMORTAL(ch))

WAIT_STATE(ch, PULSE_VIOLENCE / 2);

send_to_char("Your order was received.\n\r", ch);
    } else
	send_to_char("You have no followers here.\n\r", ch);
    return;
}

void do_group(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *gch;
    int count;
    int ratio1 = 0;
    int ratio2 = 0;
    int ratio3 = 0;
    one_argument(argument, arg);

    if (arg[0] == '\0') 
	{
		CHAR_DATA *gch;
		CHAR_DATA *leader;
		//char *clan = "None";

	leader = (ch->leader != NULL) ? ch->leader : ch;
		PERS(leader, ch, pers_buf);
		sprintf(buf, "%s's group:\n\r", pers_buf);
		send_to_char(buf, ch);

		for (gch = char_list; gch != NULL; gch = gch->next) 
		{
		    if (is_same_group(gch, ch) && (gch->incog_level < ch->level || ch == gch)) 
			{
				if (gch->max_hit != 0)
					ratio1 = 100 * gch->hit / gch->max_hit;
				if (gch->max_mana != 0)
					ratio2 = 100 * gch->mana / gch->max_mana;
				if (gch->max_move != 0)
					ratio3 = 100 * gch->move / gch->max_move;

				PERS(gch, ch, pers_buf);
				sprintf(buf,
					"[%2d %s] %-16s %s%4d{x/%4d hp %s%4d{x/%4d mana %s%4d{x/%4d mv %5d {WTNL{x\n\r",
					gch->level,
					IS_NPC(gch) ? "Mob" : class_table[gch->class].who_name,
					capitalize(pers_buf),
					ratio1 == 100 ? "{G" : ratio1 > 75 ? "{g" : ratio1 > 25 ? "{Y" : ratio1 >= 0 ? "{r" : "{R",
					gch->hit, gch->max_hit,
					ratio2 == 100 ? "{G" : ratio2 > 75 ? "{g" : ratio2 > 25 ? "{Y" : ratio2 >= 0 ? "{r" : "{R",
					gch->mana, gch->max_mana,
					ratio3 == 100 ? "{G" : ratio3 > 75 ? "{g" : ratio3 > 25 ? "{Y" : ratio3 >= 0 ? "{r" : "{R",
					gch->move, gch->max_move,
					IS_NPC(gch) ? gch->exp : ((gch->level + 1) * exp_per_level(gch, gch->pcdata->points) - gch->exp));//,
				
				//(!str_cmp(clan_table[gch->clan].who_name, "None")) ? " " : clan_table[gch->clan].who_name);
				send_to_char(buf, ch);
			} 
			else if (is_same_group(gch, ch)) 
			{
				PERS(gch, ch, pers_buf);
				sprintf(buf,
					"[?? %s] %-16s %4d percent hp %4d percent mana %4d mv\n\r",
					IS_NPC(gch) ? "Mob" : class_table[gch->class].who_name,
					capitalize(pers_buf),
					100 * gch->hit / gch->max_hit,
					100 * gch->mana / gch->max_mana,
					100 * gch->move / gch->max_move);
				send_to_char(buf, ch);
			}
		}
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) 
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
    }
    if (ch->following != NULL || (ch->leader != NULL && ch->leader != ch)) 
	{
		send_to_char("But you are following someone else!\n\r", ch);
		return;
    }
    if (victim->following != ch && ch != victim) 
	{
		act_new("$N isn't following you.", ch, NULL, victim, TO_CHAR, POS_SLEEPING);
		return;
    }
    if ((!IS_NPC(ch) && !IS_NPC(victim)) && (GET_SIDE(ch) != GET_SIDE(victim))) 
	{
		send_to_char("You cannot group with the enemy!\n\r", ch);
		return;
    }
    if (victim == ch)
 
   	{
    	send_to_char("You can't group yourself silly!\n\r", ch);
    	return;
   	}
   
   if (!IS_NPC(victim) && victim->level > ch->level + 20 && !(armageddon)) 
	{
		send_to_char("You are to inexperienced to lead, follow them instead.\n\r", ch);
		return;
    }
//    if (IS_AFFECTED(victim, AFF_CHARM)) 
//	{
//		send_to_char("You can't remove charmed mobs from your group.\n\r", ch);
//		return;
//    }
    if (IS_AFFECTED(ch, AFF_CHARM)) 
	{
		act_new("You like your master too much to leave $m!", ch, NULL, victim, TO_VICT, POS_SLEEPING);
		return;
    }
    if (is_same_group(victim, ch) && ch != victim) 
	{
		victim->leader = NULL;
		act_new("$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
		act_new("$n removes you from $s group.", ch, NULL, victim, TO_VICT, POS_SLEEPING);
		act_new("You remove $N from your group.", ch, NULL, victim, TO_CHAR, POS_SLEEPING);
		return;	
	
   }
    count = 0;
    for (gch = char_list; gch != NULL; gch = gch->next)
		if (is_same_group(gch, ch))
		    count++;

    if (count > MAX_GROUP_SIZE - 1 && !(armageddon)) 
	{
		send_to_char("Your group is already full!\n\r", ch);
		return;
    }
    victim->leader = ch;
    act_new("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
    act_new("You join $n's group.", ch, NULL, victim, TO_VICT, POS_SLEEPING);
    act_new("$N joins your group.", ch, NULL, victim, TO_CHAR, POS_SLEEPING);
    return;
}

/*
 * 'Split' originally by Gnort, God of Chaos.
 */

void do_split(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_gold = 0, amount_silver = 0;
    int share_gold, share_silver;
    int extra_gold, extra_silver;

    argument = one_argument(argument, arg1);
    one_argument(argument, arg2);

    if (arg1[0] == '\0') {
	send_to_char("Split how much?\n\r", ch);
	return;
    }
    amount_silver = atoi(arg1);

    if (arg2[0] != '\0')
	amount_gold = atoi(arg2);

    if (amount_gold < 0 || amount_silver < 0) {
	send_to_char("Your group wouldn't like that.\n\r", ch);
	return;
    }
    if (amount_gold == 0 && amount_silver == 0) {
	send_to_char("You hand out zero coins, but no one notices.\n\r", ch);
	return;
    }
    if (ch->gold < amount_gold || ch->silver < amount_silver) {
	send_to_char("You don't have that much to split.\n\r", ch);
	return;
    }
    members = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM))
	    members++;
    }

    if (members < 2) {
	send_to_char("Just keep it all.\n\r", ch);
	return;
    }
    share_silver = amount_silver / members;
    extra_silver = amount_silver % members;

    share_gold = amount_gold / members;
    extra_gold = amount_gold % members;

    if (share_gold == 0 && share_silver == 0) {
	send_to_char("Don't even bother, cheapskate.\n\r", ch);
	return;
    }
    ch->silver -= amount_silver;
    ch->silver += share_silver + extra_silver;
    ch->gold -= amount_gold;
    ch->gold += share_gold + extra_gold;

    if (share_silver > 0) {
	sprintf(buf,
		"You split {W%d{x silver coins. Your share is {W%d{x silver.\n\r",
		amount_silver, share_silver + extra_silver);
	send_to_char(buf, ch);
    }
    if (share_gold > 0) {
	sprintf(buf,
		"You split {W%d{x gold coins. Your share is {W%d{x gold.\n\r",
		amount_gold, share_gold + extra_gold);
	send_to_char(buf, ch);
    }
    if (share_gold == 0) {
	sprintf(buf, "$n splits {W%d{x silver coins. Your share is {W%d{x silver.",
		amount_silver, share_silver);
    } else if (share_silver == 0) {
	sprintf(buf, "$n splits {W%d{x gold coins. Your share is {W%d{x gold.",
		amount_gold, share_gold);
    } else {
	sprintf(buf, "$n splits {W%d{x silver and {W%d{x gold coins, giving you {W%d{x silver and {W%d{x gold.\n\r",
		amount_silver, amount_gold, share_silver, share_gold);
    }

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (gch != ch && is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM)) {
	    act(buf, ch, NULL, gch, TO_VICT);
	    gch->gold += share_gold;
	    gch->silver += share_silver;
	}
    }
    return;
}

void do_gtell(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    if (argument[0] == '\0') {
	send_to_char("Tell your group what?\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm, COMM_NOTELL)) {
	send_to_char("Your message didn't get through!\n\r", ch);
	return;
    }
    sprintf(buf, "You tell the group '{G%s{x'\n\r", argument);
    send_to_char(buf, ch);

    for (gch = char_list; gch != NULL; gch = gch->next) {
	if (is_same_group(gch, ch)) {
	    act_new("$n tells the group '{G$t{x'",
		    ch, argument, gch, TO_VICT, POS_SLEEPING);

	}
    }
    return;
}

/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */

bool is_same_group(CHAR_DATA * ach, CHAR_DATA * bch)
{
    if (ach == NULL || bch == NULL)
	return FALSE;

    if (ach->leader != NULL)
	ach = ach->leader;
    if (bch->leader != NULL)
	bch = bch->leader;
    return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */

void do_colour(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg);

    if (!*arg) {
	if (!IS_SET(ch->act, PLR_COLOUR)) {
	    SET_BIT(ch->act, PLR_COLOUR);
	    ch->desc->color = TRUE;
	    send_to_char("{gColor activated.{x\n\r", ch);
	} else {
	    send_to_char_bw("Color deactivated.\n\r", ch);
	    ch->desc->color = FALSE;
	    REMOVE_BIT(ch->act, PLR_COLOUR);
	}
	return;
    } else {
	send_to_char_bw("Colour Configuration is unavailable in this\n\r", ch);
	send_to_char_bw("version of colour, sorry\n\r", ch);
    }
    return;
}

/*
 * Local functions.
 */

FILE *popen args((const char *command, const char *type));
int pclose args((FILE * stream));
char *fgetf args((char *s, int n, register FILE * iop));

void do_pipe(CHAR_DATA * ch, char *argument)
{
    char buf[5000];
    FILE *fp;
    fp = popen(argument, "r");

    fgetf(buf, 5000, fp);

    page_to_char(buf, ch);

    pclose(fp);

    return;
}

char *fgetf(char *s, int n, register FILE * iop)
{
    register int c;
    register char *cs;

    c = '\0';
    cs = s;
    while (--n > 0 && (c = getc(iop)) != EOF)
	if ((*cs++ = c) == '\0')
	    break;
    *cs = '\0';
    return ((c == EOF && cs == s) ? NULL : s);
}

/*
 * I don't think ROM muds have this particular function, if you do...
 * just don't include it :)
 */

CHAR_DATA *get_char(CHAR_DATA * ch)
{
    if (!ch->pcdata)
	return ch->desc->original;
    else
	return ch;
}

/*
 * Copywrite 1996 by Amadeus of AnonyMUD, AVATAR, Horizon MUD, and Despair
 *                      ( amadeus@myself.com )
 *
 * Public use authorized with this header intact.
 */

void do_wpeace(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *rch;
    char buf[MAX_STRING_LENGTH];

    rch = get_char(ch);

    for (rch = char_list; rch; rch = rch->next) {
	if (ch->desc == NULL || ch->desc->connected != CON_PLAYING)
	    continue;

	if (rch->fighting) {
	    sprintf(buf, "%s has declared Peace among the kingdoms.\n\r", ch->name);
	    send_to_char(buf, rch);
	    stop_fighting(rch, TRUE);
	}
    }

    send_to_char("You have declared Peace among the kingdoms.\n\r", ch);
    return;
}

/* -----------------------------------------------------------------------
   The following snippet was written by Gary McNickle (dharvest) for
   Rom 2.4 specific MUDs and is released into the public domain. My thanks to
   the originators of Diku, and Rom, as well as to all those others who have
   released code for this mud base.  Goes to show that the freeware idea can
   actually work. ;)  In any case, all I ask is that you credit this code
   properly, and perhaps drop me a line letting me know it's being used.

   from: gary@dharvest.com
   website: http://www.dharvest.com
   or http://www.dharvest.com/resource.html (rom related)

   Send any comments, flames, bug-reports, suggestions, requests, etc... 
   to the above email address.
   ----------------------------------------------------------------------- */

/*** pload.v1 
Loads your players while they are away so that you can take a look at / modify
them as needed.  This code was inspired by the note "pload.txt" written by 
Erwin S. Andreasen (erwin@pip.dknet.dk) */

/** Installation Instructions 
Simple, save this file as "pload.c" or cut/paste these functions into
act_wiz.c and add "pload" and "punload" to your interp.c & interp.h files.

Note: I highly recommend you only allow your most trusted imms to use
these functions as they could easily be abused.
*/

/** Function: do_pload
  * Descr   : Loads a player object into the mud, bringing them (and their
  *           pet) to you for easy modification.  Player must not be connected.
  *           Note: be sure to send them back when your done with them.
  * Returns : (void)
  * Syntax  : pload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void do_pload(CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA d;
    bool isChar = FALSE;
    char name[MAX_INPUT_LENGTH];

    if (argument[0] == '\0') {
	send_to_char("Load who?\n\r", ch);
	return;
    }
    argument[0] = UPPER(argument[0]);
    argument = one_argument(argument, name);

    /* Dont want to load a second copy of a player who's allready online! */
    if (get_player_world(ch, name) != NULL) {
	send_to_char("That person is already connected!\n\r", ch);
	return;
    }

    d.host = "{RPLOADED{x";

    isChar = load_char_obj(&d, name);	/* char pfile exists? */

    if (!isChar) {
	send_to_char("Load Who? Are you sure? I cant seem to find them.\n\r", ch);
	return;
    }
    d.character->desc = NULL;
    d.character->next = char_list;
    char_list = d.character;
    d.connected = CON_PLAYING;
    reset_char(d.character);

    /* bring player to imm */
    if (d.character->in_room != NULL) {
char_to_room(d.character, ch->in_room);		/* put in room imm is in */
   }
    act("$n has pulled $N from the pattern!",
	ch, NULL, d.character, TO_ROOM);

    if (d.character->pet != NULL) {
char_to_room(d.character->pet, d.character->in_room);
act("$n has entered the game.", d.character->pet, NULL, NULL, TO_ROOM);
   }
}

/** Function: do_punload
  * Descr   : Returns a player, previously 'ploaded' back to the void from
  *           whence they came.  This does not work if the player is actually 
  *           connected.
  * Returns : (void)
  * Syntax  : punload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_punload(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char who[MAX_INPUT_LENGTH];

    argument = one_argument(argument, who);

    if ((victim = get_player_world(ch, who)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
  /** Person is legitametly logged on... was not ploaded.
   */
    if (victim->desc != NULL) {
	send_to_char("I dont think that would be a good idea...\n\r", ch);
	return;
    }
    if (victim->was_in_room != NULL) {	/* return player and pet to orig room */
char_to_room(victim, victim->was_in_room);
	if (victim->pet != NULL)
	    char_to_room(victim->pet, victim->was_in_room);
    }
    save_char_obj(victim);
    do_quit(victim, "");

    act("$n has released $N back to the Pattern.",
	ch, NULL, victim, TO_ROOM);
}

void do_avator(CHAR_DATA * ch, char *argument)
{				/* Procedure Avator *//* Declaration */
    char buf[MAX_STRING_LENGTH];	/* buf */
    char arg1[MAX_INPUT_LENGTH];	/* arg1 */
    int level;			/* level */
    int iLevel;			/* ilevel */

    argument = one_argument(argument, arg1);
/* Check statements */
    if (arg1[0] == '\0' || !is_number(arg1)) {
	send_to_char("Syntax: avator <level>.\n\r", ch);
	return;
    }
    if (IS_NPC(ch)) {
	send_to_char("Not on NPC's.\n\r", ch);
	return;
    }
    if ((level = atoi(arg1)) < 1 || level > MAX_LEVEL) {
	sprintf(buf, "Level must be 1 to %d.\n\r", MAX_LEVEL);
	send_to_char(buf, ch);
	return;
    }
    if (level > get_trust(ch)) {
	send_to_char("Limited to your trust level.\n\r", ch);
	sprintf(buf, "Your Trust is %d.\n\r", ch->trust);
	send_to_char(buf, ch);
	return;
    }
/* Your trust stays so you will have all immortal command */
    /* SO their trust stays there */
    if (ch->trust == 0) {
	ch->trust = ch->level;
    }
/* Level gains */
    if (level <= ch->level) {
	int temp_prac;

	send_to_char("Lowering a player's level!\n\r", ch);
	send_to_char("**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", ch);
	temp_prac = ch->practice;
	ch->level = 1;
	ch->exp = exp_per_level(ch, ch->pcdata->points);
	ch->max_hit = 20;
	ch->max_mana = 100;
	ch->max_move = 100;
	ch->practice = 0;
	ch->hit = ch->max_hit;
	ch->mana = ch->max_mana;
	ch->move = ch->max_move;
	advance_level(ch, TRUE);
	ch->practice = temp_prac;
    } else {
	send_to_char("Raising a player's level!\n\r", ch);
	send_to_char("**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", ch);
    }

    for (iLevel = ch->level; iLevel < level; iLevel++) {
	ch->level += 1;
	advance_level(ch, TRUE);
    }
    sprintf(buf, "You are now level %d.\n\r", ch->level);
    send_to_char(buf, ch);
    ch->exp = exp_per_level(ch, ch->pcdata->points)
	* UMAX(1, ch->level);
    return;
}

void do_bonus(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int value;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2)) {
	send_to_char("Syntax: bonus <char> <Exp>.\n\r", ch);
	return;
    }
    if ((victim = get_player_world(ch, arg1)) == NULL) {
	send_to_char("That Player is not here.\n\r", ch);
	return;
    }
    if (IS_NPC(victim)) {
	send_to_char("Not on NPC's.\n\r", ch);
	return;
    }
    if (ch == victim) {
	send_to_char("You may not bonus yourself.\n\r", ch);
	return;
    }
    if (IS_IMMORTAL(victim) || victim->level >= LEVEL_IMMORTAL) {
	send_to_char("You can't bonus immortals silly!\n\r", ch);
	return;
    }
    value = atoi(arg2);

    if (value < -50000 || value > 50000) {
	send_to_char("Value range is -50000 to 50000.\n\r", ch);
	return;
    }
    if (value == 0) {
	send_to_char("The value must not be equal to 0.\n\r", ch);
	return;
    }
    gain_exp(victim, value);

    sprintf(buf, "You have bonused %s a whopping %d experience points.\n\r",
	    victim->name, value);
    send_to_char(buf, ch);

    if (value > 0) {
	sprintf(buf, "You have been bonused %d experience points.\n\r",
		value);
	send_to_char(buf, victim);
    } else {
	sprintf(buf, "You have been penalized %d experience points.\n\r",
		value);
	send_to_char(buf, victim);
    }

    return;
}
void do_rawquit(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg);

    if ((victim = get_player_world(ch, arg)) == NULL) {
      send_to_char("That person isn't playing.\n\r", ch);
      return;
    }
    d = victim->desc;
    send_to_char("{RYou have been raw quited by an Immortal, your char has *NOT* been saved.\n\r{x", victim);
    send_to_char("You raw quited them. Their char didn't save.\n\r{x", ch);

    extract_char(victim, TRUE);
    close_socket(d);
    return;
}

void ask_quest_question(CHAR_DATA *ch, char *argument)
{
  extern CHAR_DATA * quest_mob;
  extern CHAR_DATA * quest_target;
  extern OBJ_DATA * quest_object;
  extern sh_int  quest_timer;
  extern bool    quest;
  char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

	//easter egg :)
  if ( !str_cmp( argument, "aow is awesome" ) )
  {
	  sprintf(buf, "{y[NEWS]:{x Act Of War: %s is %s's favorite game... its way cooler than WoW!!!", AOW_VERSION, ch->name);
      stc(buf, ch);
      return;
  }

  if ( !quest || IS_NPC( ch ) )
   return;

  if (  ( !str_cmp( argument, "who is the thief?")		)
     || ( !str_cmp( argument, "who was the thief?" )		)
     || ( !str_cmp( argument, "what mob?" )			)
     || ( !str_cmp( argument, "who stole the item?" )		)  )
  {
    if ( quest_mob )
    {
      if ( quest_timer < 3 )
      {
        sprintf( buf, "I don't even know who stole it yet!" );
      }
      else
      if ( quest_object && quest_target )
      {
        sprintf( buf, "It was %s {Mwho stole %s{M.",
         quest_target->short_descr, quest_object->short_descr );
      }
    }
    else
    if ( quest_object )
    {
      sprintf( buf, "Don't worry about who stole %s{M, he got the crap beatin out of him!",
        quest_object->short_descr );
    }
    if ( quest_mob != NULL )
      do_crusade( quest_mob, buf, GET_SIDE(ch) );
    return;
  }

  if ( !str_cmp( argument, "what item?" ) )
  {
    if (quest_mob && quest_object )
    {
      sprintf(buf, "%s {Mwas stolen from me.", quest_object->short_descr );
      do_crusade( quest_mob, buf, GET_SIDE(ch) );
      return;
    }
  }



  if ( !str_cmp( argument, "where are you?" ) )
    if ( quest_mob)
    {
      sprintf( buf, "You can find me in %s{M, please hurry!!", quest_mob->in_room->name);
      do_crusade( quest_mob, buf, GET_SIDE(ch) );
      return;
    }

  if  ( !str_cmp( argument, "where is the thief?" ) )
  {
    if ( quest_mob )
    {
      if ( ( quest_target ) && ( quest_timer > 3 ) )
      {  
        if ( quest_timer < 4 )
        {
          sprintf( buf, "I don't really know where %s{M is, let me try and find out.", 
            quest_target->short_descr );
        }
        else
        if ( quest_target )
        {
          sprintf( buf, "I'm not really sure, but I THINK %s{M is in %s{x", 
           quest_target->short_descr, quest_target->in_room->name);
        }
        
      }
      else if ( ( quest_target ) && ( quest_timer <= 3 )  )
      {
        sprintf( buf, "I don't even know who stole it yet!{x" );
      }
      else
      {
        sprintf( buf, "Don't worry about where the thief who stole %s{M is, he's dead now!",
          quest_object->short_descr );
      }
      do_crusade( quest_mob, buf, GET_SIDE(ch) );
    }
  }
  return;
}

void do_gnews(CHAR_DATA * ch, char *argument)
{
   CHAR_DATA *victim;
   char buf[MSL];
   DESCRIPTOR_DATA *d;

    if (IS_NPC(ch))
    return;

  if (!IS_IMMORTAL(ch))
  {
	  stc("Huh?", ch);
	  return;
  }
 	for (d = descriptor_list; d != NULL; d = d->next) {
 	    victim = d->original ? d->original : d->character;

		//if (IS_NPC(victim))
		//return;

 	    if (victim == NULL)
 		continue;

 	    sprintf(buf, "{y[NEWS]:{x %s\n\r", argument);
 		stc( buf, victim);
 	}
	return;
}

void gnews(char *argument)
{
    char buf[MSL];
    DESCRIPTOR_DATA *d;
    
    if (argument == NULL)
	return;	
	

    
    for (d = descriptor_list; d; d = d->next) {
	if (d->character == NULL)
	    continue;
    sprintf(buf, "{y[NEWS]:{x %s{x\n\r", argument);	    
	if IS_IMMORTAL (d->character)
		{
			stc(buf, d->character);
			continue;
	}
	if ((d->connected == CON_PLAYING &&
	    !IS_NPC(d->character) &&
	    !IS_SET(d->character->comm, COMM_NONEWS))
	 && !IS_SET(d->character->comm, COMM_NONEWS))
	    stc(buf, d->character);
    }
}

void do_main_channel(CHAR_DATA *ch, char *argument)
{

	if (IS_SET(ch->comm, COMM_NOCHANNELS)) {
	    stc("Your channel privileges have been revoked.\n\r", ch);
	    return;
	}
	
 
	REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
	argument = makedrunk(argument, ch);

	switch (ch->side)
	{
		case SIDE_CAIRN:
			do_inform(ch, argument);
			break;
		case SIDE_UM:
			do_snarl(ch, argument);
			break;
		case SIDE_KAEL:
			do_bellow(ch, argument);
			break;
		case SIDE_TIER:
			do_gossip(ch, argument);
			break;
		
		default:
			stc("Huh?\n\r", ch);
			break;
	
	}

	return;
}
