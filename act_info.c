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
#include <sqlite3.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"

#define MAX_MAP 72
#define MAX_MAP_DIR 4

/* command procedures needed */
DECLARE_DO_FUN(do_exits);
DECLARE_DO_FUN(do_count);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_affects);
DECLARE_DO_FUN(do_play);
DECLARE_DO_FUN(do_noclan);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_remove_all);

char *const where_name[] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on torso>     ",
    "<worn on head>      ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
    "<held>              ",
    "<floating nearby>   ",
    "<secondary weapon>  ",
};

/* for do_count */
int max_on = 0;

char *map[MAX_MAP][MAX_MAP];
int offsets[4][2] =
{
    {-1, 0},
    {0, 1},
    {1, 0},
    {0, -1}};

/*
 * Local functions.
 */
char *format_obj_to_char args((OBJ_DATA * obj, CHAR_DATA * ch, bool fShort));
void show_list_to_char args((OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing));
void show_char_to_char_0 args((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char_1 args((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char args((CHAR_DATA * list, CHAR_DATA * ch));
bool check_blind args((CHAR_DATA * ch));

void MapArea(ROOM_INDEX_DATA * room, CHAR_DATA * ch, int x, int y, int min, int max)
{

    ROOM_INDEX_DATA *prospect_room;
    EXIT_DATA *pexit;
    int door;

    /* marks the room as visited */
    switch (room->sector_type) {
        case SECT_INSIDE:
	map[x][y] = "{W#{x";
	break;
    case SECT_CITY:
	map[x][y] = "{W+{x";
	break;
    case SECT_FIELD:
	map[x][y] = "{G.{x";
	break;
    case SECT_FOREST:
	map[x][y] = "{g@{x";
	break;
    case SECT_HILLS:
	map[x][y] = "{G^{x";
	break;
    case SECT_MOUNTAIN:
	map[x][y] = "{y^{x";
	break;
    case SECT_WATER_SWIM:
	map[x][y] = "{B~{x";
	break;
    case SECT_WATER_NOSWIM:
	map[x][y] = "{b~{x";
	break;
    case SECT_UNUSED:
	map[x][y] = "{DX{x";
	break;
    case SECT_AIR:
	map[x][y] = "{C:{x";
	break;
    case SECT_DESERT:
	map[x][y] = "{Y={x";
	break;
    case SECT_LAVA:
	map[x][y] = "{R~{x"; /*Vashna 03-27-01*/
	break;             /*Minor cosmetic modification*/
    case SECT_ROAD:
	map[x][y] = "{Y+{x"; /*Vashna 04-08-01*/
	break;             /*Minor modification removed*/
    case SECT_UNDERGROUND:
	map[x][y] = "{m={x";
	break;
    case SECT_JUNGLE:
	map[x][y] = "{G@{x";
	break;
    case SECT_DENSE_FOREST:
	map[x][y] = "{g#{x";
	break;
    case SECT_SWAMP:
	map[x][y] = "{m@{x";
	break;
    case SECT_BEACH:
	map[x][y] = "{Y~{x";
	break;
    case SECT_TUNDRA:
	map[x][y] = "{c={x";
	break;
    case SECT_PLAINS:
	map[x][y] = "{y.{x";
	break;
    case SECT_FIRE_ROOM:    /*Vashna 03-07-01*/
        map[x][y] = "{RX{x";  /*Fire room sector added*/
        break;
    case SECT_FROZEN_FOREST:/*Vashna 03-25-01*/
        map[x][y] = "{W@{x";  /*Frozen forest sector added*/
        break;
    case SECT_BOG:          /*Vashna 04-05-01*/
        map[x][y] = "{g~{x";    /*New sectors added*/            
        break;
    case SECT_RUINS:                
        map[x][y] = "{R#{x";                
        break;
    case SECT_HOUSE:                
        map[x][y]="{rH{x";                
        break;
    case SECT_PYRAMID:              
        map[x][y]="{Y^{x";                
        break;
    case SECT_SCRUBLAND:    
        map[x][y]="{Gs{x";                
        break;
    case SECT_OCEAN:        
        map[x][y]="{BO{x";                
        break;
    case SECT_STREET:       
        map[x][y]="{w+{x";                
        break;
    case SECT_ALLEY:        
        map[x][y]="{D+{x";                
        break;
    case SECT_CAVE:         
        map[x][y]="{DC{x";                
        break;
    case SECT_FARMLAND:     
        map[x][y]="{y:{x";                
        break;
    case SECT_CAVERN:       
        map[x][y]="{DO{x";                
        break;
    case SECT_BRIDGE:       
        map[x][y]="{y={x";               
        break;
    case SECT_ICE:  
        map[x][y]="{W={x";                
        break;
    case SECT_BLOOD_RIVER:  
        map[x][y]="{R~{x";                
        break;
    case SECT_WOODED_ROAD:  
        map[x][y]="{G+{x";                
        break;
    case SECT_FROZEN_SEA:   
        map[x][y]="{W~{x";                
        break;
    case SECT_DOCK:         
        map[x][y]="{y%{x";                
        break;
    case SECT_ICE_MOUNTAIN:  
        map[x][y]="{W^{x";                
        break; 
    case SECT_REDOCEAN:
        map[x][y]="{R&{x";
        break;                
    case SECT_DIRTPATH:
        map[x][y]="{y~{x";
        break;
    case SECT_WHEATFIELD:
        map[x][y]="{y!{x";
        break;           
    case SECT_DARKWOOD:
        map[x][y]="{D@{x";
        break;               
    case SECT_WIND:
        map[x][y]="{C#{x";
        break;           
    case SECT_TORNADO:
        map[x][y]="{C@{x";
        break;               
    case SECT_WHIRLPOOL:
        map[x][y]="{B@{x";
        break;               
    case SECT_LAVACHUTE:
        map[x][y]="{R@{x";
        break;           
    case SECT_EARTH:
        map[x][y]="{y&{x";
        break;               
    case SECT_QUICKSAND:
        map[x][y]="{y@{x";
        break;               
    case SECT_DARK_SWAMP:
        map[x][y]="{D%{x";
        break;               
    case SECT_CAVE_FLOOR:
        map[x][y]="{m%{x";
        break;           
    case SECT_BRICK_ROAD:
        map[x][y]="{r={x";
        break;               
    case SECT_BRICK_ROAD2:
        map[x][y]="{r+{x";
        break; 
    case SECT_BCASTLE:
	map[x][y]="{c#{x";
	break;
    case SECT_BCASTLE2:
	map[x][y]="{c+{x";
	break;
    case SECT_BCASTLE3:
	map[x][y]="{c\"{x";
	break;    
    default:
	map[x][y] = "{yo{x";
    }

    for (door = 0; door < MAX_MAP_DIR; door++) {
	if ((pexit = room->exit[door]) != NULL
	    && pexit->u1.to_room != NULL
	    && can_see_room(ch, pexit->u1.to_room)	/* optional */
	    &&!IS_SET(pexit->exit_info, EX_CLOSED)) {
	    prospect_room = pexit->u1.to_room;	/* if exit there */
	    if (prospect_room->exit[rev_dir[door]] && prospect_room->exit[rev_dir[door]]->u1.to_room != room) {
		/* if not two way */
		if ((prospect_room->sector_type == SECT_CITY)
		    || (prospect_room->sector_type == SECT_INSIDE))
		    map[x][y] = "{W@";
		else
		    map[x][y] = "{D?";
		return;
	    }
	    /* end two way */
	    if ((x <= min) || (y <= min) || (x >= max) || (y >= max))
		return;
	    if (map[x + offsets[door][0]][y + offsets[door][1]] == NULL) {
		MapArea(pexit->u1.to_room, ch, x + offsets[door][0], y + offsets[door][1], min, max);
	    }
	}			/* end if exit there */
    }
    return;
}

void ShowMap(CHAR_DATA * ch, int min, int max)
{

    int x, y;

    /*  Let's try having tile inside for a bit

       if(ch->in_room->sector_type == SECT_INSIDE)
       {
       return;  
       }
     */

    for (x = min; x < max; ++x) {
	send_to_char("     ", ch);
	for (y = min; y < max; ++y) {
	    if (map[x][y] == NULL)
		send_to_char(" ", ch);
	    else
		send_to_char(map[x][y], ch);
	}
	send_to_char("\n\r", ch);
    }
    return;
}

void do_mapping(CHAR_DATA * ch, char *argument)
{

    int size, center, x, y, min, max;
    char arg1[10];

    one_argument(argument, arg1);
    size = atoi(arg1);

    size = URANGE(8, size, 72);
    center = MAX_MAP / 2;

    min = MAX_MAP / 2 - size / 2;
    max = MAX_MAP / 2 + size / 2;

    for (x = 0; x < MAX_MAP; ++x)
	for (y = 0; y < MAX_MAP; ++y)
	    map[x][y] = NULL;

    MapArea(ch->in_room, ch, center, center, min, max);

    map[center][center] = "{R*{x";
    ShowMap(ch, min, max);
    return;
}

char *format_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
	|| (obj->description == NULL || obj->description[0] == '\0'))
	return buf;

    if (IS_OBJ_STAT(obj, ITEM_INVIS))
	strcat(buf, "{W({xInvis{W){x ");
    if (IS_AFFECTED(ch, AFF_DETECT_ALIGNMENT)
	&& IS_OBJ_STAT(obj, ITEM_EVIL))
	strcat(buf, "{r({RRed Aura{r){x ");
    if (IS_AFFECTED(ch, AFF_DETECT_ALIGNMENT)
	&& IS_OBJ_STAT(obj, ITEM_BLESS))
	strcat(buf, "{b({BBlue Aura{b){x ");
    if (IS_AFFECTED(ch, AFF_DETECT_MAGIC)
	&& IS_OBJ_STAT(obj, ITEM_MAGIC))
	strcat(buf, "{g({mMagical{g){x ");
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
	strcat(buf, "({CGlowing{x) ");
    if (IS_OBJ_STAT(obj, ITEM_UNGLOW))
	strcat(buf, "{W({DBlack{W){x ");
    if (IS_OBJ_STAT(obj, ITEM_HUM))
	strcat(buf, "({cHumming{x) ");
    if ((obj->item_type == ITEM_FLAG)
	&& obj->value[0] != ch->side)
	strcat(buf, "{R({xRaidable{Y!{R){x ");

    if (fShort) {
	if (obj->short_descr != NULL)
	    strcat(buf, obj->short_descr);
    } else {
	if (obj->description != NULL)
	    strcat(buf, obj->description);
    }

    if (&obj->condition != NULL) {
	if (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_ARMOR) {
	    if (obj->condition <= 0)
		strcat(buf, " {R({rRuined{R){x");
	    else if (obj->condition <= 10)
		strcat(buf, " {r(Battered){x");
	    else if (obj->condition <= 30)
		strcat(buf, " {y(Damaged){x");
	}
    }
    return buf;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */

void show_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing)
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if (ch->desc == NULL)
	return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for (obj = list; obj != NULL; obj = obj->next_content)
	count++;
    prgpstrShow = alloc_mem(count * sizeof(char *));
    prgnShow = alloc_mem(count * sizeof(int));
    nShow = 0;

    /*
     * Format the list of objects.
     */

    for (obj = list; obj != NULL; obj = obj->next_content) {
	if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj)) {
	    pstrShow = format_obj_to_char(obj, ch, fShort);

	    fCombine = FALSE;

	    if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE)) {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for (iShow = nShow - 1; iShow >= 0; iShow--) {
		    if (!strcmp(prgpstrShow[iShow], pstrShow)) {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }
	    /*
	     * Couldn't combine, or didn't want to.
	     */

	    if (!fCombine) {
		prgpstrShow[nShow] = str_dup(pstrShow);
		prgnShow[nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */

    for (iShow = 0; iShow < nShow; iShow++) {
	if (prgpstrShow[iShow][0] == '\0') {
	    free_string(prgpstrShow[iShow]);
	    continue;
	}
	if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE)) {
	    if (prgnShow[iShow] != 1) {
		sprintf(buf, "(%2d) ", prgnShow[iShow]);
		add_buf(output, buf);
	    } else {
		add_buf(output, "     ");
	    }
	}
	add_buf(output, prgpstrShow[iShow]);
	add_buf(output, "\n\r");
	free_string(prgpstrShow[iShow]);
    }

    if (fShowNothing && nShow == 0) {
	if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
	    send_to_char("     ", ch);
	send_to_char("Nothing.\n\r", ch);
    }
    page_to_char(buf_string(output), ch);

    /*
     * Clean up.
     */

    free_buf(output);
    free_mem(prgpstrShow, count * sizeof(char *));
    free_mem(prgnShow, count * sizeof(int));
    return;
}

void show_char_to_char_0(CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH], message[MAX_STRING_LENGTH], pers_buf[MAX_STRING_LENGTH];

    buf[0] = '\0';
	
	//set_prename(victim);
	//set_prename(ch);
	
if (!IS_NPC(victim) && !victim->desc      ) strcat( buf, "{r({RLinkdead{r){x " );
    if (RIDDEN(victim)){
	if (ch != RIDDEN(victim)) {
	    strcat(buf, "(Ridden) ");
	} else {
	    strcat(buf, "(Your mount) ");
	}
    }
    if (IS_NPC(victim) && ch->questmob > 0 &&
	victim->pIndexData->vnum == ch->questmob)
	strcat(buf, "[{WTARGET{x] ");

    if (IS_SET(victim->comm, COMM_AFK))
	strcat(buf, "{b[{gAFK{b]{x ");
    if (IS_AFFECTED(victim, AFF_INVISIBLE))
	strcat(buf, "(Invis) ");
    if (victim->invis_level >= LEVEL_HERO)
	strcat(buf, "{b({BWizi{b){x ");
    if (IS_AFFECTED(victim, AFF_HIDE))
	strcat(buf, "(Hide) ");
    if (IS_AFFECTED(victim, AFF_CHARM))
	strcat(buf, "(Charmed) ");
    if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
	strcat(buf, "{m({MPink Aura{m){x ");
    if (IS_EVIL(victim) && !IS_NPC(victim)
	&& IS_AFFECTED(ch, AFF_DETECT_ALIGNMENT))
	strcat(buf, "{r(Red Aura) {x");
    if (IS_GOOD(victim) && !IS_NPC(victim)
	&& IS_AFFECTED(ch, AFF_DETECT_ALIGNMENT))
	strcat(buf, "{Y(Golden Aura){x ");
    if (IS_AFFECTED(victim, AFF_SANCTUARY))
	strcat(buf, "{W(White Aura){x ");
    if (IS_AFFECTED(victim, AFF_REFLECT))
	strcat(buf, "{w(Silver Aura){x ");
	if (is_affected(victim, skill_lookup("decay aura")))
	strcat(buf, "{w({xDecay Aura{w){x ");
	if (is_affected(victim, skill_lookup("enhance aura")))
	strcat(buf, "{C({cEnhance Aura{C){x ");
	if (IS_AFFECTED2(victim, AFF_CHAOTIC))
	strcat(buf, "{M(Chaotic){x ");
	if (IS_AFFECTED2(victim, AFF_DEFLECT))
	strcat(buf, "{C(Deflective){x ");
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER))
	strcat(buf, "{R(TRAITOR) {x");
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF))
	strcat(buf, "{R({DC{xR{DI{xM{DI{xN{DA{xL{R) {x");
    if (!IS_NPC(victim) && (victim->pcdata->judge == 3))
	strcat(buf, "{r(Mercenary){x ");
    if (!IS_NPC(victim) && victim->side == SIDE_NONE && IS_AFFECTED(ch, AFF_DETECT_ALIGNMENT))
        strcat(buf, "{B(Blue Aura){x ");
    if (victim->position == victim->start_pos && !IS_SET(victim->act, PLR_SEVERED) && victim->long_descr[0] != '\0') {
	strcat(buf, victim->long_descr);
	send_to_char(buf, ch);
	return;
    }
    PERS(victim, ch, pers_buf);
    strcat(buf, pers_buf);

    if ((IS_SET(victim->act, PLR_SEVERED)) && !IS_NPC(victim)) {
	sprintf(buf, "%s's upper torso is here, twitching.\n\r", victim->name);
	send_to_char(buf, ch);
	return;
    }
    switch (victim->position) {
    case POS_DEAD:
	strcat(buf, " is DEAD!!");
	break;
    case POS_MORTAL:
	strcat(buf, " is mortally wounded.");
	break;
    case POS_INCAP:
	strcat(buf, " is incapacitated.");
	break;
    case POS_STUNNED:
	strcat(buf, " is lying here stunned.");
	break;
    case POS_SLEEPING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], SLEEP_AT)) {
		sprintf(message, " is sleeping at %s.", victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], SLEEP_ON)) {
		sprintf(message, " is sleeping on %s.", victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is sleeping in %s.", victim->on->short_descr);
		strcat(buf, message);
	    }
	} else
	    strcat(buf, " is sleeping here.");
	break;

    case POS_RESTING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], REST_AT)) {
		sprintf(message, " is resting at %s.", victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], REST_ON)) {
		sprintf(message, " is resting on %s.", victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is resting in %s.", victim->on->short_descr);
		strcat(buf, message);
	    }
	} else
	    strcat(buf, " is resting here.");
	break;

    case POS_SITTING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], SIT_AT)) {
		sprintf(message, " is sitting at %s.", victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], SIT_ON)) {
		sprintf(message, " is sitting on %s.", victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is sitting in %s.", victim->on->short_descr);
		strcat(buf, message);
	    }
	} else
	    strcat(buf, " is sitting here.");
	break;

    case POS_STANDING:
	if (victim->on != NULL) {
	    if (IS_SET(victim->on->value[2], STAND_AT)) {
		sprintf(message, " is standing at %s.",victim->on->short_descr);
		strcat(buf, message);
	    } else if (IS_SET(victim->on->value[2], STAND_ON)) {
		sprintf(message, " is standing on %s.", victim->on->short_descr);
		strcat(buf, message);
	    } else {
		sprintf(message, " is standing in %s.", victim->on->short_descr);
		strcat(buf, message);
	    }
	} else if (MOUNTED(victim)) {
	    strcat(buf, " is here, riding ");
	    strcat(buf, MOUNTED(victim)->short_descr);

	    if (!can_see(victim, ch))
		strcat(buf, ".  They look disoriented.");
	    else
		strcat(buf, ". ");
	} else {
	    strcat(buf, " is here.");
	}
	break;

    case POS_FIGHTING:
	strcat(buf, " is here, fighting ");
	if (victim->fighting == NULL) {
	    strcat(buf, "thin air??");
	    stop_fighting(victim, FALSE);
	} else if (victim->fighting == ch)
	    strcat(buf, "YOU!");
	else if (victim->in_room == victim->fighting->in_room) {
	    PERS(victim->fighting, ch, pers_buf);
	    strcat(buf, pers_buf);
	    strcat(buf, ".");
	} else
	    strcat(buf, "someone who left??");
	break;
    }

    strcat(buf, "\n\r");
    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);
    return;
}

void show_char_to_char_1(CHAR_DATA * victim, CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    buf[0] = '\0';

    if (can_see(victim, ch)) {
	if (ch == victim)
	    act("$n looks at $mself.", ch, NULL, NULL, TO_ROOM);
	else {
	    act("$n looks at you.", ch, NULL, victim, TO_VICT);
	    act("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
	}
    }
    if (victim->description[0] != '\0') {
	send_to_char(victim->description, ch);
    } else {
	act("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
    }

    if (MOUNTED(victim)) {
	sprintf(buf, "%s is riding %s.\n\r", victim->name, MOUNTED(victim)->short_descr);
	send_to_char(buf, ch);
    } else if (RIDDEN(victim)) {
	sprintf(buf, "%s is being ridden by %s.\n\r", victim->short_descr, RIDDEN(victim)->name);
	send_to_char(buf, ch);
    }
    if (victim->max_hit > 0)
	percent = (100 * victim->hit) / victim->max_hit;
    else
	percent = -1;

    PERS(victim, ch, pers_buf);
    strcat(buf, pers_buf);

    if (percent >= 100)
	strcat(buf, " is in excellent condition.\n\r");
    else if (percent >= 90)
	strcat(buf, " has a few scratches.\n\r");
    else if (percent >= 75)
	strcat(buf, " has some small wounds and bruises.\n\r");
    else if (percent >= 50)
	strcat(buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)
	strcat(buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
	strcat(buf, " looks pretty hurt.\n\r");
    else if (percent >= 0)
	strcat(buf, " is in awful condition.\n\r");
    else
	strcat(buf, " is bleeding to death.\n\r");

    buf[0] = UPPER(buf[0]);
    send_to_char(buf, ch);
    /* clear the buffer here */

    found = FALSE;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
	if ((obj = get_eq_char(victim, iWear)) != NULL && can_see_obj(ch, obj)) {
	    if (!found) {
		send_to_char("\n\r", ch);
		act("$N is using:", ch, NULL, victim, TO_CHAR);
		found = TRUE;
	    }
	    send_to_char(where_name[iWear], ch);
	    send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
	    send_to_char("\n\r", ch);
	}
    }

    if (victim != ch && !IS_NPC(ch) && number_percent() < get_skill(ch, gsn_peek)) {
	send_to_char("\n\rYou peek at the inventory:\n\r", ch);
	check_improve(ch, gsn_peek, TRUE, 4);
	show_list_to_char(victim->carrying, ch, TRUE, TRUE);
    }
    return;
}

void show_char_to_char(CHAR_DATA * list, CHAR_DATA * ch)
{
    CHAR_DATA *rch;
    OBJ_DATA  *obj;

    for (rch = list; rch != NULL; rch = rch->next_in_room) {
	if (rch == ch || (RIDDEN(rch) && rch->in_room == RIDDEN(rch)->in_room && RIDDEN(rch) != ch))
	    continue;

	if (get_trust(ch) < rch->invis_level)
	    continue;

	if (can_see(ch, rch)) {
	    show_char_to_char_0(rch, ch);
	    if (MOUNTED(rch) && (rch->in_room == MOUNTED(rch)->in_room))
		show_char_to_char_0(MOUNTED(rch), ch);
	} else
           if (room_is_dark(ch->in_room) )
             {
                 if (!IS_AFFECTED(ch, AFF_INFRARED) &&
                !IS_SET(ch->vuln_flags, VULN_SUN)
                && ((obj = get_eq_char(ch, WEAR_LIGHT)) == NULL ||
                obj->item_type != ITEM_LIGHT ||
                obj->value[2] == 0) )
	        send_to_char("{rSomething or someone is here...{x\n\r", ch);
             }
    }
    return;
}

bool check_blind(CHAR_DATA * ch)
{

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
	return TRUE;

    if (IS_AFFECTED(ch, AFF_BLIND)) {
	send_to_char("You can't see a thing!\n\r", ch);
	return FALSE;
    }
    return TRUE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	if (ch->lines == 0)
	    send_to_char("You do not page long messages.\n\r", ch);
	else {
	    sprintf(buf, "You currently display %d lines per page.\n\r", ch->lines + 2);
	    send_to_char(buf, ch);
	}
	return;
    }
    if (!is_number(arg)) {
	send_to_char("You must provide a number.\n\r", ch);
	return;
    }
    lines = atoi(arg);

//disabled do to the possibility of causing buffer overflow (crash bugs, help c) 
//    if (lines == 0) {
//	send_to_char("Paging disabled.\n\r", ch);
//	ch->lines = 0;
//	return;
//    }
    if (lines < 10 || lines > 100) {
	send_to_char("You must provide a reasonable number.\n\r", ch);
	return;
    }
    sprintf(buf, "Scroll set to %d lines.\n\r", lines);
    send_to_char(buf, ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++) {
	sprintf(buf, "%-12s", social_table[iSocial].name);
	send_to_char(buf, ch);
	if (++col % 6 == 0)
	    send_to_char("\n\r", ch);
    }

    if (col % 6 != 0)
	send_to_char("\n\r", ch);
    return;
}

/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "motd");
}

void do_imotd(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "imotd");
}

void do_rules(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "rules");
}

void do_story(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "story");
}

void do_wizlist(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "wizlist");
}

