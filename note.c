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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/* globals from db.c for load_notes */
extern int _filbuf args((FILE *));
extern FILE *fpArea;
extern char strArea[MAX_INPUT_LENGTH];

/* local procedures */
void load_thread(char *name, NOTE_DATA ** list, int type, time_t free_time);
void parse_note(CHAR_DATA * ch, char *argument, int type);
bool hide_note(CHAR_DATA * ch, NOTE_DATA * pnote);

NOTE_DATA *note_list;
NOTE_DATA *idea_list;
NOTE_DATA *penalty_list;
NOTE_DATA *news_list;
NOTE_DATA *changes_list;
NOTE_DATA *notice_list;

int count_spool(CHAR_DATA * ch, NOTE_DATA * spool)
{
    int count = 0;
    NOTE_DATA *pnote;

    for (pnote = spool; pnote != NULL; pnote = pnote->next)
	if (!hide_note(ch, pnote))
	    count++;

    return count;
}

void do_unread(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    int count;
    bool found = FALSE;

    if (IS_NPC(ch))
	return;

    if ((count = count_spool(ch, news_list)) > 0) {
	found = TRUE;
	sprintf(buf, "There %s %d new news article%s waiting.\n\r",
		count > 1 ? "are" : "is", count, count > 1 ? "s" : "");
	send_to_char(buf, ch);
    }
    if ((count = count_spool(ch, changes_list)) > 0) {
	found = TRUE;
	sprintf(buf, "There %s %d change%s waiting to be read.\n\r",
		count > 1 ? "are" : "is", count, count > 1 ? "s" : "");
	send_to_char(buf, ch);
    }
    if ((count = count_spool(ch, note_list)) > 0) {
	found = TRUE;
	sprintf(buf, "You have %d new note%s waiting.\n\r",
		count, count > 1 ? "s" : "");
	send_to_char(buf, ch);
    }
    if ((count = count_spool(ch, idea_list)) > 0) {
	found = TRUE;
	sprintf(buf, "You have %d unread idea%s to peruse.\n\r",
		count, count > 1 ? "s" : "");
	send_to_char(buf, ch);
    }
    if (IS_TRUSTED(ch, ANGEL) && (count = count_spool(ch, penalty_list)) > 0) {
	found = TRUE;
	sprintf(buf, "%d %s been added.\n\r",
		count, count > 1 ? "penalties have" : "penalty has");
	send_to_char(buf, ch);
    }
    if ((count = count_spool(ch, notice_list)) > 0) {
	found = TRUE;
	sprintf(buf, "%d %s been added.\n\r",
		count, count > 1 ? "notices have" : "notice has");
	send_to_char(buf, ch);
    }

    if (!found)
	send_to_char("You have no unread notes.\n\r", ch);
}

void do_note(CHAR_DATA * ch, char *argument)
{
    parse_note(ch, argument, NOTE_NOTE);
}

void do_idea(CHAR_DATA * ch, char *argument)
{
    parse_note(ch, argument, NOTE_IDEA);
}

void do_penalty(CHAR_DATA * ch, char *argument)
{
	if(!IS_NPC(ch) && !IS_IMMORTAL(ch) && ch->pcdata->judge < 1)
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

    parse_note(ch, argument, NOTE_PENALTY);
}

void do_notice(CHAR_DATA * ch, char *argument)
{
    parse_note(ch, argument, NOTE_NOTICE);
}


void do_news(CHAR_DATA * ch, char *argument)
{
    parse_note(ch, argument, NOTE_NEWS);
}

void do_changes(CHAR_DATA * ch, char *argument)
{
    parse_note(ch, argument, NOTE_CHANGES);
}

