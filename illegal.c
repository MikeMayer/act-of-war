
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
*           Russ Taylor (rtaylor#efn,org)                                  *
*           Gabrielle Taylor                                               *
*           Brian Moore (zump#rom,org)                                     *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/

/***********************************************************/
/* DISCLAIMER: I do not take responsibility for what this  */
/* code may do to your health, your computer, your life,   */
/* your MUD, your friends, your house or whatever.         */
/* You use this code on your own free will.                */
/***********************************************************/

/*
   in the current code it is possible to illegalize players that are online and when they
   try to reconnect then they won't be able to since their name is illegal so be careful.
   And tada, now you can add names that are illegal from within the MUD, notice that
   the "never" mode makes it impossible for a name to contain the illegal word.
   i.e. if you decide to "illegalize stalin never" then no one can make a char with the word "stalin".
   neither "astalin" nor "astalinb" nor "stalinb" nor "stalin", if you get my point.
   The "equal" mode makes a name that are exactly the same as the illegalized word unuseable.
   i.e. illegalizing the word "stalin" with the mode "equal" would make players unable to use the word
   "stalin" as their name BUT they could have the name "astalin", "stalinb" or "astalinb".
   The "prefix" mode names a name prefixed with the illegal word unuseable.
   i.e. illegalizing the word "sir" with the mode "prefix" would stop everyone with the word "sir"
   in the beginning of their name. "esirion" would be useable but not "sirion".
   Feel free to add modes yourself, one that could be useful is "suffix".
   NOTE: the strange `'s are the colorcodes of Chronopia MUD so feel free to
   change them to your specific code.
   If you have any questions feel free to email me, and I'll try my best to help.
   Peace out!

   Feb 16 2000

   [-Implementor-] [Shadowmage] Angron of Chronopia
 */

/* Illegalize.c -by Angron of Chronopia (inhuman#mad,scientist.com) */
/* Great thanks to Erwin Andreasen for his "disable"-snippet which gave me inspiration */

/* Please email me if you use this code so I know I'm not wasting my time. */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include <ctype.h>

ILLEGAL_DATA *illegal_first;

/*
 * Parse a name for acceptability.
 */

bool check_parse_name(char *name)
{
    if (check_illegal(name))
	return FALSE;

    if (name[0] == '\0')
	return FALSE;

    if (strlen(name) < 2)
	return FALSE;

    if (strlen(name) > 12)
	return FALSE;

    {
	char *pc;
	bool fIll, adjcaps = FALSE, cleancaps = FALSE;
	int total_caps = 0;

	fIll = TRUE;
	for (pc = name; *pc != '\0'; pc++) {
	    if (!isalpha(*pc))
		return FALSE;

	    if (isupper(*pc)) {
		if (adjcaps)
		    cleancaps = TRUE;
		total_caps++;
		adjcaps = TRUE;
	    } else
		adjcaps = FALSE;

	    if (LOWER(*pc) != 'i' && LOWER(*pc) != 'l')
		fIll = FALSE;
	}

	if (fIll)
	    return FALSE;

	if (cleancaps || ((unsigned) total_caps > (strlen(name)) / 2 && strlen(name) < 3))
	    return FALSE;
    }

    return TRUE;
}

void do_illegalize(CHAR_DATA * ch, char *argument)
{
    ILLEGAL_DATA *p, *q;
    char pWord[MAX_INPUT_LENGTH];
    char pMode[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char kar;
    int col = 0;
    argument = one_argument(argument, pWord);
    argument = one_argument(argument, pMode);

    if (IS_NPC(ch)) {
	send_to_char("RETURN first.\n\r", ch);
	return;
    }
    if (pWord[0] == '\0' || pMode[0] == '\0') {
	if (!illegal_first) {
	    send_to_char("There are no illegal names.\n\r", ch);
	    return;
	}
	send_to_char("Name:        Mode:   Name:        Mode:   Name:        Mode:   Name:        Mode:  \n\r", ch);

	for (p = illegal_first; p; p = p->next) {
	    sprintf(buf, "%-12s %-8s", p->word, p->mode == '#' ? "equal" : p->mode == '$' ? "never" : p->mode == '@' ? "prefix" : "null");
	    send_to_char(buf, ch);

	    if (++col % 4 == 0)
		send_to_char("\n\r", ch);
	}

	send_to_char("\n\r", ch);

	return;
    }
    if (!str_cmp(pMode, "delete")) {

	for (p = illegal_first; p; p = p->next)
	    if (!str_cmp(pWord, p->word))
		break;

	if (p) {
	    if (illegal_first == p)	/* node to be removed == head ? */
		illegal_first = p->next;
	    else {		/* Find the node before this one */
		for (q = illegal_first; q->next != p; q = q->next);	/* empty for */
		q->next = p->next;
	    }

	    free_string(p->word);
	    free_mem(p, sizeof(ILLEGAL_DATA));	/* free node */
	    save_illegal();	/* save to disk */
	    send_to_char("Word deleted.\n\r", ch);
	    return;
	} else {
	    send_to_char("Word not found in the list.\n\r", ch);
	    return;
	}
    }
    if (strlen(pWord) > 11) {
	send_to_char("Word too long.\n\r", ch);
	return;
    }
    if (check_illegal(pWord)) {
	send_to_char("That word is already illegal.\n\r", ch);
	return;
    }
    if (!str_cmp(ch->name, pWord) && ch->level >= MAX_LEVEL) {
	send_to_char("You were actually going to illegalize yourself?!\n\r", ch);
	return;
    }
    if (!str_cmp(pMode, "equal"))
	kar = '#';
    else if (!str_cmp(pMode, "never"))
	kar = '$';
    else if (!str_cmp(pMode, "prefix"))
	kar = '@';
    else {
	send_to_char("Wrong mode.\n\r", ch);
	return;
    }

    p = alloc_mem(sizeof(ILLEGAL_DATA));
    p->mode = kar;
    p->word = str_dup(pWord);
    p->next = illegal_first;
    illegal_first = p;		/* add before the current first element */

    send_to_char("Word illegalized.\n\r", ch);
    save_illegal();		/* save to disk */
    return;
}

bool check_illegal(char *name)
{
    ILLEGAL_DATA *p;

    for (p = illegal_first; p; p = p->next) {
	if (p->mode == '#') {
	    if (!str_cmp(name, p->word) || is_name(name, p->word) || is_exact_name(name, p->word))
		return TRUE;
	} else if (p->mode == '@') {
	    if (!str_prefix(p->word, name))
		return TRUE;
	} else if (p->mode == '$') {
	    if (!str_cmp(name, p->word) || is_exact_name(name, p->word) || is_name(name, p->word) ||
		!str_infix(name, p->word) || !str_prefix(p->word, name) || strstr(name, p->word))
		return TRUE;
	}
    }
    return FALSE;
}

void load_illegal()
{
    FILE *fp;
    ILLEGAL_DATA *p;
    char kar;

    illegal_first = NULL;

    fclose(fpReserve);
    fp = fopen(NAME_FILE, "r");

    if (!fp)
	return;

    do {
	kar = fread_letter(fp);

	if (kar == EOF)
	    break;

	if (kar == '#' || kar == '$' || kar == '@') {
	    p = alloc_mem(sizeof(ILLEGAL_DATA));
	    p->mode = kar;
	    p->word = str_dup(fread_word(fp));
	    p->next = illegal_first;
	    illegal_first = p;
	}
    }
    while (kar != EOF);

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");
}

void save_illegal()
{
    FILE *fp;
    ILLEGAL_DATA *p;

    if (!illegal_first) {
	unlink(NAME_FILE);
	return;
    }
    fclose(fpReserve);
    fp = fopen(NAME_FILE, "w");

    if (!fp) {
	bug("Could not open " NAME_FILE " for writing", 0);
	return;
    }
    for (p = illegal_first; p; p = p->next)
	fprintf(fp, "%c \'%s\'\n", p->mode, p->word);

    fclose(fp);
    fpReserve = fopen(NULL_FILE, "r");
}
/**************************End Illegal.c************************************'*/
