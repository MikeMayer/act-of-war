
/***************************************************************************
 *  File: olc.c                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "tables.h"
#include "recycle.h"

/*
 * Local functions.
 */
AREA_DATA *get_area_data args((int vnum));

DECLARE_DO_FUN(do_asave);
DECLARE_DO_FUN(do_resets);

/* Executed from comm.c.  Minimizes compiling when changes are made. */
bool run_olc_editor(DESCRIPTOR_DATA * d)
{
    switch (d->editor) {
    case ED_AREA:
	aedit(d->character, d->incomm);
	break;
    case ED_ROOM:
	redit(d->character, d->incomm);
	break;
    case ED_OBJECT:
	oedit(d->character, d->incomm);
	break;
    case ED_MOBILE:
	medit(d->character, d->incomm);
	break;
    case ED_MPCODE:
	mpedit(d->character, d->incomm);
	break;
    default:
	return FALSE;
    }
    return TRUE;
}

char *olc_ed_name(CHAR_DATA * ch)
{
    static char buf[10];

    buf[0] = '\0';
    switch (ch->desc->editor) {
    case ED_AREA:
	sprintf(buf, "AEdit");
	break;
    case ED_ROOM:
	sprintf(buf, "REdit");
	break;
    case ED_OBJECT:
	sprintf(buf, "OEdit");
	break;
    case ED_MOBILE:
	sprintf(buf, "MEdit");
	break;
    case ED_MPCODE:
	sprintf(buf, "MPEdit");
	break;
    default:
	sprintf(buf, " ");
	break;
    }
    return buf;
}

char *olc_ed_vnum(CHAR_DATA * ch)
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA *pObj;
    MOB_INDEX_DATA *pMob;
    MPROG_CODE *pMprog;
    static char buf[10];

    buf[0] = '\0';
    switch (ch->desc->editor) {
    case ED_AREA:
	pArea = (AREA_DATA *) ch->desc->pEdit;
	sprintf(buf, "%d", pArea ? pArea->vnum : 0);
	break;
    case ED_ROOM:
	pRoom = ch->in_room;
	sprintf(buf, "%d", pRoom ? pRoom->vnum : 0);
	break;
    case ED_OBJECT:
	pObj = (OBJ_INDEX_DATA *) ch->desc->pEdit;
	sprintf(buf, "%d", pObj ? pObj->vnum : 0);
	break;
    case ED_MOBILE:
	pMob = (MOB_INDEX_DATA *) ch->desc->pEdit;
	sprintf(buf, "%d", pMob ? pMob->vnum : 0);
	break;
    case ED_MPCODE:
	pMprog = (MPROG_CODE *) ch->desc->pEdit;
	sprintf(buf, "%d", pMprog ? pMprog->vnum : 0);
	break;
    default:
	sprintf(buf, " ");
	break;
    }

    return buf;
}

/*****************************************************************************
 Name:		show_olc_cmds
 Purpose:	Format up the commands from given table.
 Called by:	show_commands(olc_act.c).
 ****************************************************************************/
void show_olc_cmds(CHAR_DATA * ch, const struct olc_cmd_type *olc_table)
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int cmd;
    int col;

    buf1[0] = '\0';
    col = 0;
    for (cmd = 0; olc_table[cmd].name != NULL; cmd++) {
	sprintf(buf, "%-15.15s", olc_table[cmd].name);
	strcat(buf1, buf);
	if (++col % 5 == 0)
	    strcat(buf1, "\n\r");
    }

    if (col % 5 != 0)
	strcat(buf1, "\n\r");

    send_to_char(buf1, ch);
    return;
}

/*****************************************************************************
 Name:		show_commands
 Purpose:	Display all olc commands.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_commands(CHAR_DATA * ch, char *argument)
{
    switch (ch->desc->editor) {
    case ED_AREA:
	show_olc_cmds(ch, aedit_table);
	break;
    case ED_ROOM:
	show_olc_cmds(ch, redit_table);
	break;
    case ED_OBJECT:
	show_olc_cmds(ch, oedit_table);
	break;
    case ED_MOBILE:
	show_olc_cmds(ch, medit_table);
	break;
    case ED_MPCODE:
	show_olc_cmds(ch, mpedit_table);
	break;
    }

    return FALSE;
}

/*****************************************************************************
 *                           Interpreter Tables.                             *
 *****************************************************************************/
const struct olc_cmd_type aedit_table[] =
{
/*  {   command         function        }, */

    {"age", aedit_age},
    {"builder", aedit_builder},	/* s removed -- Hugin */
    {"commands", show_commands},
    {"create", aedit_create},
    {"filename", aedit_file},
    {"name", aedit_name},
/*  {   "recall",       aedit_recall    },   ROM OLC */
    {"reset", aedit_reset},
    {"security", aedit_security},
    {"show", aedit_show},
    {"vnum", aedit_vnum},
    {"lvnum", aedit_lvnum},
    {"uvnum", aedit_uvnum},
    {"credits", aedit_credits},

    {"?", show_help},
    {"version", show_version},

    {NULL, 0,}
};

const struct olc_cmd_type redit_table[] =
{
/*  {   command         function        }, */

    {"commands", show_commands},
    {"create", redit_create},
    {"desc", redit_desc},
    {"ed", redit_ed},
    {"format", redit_format},
    {"name", redit_name},
    {"show", redit_show},
    {"heal", redit_heal},
    {"mana", redit_mana},
    {"clan", redit_clan},

    {"north", redit_north},
    {"south", redit_south},
    {"east", redit_east},
    {"west", redit_west},
    {"up", redit_up},
    {"down", redit_down},

    /* New reset commands. */
    {"mreset", redit_mreset},
    {"oreset", redit_oreset},
    {"mlist", redit_mlist},
    {"rlist", redit_rlist},
    {"olist", redit_olist},
    {"mshow", redit_mshow},
    {"oshow", redit_oshow},
    {"owner", redit_owner},

    {"?", show_help},
    {"version", show_version},

    {NULL, 0,}
};

const struct olc_cmd_type oedit_table[] =
{
/*  {   command         function        }, */

    {"addaffect", oedit_addaffect},
    {"addapply", oedit_addapply},
    {"commands", show_commands},
    {"cost", oedit_cost},
    {"create", oedit_create},
    {"delaffect", oedit_delaffect},
    {"ed", oedit_ed},
    {"long", oedit_long},
    {"name", oedit_name},
    {"short", oedit_short},
    {"show", oedit_show},
    {"v0", oedit_value0},
    {"v1", oedit_value1},
    {"v2", oedit_value2},
    {"v3", oedit_value3},
    {"v4", oedit_value4},	/* ROM */
    {"weight", oedit_weight},

    {"extra", oedit_extra},	/* ROM */
    {"wear", oedit_wear},	/* ROM */
    {"type", oedit_type},	/* ROM */
    {"material", oedit_material},	/* ROM */
    {"level", oedit_level},	/* ROM */
    {"condition", oedit_condition},	/* ROM */

    {"?", show_help},
    {"version", show_version},

    {NULL, 0,}
};