/* RT this following section holds all the auto commands from ROM, as well as replacements for config */

void do_autolist(CHAR_DATA * ch, char *argument)
{
    /* lists most player flags */
    if (IS_NPC(ch))
	return;

    send_to_char("\n\rAction             Description                  Status\n\r", ch);

    send_to_char("{m--------------------------------------------------------{x\n\r", ch);

    send_to_char("autoassist   'Automatically assist in combat'     ", ch);
    if (IS_SET(ch->act, PLR_AUTOASSIST))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("autoexit     'Displays exits when using look'     ", ch);
    if (IS_SET(ch->act, PLR_AUTOEXIT))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("battleprompt 'Provides additional combat info'    ", ch);
    if (IS_SET(ch->act, PLR_EXTRA))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("autogold     'Automatic corpse gold looting'      ", ch);
    if (IS_SET(ch->act, PLR_AUTOGOLD))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("autoloot     'Automatic looting of all corpses'   ", ch);
    if (IS_SET(ch->act, PLR_AUTOLOOT))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("autosac      'Corpses will be sacrificed'         ", ch);
    if (IS_SET(ch->act, PLR_AUTOSAC))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("autosplit    'All gold earned is split w/group'   ", ch);
    if (IS_SET(ch->act, PLR_AUTOSPLIT))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("compact      'All text will display compactly'    ", ch);
    if (IS_SET(ch->comm, COMM_COMPACT))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("brief        'Toggles room descriptions'          ", ch);
    if (IS_SET(ch->comm, COMM_BRIEF))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("prompt       'Current prompt in game'             ", ch);
    if (IS_SET(ch->comm, COMM_PROMPT))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("combine      'Items are combined for you'         ", ch);
    if (IS_SET(ch->comm, COMM_COMBINE))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("nosummon     'Toggle for summoning of players'    ", ch);
    if (IS_SET(ch->act, PLR_NOSUMMON))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("noclan       'Toggle for clan membership'         ", ch);
    if (IS_SET(ch->act, PLR_NOCLAN))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("nofollow     'Toggle to allow following'          ", ch);
    if (IS_SET(ch->act, PLR_NOFOLLOW))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("nospam       'Toggles parry and dodge messages'   ", ch);
    if (IS_SET(ch->act, PLR_NOSPAM))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("tile         'Turns on the tile view traveling'   ", ch);
    if (IS_SET(ch->act, PLR_AUTOTILE))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);

    send_to_char("show         'Toggles affects shown in score'     ", ch);
    if (IS_SET(ch->act, COMM_SHOW_AFFECTS))
	send_to_char("{gON{x\n\r", ch);
    else
	send_to_char("{rOFF{x\n\r", ch);
}

void do_autoassist(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOASSIST)) {
	send_to_char("Autoassist removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOASSIST);
    } else {
	send_to_char("You will now assist when needed.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOASSIST);
    }
}

void do_autoexit(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOEXIT)) {
	send_to_char("Exits will no longer be displayed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOEXIT);
    } else {
	send_to_char("Exits will now be displayed.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOEXIT);
    }
}

void do_extra(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_EXTRA)) {
	send_to_char("You will no longer see the battle prompt.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_EXTRA);
    } else {
	send_to_char("Battle prompt turned on.\n\r", ch);
	send_to_char("It is HIGHLY recommended that you use the %p option with prompt\n\r", ch);
	send_to_char("to customize the location.  HELP 'prompt'\n\r", ch);
	SET_BIT(ch->act, PLR_EXTRA);
    }
}

void do_autogold(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOGOLD)) {
	send_to_char("Autogold removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOGOLD);
    } else {
	send_to_char("Automatic gold looting set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOGOLD);
    }
}

void do_nospam(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_NOSPAM)) {
	send_to_char("NOSPAM removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_NOSPAM);
    } else {
	send_to_char("NOSPAM set You will no longer see parries and dodges..\n\r", ch);
	SET_BIT(ch->act, PLR_NOSPAM);
    }
}

void do_autoloot(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOLOOT)) {
	send_to_char("Autolooting removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOLOOT);
    } else {
	send_to_char("Automatic corpse looting set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOSAC)) {
	send_to_char("Autosacrificing removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOSAC);
    } else {
	send_to_char("Automatic corpse sacrificing set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOSAC);
    }
}

void do_tile(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOTILE)) {
	send_to_char("You will no longer see tiles, resorting to traditional mud screen.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOTILE);
    } else {
	send_to_char("You will now see wonderfull color tiles instead of descriptions.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOTILE);
    }
}

void do_autobuild(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (ch->desc->editor != ED_ROOM) {
	send_to_char("You must be in redit mode to use autobuild.\n\r", ch);
	return;
    }
    if (IS_SET(ch->act, PLR_AUTOBUILD)) {
	send_to_char("Autobuilding turned off.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOBUILD);
    } else {
	send_to_char("Autobuilding activated.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOBUILD);
    }
}

void do_autosplit(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_AUTOSPLIT)) {
	send_to_char("Autosplitting removed.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_AUTOSPLIT);
    } else {
	send_to_char("Automatic gold splitting set.\n\r", ch);
	SET_BIT(ch->act, PLR_AUTOSPLIT);
    }
}

void do_brief(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_BRIEF)) {
	send_to_char("Full descriptions activated.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_BRIEF);
    } else {
	send_to_char("Short descriptions activated.\n\r", ch);
	SET_BIT(ch->comm, COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_COMPACT)) {
	send_to_char("Compact mode removed.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_COMPACT);
    } else {
	send_to_char("Compact mode set.\n\r", ch);
	SET_BIT(ch->comm, COMM_COMPACT);
    }
}

void do_show(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_SHOW_AFFECTS)) {
	send_to_char("Affects will no longer be shown in score.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_SHOW_AFFECTS);
    } else {
	send_to_char("Affects will now be shown in score.\n\r", ch);
	SET_BIT(ch->comm, COMM_SHOW_AFFECTS);
    }
}

void do_prompt(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (argument[0] == '\0') {
	if (IS_SET(ch->comm, COMM_PROMPT)) {
	    send_to_char("You will no longer see prompts.\n\r", ch);
	    REMOVE_BIT(ch->comm, COMM_PROMPT);
	} else {
	    send_to_char("You will now see prompts.\n\r", ch);
	    SET_BIT(ch->comm, COMM_PROMPT);
	}
	return;
    }
    if (!strcmp(argument, "all"))
	strcpy(buf, "<{r%h{xhp {b%m{xm {g%v{xmv> ");
    else {
	if (strlen(argument) > 50)
	    argument[50] = '\0';
	strcpy(buf, argument);
	smash_tilde(buf);
	if (str_suffix("%c", buf))
	    strcat(buf, " ");

    }

    free_string(ch->prompt);
    ch->prompt = str_dup(buf);
    sprintf(buf, "Prompt set to %s\n\r", ch->prompt);
    send_to_char(buf, ch);
    return;
}

void do_combine(CHAR_DATA * ch, char *argument)
{
    if (IS_SET(ch->comm, COMM_COMBINE)) {
	send_to_char("Long inventory selected.\n\r", ch);
	REMOVE_BIT(ch->comm, COMM_COMBINE);
    } else {
	send_to_char("Combined inventory selected.\n\r", ch);
	SET_BIT(ch->comm, COMM_COMBINE);
    }
}

void do_nofollow(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_NOFOLLOW)) {
	send_to_char("You now accept followers.\n\r", ch);
	REMOVE_BIT(ch->act, PLR_NOFOLLOW);
    } else {
	send_to_char("You no longer accept followers.\n\r", ch);
	SET_BIT(ch->act, PLR_NOFOLLOW);
	die_follower(ch);
    }
}

void do_nosummon(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch)) {
	if (IS_SET(ch->imm_flags, IMM_SUMMON)) {
	    send_to_char("You are no longer immune to summon.\n\r", ch);
	    REMOVE_BIT(ch->imm_flags, IMM_SUMMON);
	} else {
	    send_to_char("You are now immune to summoning.\n\r", ch);
	    SET_BIT(ch->imm_flags, IMM_SUMMON);
	}
    } else {
	if (IS_SET(ch->act, PLR_NOSUMMON)) {
	    send_to_char("You are no longer immune to summon.\n\r", ch);
	    REMOVE_BIT(ch->act, PLR_NOSUMMON);
	} else {
	    send_to_char("You are now immune to summoning.\n\r", ch);
	    SET_BIT(ch->act, PLR_NOSUMMON);
	}
    }
}
void do_map(CHAR_DATA * ch)
{
    /* New map routine to take advantage of the tile function. */
    /* Change size to get a different size on the map.         */

    int x, y;
    int size = 30; 
    int center = MAX_MAP / 2;
    int min = center - size / 2;
    int max = center + size / 2;

    send_to_char("\n\rCurrent Location is marked by a {R*{x.\n\r\n\r", ch);

    for (x = 0; x < MAX_MAP; ++x)
	for (y = 0; y < MAX_MAP; ++y)
	    map[x][y] = NULL;

    MapArea(ch->in_room, ch, center, center, min, max);

    /* This pass through the loop strips off the detail */
/* Show the full beauty of the map for now 
    for (x = min; x < max; ++x) {
	for (y = min; y < max; ++y) {
	    if (map[x][y] != NULL) {
		switch (map[x][y][1]) {
		case 'r':
		case 'R':
		    map[x][y] = "{1 {x";
		    break;
		case 'g':
		case 'G':
		    map[x][y] = "{2 {x";
		    break;
		case 'y':
		case 'Y':
		    if (map[x][y][2] != '+')
			map[x][y] = "{3 {x";
		    else
			map[x][y] = "{3{Y+{x";
		    break;
		case 'b':
		case 'B':
		    map[x][y] = "{4 {x";
		    break;
		case 'm':
		case 'M':
		    map[x][y] = "{5 {x";
		    break;
		case 'c':
		case 'C':
		    map[x][y] = "{6 {x";
		    break;
		case 'w':
		case 'W':
		    map[x][y] = "{7 {x";
		    break;
		default:
		    break;
		}
	    }
	}
    }
*/
    map[center][center] = "{R*{x";

    /* This pass through the loop shows the map to the char */

    for (x = min; x < max; ++x) {
	send_to_char("     ", ch);
	for (y = min; y < max; ++y) {
	    if (map[x][y] == NULL)
		send_to_char(" ", ch);
	    else
		send_to_char(map[x][y], ch);
	}
	send_to_char("\n\r", ch);
    }
    return;
}

void do_mapOLD(CHAR_DATA * ch)
{
    /* New map routine to take advantage of the tile function. */
    /* Change size to get a different size on the map.         */

    int x, y;
    int size = 22;
    int center = MAX_MAP / 2;
    int min = center - size / 2;
    int max = center + size / 2;

    send_to_char("\n\rCurrent Location is marked by a {R*{x.\n\r\n\r", ch);

    for (x = 0; x < MAX_MAP; ++x)
	for (y = 0; y < MAX_MAP; ++y)
	    map[x][y] = NULL;

    MapArea(ch->in_room, ch, center, center, min, max);

    /* This pass through the loop strips off the detail */

    for (x = min; x < max; ++x) {
	for (y = min; y < max; ++y) {
	    if (map[x][y] != NULL) {
		switch (map[x][y][1]) {
		case 'r':
		case 'R':
		    map[x][y] = "{1 {x";
		    break;
		case 'g':
		case 'G':
		    map[x][y] = "{2 {x";
		    break;
		case 'y':
		case 'Y':
		    if (map[x][y][2] != '+')
			map[x][y] = "{3 {x";
		    else
			map[x][y] = "{3{Y+{x";
		    break;
		case 'b':
		case 'B':
		    map[x][y] = "{4 {x";
		    break;
		case 'm':
		case 'M':
		    map[x][y] = "{5 {x";
		    break;
		case 'c':
		case 'C':
		    map[x][y] = "{6 {x";
		    break;
		case 'w':
		case 'W':
		    map[x][y] = "{7 {x";
		    break;
		default:
		    break;
		}
	    }
	}
    }

    map[center][center] = "{R*{x";

    /* This pass through the loop shows the map to the char */

    for (x = min; x < max; ++x) {
	send_to_char("     ", ch);
	for (y = min; y < max; ++y) {
	    if (map[x][y] == NULL)
		send_to_char(" ", ch);
	    else
		send_to_char(map[x][y], ch);
	}
	send_to_char("\n\r", ch);
    }
    return;
}

void do_look(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number, count;

    if (ch->desc == NULL)
	return;

    if (ch->position < POS_SLEEPING) {
	send_to_char("You can't see anything but stars!\n\r", ch);
	return;
    }
    if (ch->position == POS_SLEEPING) {
	send_to_char("You can't see anything, you're sleeping!\n\r", ch);
	return;
    }
    if (!check_blind(ch)) {
	send_to_char("You can't see anything, you're blind!\n\r", ch);
	return;
    }
    if ((!IS_NPC(ch) && IS_SET(ch->vuln_flags, VULN_SUN)) &&
	(weather_info.sunlight == SUN_WINTER_LIGHT ||
	 weather_info.sunlight == SUN_SPRING_LIGHT ||
	 weather_info.sunlight == SUN_FALL_LIGHT ||
	 weather_info.sunlight == SUN_SUMMER_LIGHT) &&
	IS_OUTSIDE(ch) &&
	((obj = get_eq_char(ch, WEAR_LIGHT)) == NULL ||
	 obj->item_type != ITEM_DARK ||
	 obj->value[2] == 0 ||
	 ch->in_room->light > 0)) {
	send_to_char("{YYou feel weak in the sunlight..{x\n\r\n\r", ch);
	if (!IS_SET(ch->act, PLR_SUNWEAK))
	    SET_BIT(ch->act, PLR_SUNWEAK);
    } else if (IS_SET(ch->act, PLR_SUNWEAK)) {
	send_to_char("{yAhhh... You feel much better in the dark.{x\n\r\n\r", ch);
	REMOVE_BIT(ch->act, PLR_SUNWEAK);
    }
    if (!IS_NPC(ch)
	&& !IS_SET(ch->act, PLR_HOLYLIGHT)
	&& !IS_AFFECTED(ch, AFF_INFRARED)
	&& !IS_SET(ch->vuln_flags, VULN_SUN)
	&& room_is_dark(ch->in_room)
	&& !IS_OUTSIDE(ch)) {
	send_to_char("It is pitch black ... \n\r", ch);
	return;
    }
    if (!IS_NPC(ch)
	&& !IS_SET(ch->act, PLR_HOLYLIGHT)
	&& !IS_AFFECTED(ch, AFF_INFRARED)
	&& !IS_SET(ch->vuln_flags, VULN_SUN)
	&& room_is_dark(ch->in_room)
	&& IS_OUTSIDE(ch)) {
	send_to_char("It's nighttime and you can't see very well ... get a light! \n\r", ch);
	show_char_to_char(ch->in_room->people, ch);
	return;
    }
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    number = number_argument(arg1, arg3);
    count = 0;

    if (arg1[0] == '\0' || !str_cmp(arg1, "auto")) {
	if (IS_SET(ch->act, PLR_AUTOTILE))
	    do_mapping(ch, "");

	/* 'look' or 'look auto' */
	sprintf(buf, "{y%s{x", ch->in_room->name);
	send_to_char(buf, ch);

	if (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act, PLR_HOLYLIGHT))) {
	    sprintf(buf, " [Room {r%d{x]", ch->in_room->vnum);
	    send_to_char(buf, ch);
	}
	send_to_char("\n\r", ch);

	if (!IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF)) {
	    send_to_char("  ", ch);
	    send_to_char(ch->in_room->description, ch);
	}
	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT)) {
	    do_exits(ch, "auto");
	}
	show_list_to_char(ch->in_room->contents, ch, FALSE, FALSE);
	show_char_to_char(ch->in_room->people, ch);
	return;
    }
    if (!str_cmp(arg1, "i") || !str_cmp(arg1, "in") || !str_cmp(arg1, "on")) {
	/* 'look in' */
	if (arg2[0] == '\0') {
	    send_to_char("Look in what?\n\r", ch);
	    return;
	}
	if ((obj = get_obj_here(ch, arg2)) == NULL) {
	    send_to_char("You do not see that here.\n\r", ch);
	    return;
	}
	switch (obj->item_type) {
	default:
	    send_to_char("That is not a container.\n\r", ch);
	    break;

	case ITEM_DRINK_CON:
	    if (obj->value[1] <= 0) {
		send_to_char("It is empty.\n\r", ch);
		break;
	    }
	    sprintf(buf, "It's %sfilled with a %s liquid.\n\r",
		    obj->value[1] < obj->value[0] / 4
		    ? "less than half-" :
		    obj->value[1] < 3 * obj->value[0] / 4
		    ? "about half-" : "more than half-",
		    liq_table[obj->value[2]].liq_color
		);

	    send_to_char(buf, ch);
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if (IS_SET(obj->value[1], CONT_CLOSED)) {
		send_to_char("It is closed.\n\r", ch);
		break;
	    }
	    act("$p holds:", ch, obj, NULL, TO_CHAR);
	    show_list_to_char(obj->contains, ch, TRUE, TRUE);
	    break;
	}
	return;
    }
    if ((victim = get_char_room(ch, arg1)) != NULL) {
	show_char_to_char_1(victim, ch);
	return;
    }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
	if (can_see_obj(ch, obj)) {	/* player can see object */
	    pdesc = get_extra_descr(arg3, obj->extra_descr);
	    if (pdesc != NULL){
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    return;
		} else
		    continue;
            }
	    pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
	    if (pdesc != NULL){
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    return;
		} else
		    continue;
            }
	    if (is_name(arg3, obj->name))
		if (++count == number) {
		    send_to_char(obj->description, ch);
		    send_to_char("\n\r", ch);
		    return;
		}
	}
    }

    for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content) {
	if (can_see_obj(ch, obj)) {
	    pdesc = get_extra_descr(arg3, obj->extra_descr);
	    if (pdesc != NULL)
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    return;
		}
	    pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
	    if (pdesc != NULL)
		if (++count == number) {
		    send_to_char(pdesc, ch);
		    return;
		}
	    if (is_name(arg3, obj->name))
		if (++count == number) {
		    send_to_char(obj->description, ch);
		    send_to_char("\n\r", ch);
		    return;
		}
	}
    }

    pdesc = get_extra_descr(arg3, ch->in_room->extra_descr);
    if (pdesc != NULL) {
	if (++count == number) {
	    send_to_char(pdesc, ch);
	    return;
	}
    }
    if (count > 0 && count != number) {
	if (count == 1)
	    sprintf(buf, "You only see one %s here.\n\r", arg3);
	else
	    sprintf(buf, "You only see %d of those here.\n\r", count);

	send_to_char(buf, ch);
	return;
    }
    if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
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
	send_to_char("You do not see that here.\n\r", ch);
	return;
    }

    /* 'look direction' */
    if ((pexit = ch->in_room->exit[door]) == NULL) {
	send_to_char("Nothing special there.\n\r", ch);
	return;
    }
    if (pexit->description != NULL && pexit->description[0] != '\0')
	send_to_char(pexit->description, ch);
    else
	send_to_char("Nothing special there.\n\r", ch);

    if (pexit->keyword != NULL
	&& pexit->keyword[0] != '\0'
	&& pexit->keyword[0] != ' ') {
	if (IS_SET(pexit->exit_info, EX_CLOSED)) {
	    act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
	} else if (IS_SET(pexit->exit_info, EX_ISDOOR)) {
	    act("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
	}
    }
    return;
}

/* RT added back for the hell of it */
void do_read(CHAR_DATA * ch, char *argument)
{
    do_look(ch, argument);
}

void do_examine(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Examine what?\n\r", ch);
	return;
    }
    do_look(ch, arg);

    if ((obj = get_obj_here(ch, arg)) != NULL) {
	switch (obj->item_type) {
	default:
	    break;

	case ITEM_JUKEBOX:
	    do_play(ch, "list");
	    break;

	case ITEM_MONEY:
	    if (obj->value[0] == 0) {
		if (obj->value[1] == 0)
		    sprintf(buf, "Odd...there's no coins in the pile.\n\r");
		else if (obj->value[1] == 1)
		    sprintf(buf, "Wow.  One gold coin.\n\r");
		else
		    sprintf(buf, "There are %d gold coins in the pile.\n\r",
			    obj->value[1]);
	    } else if (obj->value[1] == 0) {
		if (obj->value[0] == 1)
		    sprintf(buf, "Wow.  One silver coin.\n\r");
		else
		    sprintf(buf, "There are %d silver coins in the pile.\n\r",
			    obj->value[0]);
	    } else
		sprintf(buf,
			"There are %d gold and %d silver coins in the pile.\n\r",
			obj->value[1], obj->value[0]);
	    send_to_char(buf, ch);
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    sprintf(buf, "in %s", argument);
	    do_look(ch, buf);
	}
    }
    return;
}

/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits(CHAR_DATA * ch, char *argument)
{
    extern char *const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;
    fAuto = !str_cmp(argument, "auto");

    if (!check_blind(ch))
	return;

    if (fAuto)
	sprintf(buf, "[{cExits:");
    else if (IS_IMMORTAL(ch))
	sprintf(buf, "Obvious exits from room %d:\n\r", ch->in_room->vnum);
    else
	sprintf(buf, "Obvious exits:\n\r");

    found = FALSE;
    for (door = 0; door <= 5; door++) {
	if ((pexit = ch->in_room->exit[door]) != NULL
	    && pexit->u1.to_room != NULL
	    && can_see_room(ch, pexit->u1.to_room)) {
	    found = TRUE;
	    if (fAuto) {
		strcat(buf, " ");
		if (IS_SET(pexit->exit_info, EX_CLOSED))
		    strcat(buf, "door");
		else
		    strcat(buf, dir_name[door]);
	    } else {
		if (IS_SET(ch->vuln_flags, VULN_SUN)) {
		    sprintf(buf + strlen(buf), "%-5s - %s",
			    capitalize(dir_name[door]),
			    pexit->u1.to_room->name
			);
		} else
		    sprintf(buf + strlen(buf), "%-5s - %s",
			    capitalize(dir_name[door]),
			    room_is_dark(pexit->u1.to_room)
			    ? "Too dark to tell"
			    : pexit->u1.to_room->name
			);

		if (IS_IMMORTAL(ch))
		    sprintf(buf + strlen(buf),
			    " (room %d)\n\r", pexit->u1.to_room->vnum);
		else
		    sprintf(buf + strlen(buf), "\n\r");
	    }
	}
    }

    if (!found)
	strcat(buf, fAuto ? " none" : "None.\n\r");

    if (fAuto)
	strcat(buf, "{x]\n\r");

    send_to_char(buf, ch);
    return;
}