void save_notes(int type)
{
    FILE *fp;
    char *name;
    NOTE_DATA *pnote;

    switch (type) {
    default:
	return;
    case NOTE_NOTE:
	name = NOTE_FILE;
	pnote = note_list;
	break;
    case NOTE_IDEA:
	name = IDEA_FILE;
	pnote = idea_list;
	break;
    case NOTE_PENALTY:
	name = PENALTY_FILE;
	pnote = penalty_list;
	break;
    case NOTE_NOTICE:
	name = NOTICE_FILE;
	pnote = notice_list;
	break;
    case NOTE_NEWS:
	name = NEWS_FILE;
	pnote = news_list;
	break;
    case NOTE_CHANGES:
	name = CHANGES_FILE;
	pnote = changes_list;
	break;
    }

    fclose(fpReserve);
    if ((fp = fopen(name, "w")) == NULL) {
	perror(name);
    } else {
	for (; pnote != NULL; pnote = pnote->next) {
	    fprintf(fp, "Sender  %s~\n", pnote->sender);
	    fprintf(fp, "Date    %s~\n", pnote->date);
	    fprintf(fp, "Stamp   %ld\n", pnote->date_stamp);
	    fprintf(fp, "To      %s~\n", pnote->to_list);
	    fprintf(fp, "Subject %s~\n", pnote->subject);
	    fprintf(fp, "Text\n%s~\n", pnote->text);
	}
	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	return;
    }
}
void load_notes(void)
{
    load_thread(NOTE_FILE, &note_list, NOTE_NOTE, 14 * 24 * 60 * 60);
    load_thread(IDEA_FILE, &idea_list, NOTE_IDEA, 28 * 24 * 60 * 60);
    load_thread(PENALTY_FILE, &penalty_list, NOTE_PENALTY, 0);
    load_thread(NOTICE_FILE, &notice_list, NOTE_NOTICE, 0);
    load_thread(NEWS_FILE, &news_list, NOTE_NEWS, 0);
    load_thread(CHANGES_FILE, &changes_list, NOTE_CHANGES, 0);
}

void load_thread(char *name, NOTE_DATA ** list, int type, time_t free_time)
{
    FILE *fp;
    NOTE_DATA *pnotelast;

    if ((fp = fopen(name, "r")) == NULL)
	return;

    pnotelast = NULL;
    for (;;) {
	NOTE_DATA *pnote;
	char letter;

	do {
	    letter = getc(fp);
	    if (feof(fp)) {
		fclose(fp);
		return;
	    }
	}
	while (isspace(letter));
	ungetc(letter, fp);

	pnote = alloc_perm(sizeof(*pnote));

	if (str_cmp(fread_word(fp), "sender"))
	    break;
	pnote->sender = fread_string(fp);

	if (str_cmp(fread_word(fp), "date"))
	    break;
	pnote->date = fread_string(fp);

	if (str_cmp(fread_word(fp), "stamp"))
	    break;
	pnote->date_stamp = fread_number(fp);

	if (str_cmp(fread_word(fp), "to"))
	    break;
	pnote->to_list = fread_string(fp);

	if (str_cmp(fread_word(fp), "subject"))
	    break;
	pnote->subject = fread_string(fp);

	if (str_cmp(fread_word(fp), "text"))
	    break;
	pnote->text = fread_string(fp);

	/* Been trying to find this memory leak for weeks.
	 * If the note date is too old, it doesn't add it to the spool,
	 * and it is free'd, but it doesn't get to free because the note wasn't
	 * valid, thus just sitting in memory.  Note is now set to valid first
	 * and THEN free'd.  *mutter*
	 *
	 * -Tasydra March/21/2003
	 */
	if (free_time && pnote->date_stamp < current_time - free_time) {
		pnote->valid = TRUE;
	    free_note(pnote);
	    continue;
	}
	pnote->type = type;

	if (*list == NULL)
	    *list = pnote;
	else
	    pnotelast->next = pnote;

	pnotelast = pnote;
    }

    strcpy(strArea, NOTE_FILE);
    fpArea = fp;
    bug("Load_notes: bad key word.", 0);
    exit(1);
    return;
}