const struct olc_cmd_type medit_table[] =
{
/*  {   command         function        }, */

    {"alignment", medit_align},
    {"commands", show_commands},
    {"create", medit_create},
    {"desc", medit_desc},
    {"level", medit_level},
    {"long", medit_long},
    {"name", medit_name},
    {"shop", medit_shop},
    {"short", medit_short},
    {"show", medit_show},
    {"spec", medit_spec},

    {"sex", medit_sex},		/* ROM */
    {"act", medit_act},		/* ROM */
    {"affect", medit_affect},	/* ROM */
    {"armor", medit_ac},	/* ROM */
    {"form", medit_form},	/* ROM */
    {"part", medit_part},	/* ROM */
    {"imm", medit_imm},		/* ROM */
    {"res", medit_res},		/* ROM */
    {"vuln", medit_vuln},	/* ROM */
    {"material", medit_material},	/* ROM */
    {"off", medit_off},		/* ROM */
    {"size", medit_size},	/* ROM */
    {"hitdice", medit_hitdice},	/* ROM */
    {"manadice", medit_manadice},	/* ROM */
    {"damdice", medit_damdice},	/* ROM */
    {"race", medit_race},	/* ROM */
    {"position", medit_position},	/* ROM */
    {"wealth", medit_gold},	/* ROM */
    {"hitroll", medit_hitroll},	/* ROM */
    {"damtype", medit_damtype},	/* ROM */
    {"group", medit_group},	/* ROM */
    {"addmprog", medit_addmprog},	/* ROM */
    {"delmprog", medit_delmprog},	/* ROM */

    {"?", show_help},
    {"version", show_version},

    {NULL, 0,}
};

/*****************************************************************************
 *                          End Interpreter Tables.                          *
 *****************************************************************************/

/*****************************************************************************
 Name:		get_area_data
 Purpose:	Returns pointer to area with given vnum.
 Called by:	do_aedit(olc.c).
 ****************************************************************************/
AREA_DATA *get_area_data(int vnum)
{
    AREA_DATA *pArea;

    for (pArea = area_first; pArea; pArea = pArea->next) {
	if (pArea->vnum == vnum)
	    return pArea;
    }

    return 0;
}

/*****************************************************************************
 Name:		edit_done
 Purpose:	Resets builder information on completion.
 Called by:	aedit, redit, oedit, medit(olc.c)
 ****************************************************************************/
bool edit_done(CHAR_DATA * ch)
{
    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    if (IS_SET(ch->act, PLR_AUTOBUILD))
	REMOVE_BIT(ch->act, PLR_AUTOBUILD);
	do_asave(ch, "changed");
	return FALSE;
}

/*****************************************************************************
 *                              Interpreters.                                *
 *****************************************************************************/