void do_worth(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) {
	sprintf(buf, "You have %ld gold and %ld silver.\n\r",
		ch->gold, ch->silver);
	send_to_char(buf, ch);
	return;
    }
    sprintf(buf,
	    "You have %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
	    ch->gold, ch->silver, ch->exp,
	    (ch->level + 1) * exp_per_level(ch, ch->pcdata->points) - ch->exp);

    send_to_char(buf, ch);

    return;
}
void do_kingdom(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char morale[MAX_STRING_LENGTH];
    int percent;
    int iChange;
    int excess_food;
    int excess_land;
    int vChange;
    int income, aveincome;

    if (IS_NPC(ch))
	return;

    if (ch->level < 20) {
	send_to_char("You can't see your kingdom info until higher level.\n\r", ch);
	return;
    }
    percent = 0;
    excess_food = 0;
    excess_land = 0;
    sprintf(
	       buf, "{m##########################################################{x\n\r");
    send_to_char(buf, ch);

    if (ch->pcdata->farms != 0) {
	percent =
	    (ch->pcdata->vassals * 100) / (ch->pcdata->farms * 20);
	excess_food = 100 - percent;
	excess_food /= 10;
    }
    if (ch->pcdata->parcels != 0) {
	percent =
	    (ch->pcdata->vassals * 100) / (ch->pcdata->parcels * 10);

	excess_land = 100 - percent;
	excess_land /= 10;
    }
    vChange = (excess_food + excess_land) - (ch->pcdata->tax / 35);

    if (ch->pcdata->morale > 59)
	vChange += -48;
    else if (ch->pcdata->morale > 49)
	vChange += -48;
    else if (ch->pcdata->morale > 36)
	vChange += -12;
    else if (ch->pcdata->morale > 19)
	vChange += 0;
    else if (ch->pcdata->morale > 9)
	vChange += 2;
    else if (ch->pcdata->morale > 0)
	vChange += 4;

    if (ch->pcdata->vassals == 0)
	vChange = 0;

    if (vChange > 12)
	vChange = 12;

    if (vChange > 0) {
	sprintf(buf,
		"{m#{x Vassals: %-8d Possible gain one vassal: %-2d hours.  {m#{x\n\r",
		ch->pcdata->vassals,
		(24 / vChange)
	    );
    } else if (vChange < 0) {
	sprintf(buf,
		"{m#{x Vassals: %-8d Possible loss one vassal: %-2d hours.  {m#{x\n\r",
		ch->pcdata->vassals,
		((24 / vChange) * (-1))
	    );
    } else
	sprintf(buf,
		"{m#{x Vassals: %-8d  No change detected.                 {m#{x\n\r",
		ch->pcdata->vassals
	    );

    send_to_char(buf, ch);

    if (ch->pcdata->morale > 59)
	sprintf(morale, "{RREVOLT{x");
    else if (ch->pcdata->morale > 49)
	sprintf(morale, "{rDangerous{x");
    else if (ch->pcdata->morale > 36)
	sprintf(morale, "{yResentfull{x");
    else if (ch->pcdata->morale > 19)
	sprintf(morale, "{xFair{x");
    else if (ch->pcdata->morale > 9)
	sprintf(morale, "{gGood{x");
    else if (ch->pcdata->morale > 0)
	sprintf(morale, "{gExcellent{x");
    else
	sprintf(morale, "{yUnknown{x");

    sprintf(buf,
	    "{m#{x Morale: %-50s {m#{x\n\r",
	    morale
	);
    send_to_char(buf, ch);

    iChange = (ch->pcdata->mines * 3)
	+ (ch->pcdata->farms * 4);

    aveincome = (((6 * ch->pcdata->vassals * ch->pcdata->tax / 100) - iChange) / 2) + iChange;
    sprintf(buf,
	    "{m#{x Tax Percent: %-10d  Avg Gold Earned: %-10dgp {m#{x\n\r",
	    ch->pcdata->tax, aveincome);
    send_to_char(buf, ch);

    sprintf(buf, "{m#--------------------------------------------------------#{x\n\r");
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Farms: %-5d Food Consumed: %-5d Food Produced: %-5d {m#{x\n\r",
	    ch->pcdata->farms,
	    ch->pcdata->vassals,
	    (ch->pcdata->farms * 20)
	);
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Mines: %-5d Res Consumed:  %-5d  Res Produced: %-5d {m#{x\n\r",
	    ch->pcdata->mines,
	    (ch->pcdata->vassals / 10),
	    (ch->pcdata->mines)
	);
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Parcels: %-8d  Max Land Area:  %-17d   {m#{x\n\r",
	    ch->pcdata->parcels,
	    (ch->pcdata->parcels * 10)
	);
    send_to_char(buf, ch);

    /*sprintf(buf,
	    "{m#{x Research: %-8d Research Level: %-17d   {m#{x\n\r",
	    ch->pcdata->research,
	    ch->pcdata->research_total / 2000
	);
    send_to_char(buf, ch);*/

    sprintf(buf, "{m#--------------------------------------------------------#{x\n\r");
    send_to_char(buf, ch);

    percent = ch->pcdata->tax;

    sprintf(buf,
	    "{m#{x Hourly Operating Costs:  %-27dgp {m#{x\n\r",
	    iChange
	);
    send_to_char(buf, ch);

    income = ((6 * ch->pcdata->vassals * ch->pcdata->tax / 100) - iChange) / 2;

    sprintf(buf,
	    "{m#{x Hourly Change in Income: %-27dgp {m#{x\n\r", income);
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Your Current Treasury:   %-27ldgp {m#{x\n\r",
	    ch->banked_gold
	);
    send_to_char(buf, ch);

    sprintf(
	       buf, "{m##########################################################{x\n\r");
    send_to_char(buf, ch);

    send_to_char("NOTE: An hour represents one TICK in the game.\n\r", ch);

    return;
}