void append_note(NOTE_DATA * pnote)
{
    FILE *fp;
    char *name;
    NOTE_DATA **list;
    NOTE_DATA *last;

    switch (pnote->type) {
    default:
	return;
    case NOTE_NOTE:
	name = NOTE_FILE;
	list = &note_list;
	break;
    case NOTE_IDEA:
	name = IDEA_FILE;
	list = &idea_list;
	break;
    case NOTE_PENALTY:
	name = PENALTY_FILE;
	list = &penalty_list;
	break;
    case NOTE_NOTICE:
	name = NOTICE_FILE;
	list = &notice_list;
	break;
    case NOTE_NEWS:
	name = NEWS_FILE;
	list = &news_list;
	break;
    case NOTE_CHANGES:
	name = CHANGES_FILE;
	list = &changes_list;
	break;
    }

    if (*list == NULL)
	*list = pnote;
    else {
	for (last = *list; last->next != NULL; last = last->next);
	last->next = pnote;
    }

    fclose(fpReserve);
    if ((fp = fopen(name, "a")) == NULL) {
	perror(name);
    } else {
	fprintf(fp, "Sender  %s~\n", pnote->sender);
	fprintf(fp, "Date    %s~\n", pnote->date);
	fprintf(fp, "Stamp   %ld\n", pnote->date_stamp);
	fprintf(fp, "To      %s~\n", pnote->to_list);
	fprintf(fp, "Subject %s~\n", pnote->subject);
	fprintf(fp, "Text\n%s~\n", pnote->text);
	fclose(fp);
    }
    fpReserve = fopen(NULL_FILE, "r");
}

bool is_note_to(CHAR_DATA * ch, NOTE_DATA * pnote)
{
//    if (!str_cmp(ch->name, pnote->sender))
//	return TRUE;

    if (is_name("all", pnote->to_list))
	return TRUE;

    if (IS_IMMORTAL(ch) && is_name("immortal", pnote->to_list))
	return TRUE;

    if (GET_SIDE(ch) == SIDE_TIER && is_name("tier", pnote->to_list)) 
	return TRUE;

    if (GET_SIDE(ch) == SIDE_KAEL && is_name("kael", pnote->to_list))
	return TRUE;

    if (GET_SIDE(ch) == SIDE_UM && is_name("undermountain", pnote->to_list))
	return TRUE;

	if (GET_SIDE(ch) == SIDE_UM && is_name("um", pnote->to_list))
	return TRUE;

    if (GET_SIDE(ch) == SIDE_CAIRN && is_name("cairn", pnote->to_list))
	return TRUE;

    if (ch->clan && is_name(clan_table[ch->clan].name, pnote->to_list))
	return TRUE;

    if (is_exact_name(ch->name, pnote->to_list))
	return TRUE;

	if (IS_TRUSTED(ch, ADMINISTRATOR) && is_name("admin", pnote->to_list))
	return TRUE;

	if (IS_TRUSTED(ch, IMPLEMENTOR) && (is_name("imp", pnote->to_list) || is_name("implementor", pnote->to_list) || is_name("admin", pnote->to_list)))
	return TRUE;

    if ((ch->pcdata->judge == 1 || ch->pcdata->judge == 2 || IS_IMMORTAL(ch) ) && is_name("constable", pnote->to_list) )
    return TRUE;

    return FALSE;
}

void note_attach(CHAR_DATA * ch, int type)
{
    NOTE_DATA *pnote;

    if (ch->pnote != NULL)
	return;

    pnote = new_note();

    pnote->next = NULL;
    pnote->sender = str_dup(ch->name);
    pnote->date = str_dup("");
    pnote->to_list = str_dup("");
    pnote->subject = str_dup("");
    pnote->text = str_dup("");
    pnote->type = type;
    ch->pnote = pnote;
    return;
}