/* Area Interpreter, called by do_aedit. */
void aedit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int cmd;
    int value;

    EDIT_AREA(ch, pArea);
    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (!IS_BUILDER(ch, pArea)) {
	send_to_char("AEdit:  Insufficient security to modify area.\n\r", ch);
	edit_done(ch);
	return;
    }
    if (!str_cmp(command, "done")) {
	edit_done(ch);
	return;
    }
    if (!IS_BUILDER(ch, pArea)) {
	interpret(ch, arg);
	return;
    }
    if (command[0] == '\0') {
	aedit_show(ch, argument);
	return;
    }
    if ((value = flag_value(area_flags, command)) != NO_FLAG) {
	TOGGLE_BIT(pArea->area_flags, value);

	send_to_char("Flag toggled.\n\r", ch);
	return;
    }
    /* Search Table and Dispatch Command. */
    for (cmd = 0; aedit_table[cmd].name != NULL; cmd++) {
	if (!str_prefix(command, aedit_table[cmd].name)) {
	    if ((*aedit_table[cmd].olc_fun) (ch, argument)) {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		return;
	    } else
		return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
}

void create_claim(CHAR_DATA * ch, int dir)
{
    char buf[MAX_STRING_LENGTH];
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int i, count;
    bool done;
    count = 50000;
    i = 0;

    done = TRUE;

    if (ch->pcdata->vassals < 60) {
	send_to_char("You must have 60 or more vassals to claim territory.\n\r", ch);
	return;
    }
    if (ch->in_room->exit[dir] != NULL) {
	send_to_char("That direction already exists.\n\r", ch);
	return;
    }
    /* check location of player and validate that claim is done from a claimable zone */
    if (ch->side == SIDE_TIER) {
	switch ((ch->in_room->vnum / 100)) {
	    /* valid zones to claim from for Tieran */
	case 271:
	    break;
	case 205:
	    break;
	case 15:
	    break;
	case 152:
	    break;
	case 258:
	    break;
	case 256:
	    break;
	default:
	    send_to_char("You can't make a claim here. Type help  'claim' for more info.\n\r", ch);
	    return;
	}
    }
    if (ch->side == SIDE_KAEL) {
	switch ((ch->in_room->vnum / 100)) {
	    /* valid zones to claim from for Kael */
	case 39:
	    break;
	case 220:
	    break;
	case 209:
	    break;
	case 226:
	    break;
	case 227:
	    break;
	case 228:
	    break;
	case 40:
	    break;
	case 225:
	    break;
	case 208:
	    break;
	default:
	    send_to_char("You can't make a claim here. Type help  'claim' for more info.\n\r", ch);
	    return;
	}
    }
    if (ch->side == SIDE_UM) {
	switch ((ch->in_room->vnum / 100)) {
	    /* valid zones to claim from for UM */
	case 300:
	    break;
	case 450:
	    break;
	case 400:
	    break;
	case 402:
	    break;
	case 453:
	    break;
	case 451:
	    break;
	case 454:
	    break;
	case 455:
	    break;
	case 456:
	    break;
	case 457:
	    break;
	case 458:
	    break;
	case 459:
	    break;
	default:
	    send_to_char("You can't make a claim here. Type help  'claim' for more info.\n\r", ch);
	    return;
	}
    }
    if (ch->side == SIDE_CAIRN) {
	switch ((ch->in_room->vnum / 100)) {
	    /* valid zones to claim from for Cairn */
	case 1:
	    break;
	default:
	    send_to_char("You can't make a claim here.  Type help 'claim' for more info.\n\r", ch);
	    return;
	}
    }
    /* check the sector type the person is trying to build from */
/*
    if (!IS_OUTSIDE(ch) && ch->side != SIDE_UM) {
	send_to_char("You can't make a claim here try somewhere else.\n\r", ch);
	return;
    }
*/
    /* verify that they have not already staked a claim */
    for (pArea = area_first; pArea != NULL; pArea = pArea->next) {
	if (strstr(pArea->builders, ch->name)) {
	    send_to_char("You have already staked a claim somewhere. Your request was denied.\n\r", ch);
	    return;
	}
    }

    /* this loop checks to find the first number */
    while (done) {
	if (get_vnum_area(count + i)) {
	    i++;
	    if (i > 20000) {
		send_to_char("No more claims game is full right now. Contact admin to clean up space on harddrive.\n\r", ch);
		return;
	    }
	} else
	    done = FALSE;

    }

    if (get_vnum_area((count + i) + 25)) {
	send_to_char("ERROR: in claim. Contact admin.\n\r", ch);
	return;
    }
    if (ch->banked_gold < 50000) {
	send_to_char("You don't have enough money to stake a claim the cost is 50000 gold.\n\r", ch);
	return;
    }
    ch->banked_gold -= 50000;

    /* create a new area for this character */
    (aedit_create) (ch, "9999999");

    /* set the builder permission to character only */
    (aedit_builder) (ch, ch->name);

    sprintf(buf, "%d %d", (count + i), (count + i) + 25);
    (aedit_vnum) (ch, buf);

    sprintf(buf, "c%d", (count + i));
    (aedit_file) (ch, buf);

    sprintf(buf, "Castle of %s", ch->name);
    (aedit_name) (ch, buf);

    edit_done(ch);

    /* build standard room and link it */
    sprintf(buf, "%d", (count + i));
    (redit_create) (ch, buf);

    pRoom = get_room_index(count + i);
    free_string(pRoom->name);
    sprintf(buf, "Entrance to Construction Area for %s", ch->name);
    pRoom->name = str_dup(buf);

    if (!IS_SET(pRoom->room_flags, AREA_CHANGED))
	SET_BIT(pRoom->room_flags, ROOM_NO_MOB);

    if (!IS_SET(pRoom->area->area_flags, AREA_CHANGED))
	SET_BIT(pRoom->area->area_flags, AREA_CHANGED);

    if (!IS_SET(ch->in_room->area->area_flags, AREA_CHANGED))
	SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

    /* make link */
    if (dir == 0) {		/* north */
	sprintf(buf, "link %d", count + i);
	(redit_north) (ch, buf);
    }
    if (dir == 1) {		/* east */
	sprintf(buf, "link %d", count + i);
	(redit_east) (ch, buf);
    }
    if (dir == 2) {		/* south */
	sprintf(buf, "link %d", count + i);
	(redit_south) (ch, buf);
    }
    if (dir == 3) {		/* west */
	sprintf(buf, "link %d", count + i);
	(redit_west) (ch, buf);
    }
    edit_done(ch);

    send_to_char("Your request for a claim has been approved. A small area of land has been set aside for construction of your castle, home, or fortification.\n\r", ch);

    send_to_char("{yWARNING: Your castle must fit a fantasy THEME of this game. If you create a castle which is unrealistic in a fantasy game such as PEE WEE playhouse, your area will be deleted and your character will be asked to delete.{x\n\r", ch);

    send_to_char("\n\rFollow proper castle building procedures as outlined in --> help 'castle'\n\r", ch);

    /* save the new area */
    /* kludge to fake out security */
    if (!IS_IMMORTAL(ch))
	ch->pcdata->security = 10;
    do_asave(ch, "changed");
    if (!IS_IMMORTAL(ch))
	ch->pcdata->security = 0;
}

void do_claim(CHAR_DATA * ch, char *argument)
{
    char command[MAX_INPUT_LENGTH];
    int door;

    door = -1;

    smash_tilde(argument);
    argument = one_argument(argument, command);
    if (IS_NPC(ch))
	return;

    if (ch->level < 50) {
	send_to_char("You must be 50th level or higher to claim lands.\n\r", ch);
	return;
    }
    if (command[0] == '\0') {
	send_to_char("You must specify a direction to make a claim. Example: claim north \n\r", ch);
	return;
    }
    if (!str_cmp(command, "north"))
	door = 0;
    else if (!str_cmp(command, "east"))
	door = 1;
    else if (!str_cmp(command, "south"))
	door = 2;
    else if (!str_cmp(command, "west"))
	door = 3;
    else {
	send_to_char("You must have a valid direction. North East South and West.\n\r", ch);
	return;
    }

    if (door != -1)
	create_claim(ch, door);
}

void do_design(CHAR_DATA * ch, char *argument)
{
    char command[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];

    int door;

    smash_tilde(argument);
    argument = one_argument(argument, command);

    door = -1;

    if (IS_NPC(ch))
	return;

    if (ch->level < 50) {
	send_to_char("You must be level 50 or higher to design your castle.\n\r", ch);
	return;
    }
    if (command[0] == '\0') {
	send_to_char("Design options are: \n\r", ch);
	send_to_char("name - syntax: design name <new room name>\n\r", ch);
	send_to_char("room - changes the room description syntax:design room\n\r", ch);
	send_to_char("clan - toggles clan room syntax:design clan\n\r", ch);
	send_to_char("direction - Creates a new room in the direction example syntax: design east \n\r", ch);
	send_to_char("link - creates a link to a previously built room syntax: link \n\r", ch);
	send_to_char("save - saves the current castle and its changes\n\r", ch);
	send_to_char("troop - loads up a guard or soldier in a room\n\r", ch);
	send_to_char("item - builds an additional item into the room. Throne, fountains etc.\n\r", ch);
	return;
    }
    if (!strstr(ch->in_room->area->builders, ch->name)) {
	send_to_char("You can't design here. You must design on your own property.\n\r", ch);
	return;
    }
    if (!str_cmp(command, "name")) {
	if (ch->pcdata->vassals <= 70) {
	    send_to_char("You must have more then 70 vassals to change an existing construction site or addition.\n\r", ch);
	    return;
	}
	if (ch->banked_gold < 1200) {
	    send_to_char("You don't have enough money in the bank to make modifications to this area. Minimum cost is 1200 gold.\n\r", ch);
	    return;
	}
	if (strlen(argument) > 65) {
	    send_to_char("That name is too long shorten it.\n\r", ch);
	    return;
	}
	if (strlen(argument) < 6) {
	    send_to_char("That name is too short lengthen it.\n\r", ch);
	    return;
	}
/*
	if ((strstr(argument, ch->name) == NULL)
	    && (strstr(argument, clan_table[ch->clan].name) == NULL)
	    ) {
	    send_to_char("You must include your name or clan name somewhere in the name of the room.\n\r Example: The Throne Room of Nimbus\n\r", ch);
	    return;
	}
*/
	ch->banked_gold -= 1200;

	(redit_name) (ch, argument);
	ch->in_room->sector_type = SECT_INSIDE;
	SET_BIT(ch->in_room->room_flags, ROOM_INDOORS);
	edit_done(ch);

	send_to_char("Dont forget to save your changes with the 'design save' command. \n\r", ch);
	SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

	return;
    }
    if (!str_cmp(command, "room")) {
	if (ch->pcdata->vassals <= 70) {
	    send_to_char("You must have more then 70 vassals to change an existing room.\n\r", ch);
	    return;
	}
	send_to_char("You have now entered the editor for the rooms\n\r", ch);
	send_to_char("design. This editor is a simple text editor which allows you to come up\n\r", ch);
	send_to_char("with a unique description of what other players see when they enter your\n\r", ch);
	send_to_char("castle or fortress room.\n\r", ch);

	(redit_desc) (ch, "");

	ch->in_room->sector_type = SECT_INSIDE;
	SET_BIT(ch->in_room->room_flags, ROOM_INDOORS);

	edit_done(ch);
	SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

	return;
    }
    if (!str_cmp(command, "link")) {
	char buf[MAX_STRING_LENGTH];
	int roomnum;

	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if (arg2[0] == '\0' || arg3[0] == '\0') {
	    send_to_char("Syntax: link <direction> #roomnumber\n\r", ch);
	    sprintf(buf, "This rooms roomnumber is %d.\n\r", ch->in_room->vnum);
	    send_to_char(buf, ch);
	    return;
	}
	if (ch->pcdata->vassals < 70) {
	    send_to_char("You must have at least 70 vassals to complete a link.\n\r", ch);
	    return;
	}
	if (ch->banked_gold < 1000) {
	    send_to_char("It costs 1000 gold to make a special link.\n\r You don't have enough.\n\r", ch);
	    return;
	}
	if (!str_cmp(arg2, "north"))
	    door = 0;
	else if (!str_cmp(arg2, "east"))
	    door = 1;
	else if (!str_cmp(arg2, "south"))
	    door = 2;
	else if (!str_cmp(arg2, "west"))
	    door = 3;
	else if (!str_cmp(arg2, "up"))
	    door = 4;
	else if (!str_cmp(arg2, "down"))
	    door = 5;
	else {
	    send_to_char("That is not a valid direction.\n\r North, east, south, west, up and down are.\n\r", ch);
	    return;
	}
	if (ch->in_room->exit[door] != NULL) {
	    send_to_char("Exit already exists in that direction.\n\r", ch);
	    return;
	}
	if (!is_number(arg3)) {
	    send_to_char("You must supply a number for the roomnumber field.\n\r", ch);
	    return;
	}
	roomnum = atoi(arg3);

	if (!get_room_index(roomnum)) {
	    send_to_char("That is an invalid room number.\n\r", ch);
	    return;
	}
	if (roomnum < ch->in_room->area->min_vnum ||
	    roomnum > ch->in_room->area->max_vnum) {
	    send_to_char("You can't link to any rooms outside the ones in your castle.\n\r", ch);
	    return;
	}
	if (ch->in_room->vnum == roomnum) {
	    send_to_char("You can't link back to the same place your in.\n\r", ch);
	    return;
	}
	ch->banked_gold -= 1000;

	if (door == 0) {
	    sprintf(buf, "link %d", roomnum);
	    (redit_north) (ch, buf);
	}
	if (door == 1) {
	    sprintf(buf, "link %d", roomnum);
	    (redit_east) (ch, buf);
	}
	if (door == 2) {
	    sprintf(buf, "link %d", roomnum);
	    (redit_south) (ch, buf);
	}
	if (door == 3) {
	    sprintf(buf, "link %d", roomnum);
	    (redit_west) (ch, buf);
	}
	if (door == 4) {
	    sprintf(buf, "link %d", roomnum);
	    (redit_up) (ch, buf);
	}
	if (door == 5) {
	    sprintf(buf, "link %d", roomnum);
	    (redit_down) (ch, buf);
	}
	edit_done(ch);
	send_to_char("Dont forget to save your changes with the 'design save' command. \n\r", ch);
	SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

	return;
    }
    if (!str_cmp(command, "north"))
	door = 0;
    else if (!str_cmp(command, "east"))
	door = 1;
    else if (!str_cmp(command, "south"))
	door = 2;
    else if (!str_cmp(command, "west"))
	door = 3;
    else if (!str_cmp(command, "up"))
	door = 4;
    else if (!str_cmp(command, "down"))
	door = 5;

    if (door != -1) {
	int i;
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *pRoom;

	if (ch->pcdata->vassals < 100) {
	    send_to_char("You must have at least 100 vassals to build an addition.\n\r", ch);
	    return;
	}
	if (ch->banked_gold < 5000) {
	    send_to_char("You must have at least 5000 gold to add an addition.\n\r", ch);
	    return;
	}
	ch->banked_gold -= 5000;

	/*verify exit already doesn't exist */
	if (ch->in_room->exit[door] != NULL) {
	    send_to_char("Exit already exists in that direction.\n\r", ch);
	    return;
	}
	/* find a valid vnum for a new room in the players area */
	for (i = ch->in_room->area->min_vnum; i <= ch->in_room->area->max_vnum; i++) {
	    if (!get_room_index(i)) {
		break;
	    }
	    if (i >= ch->in_room->area->max_vnum) {
		send_to_char("Your building area is full. You can't expand anymore.\n\r", ch);
		return;
	    }
	}
	/* create a new empty indoor room */
	sprintf(buf, "%d", i);
	(redit_create) (ch, buf);

	/* quick name for the room */
	pRoom = get_room_index(i);
	free_string(pRoom->name);
	sprintf(buf, "An Unfinished Addition To %ss Castle", ch->name);
	pRoom->name = str_dup(buf);

	pRoom->sector_type = SECT_INSIDE;
	SET_BIT(pRoom->room_flags, ROOM_INDOORS);

	if (door == 0) {
	    sprintf(buf, "link %d", i);
	    (redit_north) (ch, buf);
	}
	if (door == 1) {
	    sprintf(buf, "link %d", i);
	    (redit_east) (ch, buf);
	}
	if (door == 2) {
	    sprintf(buf, "link %d", i);
	    (redit_south) (ch, buf);
	}
	if (door == 3) {
	    sprintf(buf, "link %d", i);
	    (redit_west) (ch, buf);
	}
	if (door == 4) {
	    sprintf(buf, "link %d", i);
	    (redit_up) (ch, buf);
	}
	if (door == 5) {
	    sprintf(buf, "link %d", i);
	    (redit_down) (ch, buf);
	}
	/* remind player to name and add description to room */
	send_to_char("New addition added.\n\r", ch);
	send_to_char("Be sure to finish the rest of the addition, use design name and design room.\n\r", ch);

	send_to_char("Dont forget to save your changes with the 'design save' command. \n\r", ch);
	SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

	return;
    }
    if (!str_cmp(command, "clan")) {
	if (ch->clan <= 0) {
	    send_to_char("Your not in a clan.\n\r", ch);
	    return;
	}
	if (ch->pcdata->vassals < 110) {
	    send_to_char("You must have 110 vassals to clan restrict your rooms.\n\r", ch);
	    return;
	}
	if (ch->in_room->clan == 0) {
	    (redit_clan) (ch, clan_table[ch->clan].name);
	    send_to_char("Only members of your clan can enter this area.\n\r", ch);
	} else {
	    (redit_clan) (ch, clan_table[0].name);
	    send_to_char("Anyone may enter this area now.\n\r", ch);
	}
	edit_done(ch);
	send_to_char("Dont forget to save your changes with the 'design save' command. \n\r", ch);
	SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
	return;
    }
    if (!str_cmp(command, "save")) {
	do_asave(ch, "changed");
	return;
    }
    /* adding mobiles */
    if (!str_cmp(command, "troop")) {
	send_to_char("Troop code is not yet ready.\n\r", ch);
	/* search through creatures find flagged guards */
	/* check research level to display guards available */

	return;
    }
    /* adding objects */
    if (!str_cmp(command, "item")) {
	OBJ_INDEX_DATA *obj;
	int vnum;
	int nMatch;
	int itemnum;

	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	nMatch = 0;

	if (ch->pcdata->vassals < 70) {
	    send_to_char("You must have 70 vassals to add items.\n\r", ch);
	    return;
	}
	if (arg2[0] == '\0' || arg3[0] == '\0') {
	    send_to_char("Syntax: design item add #itemnumber\n\r", ch);
	    send_to_char("Syntax: design item remove #itemnumber\n\r", ch);

	    send_to_char("Items available for your rooms.\n\r", ch);

	    /* look through and show all items which are flagged special */
	    for (vnum = 0; nMatch < top_obj_index; vnum++) {
		if ((obj = get_obj_index(vnum)) != NULL) {
		    nMatch++;

		    if (obj->item_type == ITEM_CASTLE_SPECIAL) {
			sprintf(buf, "%5d - %s %5dgp \n\r", obj->vnum,
				obj->short_descr, obj->cost);
			send_to_char(buf, ch);
		    }
		}
	    }
	    return;
	}
	if (!is_number(arg3)) {
	    send_to_char("You must supply a number for the item.\n\r", ch);
	    return;
	}
	itemnum = atoi(arg3);

	if ((obj = get_obj_index(itemnum)) == NULL) {
	    send_to_char("That is an invalid number for item.\n\r", ch);
	    return;
	}
	if (obj->item_type != ITEM_CASTLE_SPECIAL) {
	    send_to_char("That is an invalid number for item.\n\r", ch);
	    return;
	}
	/* at this point we have a valid object */

	if (!str_cmp(arg2, "add")) {
	    RESET_DATA *pReset;
	    int cnt;
	    cnt = 0;
	    for (pReset = ch->in_room->reset_first; pReset; pReset = pReset->next) {
		if (pReset != NULL)
		    cnt++;

		if (pReset->arg1 == itemnum) {
		    send_to_char("You already have one of these items added.\n\r", ch);
		    return;
		}
		if (cnt > 5) {
		    send_to_char("You can't have any more items in this room.\n\r", ch);
		    return;
		}
	    }

	    if (ch->banked_gold < obj->cost) {
		send_to_char("You cant afford to buy this item.\n\r", ch);
		return;
	    }
	    ch->banked_gold -= obj->cost;

	    sprintf(buf, "%d", itemnum);
	    (redit_oreset) (ch, buf);
	    edit_done(ch);
	    send_to_char("That item has been added to the room.\n\r", ch);
	    send_to_char("Don't forget to save. Use design save.\n\r", ch);
	    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
	    return;
	} else if (!str_cmp(arg2, "remove")) {
	    RESET_DATA *pReset;
	    ROOM_INDEX_DATA *pRoom;
	    OBJ_DATA *pObj;
	    int cnt;

	    cnt = 1;

	    pRoom = ch->in_room;

	    if (!pRoom->reset_first) {
		send_to_char("No items to remove found.\n\r", ch);
		return;
	    }
	    for (pReset = ch->in_room->reset_first; pReset; pReset = pReset->next) {
		if (pReset->arg1 == itemnum) {
		    break;
		}
		cnt++;
	    }

	    /* search for the object in the room and remove it */
	    for (pObj = pRoom->contents; pObj != NULL; pObj = pObj->next_content) {
		if (pObj->pIndexData->vnum == itemnum) {
		    obj_from_room(pObj);
		    extract_obj(pObj);
		}
	    }

	    if (!IS_IMMORTAL(ch))
		ch->pcdata->security = 10;

	    sprintf(buf, "%d delete", cnt);
	    do_resets(ch, buf);

	    if (!IS_IMMORTAL(ch))
		ch->pcdata->security = 0;

	    send_to_char("That item has been removed from this room.\n\r", ch);
	    send_to_char("Don't forget to save. Use design save.\n\r", ch);
	    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);

	    return;
	} else {
	    send_to_char("You must choose add or remove.\n\r", ch);
	    return;
	}

	return;
    }
    send_to_char("Sorry didn't understand command.\n\r", ch);

    /* dont forget to call a save */

}