void do_attrib(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if(IS_NPC(ch)) return;

    sprintf(buf, "{m####################################################################{x\n\r");
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Name:    %-15s  Level:  %4d   Hits:  ({g%5d{x)%5d     {m#{x\n\r",
	    ch->name,
	    ch->level,
	    ch->hit, ch->max_hit);
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Class:   %-15s  Age:    %4d   Mana:  ({g%5d{x)%5d     {m#{x\n\r",
	    IS_NPC(ch) ? "" : class_table[ch->class].name,
	    get_age(ch),
	    ch->mana, ch->max_mana
	);
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Race:    %-15s  Played: %4dh  Moves: ({g%5d{x)%5d     {m#{x\n\r",
	    pc_race_table[ch->race].name,
	    (ch->played + (int) (current_time - ch->logon)) / 3600,
	    ch->move, ch->max_move
	);
    send_to_char(buf, ch);
    if (!is_clan(ch)) {
	sprintf(buf,
		"{m#{x Kingdom: %-13s    Warpts: %4d                           {m#{x\n\r",
		kingdom_table[ch->side].name,
		ch->warpoints
	    );
    } else
	sprintf(buf,
		"{m#{x Kingdom: %-13s    Warpts: %4d   Clan:  %-11s      {m#{x\n\r",
		kingdom_table[ch->side].name,
		ch->warpoints,
		clan_table[ch->clan].name
	    );

    send_to_char(buf, ch);

    sprintf(buf, "{m#------------------------------------------------------------------#{x\n\r");
    send_to_char(buf, ch);

    sprintf(buf,
	    "{m#{x Str: %-2d({g%-2d{x)  Hit+%-3d      Damage+%-3d     Weight:%-6ld/%-7d   {m#{x\n\r",
	    ch->perm_stat[STAT_STR],
	    get_curr_stat(ch, STAT_STR),
	    GET_HITROLL(ch), GET_DAMROLL(ch),
	    get_carry_weight(ch) / 10, can_carry_w(ch) / 10
	);
    send_to_char(buf, ch);
    sprintf(buf,
	    "{m#{x Int: %-2d({g%-2d{x)  Saves:{g%-4d{x   Gold:{y%7ld{x   Silver:{y%7ld{x          {m#{x\n\r",
	    ch->perm_stat[STAT_INT],
	    get_curr_stat(ch, STAT_INT),
	    ch->saving_throw,
	    ch->gold,
	    ch->silver
	);
    send_to_char(buf, ch);
    sprintf(buf,
	    "{m#{x Wis: %-2d({g%-2d{x)  Trains:%-4d  Pracs:%-4d     Wimpy:%-4d              {m#{x\n\r",
	    ch->perm_stat[STAT_WIS],
	    get_curr_stat(ch, STAT_WIS),
	    ch->train, ch->practice,
	    ch->wimpy
	);
    send_to_char(buf, ch);
    sprintf(buf,
	    "{m#{x Dex: %-2d({g%-2d{x)  RaisePts:%-5d              QPoints: {W%-5d{x          {m#{x\n\r",
	    ch->perm_stat[STAT_DEX],
	    get_curr_stat(ch, STAT_DEX),
	    ch->levelpoints,
	    ch->questpoints
	);
    send_to_char(buf, ch);

    if (!IS_NPC(ch) && ch->level < LEVEL_HERO) {

	sprintf(buf, "{m#{x Con: %-2d({g%-2d{x)  Exp:{g%-6d{x   NextLevelIn:{g%-6d{x                     {m#{x\n\r",
		ch->perm_stat[STAT_CON],
		get_curr_stat(ch, STAT_CON),
		ch->exp,
		((ch->level + 1) * exp_per_level(ch, ch->pcdata->points) - ch->exp));

	send_to_char(buf, ch);
    } else {

	sprintf(buf, "{m#{x Con: %-2d({g%-2d{x)  Exp:{g%-6d{x   NextLevelIn:{gMaxed{x                      {m#{x\n\r",
		ch->perm_stat[STAT_CON],
		get_curr_stat(ch, STAT_CON),
		ch->exp);
	send_to_char(buf, ch);
    }
    sprintf(buf, "{m#------------------------------------------------------------------#{x\n\r");
    send_to_char(buf,ch);
    sprintf(buf, "{m#{x Items Carried: %-4d          Max Items: %-4d                     {m#{x\n\r", ch->carry_number, can_carry_n(ch) );
    send_to_char(buf, ch);
    {
        long total_money_in_silver;
        long silver_gain;

        total_money_in_silver  = ch->banked_gold * 100 + ch->banked_silver;
        silver_gain            = total_money_in_silver / 10000;

        if (silver_gain > 50000)
            silver_gain = 50000;

        sprintf(buf, "{m#{x Earned Interest Per Tick:    Gold: {W%-4ld{x  Silver: {W%-4ld{x            {m#{x\n\r",
	    silver_gain / 100, silver_gain % 100);
    }
    send_to_char(buf, ch);

    sprintf(buf, "{m#------------------------------------------------------------------#{x\n\r");
    send_to_char(buf, ch);
    sprintf(buf, "{m#{x        Enemies Killed: %-5d Final Blows: %-5d   Warscore       {m#{x\n\r",
	    ch->enemies_killed,
	    ch->final_blows);
    send_to_char(buf, ch);
    sprintf(buf, "{m#{x  Barons and up killed: %-5d       Raids: %-5d  (personal)      {m#{x\n\r",
	    ch->leg_killed,
	    ch->raids);
    send_to_char(buf, ch);

    sprintf(buf, "{m#{x     Killed by Players: %-5d   Warpoints: %-5d    {W%-5d{x         {m#{x\n\r",
	    ch->playerkilled,
	    ch->warpoints,
	    ch->warpoints  // - ch->playerkilled //kill kbps
	);
    send_to_char(buf, ch);
    sprintf(buf, "{m####################################################################{x\n\r");
    send_to_char(buf, ch);

}

void create_bar(int number, int lownum, int highnum, char *buf_return)
{
    char buf[MAX_STRING_LENGTH];
    int percent;

    buf_return[0] = '\0';
    lownum = 0;

    percent = (number) * 100 / (highnum);

    if (percent <= 1)
	sprintf(buf, "{rLow{x[                  ]{gHigh{x");
    else if (percent <= 5)
	sprintf(buf, "{rLow{x[{r@{x                 ]{gHigh{x");
    else if (percent <= 10)
	sprintf(buf, "{rLow{x[{r@@{x                ]{gHigh{x");
    else if (percent <= 15)
	sprintf(buf, "{rLow{x[{r@@@{x               ]{gHigh{x");
    else if (percent <= 20)
	sprintf(buf, "{rLow{x[{r@@@@{x              ]{gHigh{x");
    else if (percent <= 25)
	sprintf(buf, "{rLow{x[{r@@@@@{x             ]{gHigh{x");
    else if (percent <= 30)
	sprintf(buf, "{rLow{x[{r@@@@@@{x            ]{gHigh{x");
    else if (percent <= 35)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@{x           ]{gHigh{x");
    else if (percent <= 40)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@{x          ]{gHigh{x");
    else if (percent <= 45)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@{x         ]{gHigh{x");
    else if (percent <= 50)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@{x        ]{gHigh{x");
    else if (percent <= 55)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@{x       ]{gHigh{x");
    else if (percent <= 60)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{x      ]{gHigh{x");
    else if (percent <= 65)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@{x     ]{gHigh{x");
    else if (percent <= 70)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@{x    ]{gHigh{x");
    else if (percent <= 75)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@{x   ]{gHigh{x");
    else if (percent <= 85)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@@{x  ]{gHigh{x");
    else if (percent <= 90)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@@@{x ]{gHigh{x");
    else if (percent <= 99)
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@@@{x ]{gHigh{x");
    else {
	sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@@@@{x]{gHigh{x");
    }
    strcat(buf_return, buf);
    return;
}

void do_strategy(CHAR_DATA * ch, char *argument)
{
    char argone[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    argument = one_argument(argument, argone);

    if (argone[0] == '\0') {
	send_to_char("Strategy:\n\r", ch);
	send_to_char("Flee, Defend, Engage, Assault \n\r", ch);
	return;
    }
    if (argone[0] == 'f') {
	send_to_char("You will now flee in combat.\n\r", ch);
	ch->pcdata->strategy = 0;
    } else if (argone[0] == 'd') {
	send_to_char("You take up a defensive position.\n\r", ch);
	ch->pcdata->strategy = 1;
    } else if (argone[0] == 'e') {
	send_to_char("You adjust your stance for a balanced fight.\n\r", ch);
	ch->pcdata->strategy = 2;
    } else if (argone[0] == 'a') {
	send_to_char("You commit yourself to aggressive and dangerous attacks.\n\r", ch);
	ch->pcdata->strategy = 3;
    } else {
	send_to_char("Unkown strategy. \n\r", ch);
	return;
    }
}
void do_score(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char bar_buf[MAX_STRING_LENGTH];
    int i;
    char temp[MAX_STRING_LENGTH];
    int skill;

    skill = 0;

    if (IS_NPC(ch))
	return;

    if (ch->level < 2) {
	sprintf(buf, "YOU MUST GET TO LEVEL TWO TO SAVE!\n\r");
	send_to_char(buf, ch);
    }
    sprintf(
	       buf, "{m##########################################################{x\n\r");
    send_to_char(buf, ch);

    switch (ch->pcdata->strategy) {
    case 0:
	sprintf(temp, "Flee");
	break;
    case 1:
	sprintf(temp, "Defend");
	break;
    case 2:
	sprintf(temp, "Engage");
	break;
    case 3:
	sprintf(temp, "Assault");
	break;
    default:
	sprintf(temp, "Unknown");
	break;
    }

    sprintf(buf, "{m#{x       Main Combat Strategy: %10s                 {m#{x\n\r", temp);
    send_to_char(buf, ch);

    sprintf(buf, "{m#----Offense---------------------------------------------#{x\n\r");
    send_to_char(buf, ch);

    if (!IS_NPC(ch)) {
	if (ch->pcdata->strategy <= 1)
	    skill = -35;
	if (ch->pcdata->strategy >= 3)
	    skill = 25;
    }
    if (get_eq_char(ch, WEAR_WIELD) != NULL) {

	skill += get_weapon_skill(ch, get_weapon_sn(ch, FALSE));

	create_bar(skill, 0, 110, bar_buf);
	sprintf(buf, "{m#{x    Wielded Proficiency: %s    {m#{x\n\r", bar_buf);
	send_to_char(buf, ch);
    }
    if (get_eq_char(ch, WEAR_SECONDARY) != NULL) {
	if (!IS_NPC(ch)) {
	    if (ch->pcdata->strategy <= 1)
		skill = -35;
	    if (ch->pcdata->strategy >= 3)
		skill = 25;
	}
	skill += get_weapon_skill(ch, get_weapon_sn(ch, TRUE));

	create_bar(skill, 0, 110, bar_buf);
	sprintf(buf, "{m#{x Dual Wield Proficiency: %s    {m#{x\n\r", bar_buf);
	send_to_char(buf, ch);
    }
    if (!IS_NPC(ch)) {
	if (ch->pcdata->strategy <= 1) {
	    create_bar((GET_HITROLL(ch) - (ch->level / 3)), 0, 60, bar_buf);
	    sprintf(buf, "{m#{x              Hit Bonus: %s    {m#{x\n\r", bar_buf);
	    send_to_char(buf, ch);
	    create_bar((GET_DAMROLL(ch) - ((ch->level / 2))), 0, 60, bar_buf);
	    sprintf(buf, "{m#{x           Damage Bonus: %s    {m#{x\n\r", bar_buf);
	    send_to_char(buf, ch);
	} else if (ch->pcdata->strategy >= 3) {
	    create_bar((GET_HITROLL(ch) + (ch->level / 3)), 0, 60, bar_buf);
	    sprintf(buf, "{m#{x              Hit Bonus: %s    {m#{x\n\r", bar_buf);
	    send_to_char(buf, ch);
	    create_bar((GET_DAMROLL(ch) + (ch->level / 5)), 0, 60, bar_buf);
	    sprintf(buf, "{m#{x           Damage Bonus: %s    {m#{x\n\r", bar_buf);
	    send_to_char(buf, ch);
	} else {
	    create_bar(GET_HITROLL(ch), 0, 60, bar_buf);
	    sprintf(buf, "{m#{x              Hit Bonus: %s    {m#{x\n\r", bar_buf);
	    send_to_char(buf, ch);
	    create_bar(GET_DAMROLL(ch), 0, 60, bar_buf);
	    sprintf(buf, "{m#{x           Damage Bonus: %s    {m#{x\n\r", bar_buf);
	    send_to_char(buf, ch);
	}
    }
    sprintf(buf, "{m#----Defense---------------------------------------------#{x\n\r");
    send_to_char(buf, ch);

    create_bar(get_parry(ch), 0, 85, bar_buf);
    sprintf(buf, "{m#{x       Effective Parry: %s     {m#{x\n\r", bar_buf);
    send_to_char(buf, ch);

    create_bar(get_dodge(ch), 0, 85, bar_buf);
    sprintf(buf, "{m#{x       Effective Dodge: %s     {m#{x\n\r", bar_buf);
    send_to_char(buf, ch);
    sprintf(buf, "{m#                                                        #{x\n\r");
    send_to_char(buf, ch);
    
        if (ch->level >= 25)
    {
   sprintf (buf, "{m#{x AC: Pierce:%4d  Bash:%4d  Slash:%4d  Magic:%4d     {m#{x\n\r",
                 GET_AC (ch, AC_PIERCE),
                 GET_AC (ch, AC_BASH),
                 GET_AC (ch, AC_SLASH), 
			     GET_AC (ch, AC_EXOTIC));
        send_to_char (buf, ch);
    }
    sprintf(buf, "{m#                                                        #{x\n\r");
    send_to_char(buf, ch);

    for (i = 0; i < 4; i++) {
	char *temp;

	switch (i) {
	case (AC_PIERCE):
	    temp = "Piercing";
	    break;
	case (AC_BASH):
	    temp = "Bashing";
	    break;
	case (AC_SLASH):
	    temp = "Slashing";
	    break;
	case (AC_EXOTIC):
	    temp = "Magic";
	    break;
	default:
	    temp = "Error";
	    break;
	}

	if (GET_AC(ch, i) >= 80) {
	    sprintf(buf, "{m#{x %-8s: {rdefenseless{x      ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[                  ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= 60) {
	    sprintf(buf, "{m#{x %-8s: {rbarely armored{x   ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@{x                ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= 40) {
	    sprintf(buf, "{m#{x %-8s: {rlightly armored{x  ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@{x              ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= 20) {
	    sprintf(buf, "{m#{x %-8s: {rsomewhat armored{x ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{x            ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= 0) {
	    sprintf(buf, "{m#{x %-8s: {ynicely armored{x   ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{y@@{x          ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= -20) {
	    sprintf(buf, "{m#{x %-8s: {ywell-armored{x     ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@{x        ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= -40) {
	    sprintf(buf, "{m#{x %-8s: {yvery well-armored{x", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{x      ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= -60) {
	    sprintf(buf, "{m#{x %-8s: {gheavily armored{x  ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@{x    ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= -80) {
	    sprintf(buf, "{m#{x %-8s: {gsuperbly armored{x ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@@{x  ]{gHigh{x {m#{x\n\r");
	} else if (GET_AC(ch, i) >= -100) {
	    sprintf(buf, "{m#{x %-8s: {gnear invulnerable{x", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@@@{x ]{gHigh{x {m#{x\n\r");
	} else {
	    sprintf(buf, "{m#{x %-8s: {gdivinely armored{x ", temp);
	    send_to_char(buf, ch);
	    sprintf(buf, "{rLow{x[{r@@@@@@{y@@@@@@{g@@@@@@{x]{gHigh{x {m#{x\n\r");
	}

	send_to_char(buf, ch);
    }

    sprintf(buf, "{m#--------------------------------------------------------#{x\n\r");
    send_to_char(buf, ch);

    if (get_trust(ch) != ch->level) {
	sprintf(buf, "{m#{x You are trusted at level %d.\n\r",
		get_trust(ch));
	send_to_char(buf, ch);
    }
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	send_to_char(
			"{m#{x You are drunk.                                         {m#{x\n\r",
			ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
	send_to_char(
			"{m#{x You are thirsty.                                       {m#{x\n\r",
			ch);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] == 0)
	send_to_char(
			"{m#{x You are hungry.                                        {m#{x\n\r",
			ch);
    send_to_char("{m#{x ", ch);

    switch (ch->position) {
    case POS_DEAD:
	send_to_char("You are DEAD!!                   ", ch);
	break;
    case POS_MORTAL:
	send_to_char("You are mortally wounded.        ", ch);
	break;
    case POS_INCAP:
	send_to_char("You are incapacitated.           ", ch);
	break;
    case POS_STUNNED:
	send_to_char("You are stunned.                 ", ch
	    );
	break;
    case POS_SLEEPING:
	send_to_char("You are sleeping.                ", ch
	    );
	break;
    case POS_RESTING:
	send_to_char("You are resting.                 ", ch
	    );
	break;
    case POS_SITTING:
	send_to_char("You are sitting.                 ", ch
	    );
	break;
    case POS_STANDING:
	if (MOUNTED(ch)) {
	    sprintf(buf, "You're riding %-30s", MOUNTED(ch)->short_descr);
	    send_to_char(buf, ch);
	} else {
	    send_to_char("You are standing.                ", ch
		);
	}
	break;
    case POS_FIGHTING:
	send_to_char("You are fighting.                ", ch
	    );
	break;
    }

    if (RIDDEN(ch)) {
	sprintf(buf, "You're ridden by %-18s",
		IS_NPC(RIDDEN(ch)) ? RIDDEN(ch)->short_descr : RIDDEN(ch)->name);
	send_to_char(buf, ch);
    }
    send_to_char("                      {m#{x\n\r", ch);

    sprintf(buf,

	    "{m##########################################################{x\n\r");
    send_to_char(buf, ch);

    send_to_char("Type 'attribute' for more character information.\n\r", ch);

    if (ch->pcdata->judge == 1) {
	send_to_char("You have been given the authority of a constable.\n\r", ch);
	}
    if (ch->pcdata->judge == 2){
	send_to_char("You have been trusted as your town's head constable.\n\r", ch);
	}
    //headconscore

    /* RT wizinvis and holy light */
    if (IS_IMMORTAL(ch)) {
	send_to_char("Holy Light: ", ch);
	if (IS_SET(ch->act, PLR_HOLYLIGHT))
	    send_to_char("on", ch);
	else
	    send_to_char("off", ch);

	if (ch->invis_level) {
	    sprintf(buf, "  Invisible: level %d", ch->invis_level);
	    send_to_char(buf, ch);
	}
    }
    if (ch->incog_level) {
	sprintf(buf, "  Incognito: level %d", ch->incog_level);
	send_to_char(buf, ch);
    }
    send_to_char("\n\r", ch);

    if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
	do_affects(ch, "");

}

void do_affects(CHAR_DATA * ch, char *argument)
{
    AFFECT_DATA *paf, *paf_last = NULL;
    char buf[MAX_STRING_LENGTH];

    if (ch->affected != NULL) {
	send_to_char("{wYou are affected by the following spells{D:{x\n\r", ch);
	for (paf = ch->affected; paf != NULL; paf = paf->next) {
	    if(paf->type == skill_lookup("ki power")) continue;


	    if (paf_last != NULL && paf->type == paf_last->type)
		if (ch->level >= 20)
		    sprintf(buf, "                      ");
		else
		    continue;
	    else if (paf->type == skill_lookup("noquit") && !IS_IMMORTAL(ch)) {
		if ((paf->duration + 1) > 1)
		    sprintf(buf, "{wYou cannot quit for {x%d{w more ticks.{x\n\r",
			    (paf->duration + 1));
		else
		    sprintf(buf, "{wYou cannot quit for {xone{w tick.{x\n\r");
		}  else
		sprintf(buf, "{wSpell{D:{x %-15s{x", skill_table[paf->type].name);

	    send_to_char(buf, ch);

	    if (ch->level >= 20 && ((paf->type != skill_lookup("noquit")) )) {
		sprintf(buf,
			"{D:{w modifies{x %s {wby{x %d ",
			affect_loc_name(paf->location),
			paf->modifier);
		send_to_char(buf, ch);
		if (paf->duration == -1)
		    sprintf(buf, "{wpermanently{x");
		else
		    sprintf(buf, "{wfor {x%d{w hours{x", paf->duration);
		send_to_char(buf, ch);
	    }
	    send_to_char("\n\r", ch);
	    paf_last = paf;
	}
    } else
	send_to_char("{wYou are not affected by any spells.{x\n\r", ch);

    return;
}


char *const day_name[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *const month_name[] =
{
    "winter", "late winter", "early spring", "spring",
    "late spring", "early summer", "summer", "late summer", "early fall",
    "fall", "late fall", "early winter"
	/* "the Dark Shades", "the Shadows",
	   "the Long Shadows", "the Ancient Darkness", "the Great Evil" */
};

void do_time(CHAR_DATA * ch, char *argument)
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day = time_info.day + 1;

    if (day > 4 && day < 20)
	suf = "th";
    else if (day % 10 == 1)
	suf = "st";
    else if (day % 10 == 2)
	suf = "nd";
    else if (day % 10 == 3)
	suf = "rd";
    else
	suf = "th";
    sprintf(buf,
	    "It is %d o'clock %s, season of %s.\n\r",
	    (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	    time_info.hour >= 12 ? "pm" : "am",
    /* day_name[day % 7], */
    /* day, suf, */
	    month_name[time_info.month]);
    send_to_char(buf, ch);
    sprintf(buf, "Act of War last booted at %s The cpu time is %s",
	    str_boot_time,
	    (char *) ctime(&current_time)
	);
    send_to_char(buf, ch);
    return;
}

void do_weather(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (!IS_OUTSIDE(ch)) {
	send_to_char("You can't see the weather indoors.\n\r", ch);
	return;
    }
    if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
	switch (weather_info.sky) {
	case SKY_CLOUDLESS:
	    sprintf(buf, "The sky is crystal clear. %s\n\r",
		    weather_info.change >= 0
		    ? "The winter wind is calm and serene."
		    : "A bitter winter wind blows in from the north."
		);
	    break;
	case SKY_CLOUDY:
	    sprintf(buf, "The clouds form strange shapes in the sky. %s\n\r",
		    weather_info.change >= 0
		    ? "The clouds move slowly northward."
		    : "The temperature is dropping."
		);
	    break;
	case SKY_RAINING:
	    sprintf(buf, "Snow falls softly over the lands. %s\n\r",
		    weather_info.change >= 0
		    ? "The wind blows drifts of snow across the way."
		    : "There seems to be no end to the snow."
		);
	    break;
	case SKY_LIGHTNING:
	    sprintf(buf, "Heavy snow falls and has formed a blizzard! %s\n\r",
		    weather_info.change >= 0
		    ? "Slivers of light barely penetrate the heavy downpour."
		    : "The howling wind blows snow into walls of ice."
		);
	    break;
	default:
	    break;
	}
    } else if (time_info.month >= MARCH && time_info.month <= MAY) {
	switch (weather_info.sky) {
	case SKY_CLOUDLESS:
	    sprintf(buf, "Not a cloud in the sky. %s\n\r",
		    weather_info.change >= 0
		    ? "More snow melts as summer approaches."
		    : "The lands beg for moisture."
		);
	    break;
	case SKY_CLOUDY:
	    sprintf(buf, "Clouds have formed high above. %s\n\r",
		    weather_info.change >= 0
		    ? "The clouds seem to be clearing."
		    : "The snow melts into atmospheric moisture."
		);
	    break;
	case SKY_RAINING:
	    sprintf(buf, "It is raining lightly. %s\n\r",
		    weather_info.change >= 0
		    ? "The ground soaks up the water."
		    : "The lands are in need of the spring rains."
		);
	    break;
	case SKY_LIGHTNING:
	    sprintf(buf, "It is hailing, lightning can be seen in the sky. %s\n\r",
		    weather_info.change >= 0
		    ? "The hail bounces lightly on the ground."
		    : "Nothing stirs during the downpour."
		);
	    break;
	default:
	    break;
	}
    } else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	switch (weather_info.sky) {
	case SKY_CLOUDLESS:
	    sprintf(buf, "The summer sky is clear. %s\n\r",
		    weather_info.change >= 0
		    ? "Warm winds blow softly through the kingdom."
		    : "Clouds are forming on the horizon."
		);
	    break;
	case SKY_CLOUDY:
	    sprintf(buf, "Clouds have taken residence above you. %s\n\r",
		    weather_info.change >= 0
		    ? "The wind moves them into strange shapes."
		    : "They are scarred by heavy moisture."
		);
	    break;
	case SKY_RAINING:
	    sprintf(buf, "Rain blankets the lands. %s\n\r",
		    weather_info.change >= 0
		    ? "Water droplets form puddles on the ground."
		    : "The clouds have not finished there downpour yet."
		);
	    break;
	case SKY_LIGHTNING:
	    sprintf(buf, "The summer sky is lit up by flashes of lightning. %s\n\r",
		    weather_info.change >= 0
		    ? "Thunder booms overhead."
		    : "A crack of thunder rumbles above your head."
		);
	    break;
	default:
	    break;
	}
    } else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
	switch (weather_info.sky) {
	case SKY_CLOUDLESS:
	    sprintf(buf, "The fall sky is clear. %s\n\r",
		    weather_info.change >= 0
		    ? "Leaves fall softly from the trees."
		    : "A gust of wind picks up pace."
		);
	    break;
	case SKY_CLOUDY:
	    sprintf(buf, "Large clouds drift high above. %s\n\r",
		    weather_info.change >= 0
		    ? "The lands are preparing for winter."
		    : "A cold wind bites at your neck."
		);
	    break;
	case SKY_RAINING:
	    sprintf(buf, "A fall rain is pouring from the sky. %s\n\r",
		    weather_info.change >= 0
		    ? "The temperature seems stable."
		    : "The rain begins to feel colder. The sky is getting darker."
		);
	    break;
	case SKY_LIGHTNING:
	    sprintf(buf, "Lightning lights up the sky as hail pounds the ground. %s\n\r",
		    weather_info.change >= 0
		    ? "Bits of ice bite at your equipment."
		    : "The temperature seems to be dropping."
		);
	    break;
	default:
	    break;
	}
    } else if (time_info.month == DECEMBER) {
	switch (weather_info.sky) {
	case SKY_CLOUDLESS:
	    sprintf(buf, "The sky is crystal clear. %s\n\r",
		    weather_info.change >= 0
		    ? "The winter wind is calm and serene."
		    : "A bitter winter wind blows in from the north."
		);
	    break;
	case SKY_CLOUDY:
	    sprintf(buf, "The clouds form strange shapes in the sky. %s\n\r",
		    weather_info.change >= 0
		    ? "The clouds move slowly northward."
		    : "The temperature is dropping."
		);
	    break;
	case SKY_RAINING:
	    sprintf(buf, "Snow falls softly over the lands. %s\n\r",
		    weather_info.change >= 0
		    ? "The wind blows drifts of snow across the way."
		    : "There seems to be no end to the snow."
		);
	    break;
	case SKY_LIGHTNING:
	    sprintf(buf, "Heavy snow falls and has formed a blizzard! %s\n\r",
		    weather_info.change >= 0
		    ? "Slivers of light barely penetrate the heavy downpour."
		    : "The howling wind blows snow into walls of ice."
		);
	    break;
	default:
	    break;
	}
    }
    send_to_char(buf, ch);
    return;
}

void do_help(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH * 2];
    int col = 0;
    char nohelp[MAX_STRING_LENGTH];
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
	bool rf = FALSE;
    char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH], /*argtwo[MSL],*/ hbuf[MSL];
    int level;
	int race;
	int race2;
	int skill;
    int bit;
    int iclass;
	int class2;

    output = new_buf();
    strcpy(nohelp, argument);

    if (argument[0] == '\0') {
	do_help(ch, "helptitle");
	for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
	    level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;
	    if (level > get_trust(ch))
		continue;
	    if (IS_IMMORTAL(ch)) {
		if (pHelp->keyword[0] == '{')
		    if (pHelp->level >= LEVEL_IMMORTAL)
			sprintf(buf, "{x[{R%3d{x] %-22.21s", pHelp->level, pHelp->keyword);
		    else
			sprintf(buf, "{x[%3d] %-22.21s", pHelp->level, pHelp->keyword);
		else if (pHelp->level >= LEVEL_IMMORTAL)
		    sprintf(buf, "{x[{R%3d{x] %-20.19s", pHelp->level, pHelp->keyword);
		else
		    sprintf(buf, "{x[%3d] %-20.19s", pHelp->level, pHelp->keyword);
	    } else {
		if (pHelp->keyword[0] == '{')
		    sprintf(buf, "{x> %-26.25s", pHelp->keyword);
		else
		    sprintf(buf, "{x> %-24.23s", pHelp->keyword);
	    }
	    add_buf(output, buf);
	    if (++col % 3 == 0)
		add_buf(output, "{x\n\r");
	}
	if (col % 3 != 0)
	    add_buf(output, "{x\n\r");
	page_to_char(buf_string(output), ch);
	free_buf(output);
	return;
    }
	/* Dynamic help for races. --Tasydra */
			race = pc_race_lookup(argument);

		for (race2 = 1; pc_race_table[race2].name != NULL; race2++)
		{

        if (pc_race_table[race].side == SIDE_NONE)
		continue;

		if (race == race2)
		{
			rf = TRUE;
			sprintf(hbuf, "\n\r{D+{c==========================================================================={x\n\r");
			send_to_char(hbuf, ch);
			sprintf(hbuf, "{D|{x Race Name: %s\n\r", capitalize(pc_race_table[race].name));
            send_to_char(hbuf, ch);
			sprintf(hbuf, "{D|{x Race Desc: %s\n\r", pc_race_table[race].short_description);
            send_to_char(hbuf, ch);
			if(!(pc_race_table[race].isRemort))
			{
            sprintf(hbuf, "{D|{x Starting Stats: Str: %-2d Int: %-2d Wis: %-2d Dex: %-2d Con: %-2d\n\r",
				pc_race_table[race].stats[0],
				pc_race_table[race].stats[1],
				pc_race_table[race].stats[2],
				pc_race_table[race].stats[3],
				pc_race_table[race].stats[4]);
				send_to_char(hbuf, ch);
			}

			sprintf(hbuf, "{D|{x Maximum Stats:  Str: %-2d Int: %-2d Wis: %-2d Dex: %-2d Con: %-2d\n\r",
				pc_race_table[race].max_stats[0],
				pc_race_table[race].max_stats[1],
				pc_race_table[race].max_stats[2],
				pc_race_table[race].max_stats[3],
				pc_race_table[race].max_stats[4]);
				send_to_char(hbuf, ch);

			sprintf(hbuf, "{D|{x Available to: ");
            send_to_char(hbuf, ch);

			if(IS_SET(pc_race_table[race].side, CANBE_TIER))
			{
				sprintf(hbuf, "{cTier{x ");
				send_to_char(hbuf, ch);
			}
			if(IS_SET(pc_race_table[race].side, CANBE_KAEL))
			{
				sprintf(hbuf, "{mKael{x ");
				send_to_char(hbuf, ch);
			}
			if(IS_SET(pc_race_table[race].side, CANBE_UM))
			{
				sprintf(hbuf, "{rUndermountain{x ");
				send_to_char(hbuf, ch);
			}
			if(IS_SET(pc_race_table[race].side, CANBE_CAIRN))
			{
				sprintf(hbuf, "{x ");
				send_to_char(hbuf, ch);
			}
			sprintf(hbuf, "\n\r");
			send_to_char(hbuf, ch);
			sprintf(hbuf, "{D|{x Racial Skills: ");
			send_to_char(hbuf, ch);
				for(skill = 0; pc_race_table[race].skills[skill] != NULL; skill++)
				{
                sprintf(hbuf, pc_race_table[race].skills[skill]);
				send_to_char(hbuf, ch);
                sprintf(hbuf, " ");
				send_to_char(hbuf, ch);
				}
			sprintf(hbuf, "\n\r");
			send_to_char(hbuf, ch);
			for (bit = 0; race_table[bit].name != NULL; bit++)
			{
				if(!strcmp(pc_race_table[race].name, race_table[bit].name))
				{
					sprintf(hbuf, "{D|{x Immune to: ");
					send_to_char(hbuf, ch);
					sprintf(hbuf, imm_bit_name(race_table[bit].imm));
					send_to_char(hbuf, ch);
					sprintf(hbuf, "\n\r");
					send_to_char(hbuf, ch);
					sprintf(hbuf, "{D|{x Resistant to: ");
					send_to_char(hbuf, ch);
					sprintf(hbuf, res_bit_name(race_table[bit].res));
					send_to_char(hbuf, ch);
					sprintf(hbuf, "\n\r");
					send_to_char(hbuf, ch);
					sprintf(hbuf, "{D|{x Vulnerable to: ");
					send_to_char(hbuf, ch);
					sprintf(hbuf, vuln_bit_name(race_table[bit].vuln));
					send_to_char(hbuf, ch);
					sprintf(hbuf, "\n\r");
					send_to_char(hbuf, ch);
					sprintf(hbuf, "{D|{x Affected by: ");
					send_to_char(hbuf, ch);
					sprintf(hbuf, affect_bit_name(race_table[bit].aff));
					send_to_char(hbuf, ch);
					sprintf(hbuf, "\n\r");
					send_to_char(hbuf, ch);
					break;
				}
			}
			sprintf(hbuf, "{D+{c==========================================================================={x\n\r\n\r");
			send_to_char(hbuf, ch);

		}

}
	/* Dynamic help for CLASSES. --Tasydra */
	iclass = class_lookup(argument);

		for (class2 = 0; class2 < MAX_CLASS; class2++)
		{

        if (class_table[iclass].side == SIDE_NONE)
		continue;

		if (iclass == class2)
		{
			rf = TRUE;
			sprintf(hbuf, "\n\r{D+{c==========================================================================={x\n\r");
			send_to_char(hbuf, ch);
			sprintf(hbuf, "{D|{x Class Name: %s\n\r", capitalize(class_table[iclass].name));
            send_to_char(hbuf, ch);
			sprintf(hbuf, "{D|{x Class Desc: %s\n\r", class_table[iclass].short_description);
            send_to_char(hbuf, ch);

			sprintf(hbuf, "{D|{x Available to: ");
            send_to_char(hbuf, ch);

			if(IS_SET(class_table[iclass].side, CANBE_TIER))
			{
				sprintf(hbuf, "{cTier{x ");
				send_to_char(hbuf, ch);
			}
			if(IS_SET(class_table[iclass].side, CANBE_KAEL))
			{
				sprintf(hbuf, "{mKael{x ");
				send_to_char(hbuf, ch);
			}
			if(IS_SET(class_table[iclass].side, CANBE_UM))
			{
				sprintf(hbuf, "{rUndermountain{x ");
				send_to_char(hbuf, ch);
			}
			if(IS_SET(class_table[iclass].side, CANBE_CAIRN))
			{
				sprintf(hbuf, "{x ");
				send_to_char(hbuf, ch);
			}
			sprintf(hbuf, "\n\r");
			send_to_char(hbuf, ch);
            sprintf(hbuf, "{D|{x Prime Attrib: ");
			send_to_char(hbuf, ch);
			if(class_table[iclass].attr_prime == STAT_STR)
				sprintf(hbuf, "Strength\n\r");
			else if(class_table[iclass].attr_prime == STAT_INT)
				sprintf(hbuf, "Intelligence\n\r");
			else if(class_table[iclass].attr_prime == STAT_WIS)
				sprintf(hbuf, "Wisdom\n\r");
			else if(class_table[iclass].attr_prime == STAT_DEX)
				sprintf(hbuf, "Dexterity\n\r");
			else if(class_table[iclass].attr_prime == STAT_CON)
				sprintf(hbuf, "Constitution\n\r");
			send_to_char(hbuf, ch);
			sprintf(hbuf, "{D+{c==========================================================================={x\n\r\n\r");
			send_to_char(hbuf, ch);
		}

}
    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0') {
	argument = one_argument(argument, argone);
	if (argall[0] != '\0')
	    strcat(argall, " ");
	strcat(argall, argone);
    }

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

	if (level > get_trust(ch))
	    continue;

	if (is_name(argall, (pHelp->keyword[0] == '{') ? pHelp->keyword + 2 : pHelp->keyword)) {
	    /* add seperator if found */
	    if (found)
		add_buf(output, "\n\r{c==========================================================================={x\n\r\n\r");
	    if (pHelp->level >= 0 && str_cmp(argall, "imotd")) {
		add_buf(output, pHelp->keyword);
		add_buf(output, "\n\r{c---------------------------------------------------------------------------{x\n\r");
	    }
	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if (pHelp->text[0] == '.')
		add_buf(output, pHelp->text + 1);
	    else
		add_buf(output, pHelp->text);
	    found = TRUE;
	    /* small hack :) */
	    if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
		&& ch->desc->connected != CON_GEN_GROUPS)
		break;
	}
    }

    if (!found && !rf) {
	send_to_char("No help on that word.\n\r", ch);
	append_file(ch, HELP_FILE, nohelp);
    } else
	page_to_char(buf_string(output), ch);
    free_buf(output);
}

void do_help2(CHAR_DATA * ch, char *argument)
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
    int level;

    output = new_buf();

    if (argument[0] == '\0')
	argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0') {
	argument = one_argument(argument, argone);
	if (argall[0] != '\0')
	    strcat(argall, " ");
	strcat(argall, argone);
    }

    for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next) {
	level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

	if (level > get_trust(ch))
	    continue;

	if (is_name(argall, pHelp->keyword)) {
	    /* add seperator if found */
	    if (found)
		add_buf(output,
			"\n\r{c============================================================{x\n\r\n\r");
	    if (pHelp->level >= 0 && str_cmp(argall, "imotd")) {
		add_buf(output, pHelp->keyword);
		add_buf(output, "\n\r{c--------------------------------------------{x\n\r");
	    }
	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if (pHelp->text[0] == '.')
		add_buf(output, pHelp->text + 1);
	    else
		add_buf(output, pHelp->text);
	    found = TRUE;
	    /* small hack :) */
	    if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
		&& ch->desc->connected != CON_GEN_GROUPS)
		break;
	}
    }

    if (!found)
	send_to_char("No help on that word.\n\r", ch);
    else
	send_to_char(buf_string(output), ch);
    free_buf(output);
}

void do_finger(CHAR_DATA * ch, char *argument)
{
    FILE *fp;
    int i;
    char strsave[MAX_STRING_LENGTH];
    bool laston_found = FALSE;
    bool host_found = FALSE;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    char *host = "";
    char *race = "";
    char *fClan;
    int kingdom = SIDE_NONE;
    int judge = 0;
//    char * side = "";
    int class;
    char *clan = "None";
    char *desc = "None";
    int warpoints;
    int sex = SEX_MALE;
    int playerkilled = 0;
    int plyd = 0;

    long laston;
    bool finished = FALSE;
    int d, h, m, s = 0;
    int level;
    int version = 0;

    level = 101;
    warpoints = 0;
    class = 0;
    kingdom = 0;

    if (argument[0] == '\0' || argument[0] == '/' || argument[0] == '\\' || argument[0] == '.') {
	send_to_char("Finger who?\n\r", ch);
	return;
    }
    smash_tilde(argument);
    argument = one_argument(argument, arg);
    strcpy(arg2, argument);

    sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(arg));

    if ((fp = fopen(strsave, "r")) == NULL) {
	sprintf(buf, "No such player: %s.\n\r", capitalize(arg));
	send_to_char(buf, ch);
	return;
    }
    fclose(fp);

    if (((victim = get_char_world(ch, arg)) != NULL) && (!IS_NPC(victim))) {

	sprintf(buf, "{c___/Finger: {x%-16s{c\\_______________________________{x\n\r",
		capitalize(arg));
	send_to_char(buf, ch);

	if (IS_IMMORTAL(ch)) {
	    sprintf(buf, "Level: %3d\n\r", victim->level);
	    send_to_char(buf, ch);
	}

	
	if (victim->level >= LVL_LEGEND && victim->level < HERO) {
	    if (victim->sex == SEX_MALE)
		sprintf(buf, "{yLegendary{x\n\r");
	    else
		sprintf(buf, "{yLegendary{x\n\r");
	    send_to_char(buf, ch);  
	} 
	
	if (victim->level >= LVL_REGENT && victim->level < LVL_LEGEND) {
	    if (victim->sex == SEX_MALE)
		sprintf(buf, "{yRegent{x\n\r");
	    else
		sprintf(buf, "{yRegentress{x\n\r");
	    send_to_char(buf, ch);    
	} 
	
	if (victim->level >= LVL_LORD && victim->level < LVL_REGENT) {
	    if (victim->sex == SEX_MALE)
		sprintf(buf, "{yLord{x\n\r");
	    else
		sprintf(buf, "{yLady{x\n\r");
	    send_to_char(buf, ch);
	} 
	
	if (victim->level >= LVL_MASTER && victim->level < LVL_LORD) {
	    if (victim->sex == SEX_MALE)
		sprintf(buf, "{yMaster{x\n\r");
	    else
		sprintf(buf, "{yMistress{x\n\r");
	    send_to_char(buf, ch);
	} 
	
	if (victim->level >= LVL_DUKE && victim->level < LVL_MASTER) {
	    if (victim->sex == SEX_MALE)
		sprintf(buf, "{yDuke{x\n\r");
	    else
		sprintf(buf, "{yDuchess{x\n\r");
	    send_to_char(buf, ch);
	} 
	
	if (victim->level >= LVL_COUNT && victim->level < LVL_DUKE) {
	    if (victim->sex == SEX_MALE)
		sprintf(buf, "{yCount{x\n\r");
	    else
		sprintf(buf, "{yCountess{x\n\r");
	    send_to_char(buf, ch);

	} 
	
	if (victim->level >= LVL_BARON && victim->level < LVL_COUNT) {
	    if (victim->sex == SEX_MALE)
		sprintf(buf, "{yBaron{x\n\r");
	    else
		sprintf(buf, "{yBaroness{x\n\r");
	    send_to_char(buf, ch);
	}
	    
	
//	} else if (victim->pcdata->judge == 3 && victim->level >=50) {
//	    sprintf(buf, "{rMercenary{x\n\r");
//	    send_to_char(buf, ch);
//	} else if (victim->pcdata->judge == 3 && victim->level >=54) {
//	    sprintf(buf, "{rSer{x\n\r");
//	    send_to_char(buf, ch);
	
	if (victim->level >= LEVEL_HERO && victim->level <= CREATOR) {
	    sprintf(buf, "{gImmortal{x\n\r");
	    send_to_char(buf, ch);
	} 
	
	if (victim->level == ADMINISTRATOR) {
	    sprintf(buf, "{gAdministrator{x\n\r");
	    send_to_char(buf, ch);
	}
	if (victim->level == 111)
	{
		sprintf(buf, "{gImplementor{x\n\r");
		stc(buf, ch);
	}
	sprintf(buf, "Race: %-12s        Class: %-16s\n\r",
		capitalize(pc_race_table[victim->race].name),
		class_table[victim->class].name);
	send_to_char(buf, ch);

	if (victim->level < HERO)
	    sprintf(buf, "Kingdom: %-16s Warpoints: %3d\n\r",
		    kingdom_table[victim->side].name,
		    victim->warpoints);
	else
	    sprintf(buf, "Kingdom: ALL              Warpoints: %5d\n\r",
		    victim->warpoints);
	send_to_char(buf, ch);

//	sprintf(buf, "PlayerKilled: %-5d       Warscore:  {W%5d{x\n\r",
//		victim->playerkilled, victim->warpoints - victim->playerkilled);
//	send_to_char(buf, ch);

sprintf(buf, "Played:    %5dh         Clan: %-16s{x\n\r", (victim->played + (int) (current_time - victim->logon)) / 3600, 
	clan_table[victim->clan].who_name);
	send_to_char(buf, ch);
	
/*sprintf(buf, "Clan: %-16s    Played:    %5dh\n\r", clan_table[victim->clan].who_name,
		(victim->played + (int) (current_time - victim->logon)) / 3600);
	send_to_char(buf, ch);*/

	sprintf(buf, "Description:\n\r%s{x\n\r", victim->description);
	send_to_char(buf, ch);

	if (victim->desc != NULL)
	    sprintf(buf, "%s is currently playing.\n\r", victim->name);
	else
	    sprintf(buf, "%s is currently linkdead.\n\r", victim->name);

	if ((ch->side == victim->side && can_see(ch, victim)) || IS_IMMORTAL(ch))
	    send_to_char(buf, ch);


if (victim->pcdata->judge == 1) 
	sprintf(buf, "{c                                               | {CConstable{c |{x\n\r{c____________________________________________________________{x\n\r");
else if (victim->pcdata->judge == 2) 
	sprintf(buf, "{c                                          | {CHead Constable {c|{x\n\r{c____________________________________________________________{x\n\r");
else if (victim->pcdata->judge == 3)                   
	sprintf(buf, "{r                                               | {RMercenary{r |{x\n\r{c____________________________________________________________{x\n\r");

else
	sprintf(buf, "{c____________________________________________________________{x\n\r");
	send_to_char(buf, ch);

	return;
    }
    sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(arg));
    if ((fp = fopen(strsave, "r")) == NULL) {
	sprintf(buf, "No such player: %s.\n\r", capitalize(arg));
	send_to_char(buf, ch);
	return;
    }
    i = 0;

    for (;;) {
	char *word;
	fread_to_eol(fp);
	word = feof(fp) ? "End" : fread_word(fp);

	i++;

	switch (UPPER(word[0])) {
	case 'V':
	    if (!str_cmp(word, "Vers")) {
		version = fread_number(fp);
	    }
	    break;
	case 'R':
	    if (!str_cmp(word, "Race")) {
		race = fread_string(fp);
	    }
	    break;
	case 'S':

	    if (!str_cmp(word, "Side")) {

		if (version == 5) {
		    kingdom = fread_number(fp);
		    if (kingdom == 0)
			kingdom = SIDE_TIER;
		    else if (kingdom == 1)
			kingdom = SIDE_KAEL;
		    else if (kingdom == 2)
			kingdom = SIDE_UM;
		    else
			kingdom = SIDE_NONE;
		} else if (version == 6) {
		    char *temp;
		    temp = fread_string(fp);

		    if (!str_cmp(temp, "Tier"))
			kingdom = SIDE_TIER;
		    else if (!str_cmp(temp, "Kael"))
			kingdom = SIDE_KAEL;
		    else if (!str_cmp(temp, "Undermountain"))
			kingdom = SIDE_UM;
		    else if (!str_cmp(temp, "Cairn"))
			kingdom = SIDE_CAIRN;
		}
	    }
	    if (!str_cmp(word, "Sex")) {
		sex = fread_number(fp);
	    }
	    break;
	case 'D':
	    if (!str_cmp(word, "Desc")) {
		desc = fread_string(fp);
	    }
	    break;
	case 'W':
	    if (!str_cmp(word, "Warpoints")) {
		warpoints = fread_number(fp);
	    }
	    break;
	case 'C': 
          if (!str_cmp(word, "Clan")) { 
                fClan = fread_string(fp); 
                 clan = clan_table[clan_lookup(fClan)].who_name; 
            free_string(fClan);
           /*clan = fread_string(fp);*/
          }
	    if (!str_cmp(word, "Cla")) {
		if (version == 6) {
		    char *temp;
		    temp = fread_string(fp);
		    class = 0;
		    while (str_cmp(class_table[class].name, temp))
			class++;
			free_string(temp);
		} else if (version == 5) {
		    class = fread_number(fp);
		}
	    }
	    break;

	case 'H':
	    if (!str_cmp(word, "Host")) {
		host = fread_string(fp);
		host_found = TRUE;
	    }
	    break;
      case 'J':
	    if (!str_cmp(word, "Judge")) {
		judge = fread_number(fp);
	    }
	    break;
	case 'P':
	    if (!str_cmp(word, "Playerkilled")) {
		playerkilled = fread_number(fp);
	    }
	    if (!str_cmp(word, "Plyd")) {
		plyd = fread_number(fp);
	    }
	    break;
	case 'E':
	    if (!str_cmp(word, "End")) {
		finished = TRUE;
		break;
	    }
	    break;
	case 'L':
	    if (!str_cmp(word, "LogO")) {
		laston = fread_number(fp);
		laston_found = TRUE;
	    }
	    if (!str_cmp(word, "Levl")) {
		level = fread_number(fp);
	    }
	    break;
	case '#':
	    {
		finished = TRUE;
		break;
	    }
	default:
	    /*fread_to_eol(fp); */
	    break;
	}
	if (finished) {
	    break;
	}
    }

    s = current_time - laston;
    d = s / 86400;
    s -= d * 86400;
    h = s / 3600;
    s -= h * 3600;
    m = s / 60;
    s -= m * 60;

    sprintf(buf, "{c___/Finger: {x%-16s{c\\_______________________________{x\n\r",
	    capitalize(arg));
    send_to_char(buf, ch);
    if (IS_IMMORTAL(ch)) {
	sprintf(buf, "Level: %3d\n\r", level);
	send_to_char(buf, ch);
    } 

	if (level >= LVL_LEGEND && level <= HERO) {
		sprintf(buf, "{yLegend{x\n\r");
		send_to_char(buf, ch);
	}
	

	if (level >= LVL_REGENT && level < LVL_LEGEND) {
	if (sex == SEX_MALE)
	    sprintf(buf, "{yRegent{x\n\r");
	else
	    sprintf(buf, "{yRegentress{x\n\r");
	send_to_char(buf, ch);
	
    }  
	
	if (level >= LVL_LORD && level < LVL_REGENT) {
	if (sex == SEX_MALE)
	    sprintf(buf, "{yLord{x\n\r");
	else
	    sprintf(buf, "{yLady{x\n\r");
	send_to_char(buf, ch);
    } 
	
	if (level >= LVL_MASTER && level < LVL_LORD) {
	if (sex == SEX_MALE)
	    sprintf(buf, "{yMaster{x\n\r");
	else
	    sprintf(buf, "{yMistress{x\n\r");
	send_to_char(buf, ch);
    } 
	
	if (level >= LVL_DUKE && level < LVL_MASTER) {
	if (sex == SEX_MALE)
	    sprintf(buf, "{yDuke{x\n\r");
	else
	    sprintf(buf, "{yDuchess{x\n\r");

	send_to_char(buf, ch);

    } 
	
	if (level >= LVL_COUNT && level < LVL_DUKE) {
	if (sex == SEX_MALE)
	    sprintf(buf, "{yCount{x\n\r");
	else
	    sprintf(buf, "{yCountess{x\n\r");
	send_to_char(buf, ch);

    } 
	
	if (level >= LVL_BARON && level < LVL_COUNT) {
	if (sex == SEX_MALE)
	    sprintf(buf, "{yBaron{x\n\r");
	else
	    sprintf(buf, "{yBaroness{x\n\r");
	send_to_char(buf, ch);
	
	} 
 
//    } else if (pcdata->judge == 3 && level >= 50){
//	sprintf(buf, "{rMercenary{x\n\r");
//	send_to_char(buf, ch);
//   } else if (pcdata->judge == 3 && level >= 54){
//	sprintf(buf, "{rSer{x\n\r");
//	send_to_char(buf, ch);

    else if (level >= HERO&& level <= CREATOR) {
	sprintf(buf, "{gImmortal{x\n\r");
	send_to_char(buf, ch);
    } else if (level == ADMINISTRATOR) {
	sprintf(buf, "{gAdministrator{x\n\r");
	send_to_char(buf, ch);
    }
	else if (level == IMPLEMENTOR)
	{
		sprintf(buf, "{gImplementor{x\n\r");
		stc(buf,ch);
	}
    sprintf(buf, "Race: %-12s        Class: %-16s\n\r",
	    capitalize(race),
	    class_table[class].name);
    send_to_char(buf, ch);
    free_string(race);

    if (level < HERO)
	sprintf(buf, "Kingdom: %-16s Warpoints: %5d\n\r",
		kingdom_table[kingdom].name,
		warpoints);
    else
	sprintf(buf, "Kingdom: ALL              Warpoints: %5d\n\r",
		warpoints);
    send_to_char(buf, ch);

//    sprintf(buf, "PlayerKilled: %-5d       Warscore:  {W%5d{x\n\r",
//	    playerkilled, warpoints - playerkilled);
//    send_to_char(buf, ch);

    sprintf(buf, "Played:    %5dh         Clan: %-16s\n\r", plyd / 3600, (!str_cmp(clan, "None")) ? " " : clan);
    send_to_char(buf, ch);

    //sprintf(buf, "Clan: %-16s         Played:    %5dh\n\r", clan, plyd / 3600);
    //send_to_char(buf, ch);

    sprintf(buf, "Description:\n\r%s\n\r", (!str_cmp(desc, "None")) ? " " : desc);
    send_to_char(buf, ch);
	    if (str_cmp(desc, "None"))
      free_string(desc);

    sprintf(buf, "{x%s last logged on at %s\n\r", capitalize(arg),
	    laston_found ? ctime(&laston) : "Not found.");
    if ((ch->side == kingdom && level < HERO) || IS_IMMORTAL(ch))
	send_to_char(buf, ch);
    sprintf(buf, "Last on: %d days %d hours %d minutes ago. \n\r", d, h, m);
    if ((ch->side == kingdom && level < HERO) || IS_IMMORTAL(ch))
	send_to_char(buf, ch);

    if (IS_IMMORTAL(ch)) {

	if (host_found)
	    sprintf(buf, "%s last logged on from *%s\n\r", capitalize(arg), host);
	else
	    sprintf(buf, "No site found for %s.\n\r", capitalize(arg));
	    free_string(host);

	send_to_char(buf, ch);
    }
    if (judge == 1) 
	sprintf(buf, "{c                                               | {CConstable{c |{x\n\r{c____________________________________________________________{x\n\r");
else if (judge == 2) 
	sprintf(buf, "{c                                          | {CHead Constable {c|{x\n\r{c____________________________________________________________{x\n\r");
else if (judge == 3)
	sprintf(buf, "{r                                               | {RMercenary {r|{x\n\r{c____________________________________________________________{x\n\r");
else
	sprintf(buf, "{c____________________________________________________________{x\n\r");
	send_to_char(buf, ch);


    fclose(fp);
    return;
}

void PERS(CHAR_DATA * ch, CHAR_DATA * looker, char *pers_buf)
{
    char buf[MAX_STRING_LENGTH];
    pers_buf[0] = '\0';

    if (ch == NULL || looker == NULL) {
	sprintf(buf, "{Rno one{X");
	strcat(pers_buf, buf);
	return;
    }
    if (IS_IMMORTAL(looker) && !IS_NPC(ch)) {
	sprintf(buf, "%s", ch->name);
	strcat(pers_buf, buf);
	return;
    }
    if (!can_see(looker, ch)) {
	sprintf(buf, "someone");
	strcat(pers_buf, buf);
	return;
    }
    if (IS_NPC(looker) && !IS_NPC(ch)) {
	sprintf(buf, "%s", ch->name);
	strcat(pers_buf, buf);
	return;
    }
    if (IS_NPC(ch)) {
	sprintf(buf, "%s", ch->short_descr);
	strcat(pers_buf, buf);
	return;
    }
    if (!IS_NPC(ch) && !IS_NPC(looker)) {
		
		if (ch->level == 111)
		{
			sprintf(buf, "Implementor %s", ch->name);
			strcat(pers_buf, buf);
			return;
		} else
	if (ch->level == 110) {
	    sprintf(buf, "Administrator %s", ch->name);
	    strcat(pers_buf, buf);
	    return;
	} else if (ch->level == 101) {
	    sprintf(buf, "Visitor %s",ch->name);
	    strcat(pers_buf, buf);
	    return;
	} else if (ch->level == 109) {
	    sprintf(buf, "Immortal %s",ch->name);
	    strcat(pers_buf, buf);
	    return;
	} else if (ch->level == 106) {
	    sprintf(buf, "Questor %s",ch->name);
	    strcat(pers_buf, buf);
	    return;
	} else if (ch->level == 105) {
	    sprintf(buf, "Builder %s",ch->name);
	    strcat(pers_buf, buf);
	    return;
	} else if (ch->level >= 109) {
	    sprintf(buf, "Creator %s",ch->name);
	    strcat(pers_buf, buf);
	    return;
	} else if (ch->level >= 101) {
	    sprintf(buf, "Immortal %s",ch->name);
	    strcat(pers_buf, buf);
	    return;
	}
	if (GET_SIDE(ch) == GET_SIDE(looker)) {
          if (ch->level >= LVL_LEGEND)
			sprintf(buf, "Legendary %s", ch->name);
			
	  else if (ch->level >= LVL_REGENT)
			sprintf(buf, "Regent %s", ch->name);

          else if (ch->level >= LVL_LORD)
			if (ch->sex == SEX_MALE)
				sprintf(buf, "Lord %s", ch->name);
			else
			    sprintf(buf, "Lady %s", ch->name);
	    else if (ch->level >= LVL_MASTER)
			if (ch->sex == SEX_MALE)
			    sprintf(buf, "Master %s", ch->name);
			else
				sprintf(buf, "Mistress %s", ch->name);
	    else if (ch->level >= LVL_DUKE)
			if (ch->sex == SEX_MALE)
			    sprintf(buf, "Duke %s", ch->name);
			else
				sprintf(buf, "Duchess %s", ch->name);
	
	    else if (ch->level >= LVL_COUNT)
			if (ch->sex == SEX_MALE)
			    sprintf(buf, "Count %s", ch->name);
			else
				sprintf(buf, "Countess %s", ch->name);

	    else if (ch->level >= LVL_BARON)
			if (ch->sex == SEX_MALE)
			    sprintf(buf, "Baron %s", ch->name);
			else
				sprintf(buf, "Baroness %s", ch->name);
	    else if (ch->pcdata->judge == 3 && ch->level >= 50)
			   sprintf(buf, "Mercenary %s", ch->name);
	    else if (ch->pcdata->judge == 3 && ch->level >= 54)
			   sprintf(buf, "Ser %s the Hedge Knight",ch->name);
	
		else
			sprintf(buf, "%s", ch->name);
	} else {
	    
	   if (ch->level >= LVL_LEGEND)
		sprintf(buf, "*Legendary %s*", ch->name);
		
	   else if (ch->level >= LVL_REGENT)
		sprintf(buf, "*Regent %s*", ch->name);
		
	    else if (ch->level >= LVL_LORD)
		if (ch->sex == SEX_MALE)
		    sprintf(buf, "*Lord %s*", ch->name);
		else
		    sprintf(buf, "*Lady %s*", ch->name);
	    else if (ch->level >= LVL_MASTER)
		if (ch->sex == SEX_MALE)
		    sprintf(buf, "*Master %s*", ch->name);
		else
		    sprintf(buf, "*Mistress %s*", ch->name);
	    else if (ch->level >= LVL_DUKE)
		if (ch->sex == SEX_MALE)
		    sprintf(buf, "*Duke %s*", ch->name);
		else
		    sprintf(buf, "*Duchess %s*", ch->name);
	    else if (ch->level >= LVL_COUNT)
		if (ch->sex == SEX_MALE)
		    sprintf(buf, "*Count %s*", ch->name);
		else
		    sprintf(buf, "*Countess %s*", ch->name);

	    else if (ch->level >= LVL_BARON)
		if (ch->sex == SEX_MALE)
		    sprintf(buf, "*Baron %s*", ch->name);
		else
		    sprintf(buf, "*Baroness %s*", ch->name);
	    else if (ch->pcdata->judge == 3 && ch->level >= 50)
		sprintf(buf, "*Mercenary %s*", ch->name);
	    else if (ch->pcdata->judge == 3 && ch->level >= 54)
		sprintf(buf, "*Ser %s the Hedge Knight*", ch->name);


	    else {
		if (ch->level <= LVL_NEWBY)
		    sprintf(buf, "*NEWBIE %s from %s*", pc_race_table[ch->race].room_name, kingdom_table[ch->side].name);
		else
		    sprintf(buf, "*A %s from %s*", pc_race_table[ch->race].room_name, kingdom_table[ch->side].name);
	    }
	}
    }
    strcat(pers_buf, buf);
    return;
}

void set_prename(CHAR_DATA *ch)
{
    char * buf2;
	buf2 = NULL;

    if (ch->level > HERO)
		sprintf(buf2, "Immortal");
    else if (ch->level >= LVL_LORD)
		if (ch->sex == SEX_MALE)
			sprintf(buf2, "Lord");
		else
			sprintf(buf2, "Lady");
	else if (ch->level >= LVL_MASTER)
		if (ch->sex == SEX_MALE)
			sprintf(buf2, "Master");
		else
			sprintf(buf2, "Mistress");
	else if (ch->level >= LVL_DUKE)
		if (ch->sex == SEX_MALE)
			sprintf(buf2, "Duke");
		else
			sprintf(buf2, "Duchess");
	else if (ch->level >= LVL_COUNT)
		if (ch->sex == SEX_MALE)
			sprintf(buf2, "Count");
		else
			sprintf(buf2, "Countess");

	else if (ch->level >= LVL_BARON)
		if (ch->sex == SEX_MALE)
			sprintf(buf2, "Baron");
		else
			sprintf(buf2, "Baroness");
	
	else if (ch->level >= LVL_REGENT)

		sprintf(buf2, "Regent");
			
	else if (ch->level >= LVL_LEGEND)

		sprintf(buf2, "Legendary");

	
	
	else if (ch->pcdata->judge == 3 && ch->level >= 50)
		sprintf(buf2, "Mercenary");
	else if (ch->pcdata->judge == 3 && ch->level >= 54)
		sprintf(buf2, "Hedge Knight");

	else if (ch->level < LVL_LEGEND)
		sprintf(buf2, "WEEEEE HEEEEEE");
    strcpy(ch->pcdata->prename, buf2);
}

void do_remort(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int race;

    if (IS_NPC(ch))
	return;

    one_argument(argument, arg);

    if (argument[0] == '\0') {
	send_to_char("Remort into what ?\n\r", ch);
	if (GET_SIDE(ch) == SIDE_CAIRN) {
	    send_to_char("No Choices Yet. \n\r", ch);
	    return;
	} else if (GET_SIDE(ch) == SIDE_UM) {
	    send_to_char("Your choices: Balrog, Vampire, Demon, Shadow, Lich, Cerebus, Devil, Succubi, Other, Human, Harpy, and Bristleback.\n\r", ch);
	    return;
	} else {
	    send_to_char("Your choices: Storm Giant, Phoenix, Aratar, High Elf, Mhrynn, Aiel, Archon, Grey Man, Viera, Human, Satyr, and Tauren.\n\r", ch);
	    return;
	}
    }
    race = pc_race_lookup(arg);

    if (!pc_race_table[race].isRemort) {
	send_to_char("Not a valid choice.\n\r", ch);
	return;
    }
    if (GET_SIDE(ch) == SIDE_TIER)
	if (!IS_SET(pc_race_table[race].side, CANBE_TIER)) {
	    send_to_char("Not a valid choice for Tierans.\n\r", ch);
	    return;
	}
    if (GET_SIDE(ch) == SIDE_KAEL)
	if (!IS_SET(pc_race_table[race].side, CANBE_KAEL)) {
	    send_to_char("Not a valid choice for Kaeleans.\n\r", ch);
	    return;
	}
    if (GET_SIDE(ch) == SIDE_UM)
	if (!IS_SET(pc_race_table[race].side, CANBE_UM)) {
	    send_to_char("Not a valid choice for Undermountain.\n\r", ch);
	    return;
	}
    if (GET_SIDE(ch) == SIDE_CAIRN)
	if (!IS_SET(pc_race_table[race].side, CANBE_CAIRN)) {
	    send_to_char("Not a valid choice for Cairnians.\n\r", ch);
	    return;
	}
	
	OBJ_DATA *obj_next;
    OBJ_DATA *obj;
	
	
    for (obj = ch->carrying; obj; obj = obj_next) {
	obj_next = obj->next_content;

	if (obj->wear_loc != WEAR_NONE)
	    remove_obj(ch, obj->wear_loc, TRUE);
    }

    save_char_obj(ch);
    /* put in if you already have morted once */
    if (ch->race >= 1 && ch->race <= 16) {
	send_to_char("You have already morted once.\n\r", ch);
	return;
	
	
}
if (pc_race_table[ch->race].isRemort){
	
	send_to_char("You have already morted once.\n\r",ch);
	return;
    }
    /* ok get ready to reset a character */
    ch->level = 6;
//	set_prename(ch);
//  Lets have the stuff carry over  

    ch->pcdata->perm_hit = 140 + ch->spentOnHps;
    ch->hit = 140 + ch->spentOnHps;
    ch->max_hit = 140 + ch->spentOnHps;

    ch->pcdata->perm_mana = 180 + ch->spentOnMana;
    ch->mana = 180 + ch->spentOnMana;
    ch->max_mana = 180 + ch->spentOnMana;

    ch->pcdata->perm_move = 130;
    ch->move = 130;
    ch->max_move = 130;

    ch->race = race;

    ch->pcdata->points = 65;
    ch->exp = ch->level * exp_per_level(ch, 65);

    send_to_char("The power of the gods takes hold of you.\n\r", ch);
    send_to_char("Your flesh peels from your body and burns to the ground.\n\r", ch);
    send_to_char("Your mind is blanked by powerfull magic.\n\r", ch);
    send_to_char("Thoughts fade... You feel...... \n\r", ch);
    send_to_char("Awaken... You have become a new race.\n\r", ch);
    sprintf(buf, "You now sense the true power of a %s.\n\r", pc_race_table[race].name);

    send_to_char(buf, ch);
    send_to_char("You must be logged out to reset your charcter..\n\r", ch);
    send_to_char("Please log back in now!!\n\r", ch);

    ch->pcdata->confirm_delete = TRUE;
    do_quit(ch, "");

    return;
}

void do_recruit(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (IS_NPC(ch))
	return;

    one_argument(argument, arg);

   if (!IS_SET(ch->act, PLR_CLANLEADER)) {
	send_to_char("You are not a leader you can't do it.\n\r", ch);
	return;
   }
    if (arg[0] == '\0') {
	send_to_char("Who do you want to recruit?\n\r", ch);
	return;
    }
    if ((victim = get_char_world(ch, arg)) == NULL) {
	send_to_char("No-one around by that name.\n\r", ch);
	return;
    }
    if (IS_NPC(victim)) {
	send_to_char("You can't recruit that.\n\r", ch);
	return;
    }
    if (GET_SIDE(victim) != GET_SIDE(ch)) {
	send_to_char("You can't recruit enemies!\n\r", ch);
	return;
    }
    if (victim->level < 20) {
	send_to_char("They are to low level to recruit.\n\r", ch);
	return;
    }
    if (victim->clan != 0) {
	if (victim->clan == ch->clan) {
	    send_to_char("You have removed them from the clan.\n\r", ch);
	    send_to_char("You have been removed from the clan!\n\r", victim);
	    victim->clan = 0;
	    return;
	}
    }
    if (IS_SET(victim->act, PLR_NOCLAN)) {
	send_to_char("They have noclan (ON) they don't want to be recruited.\n\r", ch);
	return;
    }
    victim->clan = ch->clan;
    send_to_char("They are now a member of your clan.\n\r", ch);

    sprintf(buf, "You have just been recruited into %s's clan. Welcome!!\n\r", ch->name);
    send_to_char(buf, victim);

    do_noclan(victim, "");
}

void do_noclan(CHAR_DATA * ch, char *argument)
{
    if (IS_NPC(ch))
	return;

    if (IS_SET(ch->act, PLR_NOCLAN)) {
	REMOVE_BIT(ch->act, PLR_NOCLAN);
	send_to_char("Your now available to be recruited into a clan.\n\r", ch);
	ch->clan = 0;
    } else {
	SET_BIT(ch->act, PLR_NOCLAN);
	send_to_char("You no longer can be recruited into a clan.\n\r", ch);
    }
    return;
}

/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
  //  char buf4[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    FILE *fp; /*Max-ever*/
    int iClass;
    int iRace;
    int iClan;
	int wlevel;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    int nEnemies;
    int nPlayers;
    int count, max;
	//int umCount=0, kaelCount=0, tierCount=0/*, cairnCount, noneCount*/;
    bool rgfClass[MAX_CLASS];
    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[MAX_CLAN];
    bool fClassRestrict = FALSE;
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
    bool ConstOnly = FALSE;
 // bool NQonly = FALSE;
 	count = 0;


 	if ( IS_NPC(ch) || ch->desc == NULL )
    	return;

//    for ( d = descriptor_list; d != NULL; d = d->next )
//		{
//		if ((d->character->side == SIDE_TIER) && (!IS_IMMORTAL(d->character)) && (d->character->level >= LVL_BARON))
//		{
//			tierCount++;
//			continue;
//		}
//		if ((d->character->side == SIDE_KAEL) && (!IS_IMMORTAL(d->character)) && (d->character->level >= LVL_BARON))
//		{
//			kaelCount++;
//			continue;
//		}
//		if ((d->character->side == SIDE_UM) && (!IS_IMMORTAL(d->character)) && (d->character->level >= LVL_BARON))
//		{
//			umCount++;
//			continue;
//		}
//		
//		}


    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && can_see( ch, d->character ) )
	    count++;

    max_on = UMAX(count,max_on);

 	
 	if ( ( fp = fopen( MAX_WHO_FILE,"r" ) ) == NULL )
	{
	    log_string("Error reading from maxwho.txt");
	    return;
	}
	max = fread_number( fp );
	fclose(fp);

	if ( max_on > max )
	{
		if ( ( fp = fopen( MAX_WHO_FILE,"w" ) ) == NULL )
		{
		    log_string("Error writing to maxwho.txt");
		    return;
		}
		fprintf( fp, "%d\n", max_on );
		fclose(fp);
	}

 

    /*
     * Set default arguments.
     */
    iLevelLower = 0;
    iLevelUpper = MAX_LEVEL;
    for (iClass = 0; iClass < MAX_CLASS; iClass++)
	rgfClass[iClass] = FALSE;
    for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
	rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan < MAX_CLAN; iClan++)
	rgfClan[iClan] = FALSE;

    /*
     * Parse arguments.
     */

    nNumber = 0;
    for (;;) {
	char arg[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);
	if (arg[0] == '\0')
	    break;

if (is_number(arg)) {
	    if (IS_IMMORTAL(ch))
		switch (++nNumber) {
		case 1:
		    iLevelLower = atoi(arg);
		    break;
		case 2:
		    iLevelUpper = atoi(arg);
		    break;
		default:
		    send_to_char("Only two level numbers allowed.\n\r", ch);
		    return;
		}
	} else {
	    /*
	     * Look for classes to turn on.
	     */
	    if (!str_prefix(arg, "immortals")) 
		fImmortalOnly = TRUE;
	     else
	    if (!str_prefix(arg, "constable"))
	      ConstOnly = TRUE;
	       else 
	    //if (ch == IS_IMMORTAL(!str_prefix(arg, "noquit")) 
	    // 	NQonly = TRUE;
	    //   else
	       {
		iClass = class_lookup(arg);
		if (iClass == -1) {
		    iRace = pc_race_lookup(arg);

		    if (iRace == 0 || iRace >= MAX_PC_RACE) {
			if (!str_prefix(arg, "clan"))
			    fClan = TRUE;
			else {
			
			// Izlude, trying to fix who crash bug. No more who "clan" until a better fix comes in.
			send_to_char( "You can no longer look up clans.\n\r",ch);
			return;
		}
		/*	{
			    iClan = clan_lookup(arg);
			    if (iClan) {
				fClanRestrict = TRUE;
				rgfClan[iClan] = TRUE;
			    } else {
				send_to_char( "That's not a valid race, class, or clan.\n\r", ch);
				return;
			    }
			} 
		*/
		    
			
			} else {
			fRaceRestrict = TRUE;
			rgfRace[iRace] = TRUE;
		    }
		} else {
		    fClassRestrict = TRUE;
		    rgfClass[iClass] = TRUE;
		}
	    }
	}
    }

    /*
     * Now show matching chars.
     */
    nMatch = 0;
    nPlayers = 0;
    nEnemies = 0;
    buf[0] = '\0';
    output = new_buf();

    sprintf(buf2, "{m##############{x Who Is Playing? {m-{x AoW %s {m#################{x\n\r", AOW_VERSION);
    add_buf(output, buf2);

/* Akuji -- Organizing Who by level */
  for( wlevel=MAX_LEVEL; wlevel>0; wlevel-- )
 {
    for (d = descriptor_list; d != NULL; d = d->next) {
	CHAR_DATA *wch;
	char const *class;
	char const *race;



	/*
	 * Check for match against restrictions.
	 * Don't use trust as that exposes trusted mortals.
	 */
	if (d->connected != CON_PLAYING || !can_see(ch, d->character))
	    continue;

	wch = (d->original != NULL) ? d->original : d->character;
      if( wch->level != wlevel )
      continue;
	nPlayers++;

	if (!can_see(ch, wch))
	    continue;

	if (wch->level < iLevelLower
	    || wch->level > iLevelUpper
	    || (fImmortalOnly && wch->level < LEVEL_IMMORTAL)
	    || (ConstOnly && wch->pcdata->judge != 1 && wch->pcdata->judge != 2)
	    || (fClassRestrict && !rgfClass[wch->class])
	    || (fRaceRestrict && !rgfRace[wch->race])
	    || (fClan && !is_clan(wch))
	    || (fClanRestrict && !rgfClan[wch->clan]))
	//  || (NQonly && IS_AFFECTED(wch, AFF_NOQUIT))
	    continue;

	if ((ch->side != wch->side) && (wch->level < LEVEL_IMMORTAL) && (ch->level < LEVEL_IMMORTAL)) {
	    nEnemies++;
	    continue;
	}
	if ((ch->side == wch->side) && (wch->level < LEVEL_IMMORTAL) && (ch->level < LEVEL_IMMORTAL))
	    nMatch++;

	/*
	 * Figure out what to print for class.
	 */
	class = class_table[wch->class].who_name;
	race = pc_race_table[wch->race].who_name;
	switch (wch->level) {
	default:
	    break;
	    {
	case MAX_LEVEL:
		race = "{CIMP{x";
		break;
	case MAX_LEVEL - 1:
		race = "{cADM{x";
		break;
	case MAX_LEVEL - 2:
		race = "{cIMM{x";
		break;
	case MAX_LEVEL - 3:
		race = "{cIMM{x";
		break;
	case MAX_LEVEL - 4:
		race = "{cIMM{x";
		break;
	case MAX_LEVEL - 5:
		race = "{cIMM{x";
		break;
	case MAX_LEVEL - 6:
		race = "{cBLD{x";
		break;
	case MAX_LEVEL - 7:
		race = "{cFAG{x";
		break;
	case MAX_LEVEL - 8:
		race = "{cDOG{x";
		break;
	case MAX_LEVEL - 9:
		race = "{cZIG{x";
		break;
	case MAX_LEVEL - 10:
		race = "{cVIS{x";
		break;
	case MAX_LEVEL - 11:
		race = "{c100{x";
	    }
	}

	
	

	if (wch->level >= LVL_BARON && wch->level <= HERO)
	    race = "{gBAR{x";

	if (wch->level >= LVL_COUNT && wch->level <= HERO){
	    if (wch->sex == SEX_MALE)
		race = "{gCNT{x";
	    else
		race = "{gCTS{x";
        }

	if (wch->level >= LVL_DUKE && wch->level <= HERO){
	    if (wch->sex == SEX_MALE)
		race = "{gDKE{x";
	    else
		race = "{gDCH{x";
        }
	if (wch->level >= LVL_MASTER && wch->level <= HERO){
	    if (wch->sex == SEX_MALE)
		race = "{gMAS{x";
	    else
		race = "{gMIS{x";
        } 
	if (wch->level >= LVL_LORD && wch->level <= HERO){
	    if (wch->sex == SEX_MALE)
		race = "{gLOR{x";
	    else
		race = "{gLDY{x";
        }
     
    if (wch->level >= LVL_REGENT && wch->level <= HERO)
	    race = "{gREG{x";
	
	if (wch->level >= LVL_LEGEND && wch->level <= HERO)
	    race = "{gLEG{x";

	//if (wch->pcdata->judge == 1 && wch->level <= 100)
	//	race = "{cCON{x";
      //
	//if (wch->pcdata->judge == 2 && wch->level <= 100)
	//	race = "{cHCN{x";

	//headcon
	if (IS_IMMORTAL(ch)) {
	    sprintf(buf, "{x[%-9s][{y%-3s {y%-3s{x] %s%s{r%s{x%s%s%s%s%s%s{x\n\r",
		        kingdom_table[wch->side].cname,
		    class,
                    (wch->pcdata->judge == 1 ? "{mCON{x" :
		    wch->pcdata->judge == 2 ? "{mHCN{x" :
		    wch->pcdata->judge == 3 ? "{rMRC{x" : 	
                    wch->race <= MAX_PC_RACE ? race
		    : "     "),
		    wch->incog_level >= LEVEL_HERO ? "{b(Incog){x " : "",
		    wch->invis_level >= LEVEL_HERO ? "{B({bWizi{B){x " : "",
		    clan_table[wch->clan].who_name,
		    IS_SET(wch->comm, COMM_AFK) ? "{g[AFK]{x " : "",
		    IS_SET(wch->act, PLR_KILLER) ? "{R(TRAITOR){x " : "",
		    IS_SET(wch->act, PLR_THIEF) ? "{R(CRIMINAL){x " : "",
		    wch->name,
	        IS_AFFECTED(wch, AFF_NOQUIT) ? " {R>NQ<{x " : "",
		    IS_NPC(wch) ? "" : wch->pcdata->title);
		    

	} else

	    sprintf(buf, "{x[{y%-3s {y%-3s{x] %s%s{r%s{x%s%s%s%s%s{x\n\r",
	    /* wch->level, */
		    class,
                    (wch->pcdata->judge == 1 ? "{mCON{x" :
		    wch->pcdata->judge == 2 ? "{mHCN{x" :
		    wch->pcdata->judge == 3 ? "{rMRC{x" :
                    wch->race <= MAX_PC_RACE ? race
                    : "     "),	
                    wch->incog_level >= LEVEL_HERO ? "{b(Incog){x " : "",
		    wch->invis_level >= LEVEL_HERO ? "{B({bWizi{B){x{x " : "",
		    clan_table[wch->clan].who_name,
		    IS_SET(wch->comm, COMM_AFK) ? "{g[AFK]{x " : "",
		    IS_SET(wch->act, PLR_KILLER) ? "{R(TRAITOR){x " : "",
		    IS_SET(wch->act, PLR_THIEF) ? "{R(CRIMINAL){x " : "",
		    wch->name,
		    //IS_AFFECTED(wch, AFF_NOQUIT)"{R>NQ<{x " : "",
		    IS_NPC(wch) ? "" : wch->pcdata->title);
	add_buf(output, buf);
    }
    } /* End of wlevel for-loop for sorted who */

    sprintf(buf2, "\n\rPlayers found in your kingdom: %d\n\r", nMatch);
    add_buf(output, buf2);
    if (ch->level >= 20) {
	sprintf(buf2, "Total enemy players playing: %d\n\r", nEnemies);
	add_buf(output, buf2);
    }
    sprintf(buf2, "TOTAL players playing right now: %d\n\r", nPlayers);
    max_on = UMAX(nPlayers, max_on);
    sprintf(buf3, "Max players on [today/ever]: %d/%d\n\r", max_on, max);

//	sprintf(buf4, "Baron+ Distrubution: [{cT:{x%2d {mK:{x%2d {rU:{x%2d]\n\r", tierCount, kaelCount, umCount);

    add_buf(output, buf2);
    add_buf(output, buf3);
  //  add_buf(output, buf4);
    page_to_char(buf_string(output), ch);
    free_buf(output);

    return;
}
typedef struct 
{
  BUFFER * buffer;
  int rank;
} TOP_CB_DATA;

int toplist_callback(void * data, int argc, char **argv, char ** columns)
{
  BUFFER * buffer = ((TOP_CB_DATA*)data)->buffer;
  int * rank = &((TOP_CB_DATA*)(data))->rank;
  char buf[MAX_STRING_LENGTH];

	//zero warscore guys -Akuji
  if (atoi(argv[3]) == 0)
  {
	  return 0;
  }

	//no imms
  if (atoi(argv[4]) >= 100)
  {
	  return 0;
  }

  //sprintf(buf, "%d. %s the %s from %s has %s warscore.\n\r",

//changed the formatting a bit :) -Akuji
sprintf(buf, " #{W%-2d{x   %-18s %-13s %-14s{W     %-4s{x\n\r",
          ++*rank,
          argv[0],
		    capitalize(pc_race_table[atoi(argv[1])].name),
		    kingdom_table[atoi(argv[2])].cname,
		    argv[3]

		);
  add_buf(buffer,buf);
  return 0;
}

void do_toplist(CHAR_DATA* ch, char * argument)
{
  const char * const query = "SELECT name, race, kingdom, warpoints, level, class FROM topplayer ORDER BY level DESC, warpoints DESC LIMIT 50";
  char cquery[MSL];
  sqlite3 * db;
  int rc = sqlite3_open(TOPPLAYER_DB,&db);
  char * error = NULL;
  BUFFER *output;
  char buf[MSL];

	//class argument addin
  int class = -1;
  char arg1[MIL];
  argument = one_argument(argument, arg1);
	if (arg1[0] != '\0')
	{	
		class = class_lookup(arg1);

	if (class == -1) {

	    strcpy(buf, "Possible classes are:\n\r");
			for (class = 0; class < MAX_CLASS; class++) 
			{

			if (class > 0)
				strcat(buf, " ");

			strcat(buf, class_table[class].name);
			}

	    strcat(buf, ".\n\r");

	    stc(buf, ch);
	    return;
	}
		sprintf(cquery, "SELECT name, race, kingdom, warpoints, level, class FROM topplayer WHERE class==%d ORDER BY level DESC, warpoints DESC LIMIT 25", class);
	}
	//end add-in

  if(rc)
  {
    bug("Couldn't open TOPPLAYERS DB. Reason:",0);
    bug(sqlite3_errmsg(db),0);
    sqlite3_close(db);
    send_to_char("Something went wrong. We'll try to fix it.\n\r",ch);
    return;
  }
  output = new_buf();
  TOP_CB_DATA data = {output,0};

  //class argument addin
	if (class != -1 && cquery != NULL)
	{
	  sprintf(buf, "{m############################{x 25 Top %ss {m#############################{x\n\r", class_table[class].name);
	  add_buf(output, buf);
	  sprintf(buf, "{x Rank  Name               Race          Side         Warscore\n\r");
	  add_buf(output, buf);
		rc = sqlite3_exec(db, cquery, toplist_callback, &data, &error);
	}
	else
	{
	  sprintf(buf, "{m############################{x AoW's BEST {m#############################{x\n\r");
	  add_buf(output, buf);
      sprintf(buf, "{x                    Top 50 killers in the realm.{x \n\r\n\r");
	  add_buf(output, buf);
	  sprintf(buf, "{x Rank  Name               Race          Side         Warscore\n\r");
	  add_buf(output, buf);
		rc = sqlite3_exec(db,query,toplist_callback,&data,&error);
	}
  //end addin

  if(rc != SQLITE_OK)
  { 
    bug("Couldn't execute command. Reason:",0);
    bug(sqlite3_errmsg(db),0);
    send_to_char("Something went wrong. We'll try to fix it.\n\r",ch);
  }
  else
  {
   page_to_char(buf_string(output), ch);
  }
  free_buf(output);
  sqlite3_close(db);
}


void do_count(CHAR_DATA * ch, char *argument)
{
    int count;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    count = 0;

    for (d = descriptor_list; d != NULL; d = d->next)
	if (d->connected == CON_PLAYING && can_see(ch, d->character))
	    count++;

    max_on = UMAX(count, max_on);

    sprintf(buf, "Most players on since last reboot was %d.\n\r",
	    max_on);

    send_to_char(buf, ch);
}

void do_listbox(CHAR_DATA * ch)
{
    char buf[MAX_STRING_LENGTH];

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK)) {
	send_to_char("You can't get a balance here.\n\r", ch);
	return;
    }
    send_to_char("In Storage:\n\r", ch);
    sprintf(buf, "Money: %ld gold, %ld silver\n\r", ch->banked_gold, ch->banked_silver);
    send_to_char(buf, ch);
    send_to_char("Equipment:\n\r", ch);
    show_list_to_char(ch->storage, ch, TRUE, TRUE);

    return;
}

void do_inventory(CHAR_DATA * ch, char *argument)
{
    send_to_char("You are carrying:\n\r", ch);
    show_list_to_char(ch->carrying, ch, TRUE, TRUE);
    return;
}

void do_equipment(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char("You are using:\n\r", ch);
    found = FALSE;
    for (iWear = 0; iWear < MAX_WEAR; iWear++) {
	if ((obj = get_eq_char(ch, iWear)) == NULL)
	    continue;

	send_to_char(where_name[iWear], ch);
	if (can_see_obj(ch, obj)) {
	    send_to_char(format_obj_to_char(obj, ch, TRUE), ch);
	    send_to_char("\n\r", ch);
	} else {
	    send_to_char("something.\n\r", ch);
	}
	found = TRUE;
    }

    if (!found)
	send_to_char("Nothing.\n\r", ch);

    return;
}

void do_compare(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if (arg1[0] == '\0') {
	send_to_char("Compare what to what?\n\r", ch);
	return;
    }
    if ((obj1 = get_obj_carry(ch, arg1, ch)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    if (arg2[0] == '\0') {
	for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content) {
	    if (obj2->wear_loc != WEAR_NONE
		&& can_see_obj(ch, obj2)
		&& obj1->item_type == obj2->item_type
		&& (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
		break;
	}

	if (obj2 == NULL) {
	    send_to_char("You aren't wearing anything comparable.\n\r", ch);
	    return;
	}
    } else if ((obj2 = get_obj_carry(ch, arg2, ch)) == NULL) {
	send_to_char("You do not have that item.\n\r", ch);
	return;
    }
    msg = NULL;
    value1 = 0;
    value2 = 0;

    if (obj1 == obj2) {
	msg = "You compare $p to itself.  It looks about the same.";
    } else if (obj1->item_type != obj2->item_type) {
	msg = "You can't compare $p and $P.";
    } else {
	switch (obj1->item_type) {
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
	    break;

	case ITEM_WEAPON:
	    if (obj1->pIndexData->new_format)
		value1 = (1 + obj1->value[2]) * obj1->value[1];
	    else
		value1 = obj1->value[1] + obj1->value[2];

	    if (obj2->pIndexData->new_format)
		value2 = (1 + obj2->value[2]) * obj2->value[1];
	    else
		value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if (msg == NULL) {
	if (value1 == value2)
	    msg = "$p and $P look about the same.";
	else if (value1 > value2)
	    msg = "$p looks better than $P.";
	else
	    msg = "$p looks worse than $P.";
    }
    act(msg, ch, obj1, obj2, TO_CHAR);
    return;
}

void do_credits(CHAR_DATA * ch, char *argument)
{
    do_help(ch, "diku");
    return;
}

void do_where(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument(argument, arg);

    if (arg[0] == '\0') 
    {
		send_to_char("Players near you:\n\r", ch);
		found = FALSE;
	
		for (d = descriptor_list; d; d = d->next) 
			{
	    	if (d->connected == CON_PLAYING
			&& (victim = d->character) != NULL
			&& !IS_NPC(victim)
			&& victim->in_room != NULL
			&& !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE)
			&& (is_room_owner(ch, victim->in_room)
		    	|| !room_is_private(victim->in_room))
			&& victim->in_room->area == ch->in_room->area
			&& can_see(ch, victim)
			&& ( (victim->side == ch->side) || (is_affected(victim, skill_lookup("tracer cloud"))) ) )
				{
				found = TRUE;
				if (!is_affected(victim, skill_lookup("tracer cloud"))){
				sprintf(buf, "%-28s %s\n\r",
	    		/*victim->class,*/ victim->name, victim->in_room->name);}
	    		else if (is_affected(victim, skill_lookup("tracer cloud"))){
		    	sprintf(buf, "{R%-28s {B%s{X\n\r",
	    		/*victim->class,*/ victim->name, victim->in_room->name);
	    		}
				send_to_char(buf, ch);
				}
			}
	if (!found)
	    send_to_char("None\n\r", ch);
    } else {
	found = FALSE;
	for (victim = char_list; victim != NULL; victim = victim->next) {
	    if (victim->in_room != NULL
		&& victim->in_room->area == ch->in_room->area
		&& !IS_AFFECTED(victim, AFF_HIDE)
		&& !IS_AFFECTED(victim, AFF_SNEAK)
		&& can_see(ch, victim)
		&& is_name(arg, victim->name)
		&& ( (victim->side == ch->side) || (is_affected(victim,skill_lookup("tracer cloud") )) ) ) {
		found = TRUE;
		PERS(victim, ch, pers_buf);
		sprintf(buf, "%-28s %s\n\r",
			pers_buf, victim->in_room->name);
		send_to_char(buf, ch);
		break;
	    }
	}
	if (!found)
	    act("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
    }

    return;
}

void do_consider(CHAR_DATA * ch, char *argument)
{
    extern int xp_compute(CHAR_DATA *, CHAR_DATA *, int, bool);
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    char *msgHP;
    char *msgExp;
    char *msgac;
    int diff;
    int hpDiff;
    int ac;

    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    int xp;
    int members;
    int group_levels;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Consider killing whom?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They're not here.\n\r", ch);
	return;
    }
    if (is_safe(ch, victim)) {
	send_to_char("Don't even think about it.\n\r", ch);
	return;
    }
    members = 0;
    group_levels = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room) {
	if (is_same_group(gch, ch)) {
	    members++;
	    group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
	}
    }

    if (members == 0) {
	bug("Group_gain: members.", members);
	members = 1;
	group_levels = ch->level;
    }
    if (members == 1)
    xp = xp_compute(ch, victim, group_levels, FALSE);
    else xp = xp_compute(ch, victim, group_levels, TRUE);

    hpDiff = victim->hit - ch->hit;
    ac = GET_AC(victim, AC_SLASH) / 10;

    diff = victim->level - ch->level;

    if (hpDiff <= -300)
	msgHP = "Your are tremendously healthier than $N.";
    else if (hpDiff <= -200)
	msgHP = "You are a lot healthier than $N.";
    else if (hpDiff <= -100)
	msgHP = "You are healthier than $N.";
    else if (hpDiff <= -50)
	msgHP = "You are a little healthier than $N.";
    else if (hpDiff <= 50)
	msgHP = "$N looks about as healthy as you.";
    else if (hpDiff <= 100)
	msgHP = "$N is a little healthier than you.";
    else if (hpDiff <= 200)
	msgHP = "$N is a lot healthier than you.";
    else if (hpDiff <= 300)
	msgHP = "$N is a whole lot healthier than you.";
    else
	msgHP = "$N has incredible health compared to you.";

    if (ac <= -10)
	msgac = "$N looks nearly invulnerable!";
    else if (ac <= -8)
	msgac = "$N looks superbly armored.";
    else if (ac <= -6)
	msgac = "$N looks heavily armored.";
    else if (ac <= -4)
	msgac = "$N looks very well-armored.";
    else if (ac <= -2)
	msgac = "$N looks well-armored.";
    else if (ac <= 0)
	msgac = "$N looks nicely armored.";
    else if (ac <= 2)
	msgac = "$N looks armored.";
    else if (ac <= 4)
	msgac = "$N looks somewhat armored.";
    else if (ac <= 6)
	msgac = "$N looks lightly armored.";
    else if (ac <= 8)
	msgac = "$N looks barely armored.";
    else
	msgac = "$N looks almost defenseless.";

    if (diff <= -10)
	msg = "You could kill $N with your little toe.";
    else if (diff <= -5)
	msg = "Easy, $N doesn't stand a chance against you.";
    else if (diff <= -2)
	msg = "Looks like $N would be a fairly easy kill.";
    else if (diff <= 1)
	msg = "You seem evenly matched with $N.";
    else if (diff <= 5)
	msg = "Hmm, $N would be a tough adversary.";
    else if (diff <= 10)
	msg = "Attacking $N could be hazardous to your health.";
    else
	msg = "Your chances against $N: dig your own grave and lie down in it.";

    if (xp >= 150)
	msgExp = "You would learn a tremendous amount fighting $N.";
    else if (xp >= 100)
	msgExp = "You would learn a lot fighting $N.";
    else if (xp >= 70)
	msgExp = "$N looks skilled, you could use the combat experience.";
    else if (xp >= 50)
	msgExp = "$N is worth defeating for experience.";
    else if (xp >= 30)
	msgExp = "$N has good experience value in combat.";
    else if (xp >= 10)
	msgExp = "You would get little experience defeating $N.";
    else if (xp >= 1)
	msgExp = "$N is worth meager experience.";
    else
	msgExp = "$N would not be worth the time or effort.";

    sprintf(buf, "%s\n\r -%s\n\r -%s\n\r -%s", msg, msgHP, msgac, msgExp);

    act(buf, ch, NULL, victim, TO_CHAR);
    return;
}

void set_title(CHAR_DATA * ch, char *title)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) {
	bug("Set_title: NPC.", 0);
	return;
    }
    if (title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?') {
	buf[0] = ' ';
	strcpy(buf + 1, title);
    } else {
	strcpy(buf, title);
    }

    free_string(ch->pcdata->title);
    ch->pcdata->title = str_dup(buf);
    return;
}

void do_title(CHAR_DATA * ch, char *argument)
{
    int size;

    if (IS_NPC(ch))
	return;

    if (argument[0] == '\0') {
	send_to_char("Change your title to what?\n\r", ch);
	return;
    }
    size = sizeof(argument);

    if (strlen(argument) > 44)
	argument[45] = '\0';

    smash_tilde(argument);
    set_title(ch, argument);
    send_to_char("Your title has been changed.\n\r", ch);
}

void do_description(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if (argument[0] != '\0') {
	buf[0] = '\0';
	smash_tilde(argument);

	if (argument[0] == '-') {
	    int len;
	    bool found = FALSE;

	    if (ch->description == NULL || ch->description[0] == '\0') {
		send_to_char("No lines left to remove.\n\r", ch);
		return;
	    }
	    strcpy(buf, ch->description);

	    for (len = strlen(buf); len > 0; len--) {
		if (buf[len] == '\r') {
		    if (!found) {	/* back it up */
			if (len > 0)
			    len--;
			found = TRUE;
		    } else {	/* found the second one */
			buf[len + 1] = '\0';
			free_string(ch->description);
			ch->description = str_dup(buf);
			send_to_char("Your description is:\n\r", ch);
			send_to_char(ch->description ? ch->description :
				     "(None).\n\r", ch);
			return;
		    }
		}
	    }
	    buf[0] = '\0';
	    free_string(ch->description);
	    ch->description = str_dup(buf);
	    send_to_char("Description cleared.\n\r", ch);
	    return;
	}
	if (argument[0] == '+') {
	    if (ch->description != NULL)
		strcat(buf, ch->description);
	    argument++;
	    while (isspace(*argument))
		argument++;
	}
	if (strlen(buf) >= 1024) {
	    send_to_char("Description too long.\n\r", ch);
	    return;
	}
	strcat(buf, argument);
	strcat(buf, "\n\r");
	free_string(ch->description);
	ch->description = str_dup(buf);
    }
    send_to_char("Your description is:\n\r", ch);
    send_to_char(ch->description ? ch->description : "(None).\n\r", ch);
    return;
}

void do_report(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    sprintf(buf,
	    "You say 'I have %d/%d hp %d/%d mana %d/%d mv %d xp.'\n\r",
	    ch->hit, ch->max_hit,
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move,
	    ch->exp);

    send_to_char(buf, ch);

    sprintf(buf, "$n says 'I have %d/%d hp %d/%d mana %d/%d mv %d xp.'",
	    ch->hit, ch->max_hit,
	    ch->mana, ch->max_mana,
	    ch->move, ch->max_move,
	    ch->exp);

    act(buf, ch, NULL, NULL, TO_ROOM);

    return;
}
void do_purchase(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *keeper;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

// Merc fix.
if (ch->pcdata->judge == 3){
	send_to_char("Youre a mercenary, go sell your soul for a few gold.\n\r", ch);
	return;
	}

    smash_tilde(argument);

    argument = one_argument(argument, arg1);

    keeper = NULL;

    for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room) {
	if (IS_NPC(keeper)) {
	    if (keeper->spec_fun != NULL)
		if (!strcmp(spec_name(keeper->spec_fun), "spec_survey"))
		    break;
	}
    }

    if (keeper == NULL) {
	send_to_char("You must go to the kingdoms surveyor to do that.\n\r", ch);
	return;
    }
    if (ch->level < 50) {
	sprintf(buf, "%s tells you 'Don't bother me with petty things. Your not worthy of rulership.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	send_to_char("You must be level 50 or higher to run a kingdom.\n\r", ch);
	return;
    }
    if (arg1[0] == '\0') {
	send_to_char("What do you want to purchase parcel, farm, or a mine?\n\r", ch);
	sprintf(buf, "Cost Parcel: %dgp Farm: %dgp Mine: %dgp\n\r", 3000 + (ch->pcdata->vassals / 10) * 12, FARM_COST, 4300 + (ch->pcdata->vassals / 2));
	send_to_char(buf, ch);
	return;
    }
    if (arg1[0] == 'p') {
	if (ch->banked_gold < 3000 + (ch->pcdata->vassals / 10) * 12) {
	    sprintf(buf, "%s tells you 'You do not have enough money in the bank. I don't deal with brought in gold!'\n\r", keeper->short_descr);
	    send_to_char(buf, ch);
	    return;
	}
	sprintf(buf, "%s tells you 'A new piece of land has been put under your watchfull eye.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	send_to_char("You now own a piece of land to manage on the otherside of the sea.\n\r", ch);
	send_to_char("You never have to visit or even see this property but you do have to feed the people on it.\n\r", ch);
	send_to_char("You gain 2 vassals for every parcel you purchase.\n\r", ch);
	ch->pcdata->parcels += 1;
	ch->pcdata->vassals += 2;
	ch->banked_gold -= 3000 + (ch->pcdata->vassals / 10) * 12;

    }
    if (arg1[0] == 'f') {
	if (ch->banked_gold < FARM_COST) {
	    sprintf(buf, "%s tells you 'You do not have enough money in the bank. I don't deal with brought in gold!'\n\r", keeper->short_descr);
	    send_to_char(buf, ch);
	    return;
	}
	sprintf(buf, "%s tells you 'I have just ordered a new farm into your control.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	send_to_char("The new farm you have bought can produce food to feed your vassals.\n\r", ch);
	send_to_char("If your people starve they will abandon your cause.\n\r", ch);
	send_to_char("You need food to support a castle here in this kingdom.\n\r", ch);
	ch->pcdata->farms += 1;
	ch->banked_gold -= FARM_COST;
    }
    if (arg1[0] == 'm') {
	if (ch->banked_gold < 4300 + (ch->pcdata->vassals / 2)) {
	    sprintf(buf, "%s tells you 'You do not have enough money in the bank. I don't deal with brought in gold!'\n\r", keeper->short_descr);
	    send_to_char(buf, ch);
	    return;
	}
	sprintf(buf, "%s tells you 'A mine has been ordered into your holdings.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	send_to_char("Mines produce resources like wood, steel, stone etc.\n\r", ch);
	send_to_char("Your vassals will need resources in order to grow.\n\r", ch);
	ch->pcdata->mines += 1;
	ch->banked_gold -= 4300 + (ch->pcdata->vassals / 2);
    }
}
void do_sale(CHAR_DATA * ch, char *argument)
{

    CHAR_DATA *keeper;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
	return;

    smash_tilde(argument);

    if (ch->pcdata->judge == 3) {
	send_to_char ("Why not just sell your soul, you already did your sword...\n\r", ch);
	return;
	}

    argument = one_argument(argument, arg1);

    keeper = NULL;

    for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room) {
	if (IS_NPC(keeper)) {
	    if (keeper->spec_fun != NULL)
		if (!strcmp(spec_name(keeper->spec_fun), "spec_survey"))
		    break;
	}
    }

    if (keeper == NULL) {
	send_to_char("You must go to the kingdoms surveyor to do that.\n\r", ch);
	return;
    }
    if (ch->level < 50) {
	sprintf(buf, "%s tells you 'Don't bother me with petty things. Your not worthy of rulership.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	send_to_char("You must be level 50 or higher to run a kingdom.\n\r", ch);
	return;
    }
    if (arg1[0] == '\0') {
	send_to_char("What do you want to part with a parcel, farm, or a mine?\n\r", ch);
	sprintf(buf, "Sale value Parcel: %dgp Farm: %dgp Mine: %dgp \n\r",
		(3000 + (ch->pcdata->vassals / 10) * 12) / 3, FARM_COST / 3, (4300 + (ch->pcdata->vassals / 2)) / 3);
	send_to_char(buf, ch);
	return;
    }
    if (arg1[0] == 'p') {
	if (ch->pcdata->parcels == 0) {
	    send_to_char("You have no parcels to drop.\n\r", ch);
	    return;
	}
	sprintf(buf, "%s tells you 'Done, the parcel is gone along with some vassals.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	ch->pcdata->parcels -= 1;

	if (ch->pcdata->vassals <= 2)
	    ch->pcdata->vassals = 0;
	else
	    ch->pcdata->vassals -= 2;

	ch->banked_gold += (3000 + (ch->pcdata->vassals / 10) * 12) / 3;
	send_to_char("The money has been added to your treasury.\n\r", ch);

    }
    if (arg1[0] == 'm') {
	if (ch->pcdata->mines == 0) {
	    send_to_char("You have no mines to drop.\n\r", ch);
	    return;
	}
	sprintf(buf, "%s tells you 'A mine is now history.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	ch->pcdata->mines -= 1;
	ch->banked_gold += (4300 + (ch->pcdata->vassals / 2)) / 3;
	send_to_char("The money has been added to your treasury.\n\r", ch);

    }
    if (arg1[0] == 'f') {
	if (ch->pcdata->farms == 0) {
	    send_to_char("You have no farms to drop.\n\r", ch);
	    return;
	}
	sprintf(buf, "%s tells you 'A farm was sold to someone else.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	ch->pcdata->farms -= 1;
	ch->banked_gold += FARM_COST / 3;
	send_to_char("The money has been added to your treasury.\n\r", ch);

    }
}
void do_tax(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *keeper;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    int value;

    if (IS_NPC(ch))
	return;

    smash_tilde(argument);

    argument = one_argument(argument, arg1);

    keeper = NULL;

    for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room) {
	if (IS_NPC(keeper)) {
	    if (keeper->spec_fun != NULL)
		if (!strcmp(spec_name(keeper->spec_fun), "spec_survey"))
		    break;
	}
    }

    if (keeper == NULL) {
	send_to_char("You must go to the kingdoms surveyor to do that.\n\r", ch);
	return;
    }
    if (ch->level < 50) {
	sprintf(buf, "%s tells you 'Don't bother me with petty things. Your not worthy of rulership.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	send_to_char("You must be level 50 or higher to run a kingdom.\n\r", ch);
	return;
    }
    if (arg1[0] == '\0') {
	send_to_char("What tax rate do you want to set? 1-99?\n\r", ch);
	return;
    }
    if (!is_number(arg1)) {
	send_to_char("You must supply a number as tax. 1-99?\n\r", ch);
	return;
    }
    value = atoi(arg1);

    if (value < 1 || value > 100) {
	send_to_char("The tax rate as a percent cannot be less then one or greater then 100.\n\r", ch);
	return;
    }
    ch->pcdata->tax = value;
    sprintf(buf, "%s tells you 'I will notify your lands of the new change.'\n\r", keeper->short_descr);
    send_to_char(buf, ch);

}
/*void do_research(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *keeper;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    int value;

    if (IS_NPC(ch))
	return;

    smash_tilde(argument);

    argument = one_argument(argument, arg1);

    keeper = NULL;

    for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room) {
	if (IS_NPC(keeper)) {
	    if (keeper->spec_fun != NULL)
		if (!strcmp(spec_name(keeper->spec_fun), "spec_survey"))
		    break;
	}
    }

    if (keeper == NULL) {
	send_to_char("You must go to the kingdoms surveyor to do that.\n\r", ch);
	return;
    }
    if (ch->level < 50) {
	sprintf(buf, "%s tells you 'Don't bother me with petty things. Your not worthy of rulership.'\n\r", keeper->short_descr);
	send_to_char(buf, ch);
	send_to_char("You must be level 50 or higher to run a kingdom.\n\r", ch);
	return;
    }
    if (arg1[0] == '\0') {
	send_to_char("How much gold do you want to invest in research?\n\r", ch);
	return;
    }
    if (!is_number(arg1)) {
	send_to_char("You must supply a number for how much gold.\n\r", ch);
	return;
    }
    value = atoi(arg1);

    if (value < 10 || value > 1000) {
	send_to_char("Minimum to be invested is 10 and max is 1000gp.\n\r", ch);
	return;
    }
    ch->pcdata->research = value;
    sprintf(buf, "%s tells you 'Your thinkers have started on new projects. They will be pleased.'\n\r", keeper->short_descr);
    send_to_char(buf, ch);

}*/

void do_raise(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int value;

    if (IS_NPC(ch))
	return;

    smash_tilde(argument);

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (ch->levelpoints <= 0) {
	send_to_char("You don't have any points left to distribute.\n\r", ch);
	return;
    }
    if (arg1[0] == '\0' || arg2[0] == '\0') {
	sprintf(buf, "You have %d points left to distribute.\n\r", ch->levelpoints);
	send_to_char(buf, ch);
	send_to_char("Syntax: raise hp or mana <number> \n\r", ch);
	return;
    }
    if (!is_number(arg2)) {
	send_to_char("You must supply a number for the hp or mana.\n\r", ch);
	return;
    }
    value = atoi(arg2);

    if (value < 0) {
	send_to_char("You trying to cheat or what?\n\r", ch);
	return;
    }

    if (value > ch->levelpoints) {
	send_to_char("You don't have that many raise points.\n\r", ch);
	return;
    }
    if ((arg1[0] == 'h' || arg1[0] == 'H') && arg2[0] != '\0') {
	ch->pcdata->perm_hit += value;
	ch->max_hit += value;
	ch->spentOnHps += value;
	ch->levelpoints -= value;
	sprintf(buf, "You have %d points left to distribute.\n\r", ch->levelpoints);
    } else if ((arg1[0] == 'm' || arg1[0] == 'M')
	       && (arg1[1] == 'a' || arg1[1] == 'A' || arg1[1] == '\0')
	       && arg2[0] != '\0') {
	ch->pcdata->perm_mana += value;
	ch->max_mana += value;
	ch->spentOnMana += value;
	ch->levelpoints -= value;
	sprintf(buf, "You have %d points left to distribute.\n\r", ch->levelpoints);
    } else {
	sprintf(buf, "Syntax: raise hp or mana <number> \n\r");
    }

    send_to_char(buf, ch);
    return;
}

void do_practice(CHAR_DATA * ch, char *argument)
{
	BUFFER *buffer = new_buf();
    char buf[MAX_STRING_LENGTH];
    char number_color[3];
    int sn;
    CHAR_DATA *mob;
    bool pracLow = FALSE;
    bool pracAll = FALSE;
    int adept;
    int high_gm_level = 0; // you need a high gm to practice spells of this level

    if (IS_NPC(ch))
	return;

    if (argument[0] == '\0') {
		int col;

		col = 0;
		// do spells first
		add_buf(buffer," Spells:{x\n\r");
		for (sn = 0; sn < MAX_SKILL; sn++) 
		{
			if (skill_table[sn].name == NULL)
				break;

		    if(IS_SPELL(sn))
		        continue;

			if (ch->level < skill_table[sn].skill_level[ch->class]
			|| ch->pcdata->learned[sn] < 1 /* skill is not known */ )
				continue;
            
            if(ch->pcdata->learned[sn] < 15)
                strcpy(number_color, "{R");
		else if(ch->pcdata->learned[sn] < 25)
		    strcpy(number_color, "{r");
            else if(ch->pcdata->learned[sn] < 50)
                strcpy(number_color, "{y");
            else if(ch->pcdata->learned[sn] < 75)
                strcpy(number_color, "{Y");
            else if(ch->pcdata->learned[sn] <= 99)
                strcpy(number_color, "{g");
            else
                strcpy(number_color, "{G");

			sprintf(buf, "{r%-19s %s%3d{W%%  {x",
				skill_table[sn].name, number_color, ch->pcdata->learned[sn]);
			add_buf(buffer, buf);
			if (++col % 3 == 0)
				add_buf(buffer, "\n\r");
		}
		
		col = 0;
		// then skills
		add_buf(buffer, "\n\r\n\r Skills:{x\n\r");
		for (sn = 0; sn < MAX_SKILL; sn++) 
		{
			if (skill_table[sn].name == NULL)
				break;

		    if(IS_SKILL(sn))
		        continue;

			if (ch->level < skill_table[sn].skill_level[ch->class]
			|| ch->pcdata->learned[sn] < 1 /* skill is not known */ )
				continue;

            if(ch->pcdata->learned[sn] < 15)
                strcpy(number_color, "{R");
		else if(ch->pcdata->learned[sn] < 25)
		    strcpy(number_color, "{r");
            else if(ch->pcdata->learned[sn] < 50)
                strcpy(number_color, "{y");
            else if(ch->pcdata->learned[sn] < 75)
                strcpy(number_color, "{Y");
            else if(ch->pcdata->learned[sn] <= 99)
                strcpy(number_color, "{g");
            else
                strcpy(number_color, "{G");


			sprintf(buf, "{g%-19s %s%3d{W%%  {x",
				skill_table[sn].name, number_color, ch->pcdata->learned[sn]);
			add_buf(buffer, buf);
			if (++col % 3 == 0)
				add_buf(buffer, "\n\r");
        }
        
		if (col % 3 != 0)
			add_buf(buffer, "\n\r");

		sprintf(buf, "You have {W%d{x practice sessions left.\n\r",
			ch->practice);
		add_buf(buffer, buf);

        page_to_char(buf_string(buffer), ch);
        free_buf(buffer);
		return;

}			//end null arguement

	if (!IS_AWAKE(ch)) 
	{
	    send_to_char("In your dreams, or what?\n\r", ch);
	    return;
	}

	for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room) 
	{
	    if (IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN)) 
	    {
if (IS_SET(mob->act, ACT_WARRIOR)) 
    if (     !strcmp(class_table[ch->class].name, "Warrior") ||
	!strcmp(class_table[ch->class].name, "Barbarian") ||
	!strcmp(class_table[ch->class].name, "Deathknight") ||
	!strcmp(class_table[ch->class].name, "Warlock") ||
	!strcmp(class_table[ch->class].name, "Ranger") ||
	!strcmp(class_table[ch->class].name, "Lancer") ||
	!strcmp(class_table[ch->class].name, "Dragoon")||
	!strcmp(class_table[ch->class].name, "Knight"))
	    pracAll = TRUE;
if (IS_SET(mob->act, ACT_MAGE)) 
    if (     !strcmp(class_table[ch->class].name, "Mage") ||
	!strcmp(class_table[ch->class].name, "Illusionist") ||
	!strcmp(class_table[ch->class].name, "Temporalist") ||
	!strcmp(class_table[ch->class].name, "Geomancer")   ||
	!strcmp(class_table[ch->class].name, "Witch"))
	    pracAll = TRUE;
if (IS_SET(mob->act, ACT_CLERIC)) 
    if (     !strcmp(class_table[ch->class].name, "Cleric") ||
	!strcmp(class_table[ch->class].name, "Druid") ||
	!strcmp(class_table[ch->class].name, "Zealot") ||
	!strcmp(class_table[ch->class].name, "Necromancer") ||
	!strcmp(class_table[ch->class].name, "Defiler"))
	    pracAll = TRUE;
if (IS_SET(mob->act, ACT_THIEF)) 
    if (     !strcmp(class_table[ch->class].name, "Assassin") ||
	!strcmp(class_table[ch->class].name, "Monk") ||
	!strcmp(class_table[ch->class].name, "Bard"))
	    pracAll = TRUE;

if (!strcmp(class_table[ch->class].name, "Mage")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildMageLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildMageHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Druid")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildDruidLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildDruidHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Shedaklah")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildShedaklahLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildShedaklahHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Cleric")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildClericLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildClericHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Warlock")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildWarlockLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildWarlockHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Defiler")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildDefilerLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildDefilerHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Necromancer")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildNecromancerLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildNecromancerHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Deathknight")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildDeathKnightLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildDeathKnightHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Knight")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildKnightLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildKnightHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Warrior")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildWarriorLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildWarriorHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Witch")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildWitchLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildWitchHigh")) 	pracAll = TRUE;}
if (!strcmp(class_table[ch->class].name, "Barbarian")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildBarbarianLow")) 	pracLow = TRUE;   
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildBarbarianHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Ranger")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildRangerLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildRangerHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Assassin")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildAssassinLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildAssassinHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Monk")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildMonkLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildMonkHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Temporalist")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildMonkLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildMonkHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Lancer")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildMonkLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildMonkHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Bard")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildBardLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildBardHigh")) 	pracAll = TRUE;}

if (!strcmp(class_table[ch->class].name, "Illusionist")) if (mob->spec_fun != NULL)
	{if (!strcmp(spec_name(mob->spec_fun), "spec_guildIllusionistLow")) 	pracLow = TRUE;
	  if (!strcmp(spec_name(mob->spec_fun), "spec_guildIllusionistHigh")) 	pracAll = TRUE;}

	    }//end is npc & train
	} //end mob search

if ((sn = find_spell(ch, argument)) < 0
	    || (ch->pcdata->learned[sn] < 1)	/* skill is not known */
	|| (ch->level < skill_table[sn].skill_level[ch->class]) )
{
	    send_to_char("You can't practice that.\n\r", ch);
	    return;
}

if(pracAll)
{ //do nothing
}
else if(pracLow &&  skill_table[sn].skill_level[ch->class] > high_gm_level   )
{
    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
    return;
}
/*else if ( !pracLow)//no trainer found
{
    send_to_char("You must find your Guildmaster.\n\r", ch);
    return;
}*/

if (ch->practice < 1)
{
    send_to_char("You are out of practice sessions.\n\r", ch);
    return;
}



	adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;

	if (ch->pcdata->learned[sn] >= adept) {
	    sprintf(buf, "You are already learned at %s.\n\r",
		    skill_table[sn].name);
	    send_to_char(buf, ch);
	} else {
	    ch->practice--;
	    if (skill_table[sn].spell_fun == spell_null)
		ch->pcdata->learned[sn] +=
		    int_app[get_curr_stat(ch, STAT_INT)].learn / 2;
	    else
		ch->pcdata->learned[sn] +=
		    int_app[get_curr_stat(ch, STAT_INT)].learn;

	    if (ch->pcdata->learned[sn] < adept) {
		act("You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR);
		act("$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM);
	    } else {
		ch->pcdata->learned[sn] = adept;
		act("You are now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR);
		act("$n is now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM);
	    }
	}








}
/*
void do_practice2(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    CHAR_DATA *mob;
    int adept;
    bool isOldMaster;

    if (IS_NPC(ch))
	return;

    if (argument[0] == '\0') {
	int col;

	col = 0;
	for (sn = 0; sn < MAX_SKILL; sn++) {
	    if (skill_table[sn].name == NULL)
		break;
	    if (ch->level < skill_table[sn].skill_level[ch->class]
		|| ch->pcdata->learned[sn] < 1  )
		continue;

	    sprintf(buf, "%-18s %3d%%  ",
		    skill_table[sn].name, ch->pcdata->learned[sn]);
	    send_to_char(buf, ch);
	    if (++col % 3 == 0)
		send_to_char("\n\r", ch);
	}

	if (col % 3 != 0)
	    send_to_char("\n\r", ch);

	sprintf(buf, "You have %d practice sessions left.\n\r",
		ch->practice);
	send_to_char(buf, ch);
    }				//end null arguement

    else {

	isOldMaster = FALSE;

	if (!IS_AWAKE(ch)) {
	    send_to_char("In your dreams, or what?\n\r", ch);
	    return;
	}
	for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room) {

	    if (IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN)) {

		if (IS_SET(mob->act, ACT_WARRIOR)) {

		    if (
			   !strcmp(class_table[ch->class].name, "warrior") ||
			   !strcmp(class_table[ch->class].name, "barbarian") ||
			   !strcmp(class_table[ch->class].name, "warlock") ||
			   !strcmp(class_table[ch->class].name, "deathknight") ||
			   !strcmp(class_table[ch->class].name, "knight")
			) {
			isOldMaster = TRUE;
			break;
		    }
		}
		if (IS_SET(mob->act, ACT_THIEF)) {
		    if (!strcmp(class_table[ch->class].name, "assassin") ||
			!strcmp(class_table[ch->class].name, "bard") ||
			!strcmp(class_table[ch->class].name, "monk") ||
			!strcmp(class_table[ch->class].name, "ranger")
			) {
			isOldMaster = TRUE;
			break;
		    }
		}
		if (IS_SET(mob->act, ACT_MAGE)) {
		    if (!strcmp(class_table[ch->class].name, "mage") ||
			!strcmp(class_table[ch->class].name, "illusionist") ||
			!strcmp(class_table[ch->class].name, "witch")
			) {
			isOldMaster = TRUE;
			break;
		    }
		}
		if (IS_SET(mob->act, ACT_CLERIC)) {
		    if (!strcmp(class_table[ch->class].name, "cleric") ||
			!strcmp(class_table[ch->class].name, "druid") ||
			!strcmp(class_table[ch->class].name, "defiler") ||
			!strcmp(class_table[ch->class].name, "necromancer") ||
			!strcmp(class_table[ch->class].name, "shedaklah")
			) {
			isOldMaster = TRUE;
			break;
		    }
		}
	    }
	}
	if (mob == NULL) {
	    send_to_char("You can't do that here. Find your guildmaster.\n\r", ch);
	    return;
	}
	if (ch->practice <= 0) {
	    send_to_char("You have no practice sessions left.\n\r", ch);
	    return;
	}
	if ((sn = find_spell(ch, argument)) < 0
		    || (ch->pcdata->learned[sn] < 1)	// skill is not known 
		|| (ch->level < skill_table[sn].skill_level[ch->class]) )
{
	    send_to_char("You can't practice that.\n\r", ch);
	    return;
}
//	if ((sn = find_spell(ch, argument)) < 0
//	    || (!IS_NPC(ch)
//		&& (ch->level < skill_table[sn].skill_level[ch->class]
//		    || ch->pcdata->learned[sn] < 1	
//		    || skill_table[sn].rating[ch->class] == 0))) {
//	    send_to_char("You can't practice that.\n\r", ch);
//	    return;
//	}
	if (isOldMaster == FALSE) {

	    // Check for the type of guildmaster here
	    if (!strcmp(class_table[ch->class].name, "warrior")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildWarriorLow")) {
			if (skill_table[sn].skill_level[ch->class] > 12) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "ranger")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildRangerLow")) {
			if (skill_table[sn].skill_level[ch->class] > 12) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "bard")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildBardLow")) {
			if (skill_table[sn].skill_level[ch->class] > 12) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "barbarian")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildBarbarianLow")) {
			if (skill_table[sn].skill_level[ch->class] > 12) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "knight")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildKnightLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "warlock")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildWarlockLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "deathknight")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildDarkKnightLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "mage")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildMageLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "illusionist")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildIllusionistLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "warlock")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildWarlockLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "druid")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildDruidLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "cleric")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildClericLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "shedaklah")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildShedaklahLow")) {
			if (skill_table[sn].skill_level[ch->class] > 25) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "defiler")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildDefilerLow")) {
			if (skill_table[sn].skill_level[ch->class] > 25) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "monk")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildMonkLow")) {
			if (skill_table[sn].skill_level[ch->class] > 25) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "witch")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildWitchLow")) {
			if (skill_table[sn].skill_level[ch->class] > 25) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "assassin")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildAssassinLow")) {
			if (skill_table[sn].skill_level[ch->class] > 15) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	    if (!strcmp(class_table[ch->class].name, "necromancer")) {
		if (mob->spec_fun != NULL)
		    if (!strcmp(spec_name(mob->spec_fun), "spec_guildNecromancerLow")) {
			if (skill_table[sn].skill_level[ch->class] > 20) {
			    send_to_char("You can't learn that here its to high level. You must find another master to teach you.\n\r", ch);
			    return;
			}
		    }
	    }
	}			// END of isOldMaster check

	adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;

	if (ch->pcdata->learned[sn] >= adept) {
	    sprintf(buf, "You are already learned at %s.\n\r",
		    skill_table[sn].name);
	    send_to_char(buf, ch);
	} else {
	    ch->practice--;
	    if (skill_table[sn].spell_fun == spell_null)
		ch->pcdata->learned[sn] +=
		    int_app[get_curr_stat(ch, STAT_INT)].learn / 2;
	    else
		ch->pcdata->learned[sn] +=
		    int_app[get_curr_stat(ch, STAT_INT)].learn;

	    if (ch->pcdata->learned[sn] < adept) {
		act("You practice $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR);
		act("$n practices $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM);
	    } else {
		ch->pcdata->learned[sn] = adept;
		act("You are now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_CHAR);
		act("$n is now learned at $T.",
		    ch, NULL, skill_table[sn].name, TO_ROOM);
	    }
	}
    }
    return;
} */

/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument(argument, arg);

    if (arg[0] == '\0')
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi(arg);

    if (wimpy < 0) {
	send_to_char("Your courage exceeds your wisdom.\n\r", ch);
	return;
    }
    if (wimpy > ch->max_hit / 2) {
	send_to_char("Such cowardice ill becomes you.\n\r", ch);
	return;
    }
    ch->wimpy = wimpy;
    sprintf(buf, "Wimpy set to %d hit points.\n\r", wimpy);
    send_to_char(buf, ch);
    return;
}

void do_warscore(CHAR_DATA * ch)
{
    extern WARSCORE_DATA *main_warscore;
    char buf[MAX_STRING_LENGTH];
    int x1, x2, x3, x4;

    send_to_char("{m######################################{xWARSCORE{m##############################{x\n\r", ch);

    sprintf(buf, "                        Tier       Kael       Undermountain   Cairn\n\r");
    send_to_char(buf, ch);
    sprintf(buf, "  Enemies Vanquished:   %-8d   %-8d     %-8d       %-8d\n\r",
	    main_warscore->tier_killed,
	    main_warscore->kael_killed,
	    main_warscore->um_killed,
	    main_warscore->cairn_killed
	);
    send_to_char(buf, ch);

    sprintf(buf, "          Casualties:   %-8d   %-8d     %-8d       %-8d\n\r",
	    main_warscore->tier_lost,
	    main_warscore->kael_lost,
	    main_warscore->um_lost,
	    main_warscore->cairn_lost
	);
    send_to_char(buf, ch);
    sprintf(buf, "Baron and up killed:    %-8d   %-8d     %-8d       %-8d\n\r",
	    main_warscore->tier_legends_killed,
	    main_warscore->kael_legends_killed,
	    main_warscore->um_legends_killed,
	    main_warscore->cairn_legends_killed
	);
    send_to_char(buf, ch);
    sprintf(buf, "Baron and up lost:      %-8d   %-8d     %-8d       %-8d\n\r",
	    main_warscore->tier_legends_lost,
	    main_warscore->kael_legends_lost,
	    main_warscore->um_legends_lost,
	    main_warscore->cairn_legends_lost
	);
    send_to_char(buf, ch);
    sprintf(buf, "    Successful Raids:   %-8d   %-8d     %-8d       %-8d\n\r",
	    main_warscore->tier_raids,
	    main_warscore->kael_raids,
	    main_warscore->um_raids,
	    main_warscore->cairn_raids);
    send_to_char(buf, ch);

    sprintf(buf, "   Raided by enemies:   %-8d   %-8d     %-8d       %-8d\n\r",
	    main_warscore->tier_been_raided,
	    main_warscore->kael_been_raided,
	    main_warscore->um_been_raided,
	    main_warscore->cairn_been_raided
	);
    send_to_char(buf, ch);

    x1 = (main_warscore->tier_killed - main_warscore->tier_lost) +
	(main_warscore->tier_legends_killed - main_warscore->tier_legends_lost) +
	(main_warscore->tier_raids - main_warscore->tier_been_raided) * 1;

    x2 = (main_warscore->kael_killed - main_warscore->kael_lost) +
	(main_warscore->kael_legends_killed - main_warscore->kael_legends_lost) +
	(main_warscore->kael_raids - main_warscore->kael_been_raided) * 1;

    x3 = (main_warscore->um_killed - main_warscore->um_lost) +
	(main_warscore->um_legends_killed - main_warscore->um_legends_lost) +
	(main_warscore->um_raids - main_warscore->um_been_raided) * 1;

    x4 = (main_warscore->cairn_killed - main_warscore->cairn_lost) +
	(main_warscore->cairn_legends_killed - main_warscore->cairn_legends_lost) +
	(main_warscore->cairn_raids - main_warscore->cairn_been_raided) * 1;

    sprintf(buf, "            Warscore:   %-8d   %-8d     %-8d       %-8d\n\r",
	    x1, x2, x3, x4
	);
    send_to_char(buf, ch);

    return;
}

void do_topplayers(CHAR_DATA * ch, char *argument)
{
/*
    extern TOPPLAYER_DATA *main_warlist;
    TOPPLAYER_DATA *temp;
    int i = 0;
    int class = 0;
    BUFFER *output;
    char buf[MAX_STRING_LENGTH];

    if (argument[0] != '\0')
	class = class_lookup(argument);

    if ((argument[0] != '\0') && (class == -1)) {
	send_to_char("That isn't a valid class.\n\r", ch);
	return;
    }
    output = new_buf();

    sprintf(buf, "{m############################{x AoW's BEST {m#############################{x\n\r");
    add_buf(output, buf);

    if (argument[0] == '\0') {
	sprintf(buf, "{W                    {WTop 100 killers in the realm.{x \n\r\n\r");
	add_buf(output, buf);
    } else {
	sprintf(buf, "{W                    Top 15 %ss in the realm.{x\n\r\n\r", capitalize(class_table[class].name));
	add_buf(output, buf);
    }

    temp = main_warlist;

    for (; temp != NULL; temp = temp->next) {
	if (!str_cmp(temp->name, "Nobody"))
	    break;
	if ((argument[0] != '\0') && (i > 14))
	    break;
	if ((argument[0] != '\0') && (class != temp->class))
	    continue;
	else {

//TEMPORARY
		sprintf(buf, "%d. %s the %s from %s has %d warscore.\n\r",
		    ++i,
		    temp->name,
		    pc_race_table[temp->race].name,
		    kingdom_table[temp->kingdom].name,
		    temp->warpoints

		);
	    add_buf(output, buf);
	}
    }
    sprintf(buf, "\n\rNOTE: This list is updated after every player kill occurs.\n\r");
    add_buf(output, buf);

    page_to_char(buf_string(output), ch);
    free_buf(output);
    return;
*/
}

void do_trophy(CHAR_DATA * ch)
{
    BUFFER *output;
    int pos, ret, total;
    char buf[MAX_STRING_LENGTH];
    output = new_buf();

    if (IS_NPC(ch))
	return;
    ret = 0;
    total = 0;
    send_to_char("{mYour trophy consists of..{x\n\r", ch);
    for (pos = 0; pos < MAX_TROPHY; pos++) {
	if (ch->pcdata->trophy[pos] == NULL ||
	    ch->pcdata->trophy_sub[pos] == 0) {
	    break;
	}
	sprintf(buf, "%-35.35s %-3d",
		ch->pcdata->trophy[pos],
		ch->pcdata->trophy_sub[pos]
	    );
	add_buf(output, buf);

	total += ch->pcdata->trophy_sub[pos];
	ret++;
	if (ret == 2) {
	    ret = 0;
	    add_buf(output, "\n\r");
	}
    }
    if (ret == 1)
	add_buf(output, "\n\r");

    pos == 0 ? sprintf(buf, "{mYou have killed nothing.{x\n\r") :
	sprintf(buf, "{mYou have killed %d creatures and/or players total.{x\n\r", total);
    add_buf(output, buf);
    page_to_char(buf_string(output), ch);
    free_buf(output);

}

void do_password(CHAR_DATA * ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if (IS_NPC(ch))
	return;
/*Vashna 02-25-01*/
/*Update to prevent characters to change their passwords at level 1 */
/*and saving. */

    if( ch->level < 2)
      {
      send_to_char("You are not high level enough to save.\n\r",ch);
      return;
      }

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while (isspace(*argument))
	argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
	cEnd = *argument++;

    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while (isspace(*argument))
	argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
	cEnd = *argument++;

    while (*argument != '\0') {
	if (*argument == cEnd) {
	    argument++;
	    break;
	}
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if (arg1[0] == '\0' || arg2[0] == '\0') {
	send_to_char("Syntax: password <old> <new>.\n\r", ch);
	return;
    }
    if (strcmp(crypt(arg1, ch->pcdata->pwd), ch->pcdata->pwd)) {
	WAIT_STATE(ch, 40);
	send_to_char("Wrong password.  Wait 10 seconds.\n\r", ch);
	return;
    }
    if (strlen(arg2) < 5) {
	send_to_char(
			"New password must be at least five characters long.\n\r", ch);
	return;
    }
    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt(arg2, ch->name);
    for (p = pwdnew; *p != '\0'; p++) {
	if (*p == '~') {
	    send_to_char(
			    "New password not acceptable, try again.\n\r", ch);
	    return;
	}
    }

    free_string(ch->pcdata->pwd);
    ch->pcdata->pwd = str_dup(pwdnew);
    save_char_obj(ch);
    send_to_char("Your password has been changed.\n\r", ch);
    return;
}

void do_sever(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Who are you going to rip in half?\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (victim == ch) {
	send_to_char("You reach your neck fine, but cant' get your ankles.\n\r", ch);
	return;
    }
    if (IS_NPC(victim)) {
	send_to_char("You can not rip mobs, just players..\n\r", ch);
	return;
    }
    if (IS_SET(victim->act, PLR_SEVERED)) {
	send_to_char("They have no legs to rip off.\n\r", ch);
	return;
    }
    if (!IS_NPC(victim)) {
	SET_BIT(victim->act, PLR_SEVERED);
	act("{R $n picks you up and rips you in half! Oh no!{x", ch, NULL, victim, TO_VICT);
	act("{R $n picks up $N and rips $S legs off!{x", ch, NULL, victim, TO_NOTVICT);
	send_to_char("{R You rip them in half!{x\n\r", ch);
	obj = create_object(get_obj_index(OBJ_VNUM_LEGS), 0);
	obj->timer = 5;
	sprintf(buf, "A pair of %s's legs are here, twitching.", victim->name);
	free_string(obj->description);
	obj->description = str_dup(buf);
	sprintf(buf, "A pair of %s's legs", victim->name);
	free_string(obj->short_descr);
	obj->short_descr = str_dup(buf);
	obj_to_room(obj, ch->in_room);
    }
}

void do_shrink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *shrunken_head;
    
    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
	    send_to_char( "Shrink what severed head?\n\r", ch );
	    return;
    }

    obj = get_obj_list(ch, arg, ch->carrying);

    if ((obj = get_obj_carry(ch, arg, ch)) == NULL ||
         (obj->pIndexData->vnum != OBJ_VNUM_SEVERED_HEAD))
    {
        send_to_char( "You are not carrying a severed head to shrink...\n\r", ch );
        return;
    }

    if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_SEVERED_HEAD)
    {
        act( "You shrink {R$p{x, using the forbidden, dark arts.", ch, obj, 0, TO_CHAR );
        act( "$n shrinks {R$p{x, using the forbidden, dark arts.", ch, obj, NULL, TO_ROOM );
        wiznet( "$N shrinks {R$p{x using the forbidden, dark arts.", ch, obj, WIZ_SACCING, 0, 0 );
        shrunken_head = create_object(get_obj_index( OBJ_VNUM_SHRUNKEN_HEAD),0);
        sprintf( buf, "{R%s's {Dshrunken trophy, %s, lies here.{x", ch->name, obj->short_descr );
        free_string(shrunken_head->description);
        shrunken_head->description = str_dup(buf);
        sprintf( buf, "{R%s's{D shrunken trophy, %s{x", ch->name, obj->short_descr );
        free_string(shrunken_head->short_descr);
        shrunken_head->short_descr = str_dup(buf);
        obj_to_char(shrunken_head, ch);
        extract_obj(obj);
	    return;
    }
    else
    {
        send_to_char( "You can only shrink severed heads.\n\r", ch );
        return;
    }
}

void do_spellskill args((CHAR_DATA * ch, char *argument));

void do_slist(CHAR_DATA *ch,char *argument)
{
	char arg1[MIL];
	int skill,iclass = 0,cl = 0;
	int sn = 0,col = 0,level = 0;
	int skCount = 0,spCount = 0, class_name = 0;

	argument = one_argument(argument,arg1);

    if (arg1[0] == '\0') {
		stc("{xWhat class or skill would you like to lookup?\n\r", ch);
	    //do_spellskill(ch, argument);
        return;
    }
        if (((skill = skill_lookup(arg1)) == -1) && (cl = class_lookup(arg1)) == -1) {
        stc("{xThat is not a valid class or skill.{x\n\r", ch);
        return;
    }   

    if (((skill = skill_lookup(arg1)) == -1) && (iclass = class_lookup(arg1)) != -1) {
      for (class_name = 0; class_name < MAX_CLASS; class_name++) {
          if (!str_cmp(arg1, class_table[class_name].name))
            break;
      }


      if (class_name == MAX_CLASS) {
        stc("{xPlease spell out the full class name.{x\n\r", ch);
        return;
      } else {
		  
          ptc(ch,"{xSkill{x/{xspells for the class{x:{W %s{x\n\r", class_table[cl].name);
          stc("{xSpells{x:{x\n\r", ch);
          for (level = 1 ;level <= 49;level++) {
              for (sn = 0; sn < MAX_SKILL; sn++) {
				  if (skill_table[sn].rating[cl] <= 0)
					  continue; 

                  if (skill_table[sn].skill_level[cl] != level)
                      continue;
                  if (skill_table[sn].spell_fun == spell_null)
                      continue;


                  ptc(ch, "{x[{W%3d{x]{x %-22s{x",
                  level,
                  skill_table[sn].name);

                  spCount++;

                  if (++col % 3 == 0)
                      stc("\n\r",ch);
              }
          }
          stc("\n\r",ch); col = 0;
          stc("{xSkills{x:{x\n\r", ch);
          for (level = 1 ;level <= 49;level++) {
              for (sn = 0; sn < MAX_SKILL; sn++) {
				  if (skill_table[sn].rating[cl] <= 0)
					  continue; 
                  if (skill_table[sn].skill_level[cl] != level)
                      continue;
                  if (skill_table[sn].spell_fun != spell_null)
                      continue;
				  ptc(ch, "{x[{W%3d{x]{x %-22s{x",
                  level,
                  skill_table[sn].name);

                  skCount++;

                  if (++col % 3 == 0)
                      stc("\n\r",ch);
              }
          }
          stc("\n\r",ch);
          ptc(ch, "{+Total skills found{x: {W%d{x\n\r", skCount); 
          ptc(ch, "{+Total spells found{x: {W%d{x\n\r", spCount);     
          ptc(ch, "{+Total skills/spells found{x: {W%d{x\n\r", (spCount + skCount));
		  stc("Doesn't include skills gained at level 50!!!\n\r", ch);
          return;
      }
    }
        ptc(ch,"{xSettings for skill{x/{xspell{x:{W %s{x\n\r", skill_table[skill].name);
        stc("{xClass           Level Gain  Wait-Time Min-Mana Rating{x\n\r", ch);
        stc("{x--------------  ----- ----  --------- -------- ------{x\n\r", ch);       
        for (iclass = 0; iclass < MAX_CLASS;iclass++) {
        if (skill_table[skill].skill_level[iclass] > 49)
            continue;
		if (skill_table[sn].skill_level[cl] < 1)
			continue;
		if (skill_table[sn].rating[cl] <= 0)
		    continue;
        if (class_table[iclass].name == NULL)
            return;

        ptc(ch,"{x %-11s {W %5d %4d %9d %8d %6d\n\r",
        class_table[iclass].name,
        skill_table[skill].skill_level[iclass],
        skill_table[skill].rating[iclass],
        skill_table[skill].beats,
        skill_table[skill].min_mana,
        skill_table[skill].rating[iclass]);
        }
		  stc("Doesn't include skills gained at level 50!!!\n\r", ch);
        return;
    }

int mana_cost args( (CHAR_DATA * ch, int min_mana, int skilllevel) );

void do_spellskill(CHAR_DATA * ch, char *argument)
{
    char tbuf[MSL];
    char spell_list[LEVEL_HERO + 1][SmSL];
    char spell_columns[LEVEL_HERO + 1];
    int sn, level, min_lev = 1, max_lev = LEVEL_HERO, mana = 0;
    bool fAll = FALSE, found = FALSE;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
	return;
	fAll = TRUE;

	
    stc("{xLevel   {x|{x Skill Name             Cost  Prac{x|{xSkill Name             Cost Prac{x\n\r", ch);
	stc("{m============================================================================{x", ch);

	for (level = 0; level < LEVEL_HERO + 1; level++) {
	spell_columns[level] = 0;
	spell_list[level][0] = '\0';
    }

    for (sn = 0; sn < MAX_SKILL; sn++) {
	if (skill_table[sn].name == NULL)
	    break;

	if ((level = skill_table[sn].skill_level[ch->class]) < LEVEL_HERO + 1
	    && (fAll || level <= ch->level)
	    && level >= min_lev && level <= max_lev
	   // && skill_table[sn].spell_fun != spell_null
	    && ch->pcdata->learned[sn] > 0) {
	    found = TRUE;
	    level = skill_table[sn].skill_level[ch->class];
	    if (ch->level < level)
		sprintf(buf, "{x%-19s n{x/{xa{x      ", skill_table[sn].name);
	    else {
		mana = mana_cost(ch, skill_table[sn].min_mana, level);
		if (skill_table[sn].spell_fun != spell_null)
		{
		sprintf(buf, "{x%-19s{W  %3d{xmana{W %3d{x ", skill_table[sn].name, mana, ch->pcdata->learned[sn]);
	} else {
		sprintf(buf, "{x%-19s          {W%3d{x ", skill_table[sn].name, ch->pcdata->learned[sn]);
	}
	    }

	    if (spell_list[level][0] == '\0')
		sprintf(spell_list[level], "\n\r{xLevel{W %2d{x:{x %s{x", level, buf);
	    else {		
		if (++spell_columns[level] % 2 == 0)
		    strcat(spell_list[level], "\n\r          ");
		strcat(spell_list[level], buf);
	    }
	}
    }


    if (!found) {
	stc("No spells or skills found for your character.\n\r", ch);
	return;
    }

    sprintf(tbuf, "\n\r");
    for (level = 0; level < LEVEL_HERO + 1; level++)
	if (spell_list[level][0] != '\0')
	    strcat(tbuf, spell_list[level]);
    strcat(tbuf, "\n\r");
    page_to_char(tbuf, ch);

}