void note_remove(CHAR_DATA * ch, NOTE_DATA * pnote, bool delete)
{
    char to_new[MAX_INPUT_LENGTH];
    char to_one[MAX_INPUT_LENGTH];
    NOTE_DATA *prev;
    NOTE_DATA **list;
    char *to_list;

    if (!delete) {
	/* make a new list */
	to_new[0] = '\0';
	to_list = pnote->to_list;
	while (*to_list != '\0') {
	    to_list = one_argument(to_list, to_one);
	    if (to_one[0] != '\0' && str_cmp(ch->name, to_one)) {
		strcat(to_new, " ");
		strcat(to_new, to_one);
	    }
	}
	/* Just a simple recipient removal? */
	if (str_cmp(ch->name, pnote->sender) && to_new[0] != '\0') {
	    free_string(pnote->to_list);
	    pnote->to_list = str_dup(to_new + 1);
	    return;
	}
    }
    /* nuke the whole note */

    switch (pnote->type) {
    default:
	return;
    case NOTE_NOTE:
	list = &note_list;
	break;
    case NOTE_IDEA:
	list = &idea_list;
	break;
    case NOTE_PENALTY:
	list = &penalty_list;
	break;
    case NOTE_NOTICE:
	list = &notice_list;
	break;
    case NOTE_NEWS:
	list = &news_list;
	break;
    case NOTE_CHANGES:
	list = &changes_list;
	break;
    }

    /*
     * Remove note from linked list.
     */
    if (pnote == *list) {
	*list = pnote->next;
    } else {
	for (prev = *list; prev != NULL; prev = prev->next) {
	    if (prev->next == pnote)
		break;
	}

	if (prev == NULL) {
	    bug("Note_remove: pnote not found.", 0);
	    return;
	}
	prev->next = pnote->next;
    }

    save_notes(pnote->type);
    free_note(pnote);
    return;
}

bool hide_note(CHAR_DATA * ch, NOTE_DATA * pnote)
{
    time_t last_read;

    if (IS_NPC(ch))
	return TRUE;

    switch (pnote->type) {
    default:
	return TRUE;
    case NOTE_NOTE:
	last_read = ch->pcdata->last_note;
	break;
    case NOTE_IDEA:
	last_read = ch->pcdata->last_idea;
	break;
    case NOTE_PENALTY:
	last_read = ch->pcdata->last_penalty;
	break;
    case NOTE_NOTICE:
	last_read = ch->pcdata->last_notice;
	break;
    case NOTE_NEWS:
	last_read = ch->pcdata->last_news;
	break;
    case NOTE_CHANGES:
	last_read = ch->pcdata->last_changes;
	break;
    }

    if (pnote->date_stamp <= last_read)
	return TRUE;

    if (!str_cmp(ch->name, pnote->sender))
	return TRUE;

    if (!is_note_to(ch, pnote))
	return TRUE;

    return FALSE;
}

void update_read(CHAR_DATA * ch, NOTE_DATA * pnote)
{
    time_t stamp;

    if (IS_NPC(ch))
	return;

    stamp = pnote->date_stamp;

    switch (pnote->type) {
    default:
	return;
    case NOTE_NOTE:
	ch->pcdata->last_note = UMAX(ch->pcdata->last_note, stamp);
	break;
    case NOTE_IDEA:
	ch->pcdata->last_idea = UMAX(ch->pcdata->last_idea, stamp);
	break;
    case NOTE_PENALTY:
	ch->pcdata->last_penalty = UMAX(ch->pcdata->last_penalty, stamp);
	break;
    case NOTE_NOTICE:
	ch->pcdata->last_notice = UMAX(ch->pcdata->last_notice, stamp);
	break;
    case NOTE_NEWS:
	ch->pcdata->last_news = UMAX(ch->pcdata->last_news, stamp);
	break;
    case NOTE_CHANGES:
	ch->pcdata->last_changes = UMAX(ch->pcdata->last_changes, stamp);
	break;
    }
}