/* Room Interpreter, called by do_redit. */
void redit(CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *pRoom;
    AREA_DATA *pArea;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;
    int value;

    EDIT_ROOM(ch, pRoom);
    pArea = pRoom->area;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    if (!IS_BUILDER(ch, pArea)) {
	send_to_char("REdit:  Insufficient security to modify room.\n\r", ch);
	edit_done(ch);
	return;
    }
    if (!str_cmp(command, "done")) {
	edit_done(ch);
	return;
    }
    if (!IS_BUILDER(ch, pArea)) {
	interpret(ch, arg);
	return;
    }
    if (command[0] == '\0') {
	redit_show(ch, argument);
	return;
    }
    if ((value = flag_value(room_flags, command)) != NO_FLAG) {
	TOGGLE_BIT(pRoom->room_flags, value);

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Room flag toggled.\n\r", ch);
	return;
    }
    if ((value = flag_value(sector_flags, command)) != NO_FLAG) {
	pRoom->sector_type = value;

	SET_BIT(pArea->area_flags, AREA_CHANGED);
	send_to_char("Sector type set.\n\r", ch);
	return;
    }
    /* Search Table and Dispatch Command. */
    for (cmd = 0; redit_table[cmd].name != NULL; cmd++) {
	if (!str_prefix(command, redit_table[cmd].name)) {
	    if ((*redit_table[cmd].olc_fun) (ch, argument)) {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		return;
	    } else
		return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
}

/* Object Interpreter, called by do_oedit. */
void oedit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObj;
    char arg[MAX_STRING_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;
/*  int  value;   ROM */

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    EDIT_OBJ(ch, pObj);
    pArea = pObj->area;

    if (!IS_BUILDER(ch, pArea)) {
	send_to_char("OEdit: Insufficient security to modify area.\n\r", ch);
	edit_done(ch);
	return;
    }
    if (!str_cmp(command, "done")) {
	edit_done(ch);
	return;
    }
    if (!IS_BUILDER(ch, pArea)) {
	interpret(ch, arg);
	return;
    }
    if (command[0] == '\0') {
	oedit_show(ch, argument);
	return;
    }
    /* Search Table and Dispatch Command. */
    for (cmd = 0; oedit_table[cmd].name != NULL; cmd++) {
	if (!str_prefix(command, oedit_table[cmd].name)) {
	    if ((*oedit_table[cmd].olc_fun) (ch, argument)) {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		return;
	    } else
		return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
}

/* Mobile Interpreter, called by do_medit. */
void medit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int cmd;
/*  int  value;    ROM */

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument(argument, command);

    EDIT_MOB(ch, pMob);
    pArea = pMob->area;

    if (!IS_BUILDER(ch, pArea)) {
	send_to_char("MEdit: Insufficient security to modify area.\n\r", ch);
	edit_done(ch);
	return;
    }
    if (!str_cmp(command, "done")) {
	edit_done(ch);
	return;
    }
    if (!IS_BUILDER(ch, pArea)) {
	interpret(ch, arg);
	return;
    }
    if (command[0] == '\0') {
	medit_show(ch, argument);
	return;
    }
    /* Search Table and Dispatch Command. */
    for (cmd = 0; medit_table[cmd].name != NULL; cmd++) {
	if (!str_prefix(command, medit_table[cmd].name)) {
	    if ((*medit_table[cmd].olc_fun) (ch, argument)) {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		return;
	    } else
		return;
	}
    }

    /* Default to Standard Interpreter. */
    interpret(ch, arg);
    return;
}

const struct editor_cmd_type editor_table[] =
{
/*  {   command         function        }, */

    {"area", do_aedit},
    {"room", do_redit},
    {"object", do_oedit},
    {"mobile", do_medit},
    {"mpcode", do_mpedit},

    {NULL, 0,}
};

/* Entry point for all editors. */
void do_olc(CHAR_DATA * ch, char *argument)
{
    char command[MAX_INPUT_LENGTH];
    int cmd;

    argument = one_argument(argument, command);

    if (command[0] == '\0') {
	do_help(ch, "olc");
	return;
    }
    /* Search Table and Dispatch Command. */
    for (cmd = 0; editor_table[cmd].name != NULL; cmd++) {
	if (!str_prefix(command, editor_table[cmd].name)) {
	    (*editor_table[cmd].do_fun) (ch, argument);
	    return;
	}
    }

    /* Invalid command, send help. */
    do_help(ch, "olc");
    return;
}

/* Entry point for editing area_data. */
void do_aedit(CHAR_DATA * ch, char *argument)
{
    AREA_DATA *pArea;
    int value;
    char value2[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];

    pArea = ch->in_room->area;

    argument = one_argument(argument, arg);
    if (is_number(arg)) {
	value = atoi(arg);
	if (!(pArea = get_area_data(value))) {
	    send_to_char("That area vnum does not exist.\n\r", ch);
	    return;
	}
    } else {
	if (!str_cmp(arg, "create")) {
	    if (!IS_NPC(ch) && (ch->pcdata->security < 10)) {
		send_to_char("Your security is to low. You must get assigned this area.\n\r", ch);
		return;
	    }
	    argument = one_argument(argument, value2);
	    value = atoi(value2);
	    if (get_area_data(value) != NULL) {
		send_to_char("Esa area ya existe!", ch);
		return;
	    }
	    pArea = new_area();
	    area_last->next = pArea;
	    area_last = pArea;	/* Thanks, Walker. */
	    SET_BIT(pArea->area_flags, AREA_ADDED);
	    send_to_char("Area created.\n\r", ch);
	}
    }

    if (!IS_BUILDER(ch, pArea)) {
	send_to_char("Insuficiente seguridad para editar areas.\n\r", ch);
	return;
    }
    ch->desc->pEdit = (void *) pArea;
    ch->desc->editor = ED_AREA;
    return;
}

/* Entry point for editing room_index_data. */
void do_redit(CHAR_DATA * ch, char *argument)
{
    ROOM_INDEX_DATA *pRoom, *pRoom2;
    char arg1[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg1);

    pRoom = ch->in_room;

    if (!str_cmp(arg1, "reset")) {
	if (!IS_BUILDER(ch, pRoom->area)) {
	    send_to_char("Insuficiente seguridad para modificar cuartos.\n\r", ch);
	    return;
	}
	reset_room(pRoom);
	send_to_char("Room reset.\n\r", ch);
	return;
    } else if (!str_cmp(arg1, "create")) {
	if (argument[0] == '\0' || atoi(argument) == 0) {
	    send_to_char("Syntax:  edit room create [vnum]\n\r", ch);
	    return;
	}
	if (redit_create(ch, argument)) {
	    char_from_room(ch);
	    char_to_room(ch, ch->desc->pEdit);
	    SET_BIT(pRoom->area->area_flags, AREA_CHANGED);
	    pRoom = ch->in_room;
	}
    } else {
	pRoom2 = get_room_index(atoi(arg1));

	if ((pRoom2 != NULL) && IS_BUILDER(ch, pRoom2->area)) {
	    char_from_room(ch);
	    char_to_room(ch, pRoom2);
	    pRoom = ch->in_room;
	} else if (atoi(arg1) != 0) {
	    send_to_char("Insuficiente seguridad para editar cuarto, o cuarto inexistente.\n\r", ch);
	    return;
	}
    }

    if (!IS_BUILDER(ch, pRoom->area)) {
	send_to_char("Insuficiente seguridad para modificar cuartos.\n\r", ch);
	return;
    }
    ch->desc->editor = ED_ROOM;
    return;
}

/* Entry point for editing obj_index_data. */
void do_oedit(CHAR_DATA * ch, char *argument)
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    char arg1[MAX_STRING_LENGTH];
    int value;

    if (IS_NPC(ch))
	return;

    argument = one_argument(argument, arg1);

    if (is_number(arg1)) {
	value = atoi(arg1);
	if (!(pObj = get_obj_index(value))) {
	    send_to_char("OEdit:  That vnum does not exist.\n\r", ch);
	    return;
	}
	if (!IS_BUILDER(ch, pObj->area)) {
	    send_to_char("Insuficiente seguridad para modificar objetos.\n\r", ch);
	    return;
	}
      ch->build_vnum = value;
	ch->desc->pEdit = (void *) pObj;
	ch->desc->editor = ED_OBJECT;
	return;
    } else {
	if (!str_cmp(arg1, "create")) {
	    value = atoi(argument);
	    if (argument[0] == '\0' || value == 0) {
		send_to_char("Syntax:  edit object create [vnum]\n\r", ch);
		return;
	    }
	    pArea = get_vnum_area(value);

	    if (!pArea) {
		send_to_char("OEdit:  That vnum is not assigned an area.\n\r", ch);
		return;
	    }
	    if (!IS_BUILDER(ch, pArea)) {
		send_to_char("Insuficiente seguridad para modificar objetos.\n\r", ch);
		return;
	    }
	    if (oedit_create(ch, argument)) {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		ch->desc->editor = ED_OBJECT;
	    }
	    return;
	}
    }

    send_to_char("OEdit:  There is no default object to edit.\n\r", ch);
    return;
}

/* Entry point for editing mob_index_data. */
void do_medit(CHAR_DATA * ch, char *argument)
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int value;
    char arg1[MAX_STRING_LENGTH];

    argument = one_argument(argument, arg1);

    if (is_number(arg1)) {
	value = atoi(arg1);
	if (!(pMob = get_mob_index(value))) {
	    send_to_char("MEdit:  That vnum does not exist.\n\r", ch);
	    return;
	}
	if (!IS_BUILDER(ch, pMob->area)) {
	    send_to_char("Insuficiente seguridad para modificar mobs.\n\r", ch);
	    return;
	}
      ch->build_vnum = value;
	ch->desc->pEdit = (void *) pMob;
	ch->desc->editor = ED_MOBILE;
	return;
    } else {
	if (!str_cmp(arg1, "create")) {
	    value = atoi(argument);
	    if (arg1[0] == '\0' || value == 0) {
		send_to_char("Syntax:  edit mobile create [vnum]\n\r", ch);
		return;
	    }
	    pArea = get_vnum_area(value);

	    if (!pArea) {
		send_to_char("OEdit:  That vnum is not assigned an area.\n\r", ch);
		return;
	    }
	    if (!IS_BUILDER(ch, pArea)) {
		send_to_char("Insuficiente seguridad para modificar mobs.\n\r", ch);
		return;
	    }
	    if (medit_create(ch, argument)) {
		SET_BIT(pArea->area_flags, AREA_CHANGED);
		ch->desc->editor = ED_MOBILE;
	    }
	    return;
	}
    }

    send_to_char("MEdit:  There is no default mobile to edit.\n\r", ch);
    return;
}

void display_resets(CHAR_DATA * ch)
{
    ROOM_INDEX_DATA *pRoom;
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pMob = NULL;
    char buf[MAX_STRING_LENGTH];
    char final[MAX_STRING_LENGTH];
    int iReset = 0;

    EDIT_ROOM(ch, pRoom);
    final[0] = '\0';

    send_to_char(
		    " No.  Loads    Description       Location         Vnum   Mx Mn Description"
		    "\n\r"
		    "==== ======== ============= =================== ======== ===== ==========="
		    "\n\r", ch);

    for (pReset = pRoom->reset_first; pReset; pReset = pReset->next) {
	OBJ_INDEX_DATA *pObj;
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_INDEX_DATA *pObjToIndex;
	ROOM_INDEX_DATA *pRoomIndex;

	final[0] = '\0';
	sprintf(final, "[%2d] ", ++iReset);

	switch (pReset->command) {
	default:
	    sprintf(buf, "Bad reset command: %c.", pReset->command);
	    strcat(final, buf);
	    break;

	case 'M':
	    if (!(pMobIndex = get_mob_index(pReset->arg1))) {
		sprintf(buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1);
		strcat(final, buf);
		continue;
	    }
	    if (!(pRoomIndex = get_room_index(pReset->arg3))) {
		sprintf(buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3);
		strcat(final, buf);
		continue;
	    }
	    pMob = pMobIndex;
	    sprintf(buf, "M[%5d] %-13.13s in room             R[%5d] %2d-%2d %-15.15s\n\r",
		    pReset->arg1, pMob->short_descr, pReset->arg3,
		    pReset->arg2, pReset->arg4, pRoomIndex->name);
	    strcat(final, buf);

	    /*
	     * Check for pet shop.
	     * -------------------
	     */
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;

		pRoomIndexPrev = get_room_index(pRoomIndex->vnum - 1);
		if (pRoomIndexPrev
		    && IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP))
		    final[5] = 'P';
	    }

	    break;

	case 'O':
	    if (!(pObjIndex = get_obj_index(pReset->arg1))) {
		sprintf(buf, "Load Object - Bad Object %d\n\r",
			pReset->arg1);
		strcat(final, buf);
		continue;
	    }
	    pObj = pObjIndex;

	    if (!(pRoomIndex = get_room_index(pReset->arg3))) {
		sprintf(buf, "Load Object - Bad Room %d\n\r", pReset->arg3);
		strcat(final, buf);
		continue;
	    }
	    sprintf(buf, "O[%5d] %-13.13s in room             "
		    "R[%5d]       %-15.15s\n\r",
		    pReset->arg1, pObj->short_descr,
		    pReset->arg3, pRoomIndex->name);
	    strcat(final, buf);

	    break;

	case 'P':
	    if (!(pObjIndex = get_obj_index(pReset->arg1))) {
		sprintf(buf, "Put Object - Bad Object %d\n\r",
			pReset->arg1);
		strcat(final, buf);
		continue;
	    }
	    pObj = pObjIndex;

	    if (!(pObjToIndex = get_obj_index(pReset->arg3))) {
		sprintf(buf, "Put Object - Bad To Object %d\n\r",
			pReset->arg3);
		strcat(final, buf);
		continue;
	    }
	    sprintf(buf,
		    "O[%5d] %-13.13s inside              O[%5d] %2d-%2d %-15.15s\n\r",
		    pReset->arg1,
		    pObj->short_descr,
		    pReset->arg3,
		    pReset->arg2,
		    pReset->arg4,
		    pObjToIndex->short_descr);
	    strcat(final, buf);

	    break;

	case 'G':
	case 'E':
	    if (!(pObjIndex = get_obj_index(pReset->arg1))) {
		sprintf(buf, "Give/Equip Object - Bad Object %d\n\r",
			pReset->arg1);
		strcat(final, buf);
		continue;
	    }
	    pObj = pObjIndex;

	    if (!pMob) {
		sprintf(buf, "Give/Equip Object - No Previous Mobile\n\r");
		strcat(final, buf);
		break;
	    }
	    if (pMob->pShop) {
		sprintf(buf,
			"O[%5d] %-13.13s in the inventory of S[%5d]       %-15.15s\n\r",
			pReset->arg1,
			pObj->short_descr,
			pMob->vnum,
			pMob->short_descr);
	    } else
		sprintf(buf,
			"O[%5d] %-13.13s %-19.19s M[%5d]       %-15.15s\n\r",
			pReset->arg1,
			pObj->short_descr,
			(pReset->command == 'G') ?
			flag_string(wear_loc_strings, WEAR_NONE)
			: flag_string(wear_loc_strings, pReset->arg3),
			pMob->vnum,
			pMob->short_descr);
	    strcat(final, buf);

	    break;

	    /*
	     * Doors are set in rs_flags don't need to be displayed.
	     * If you want to display them then uncomment the new_reset
	     * line in the case 'D' in load_resets in db.c and here.
	     */
	case 'D':
	    pRoomIndex = get_room_index(pReset->arg1);
	    sprintf(buf, "R[%5d] %s door of %-19.19s reset to %s\n\r",
		    pReset->arg1,
		    capitalize(dir_name[pReset->arg2]),
		    pRoomIndex->name,
		    flag_string(door_resets, pReset->arg3));
	    strcat(final, buf);

	    break;
	    /*
	     * End Doors Comment.
	     */
	case 'R':
	    if (!(pRoomIndex = get_room_index(pReset->arg1))) {
		sprintf(buf, "Randomize Exits - Bad Room %d\n\r",
			pReset->arg1);
		strcat(final, buf);
		continue;
	    }
	    sprintf(buf, "R[%5d] Exits are randomized in %s\n\r",
		    pReset->arg1, pRoomIndex->name);
	    strcat(final, buf);

	    break;
	}
	send_to_char(final, ch);
    }

    return;
}

