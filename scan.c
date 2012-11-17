
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

char *const distance[6] =
{
    "is right here.", "is nearby.", "is not far.", "is off in the distance.", "is far off.", "is very far off."
};
char *const filler[6] =
{
    "", "{y%s{x: ", "{y%s{x: ", "{y%s{x: ", "{y%s{x: ", "{y%s{x: "
};

void scan_list args((ROOM_INDEX_DATA * scan_room, CHAR_DATA * ch,
		     sh_int depth, sh_int door));
void scan_char args((CHAR_DATA * victim, CHAR_DATA * ch,
		     sh_int depth, sh_int door));
void do_scan(CHAR_DATA * ch, char *argument)
{
    extern char *const dir_name[];
    char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *scan_room;
    EXIT_DATA *pExit;
    sh_int door, depth;

    argument = one_argument(argument, arg1);

    if (arg1[0] == '\0') {
	act("$n scans around.", ch, NULL, NULL, TO_ROOM);
	send_to_char("Scanning:\n\r", ch);
	scan_list(ch->in_room, ch, 0, -1);

	for (door = 0; door < 6; door++) {
	    if ((pExit = ch->in_room->exit[door]) != NULL)
		scan_list(pExit->u1.to_room, ch, 1, door);
	}
	return;
    } else if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
	door = 0;
    else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
	door = 1;
    else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
	door = 2;
    else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
	door = 3;
    else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
	door = 4;
    else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
	door = 5;
    else {
	send_to_char("Which way do you want to scan?\n\r", ch);
	return;
    }

    act("You peer intently $T.", ch, NULL, dir_name[door], TO_CHAR);
    act("$n peers intently $T.", ch, NULL, dir_name[door], TO_ROOM);
    sprintf(buf, "Looking %s you see:\n\r", dir_name[door]);

    scan_room = ch->in_room;

    for (depth = 1; depth < 4; depth++) {
	if ((pExit = scan_room->exit[door]) != NULL) {
	    scan_room = pExit->u1.to_room;
	    scan_list(pExit->u1.to_room, ch, depth, door);
	}
    }
    return;
}

void scan_list(ROOM_INDEX_DATA * scan_room, CHAR_DATA * ch, sh_int depth,
	       sh_int door)
{
    CHAR_DATA *rch;

    if (scan_room == NULL)
	return;
    for (rch = scan_room->people; rch != NULL; rch = rch->next_in_room) {
	if (rch == ch)
	    continue;
	if (!IS_NPC(rch) && rch->invis_level > get_trust(ch))
	    continue;
	if (can_see(ch, rch)) {
	    scan_char(rch, ch, depth, door);
	}
    }
    return;
}

void scan_char(CHAR_DATA * victim, CHAR_DATA * ch, sh_int depth, sh_int door)
{
    extern char *const dir_name[];
    extern char *const distance[];
    char buf[MAX_INPUT_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    buf[0] = '\0';
    sprintf(buf, filler[depth], dir_name[door]);

    PERS(victim, ch, pers_buf);
    strcat(buf, pers_buf);

    strcat(buf, " ");
    strcat(buf, distance[depth]);
    strcat(buf, "\n\r");

    send_to_char(buf, ch);
    return;
}

void do_scout(CHAR_DATA * ch, char *argument)
{
    extern char *const dir_name[];
    char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *scan_room;
    EXIT_DATA *pExit;
    sh_int door, depth;

if (get_skill(ch, gsn_scout) == 0) 
{
send_to_char("You don't know how to scout", ch);
return;
}

    argument = one_argument(argument, arg1);
WAIT_STATE(ch, PULSE_VIOLENCE / 2);

check_improve(ch, gsn_scout, TRUE, 1);


    act("You scout the area..", ch, NULL, NULL , TO_CHAR);
    act("$n scouts the area.", ch, NULL, NULL, TO_ROOM);

    scan_room = ch->in_room;

for (door = 0; door < 6; door++) 
{
scan_room = ch->in_room;
sprintf(buf, "Looking %s you see:\n\r", dir_name[door]);
	for (depth = 1; depth < 6; depth++) 
	{
		if ((pExit = scan_room->exit[door]) != NULL) 
		{
			scan_room = pExit->u1.to_room;
			if(number_percent() < get_skill(ch, gsn_scout)     )
			{
			scan_list(pExit->u1.to_room, ch, depth, door);
			}
		}
	}
}
    return;
}