void parse_note(CHAR_DATA * ch, char *argument, int type)
{
    BUFFER *buffer;
	BUFFER *buffer2;
    char buf[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    NOTE_DATA *pnote;
    NOTE_DATA **list;
    DESCRIPTOR_DATA *d;
    char *list_name;
    char *nmsg;
    int vnum;
    int anum;

    if (IS_NPC(ch))
	return;
	
    if (IS_SET(ch->comm, COMM_NONOTE))
	       	{
 		       	send_to_char("Your note writing privileges are revoked.\n\r", ch);
 	       		return;
        		} 

    switch (type) {
    default:
	return;
    case NOTE_NOTE:
	list = &note_list;
	list_name = "notes";
      nmsg = "note";
	break;
    case NOTE_IDEA:
	list = &idea_list;
	list_name = "ideas";
      nmsg = "idea";
	break;
    case NOTE_PENALTY:
	list = &penalty_list;
	list_name = "penalties";
      nmsg = "penalty";
	break;
    case NOTE_NOTICE:
	list = &notice_list;
	list_name = "notices";
      nmsg = "notice";
	break;
    case NOTE_NEWS:
	list = &news_list;
	list_name = "news";
      nmsg = "news item";
	break;
    case NOTE_CHANGES:
	list = &changes_list;
	list_name = "changes";
      nmsg = "change";
	break;
    }

    argument = one_argument(argument, arg);
    smash_tilde(argument);

    if (arg[0] == '\0' || !str_prefix(arg, "read")) {
	bool fAll;

	if (!str_cmp(argument, "all")) {
	    fAll = TRUE;
	    anum = 0;
	} else if (argument[0] == '\0' || !str_prefix(argument, "next"))
	    /* read next unread note */
	{
	    vnum = 0;
	    for (pnote = *list; pnote != NULL; pnote = pnote->next) {
		if (!hide_note(ch, pnote)) {
		    sprintf(buf, "[%3d  ] %-14s- %s{x\n\r%s\n\rTo: %s\n\r{m==========================================================================={x\n\r",
			    vnum,
			    pnote->sender,
			    pnote->subject,
			    pnote->date,
			    pnote->to_list);
		    send_to_char(buf, ch);
		    page_to_char(pnote->text, ch);
		    update_read(ch, pnote);
		    return;
		} else if (is_note_to(ch, pnote))
		    vnum++;
	    }
	    sprintf(buf, "You have no unread %s.\n\r", list_name);
	    send_to_char(buf, ch);
	    return;
	} else if (is_number(argument)) {
	    fAll = FALSE;
	    anum = atoi(argument);
	} else {
	    send_to_char("Read which number?\n\r", ch);
	    return;
	}

	vnum = 0;
	for (pnote = *list; pnote != NULL; pnote = pnote->next) {
	    if (is_note_to(ch, pnote) && (vnum++ == anum || fAll)) {
		sprintf(buf, "[%3d  ] %-14s- %s{x\n\r%s\n\rTo: %s\n\r{m==========================================================================={x\n\r",
			vnum - 1,
			pnote->sender,
			pnote->subject,
			pnote->date,
			pnote->to_list
		    );
		send_to_char(buf, ch);
		page_to_char(pnote->text, ch);
		update_read(ch, pnote);
		return;
	    }
	}

	sprintf(buf, "There aren't that many %s.\n\r", list_name);
	send_to_char(buf, ch);
	return;
    }
    if (!str_prefix(arg, "list")) {
	vnum = 0;
	buffer2 = new_buf();
	for (pnote = *list; pnote != NULL; pnote = pnote->next) {
	    if (is_note_to(ch, pnote)) {
		sprintf(buf, "[%3d%s%s] %-14s- %-25s{x\n\r",
			vnum, hide_note(ch, pnote) ? " " : "N",
                   ( str_infix( kingdom_table[ch->side].name, pnote->to_list)
                   && !str_infix( ch->name, pnote->to_list ) ) ? "{RP{x" : " ",
			pnote->sender, pnote->subject);
		add_buf(buffer2, buf);
		vnum++;
	    }
	}
	if (!vnum) {
	    switch (type) {
	    case NOTE_NOTE:
		send_to_char("There are no notes for you.\n\r", ch);
		break;
	    case NOTE_IDEA:
		send_to_char("There are no ideas for you.\n\r", ch);
		break;
	    case NOTE_PENALTY:
		send_to_char("There are no penalties for you.\n\r", ch);
		break;
	    case NOTE_NOTICE:
		send_to_char("There are no notices for you.\n\r", ch);
		break;
	    case NOTE_NEWS:
		send_to_char("There is no news for you.\n\r", ch);
		break;
	    case NOTE_CHANGES:
		send_to_char("There are no changes for you.\n\r", ch);
		break;
	    }
	}

		if (vnum != 0)
		{
		page_to_char(buf_string(buffer2), ch);
		free_buf(buffer2);
		}

	return;
    }
    if (!str_prefix(arg, "remove")) {
	if (!is_number(argument)) {
	    send_to_char("Note remove which number?\n\r", ch);
	    return;
	}
	anum = atoi(argument);
	vnum = 0;
	for (pnote = *list; pnote != NULL; pnote = pnote->next) {
	    if (is_note_to(ch, pnote) && vnum++ == anum) {
		note_remove(ch, pnote, FALSE);
		send_to_char("Note removed.\n\r", ch);
		return;
	    }
	}

	sprintf(buf, "There aren't that many %s.", list_name);
	send_to_char(buf, ch);
	return;
    }
    if (!str_prefix(arg, "delete") && get_trust(ch) >= MAX_LEVEL - 1) {
	if (!is_number(argument)) {
	    send_to_char("Note delete which number?\n\r", ch);
	    return;
	}
	anum = atoi(argument);
	vnum = 0;
	for (pnote = *list; pnote != NULL; pnote = pnote->next) {
	    if (is_note_to(ch, pnote) && vnum++ == anum) {
		note_remove(ch, pnote, TRUE);
		send_to_char("Note deleted.\n\r", ch);
		return;
	    }
	}

	sprintf(buf, "There aren't that many %s.", list_name);
	send_to_char(buf, ch);
	return;
    }
    if (!str_prefix(arg, "catchup")) {
	switch (type) {
	case NOTE_NOTE:
	    ch->pcdata->last_note = current_time;
	    break;
	case NOTE_IDEA:
	    ch->pcdata->last_idea = current_time;
	    break;
	case NOTE_PENALTY:
	    ch->pcdata->last_penalty = current_time;
	    break;
	case NOTE_NOTICE:
	    ch->pcdata->last_notice = current_time;
	    break;
	case NOTE_NEWS:
	    ch->pcdata->last_news = current_time;
	    break;
	case NOTE_CHANGES:
	    ch->pcdata->last_changes = current_time;
	    break;
	}
	return;
    }
    /* below this point only certain people can edit notes */
    if ((type == NOTE_NEWS && !IS_TRUSTED(ch, ANGEL))
	|| (type == NOTE_CHANGES && !(IS_TRUSTED(ch, CREATOR)))
	|| (type == NOTE_NOTICE && (!IS_IMMORTAL(ch) && 
ch->pcdata->judge==0))
	|| (type == NOTE_PENALTY && !IS_IMMORTAL(ch))) {
	sprintf(buf, "You aren't high enough level to write %s.\n\r", list_name);
	send_to_char(buf, ch);
	return;
    }
    if (!str_cmp(arg, "+")) {
	note_attach(ch, type);
	if (ch->pnote->type != type) {
	    send_to_char(
			    "You already have a different note in progress.\n\r", ch);
	    return;
	}
	if (strlen(ch->pnote->text) + strlen(argument) >= 4096) {
	    send_to_char("Note too long.\n\r", ch);
	    return;
	}
	buffer = new_buf();
	add_buf(buffer, ch->pnote->text);
	add_buf(buffer, argument);
	add_buf(buffer, "\n\r");
	free_string(ch->pnote->text);
	ch->pnote->text = str_dup(buf_string(buffer));
	free_buf(buffer);
	send_to_char("Line added.\n\r", ch);
	return;
    }

	if (!str_cmp(arg,"append"))
       {
       	note_attach( ch,type );
      	if (ch->pnote->type != type)
         	{
          	    send_to_char(
              		"You already have a different note in progress.\n\r",ch);
             	    return;
           	}
        string_append(ch, &ch->pnote->text);
        return;
       }


    if (!str_cmp(arg, "-")) 
{
	int len;
	bool found = FALSE;

	note_attach(ch, type);
	if (ch->pnote->type != type) {
	    send_to_char(
			    "You already have a different note in progress.\n\r", ch);
	    return;
	}
	if (ch->pnote->text == NULL || ch->pnote->text[0] == '\0') {
	    send_to_char("No lines left to remove.\n\r", ch);
	    return;
	}
	strcpy(buf, ch->pnote->text);

	for (len = strlen(buf); len > 0; len--) {
	    if (buf[len] == '\r') {
		if (!found) {	/* back it up */
		    if (len > 0)
			len--;
		    found = TRUE;
		} else {	/* found the second one */
		    buf[len + 1] = '\0';
		    free_string(ch->pnote->text);
		    ch->pnote->text = str_dup(buf);
		    return;
		}
	    }
	}
	buf[0] = '\0';
	free_string(ch->pnote->text);
	ch->pnote->text = str_dup(buf);
	return;
    }
    if (!str_prefix(arg, "subject")) {

// Prevention of note writing in the subject - Akuji 3/23/03
// Immortals have no limit on note subjects  - Akuji 3/27/03
	
if (ch->level < 102)
{
	if (strlen(argument) > 25) 
	{
	    send_to_char("You can't write your note in the subject!\n\r", ch);
	    return;
	}

	if(ch->level < 20) 
	{
	    send_to_char("You cannot post messages yet.\n\r", ch);
	    return;
	}
}

	note_attach(ch, type);
	if (ch->pnote->type != type) {
	    send_to_char(
			    "You already have a different note in progress.\n\r", ch);
	    return;
	}
	free_string(ch->pnote->subject);
	ch->pnote->subject = str_dup(argument);
	send_to_char("Subject added.\n\r", ch);
	return;
    }
    if (!str_prefix(arg, "to")) {

	if(ch->level < 20) {
	    send_to_char("You cannot post messages yet.\n\r", ch);
	    return;
	}
	note_attach(ch, type);
	if (ch->pnote->type != type) {
	    send_to_char(
			    "You already have a different note in progress.\n\r", ch);
	    return;
	}
	if (strstr(lowerall(argument), "kael") != NULL) {
	    if (!IS_IMMORTAL(ch) && GET_SIDE(ch) != SIDE_KAEL) {
		send_to_char("You cannot post messages to Kael.\n\r", ch);
		return;
	    }
	}
	if (strstr(lowerall(argument), "tier") != NULL) {
	    if (!IS_IMMORTAL(ch) && GET_SIDE(ch) != SIDE_TIER) {
		send_to_char("You cannot post messages to Tier.\n\r", ch);
		return;
	    }
	}
	if (strstr(lowerall(argument), "undermountain") != NULL) {
	    if (!IS_IMMORTAL(ch) && GET_SIDE(ch) != SIDE_UM) {
		send_to_char("You cannot post messages to Undermountain.\n\r", ch);
		return;
	    }
	}
if (strstr(lowerall(argument), "um") != NULL) {
	    if (!IS_IMMORTAL(ch) && GET_SIDE(ch) != SIDE_UM) {
		send_to_char("You cannot post messages to Undermountain.\n\r", ch);
		return;
	    }
	}
	if (strstr(lowerall(argument), "cairn") != NULL) {
	    if (!IS_IMMORTAL(ch) && GET_SIDE(ch) != SIDE_CAIRN) {
		send_to_char("You cannot post messages to Cairn.\n\r", ch);
		return;
	    }
	}
	if (strstr(lowerall(argument), "all") != NULL) {
	    if (!IS_IMMORTAL(ch)) {
		send_to_char("Only immortals can do that.\n\r", ch);
		return;
	    }
	}

	if (strstr(lowerall(argument), "constable") != NULL) {
	    if (!IS_IMMORTAL(ch) && ch->pcdata->judge <= 1 ) {
		send_to_char("Only immortals and constables can do that.\n\r", ch);
		return;
	    }
	}

	free_string(ch->pnote->to_list);
	ch->pnote->to_list = str_dup(argument);
	send_to_char("Note to added.\n\r", ch);
	return;
    }
    if (!str_prefix(arg, "clear")) {
	if (ch->pnote != NULL) {
	    free_note(ch->pnote);
	    ch->pnote = NULL;
	}
	send_to_char("Note cleared.\n\r", ch);
	return;
    }
    if (!str_prefix(arg, "show")) {
	if (ch->pnote == NULL) {
	    send_to_char("You have no note in progress.\n\r", ch);
	    return;
	}
	if (ch->pnote->type != type) {
	    send_to_char("You aren't working on that kind of note.\n\r", ch);
	    return;
	}
	sprintf(buf, "%-14s- %s{x\n\rTo: %s\n\r",
		ch->pnote->sender,
		ch->pnote->subject,
		ch->pnote->to_list
	    );
	send_to_char(buf, ch);

	sprintf(buf, "{m==========================================================================={x\n\r");
      send_to_char(buf, ch);

	send_to_char(ch->pnote->text, ch);
	return;
    }
    if (!str_prefix(arg, "post") || !str_prefix(arg, "send")) {
	char *strtime;

	if (ch->pnote == NULL) {
	    send_to_char("You have no note in progress.\n\r", ch);
	    return;
	}
	if (ch->pnote->type != type) {
	    send_to_char("You aren't working on that kind of note.\n\r", ch);
	    return;
	}
	if (!str_cmp(ch->pnote->to_list, "")) {
	    send_to_char(
			    "You need to provide a recipient:\n\r(Constable, Name, All, Immortal, Admin, Clan, Kael, Tier, Undermountain, or Cairn).\n\r",
			    ch);
	    return;
	}
	if (!str_cmp(ch->pnote->subject, "")) {
	    send_to_char("You need to provide a subject.\n\r", ch);
	    return;
	}

	send_to_char( "Note Posted.\n\r", ch );

      for (d = descriptor_list; d != NULL; d = d->next) 
        {
	if (d->character == NULL)
	    continue;

         if( is_note_to( d->character, ch->pnote ) && d->connected == CON_PLAYING )
          {
             if( d->character != ch )
              {
            sprintf(buf3, "A {Dnew %s{x has arrived!\n\r", nmsg);
            send_to_char( buf3, d->character);
              }
           }
        }

	ch->pnote->next = NULL;
	strtime = ctime(&current_time);
	strtime[strlen(strtime) - 1] = '\0';
	ch->pnote->date = str_dup(strtime);
	ch->pnote->date_stamp = current_time;

	append_note(ch->pnote);
	ch->pnote = NULL;

	return;
    }
    send_to_char("You can't do that.\n\r", ch);
    return;
}

void parse_penalty(CHAR_DATA *ch, char *subject, char *body)
{
	NOTE_DATA *temp = NULL;
	char notearg[MSL];
	bool used = FALSE;

	if (ch->pnote)
	{
		temp = ch->pnote;
		used = TRUE;
	}


	ch->pnote = NULL;
	parse_note(ch, "to Immortal Immortals", NOTE_PENALTY);
	sprintf(notearg, "subject %s", subject);
	parse_note(ch, notearg, NOTE_PENALTY);
	free_string(ch->pnote->text);
	ch->pnote->text = str_dup(body);
	parse_note(ch, "+ ", NOTE_PENALTY);
	parse_note(ch, "post", NOTE_PENALTY);

	if (used)
	{
	ch->pnote = temp;
	temp = NULL;
	}

	return;
}