/*****************************************************************************
 Name:		add_reset
 Purpose:	Inserts a new reset in the given index slot.
 Called by:	do_resets(olc.c).
 ****************************************************************************/
void add_reset(ROOM_INDEX_DATA * room, RESET_DATA * pReset, int index)
{
    RESET_DATA *reset;
    int iReset = 0;

    if (!room->reset_first) {
	room->reset_first = pReset;
	room->reset_last = pReset;
	pReset->next = NULL;
	return;
    }
    index--;

    if (index == 0) {		/* First slot (1) selected. */
	pReset->next = room->reset_first;
	room->reset_first = pReset;
	return;
    }
    /*
     * If negative slot( <= 0 selected) then this will find the last.
     */
    for (reset = room->reset_first; reset->next; reset = reset->next) {
	if (++iReset == index)
	    break;
    }

    pReset->next = reset->next;
    reset->next = pReset;
    if (!pReset->next)
	room->reset_last = pReset;
    return;
}

void do_resets(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MAX_INPUT_LENGTH];
    char arg5[MAX_INPUT_LENGTH];
    char arg6[MAX_INPUT_LENGTH];
    char arg7[MAX_INPUT_LENGTH];
    RESET_DATA *pReset = NULL;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);
    argument = one_argument(argument, arg4);
    argument = one_argument(argument, arg5);
    argument = one_argument(argument, arg6);
    argument = one_argument(argument, arg7);

    if (!IS_BUILDER(ch, ch->in_room->area)) {
	send_to_char("Resets: Invalid security for editing this area.\n\r",
		     ch);
	return;
    }
    /*
     * Display resets in current room.
     * -------------------------------
     */
    if (arg1[0] == '\0') {
	if (ch->in_room->reset_first) {
	    send_to_char(
			    "Resets: M = mobile, R = room, O = object, "
			    "P = pet, S = shopkeeper\n\r", ch);
	    display_resets(ch);
	} else
	    send_to_char("No resets in this room.\n\r", ch);
    }
    /*
     * Take index number and search for commands.
     * ------------------------------------------
     */
    if (is_number(arg1)) {
	ROOM_INDEX_DATA *pRoom = ch->in_room;

	/*
	 * Delete a reset.
	 * ---------------
	 */
	if (!str_cmp(arg2, "delete")) {
	    int insert_loc = atoi(arg1);

	    if (!ch->in_room->reset_first) {
		send_to_char("No resets in this area.\n\r", ch);
		return;
	    }
	    if (insert_loc - 1 <= 0) {
		pReset = pRoom->reset_first;
		pRoom->reset_first = pRoom->reset_first->next;
		if (!pRoom->reset_first)
		    pRoom->reset_last = NULL;
	    } else {
		int iReset = 0;
		RESET_DATA *prev = NULL;

		for (pReset = pRoom->reset_first;
		     pReset;
		     pReset = pReset->next) {
		    if (++iReset == insert_loc)
			break;
		    prev = pReset;
		}

		if (!pReset) {
		    send_to_char("Reset not found.\n\r", ch);
		    return;
		}
		if (prev)
		    prev->next = prev->next->next;
		else
		    pRoom->reset_first = pRoom->reset_first->next;

		for (pRoom->reset_last = pRoom->reset_first;
		     pRoom->reset_last->next;
		     pRoom->reset_last = pRoom->reset_last->next);
	    }

	    free_reset_data(pReset);
	    send_to_char("Reset deleted.\n\r", ch);
	} else
	    /*
	     * Add a reset.
	     * ------------
	     */
	    if ((!str_cmp(arg2, "mob") && is_number(arg3))
		|| (!str_cmp(arg2, "obj") && is_number(arg3))) {
	    /*
	     * Check for Mobile reset.
	     * -----------------------
	     */
	    if (!str_cmp(arg2, "mob")) {
		if (get_mob_index(is_number(arg3) ? atoi(arg3) : 1) == NULL) {
		    send_to_char("Mob no existe.\n\r", ch);
		    return;
		}
		pReset = new_reset_data();
		pReset->command = 'M';
		pReset->arg1 = atoi(arg3);
		pReset->arg2 = is_number(arg4) ? atoi(arg4) : 1;	/* Max # */
		pReset->arg3 = ch->in_room->vnum;
		pReset->arg4 = is_number(arg5) ? atoi(arg5) : 1;	/* Min # */
	    } else
		/*
		 * Check for Object reset.
		 * -----------------------
		 */
	    if (!str_cmp(arg2, "obj")) {
		pReset = new_reset_data();
		pReset->arg1 = atoi(arg3);
		/*
		 * Inside another object.
		 * ----------------------
		 */
		if (!str_prefix(arg4, "inside")) {
		    OBJ_INDEX_DATA *temp;

		    temp = get_obj_index(is_number(arg5) ? atoi(arg5) : 1);
		    if ((temp->item_type != ITEM_CONTAINER) &&
			(temp->item_type != ITEM_CORPSE_NPC)) {
			send_to_char("Objeto 2 no es container.\n\r", ch);
			return;
		    }
		    pReset->command = 'P';
		    pReset->arg2 = is_number(arg6) ? atoi(arg6) : 1;
		    pReset->arg3 = is_number(arg5) ? atoi(arg5) : 1;
		    pReset->arg4 = is_number(arg7) ? atoi(arg7) : 1;
		} else
		    /*
		     * Inside the room.
		     * ----------------
		     */
		if (!str_cmp(arg4, "room")) {
		    if (get_obj_index(atoi(arg3)) == NULL) {
			send_to_char("Vnum no existe.\n\r", ch);
			return;
		    }
		    pReset->command = 'O';
		    pReset->arg2 = 0;
		    pReset->arg3 = ch->in_room->vnum;
		    pReset->arg4 = 0;
		} else
		    /*
		     * Into a Mobile's inventory.
		     * --------------------------
		     */
		{
		    if (flag_value(wear_loc_flags, arg4) == NO_FLAG) {
			send_to_char("Resets: '? wear-loc'\n\r", ch);
			return;
		    }
		    if (get_obj_index(atoi(arg3)) == NULL) {
			send_to_char("Vnum no existe.\n\r", ch);
			return;
		    }
		    pReset->arg1 = atoi(arg3);
		    pReset->arg3 = flag_value(wear_loc_flags, arg4);
		    if (pReset->arg3 == WEAR_NONE)
			pReset->command = 'G';
		    else
			pReset->command = 'E';
		}
	    }
	    add_reset(ch->in_room, pReset, atoi(arg1));
	    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
	    send_to_char("Reset added.\n\r", ch);
	} else if (!str_cmp(arg2, "random") && is_number(arg3)) {
	    if (atoi(arg3) < 1 || atoi(arg3) > 6) {
		send_to_char("Invalid argument.\n\r", ch);
		return;
	    }
	    pReset = new_reset_data();
	    pReset->command = 'R';
	    pReset->arg1 = ch->in_room->vnum;
	    pReset->arg2 = atoi(arg3);
	    add_reset(ch->in_room, pReset, atoi(arg1));
	    SET_BIT(ch->in_room->area->area_flags, AREA_CHANGED);
	    send_to_char("Random exits reset added.\n\r", ch);
	} else {
	    send_to_char("Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch);
	    send_to_char("        RESET <number> OBJ <vnum> inside <vnum> [limit] [count]\n\r", ch);
	    send_to_char("        RESET <number> OBJ <vnum> room\n\r", ch);
	    send_to_char("        RESET <number> MOB <vnum> [max #x area] [max #x room]\n\r", ch);
	    send_to_char("        RESET <number> DELETE\n\r", ch);
	    send_to_char("        RESET <number> RANDOM [#x exits]\n\r", ch);
	}
    }
    return;
}

/*****************************************************************************
 Name:		do_alist
 Purpose:	Normal command to list areas and display area information.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_alist(CHAR_DATA * ch, char *argument)
{
    BUFFER *output;
    char buf[MAX_STRING_LENGTH];
    char result[MAX_STRING_LENGTH * 2];		/* May need tweaking. */
    AREA_DATA *pArea;

    output = new_buf();

    sprintf(result, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r",
	    "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders");
    send_to_char(result, ch);

    for (pArea = area_first; pArea; pArea = pArea->next) {
	sprintf(buf, "[%3d] %-29.29s (%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
		pArea->vnum,
		pArea->name,
		pArea->min_vnum,
		pArea->max_vnum,
		pArea->file_name,
		pArea->security,
		pArea->builders);
	add_buf(output, buf);
    }
    page_to_char(buf_string(output), ch);
    free_buf(output);
    return;
}
