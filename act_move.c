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
#include "merc.h"
#include "olc.h"

/* command procedures needed */
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_stand);

char *const dir_name[] =
{
    "north", "east", "south", "west", "up", "down"
};

const sh_int rev_dir[] =
{
    2, 3, 0, 1, 5, 4
};

const sh_int movement_loss[SECT_MAX] =
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6, 0, 6, 2, 5, 1, 2, 2, 6, 10
};

/*
 * Local functions.
 */
int find_door args((CHAR_DATA * ch, char *arg));
bool has_key args((CHAR_DATA * ch, int key));
void trap_trigger args((CHAR_DATA * ch, ROOM_INDEX_DATA * tRoom));

bool move_char(CHAR_DATA * ch, int door, bool follow)
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    if (door < 0 || door > 5) {
	bug("Do_move: bad door %d.", door);
	return FALSE;
    }
    if (IS_SET(ch->act, ACT_NO_MOVE)) {
	send_to_char("You are not capable of walking.\n\r", ch);
	return FALSE;

    }
    if (IS_SET(ch->act, PLR_SEVERED)) {
	send_to_char("Move without legs?  How?\n\r", ch);
	return FALSE;
    }

    /*
     * Exit trigger, if activated, bail out. Only PCs are triggered.
     */
    if (!IS_NPC(ch) && mp_exit_trigger(ch, door))
	return FALSE;

    if (IS_SET(ch->affected_by, AFF_WEB)) {
	send_to_char("You can't move you are stuck!\n\r", ch);
	return FALSE;
    }

    in_room = ch->in_room;
     if ((pexit = in_room->exit[door]) == NULL
	|| (to_room = pexit->u1.to_room) == NULL 
	|| !can_see_room(ch, pexit->u1.to_room)) {
   if (!IS_SET(ch->act, PLR_AUTOBUILD) )
	send_to_char("There seems to be no exit in that direction.\n\r", ch); 
	return FALSE;

    }

    if (IS_SET(pexit->exit_info, EX_CLOSED)
	&& (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info, EX_NOPASS))
	&& !IS_TRUSTED(ch, ANGEL)) {
	act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
	return FALSE;
    }

//    if (IS_AFFECTED(ch, AFF_CHARM)
//	&& ch->master != NULL
//	&& in_room == ch->master->in_room) {
//	send_to_char("What?  And leave your beloved master?\n\r", ch);
//	return FALSE;
  //  }

    if (!is_room_owner(ch, to_room) && room_is_private(to_room)) {
	send_to_char("That room is private right now.\n\r", ch);
	return FALSE;
    }

    if (MOUNTED(ch)) {
	if (MOUNTED(ch)->position < POS_FIGHTING) {
	    send_to_char("Your mount must be standing.\n\r", ch);
	    return FALSE;
	}
	if (IS_SET(to_room->room_flags, ROOM_INDOORS) &&
	    to_room->sector_type == SECT_INSIDE) {
	    send_to_char("You can't take your mount inside.\n\r", ch);
	    return FALSE;
	}
	if (!mount_success(ch, MOUNTED(ch), FALSE)) {
	    send_to_char("Your mount stubbornly refuses to go that way.\n\r", ch);
	    return FALSE;
	}
    }

//    if ( !IS_NPC(ch) )
    {
	CHAR_DATA *rch;
	int move;

//search the mobs
	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {
	    if (IS_NPC(rch) && rch->spec_fun != NULL) {
		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier")
                   && !(armageddon)) {
		    act("$N prevents you from escaping!!", ch, NULL, rch, TO_CHAR);
		    return FALSE;
		}
		if (GET_SIDE(ch) != SIDE_TIER
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_tier2")
                   && !(armageddon)) {
		    act("$N prevents you from escaping!!", ch, NULL, rch, TO_CHAR);
		    return FALSE;
		}
		if (GET_SIDE(ch) != SIDE_KAEL
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_kael")
                   && !(armageddon)) {
		    act("$N blocks your path!!", ch, NULL, rch, TO_CHAR);
		    return FALSE;
		}
		if (GET_SIDE(ch) != SIDE_UM
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_evil")
                   && !(armageddon)) {
		    act("$N blocks your path!!  You can't get away!", ch, NULL, rch, TO_CHAR);
		    return FALSE;
		}
		if (GET_SIDE(ch) != SIDE_CAIRN
		    && !IS_NPC(ch)
		    && IS_SET(rch->act, ACT_NOFLEE)
		    && ch->level <= 100
		    && !strcmp(spec_name(rch->spec_fun), "spec_guard_cairn")
                   && !(armageddon)) {
		    act("$N blocks your path!!  You can't get away!", ch, NULL, rch, TO_CHAR);
		    return FALSE;
		}
//wall code
		if (!strcmp(spec_name(rch->spec_fun), "spec_wall_force") && (ch->level <= 100) ) {
		    int isBlocked = 0;
		    if (door == 0 && is_name("north", rch->long_descr))
			isBlocked = 1;
		    else if (door == 1 && is_name("east", rch->long_descr))
			isBlocked = 1;
		    else if (door == 2 && is_name("south", rch->long_descr))
			isBlocked = 1;
		    else if (door == 3 && is_name("west", rch->long_descr))
			isBlocked = 1;
		    else if (door == 4 && is_name("up", rch->long_descr))
			isBlocked = 1;
		    else if (door == 5 && is_name("down", rch->long_descr))
			isBlocked = 1;
		    if (isBlocked) {
			act("A $N blocks you from going that way!", ch, NULL, rch, TO_CHAR);
			return FALSE;
		    }
		}
	    }
	}			//end search mobs

//search the room affects
	if ( (IS_SET(ch->in_room->affected_by, AFF_SCREAMING_WALL) ||
	      IS_SET(ch->in_room->affected_by, AFF_FLAMING_WALL) )   &&
	     (ch->level <= 100) ) {

//search to see if exits blocked
	    for (paf = ch->in_room->affected; paf != NULL; paf = paf_next) {
		paf_next = paf->next;

//screamer
		if (paf->bitvector == AFF_SCREAMING_WALL &&
		    paf->modifier == door)
//yell
		{
		    act("A wall screams ' AAAUUUGHHH $n is heeere!!! AAAUUUGGHHH!!! '", 
			ch, NULL, ch, TO_CHAR);
		    for (d = descriptor_list; d != NULL; d = d->next) {
			if (d->connected == CON_PLAYING &&
			    d->character->in_room != NULL &&
			    d->character->in_room->area == ch->in_room->area) {
			    act("A wall screams ' AAAUUUGHHH $n is heeere!!! AAAUUUGGHHH!!! '", ch, NULL, d->character, TO_VICT);
			}
		    }
		}		//end yell

//flamer
		if (paf->bitvector == AFF_FLAMING_WALL &&
		    paf->modifier == door)
//burn
		{
		    int dam;

		    dam = saves_spell(paf->level, ch, DAM_FIRE,0) ? 200 : 100;
		if(check_immune(ch, DAM_FIRE) == IS_IMMUNE)
			dam = 0;
		    send_to_char("{RA wall of flame BURNS you!!!{N\n\r", ch);

damage(ch, ch, dam, TYPE_UNDEFINED, DAM_FIRE, FALSE);


		}
	    }			//end exit blocked

	}			//end search room affects

	if (in_room->sector_type == SECT_AIR
	    || to_room->sector_type == SECT_AIR) {
	    if (MOUNTED(ch)) {
		if (!IS_AFFECTED(MOUNTED(ch), AFF_FLYING)) {
		    send_to_char("Your mount can't fly.\n\r", ch);
		    return FALSE;
		}
	    } else {
		if (!IS_AFFECTED(ch, AFF_FLYING) && !IS_IMMORTAL(ch)) {
		    send_to_char("You can't fly.\n\r", ch);
		    return FALSE;
		}
	    }
	}
	if ((in_room->sector_type == SECT_WATER_NOSWIM
	     || to_room->sector_type == SECT_WATER_NOSWIM)
	    && MOUNTED(ch)) {
	    sprintf(buf, "You can't take your mount there.\n\r");
	    send_to_char(buf, ch);
	    return FALSE;
	}
	if ((in_room->sector_type == SECT_WATER_NOSWIM
	     || to_room->sector_type == SECT_WATER_NOSWIM)
	    && !IS_AFFECTED(ch, AFF_FLYING)) {
	    OBJ_DATA *obj;
	    bool found;

	    /*
	     * Look for a boat.
	     */
	    found = FALSE;

	    if (IS_IMMORTAL(ch))
		found = TRUE;

	    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
		if (obj->item_type == ITEM_BOAT) {
		    found = TRUE;
		    break;
		}
	    }
	    if (!found) {
		send_to_char("You need a boat to go there.\n\r", ch);
		return FALSE;
	    }
	}
	move = movement_loss[UMIN(SECT_MAX - 1, in_room->sector_type)]
	    + movement_loss[UMIN(SECT_MAX - 1, to_room->sector_type)];

	move /= 2;		/* i.e. the average */

	if (!MOUNTED(ch)) {
	    /* conditional effects */
	    if (IS_AFFECTED(ch, AFF_FLYING) || IS_AFFECTED(ch, AFF_HASTE))
		move /= 2;

	    if (IS_AFFECTED(ch, AFF_SLOW))
		move *= 2;

	    if (ch->move < move) {
		send_to_char("You are too exhausted.\n\r", ch);
		return FALSE;
	    }
	} else {
	    if (IS_AFFECTED(MOUNTED(ch), AFF_FLYING)
		|| IS_AFFECTED(MOUNTED(ch), AFF_HASTE))
		move /= 2;

	    if (IS_AFFECTED(MOUNTED(ch), AFF_SLOW))
		move *= 2;

	    if (MOUNTED(ch)->move < move) {
		send_to_char("Your mount is too exhausted.\n\r", ch);
		return FALSE;
	    }
	}

if (!IS_IMMORTAL(ch)) { 
	WAIT_STATE(ch, 1); } // No move lag for imms - Akuji
	else {
		move = 0;
	}


if (!MOUNTED(ch))
    ch->move -= move;
	/*
	   else
	   MOUNTED(ch)->move -= move;
	 */
    }

    if (RIDDEN(ch)) {
	CHAR_DATA *rch;
	rch = RIDDEN(ch);

	if (!mount_success(rch, ch, FALSE)) {
	    act("Your mount escapes your control, and leaves $T.", rch, NULL, dir_name[door], TO_CHAR);
	    if (RIDDEN(ch))
		ch = RIDDEN(ch);
	} else {
	    send_to_char("You steady your mount.\n\r", rch);
	    return FALSE;
	}
    }
    /* act( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM ); */

    if (MOUNTED(ch)) {
	if (!IS_AFFECTED(MOUNTED(ch), AFF_FLYING))
	    sprintf(buf, "$n leaves $T, riding on %s.", MOUNTED(ch)->short_descr);
	else
	    sprintf(buf, "$n soars $T, on %s.", MOUNTED(ch)->short_descr);
	act(buf, ch, NULL, dir_name[door], TO_ROOM);
    } else
	/*if ( !IS_AFFECTED(ch, AFF_SNEAK) */
    if (ch->invis_level < LEVEL_HERO) {
	if (IS_AFFECTED(ch, AFF_SNEAK))
	    if (number_percent() < get_skill(ch, gsn_sneak));
	    else
		act("$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM);
	else
	    act("$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM);

    }
    if (IS_AFFECTED(ch, AFF_SNEAK) && !IS_NPC(ch)) {
	check_improve(ch, gsn_sneak, TRUE, 1);
    }
    char_from_room(ch);
    char_to_room(ch, to_room);
    
   if (ch->invis_level < LEVEL_HERO) {
	if (!MOUNTED(ch)) {
	    if (IS_AFFECTED(ch, AFF_SNEAK))
		if (number_percent() < get_skill(ch, gsn_sneak));
		else
		    act("$n has arrived.", ch, NULL, NULL, TO_ROOM);
	    else
		act("$n has arrived.", ch, NULL, NULL, TO_ROOM);
	} else {
	    if (!IS_AFFECTED(MOUNTED(ch), AFF_FLYING))
		act("$n has arrived, riding on $N.", ch, NULL, MOUNTED(ch), TO_ROOM);
	    else
		act("$n soars in, riding on $N.", ch, NULL, MOUNTED(ch), TO_ROOM);
	}
    }
    if (MOUNTED(ch)) {
	char_from_room(MOUNTED(ch));
	char_to_room(MOUNTED(ch), to_room);
    }
    do_look(ch, "auto");

    if (in_room == to_room)	/* no circular follows */
	return FALSE;
    for (fch = in_room->people; fch != NULL; fch = fch_next) {
	fch_next = fch->next_in_room;

	if (fch->master == ch && IS_AFFECTED(fch, AFF_CHARM)
	    && fch->position < POS_STANDING)
	    do_stand(fch, "");

	if ((fch->following == ch && fch->master == NULL) || fch->master == ch)
	if (fch->position == POS_STANDING && can_see_room(fch, to_room)) {

	    if (IS_SET(ch->in_room->room_flags, ROOM_LAW)
		&& (IS_NPC(fch) && IS_SET(fch->act, ACT_AGGRESSIVE))) {
		act("You can't bring $N into the city.",
		    ch, NULL, fch, TO_CHAR);
		act("You aren't allowed in the city.",
		    fch, NULL, NULL, TO_CHAR);
		continue;
	    }
		{
			char pers_buf[MAX_STRING_LENGTH];
			PERS(ch, fch, pers_buf);
			sprintf(buf, "You follow %s %s.\n\r", (IS_NPC(ch) ? ch->short_descr : pers_buf) , dir_name[door]);
			send_to_char(buf, fch);
		}
		
//		act("You follow $N $T.", fch, NULL, dir_name[door], ch, TO_CHAR);

//	    act("You follow $n $T.", fch, NULL, dir_name[door], ch, TO_CHAR);


	    move_char(fch, door, TRUE);

	}
    }

    /* 
     * If someone is following the char, these triggers get activated
     * for the followers before the char, but it's safer this way...
     */
    if (IS_NPC(ch) && HAS_TRIGGER(ch, TRIG_ENTRY))
	mp_percent_trigger(ch, NULL, NULL, NULL, TRIG_ENTRY);
    if (!IS_NPC(ch))
	mp_greet_trigger(ch);

if (IS_SET(ch->in_room->affected_by, AFF_CANT_TRAP) && ((IS_IMMORTAL(ch)) || (get_skill(ch, gsn_detect_trap) >= 85 ) || (get_skill(ch, gsn_geomancy) >= 1)) )
send_to_char("{cThe ground here is too hard to trap!\n\r",ch);

    if (to_room->trap->is_set == TRUE
	&& !IS_NPC(ch))
	trap_trigger(ch, to_room);

	if ((!IS_NPC(ch)) && get_skill(ch, gsn_nature_walk) != 0)
	{
		int hitchance = 0;
		int manachance = 0;
		int movechance = 0;
		int hitamount = 0;
		int manaamount = 0;
		int moveamount = 0;

		hitchance = number_percent();
		manachance = number_percent();
		movechance = number_percent();
		
		if (hitchance > 92)
		{
			hitamount = number_range(ch->level/10, ch->level/2);
			ch->hit += hitamount;
		}
		
		if (manachance > 92)
		{
			manaamount = number_fuzzy(10);
			ch->mana += manaamount;
		}
		
		if (movechance > 90)
		{
			moveamount = number_range(-5,5);
			ch->move += moveamount;
		}
		
		if ( (60 > movechance > 50) && (60 > manachance > 50) && (60 > hitchance > 50) )
		{
		send_to_char("Mother nature blesses you!\n\r",ch);
			ch->hit += 50;
			ch->mana += 50;
			ch->move += 50;
		}

	check_improve(ch, gsn_nature_walk, TRUE, 1);
}

	if (!IS_NPC(ch) && is_affected(ch, skill_lookup("hex")))
	{
		AFFECT_DATA af;
		int chance=0;
		
		chance = number_percent();

		if (chance > 28)
		chance = chance*1; // Sloppy place holder >_>
		else if (chance >= 26){ //poison
		
		    af.where = TO_AFFECTS;
   			af.type = skill_lookup("poison");
   			af.level = 1;
    		af.duration = 3;
    		af.location = APPLY_STR;
    		af.modifier = -1;
    		af.bitvector = AFF_POISON;
    		affect_join(ch, &af);
    		send_to_char("You feel very sick.\n\r", ch);
    		act("$n looks very ill.", ch, NULL, NULL, TO_ROOM);
		}
		else if (chance >= 23){ //plague
		 af.where = TO_AFFECTS;
   	 af.type = skill_lookup("plague");
   	 af.level = 1;
   	 af.duration = 3;
   	 af.location = APPLY_STR;
   	 af.modifier = -1;
   	 af.bitvector = AFF_PLAGUE;
   	 affect_join(ch, &af);

    send_to_char
	("You scream in agony as plague sores erupt from your skin.\n\r", ch);
    act("$n screams in agony as plague sores erupt from $s skin.",
	ch, NULL, NULL, TO_ROOM);}
		else if (chance >= 20){ //life drain
		  af.where = TO_AFFECTS;
    af.type = skill_lookup("life drain");
    af.level = 1;
    af.location = APPLY_DAMROLL;
    af.modifier = -10;
    af.duration = 1;
    af.bitvector = 0;
    affect_to_char(ch, &af);
    send_to_char("You feel as if your life is draining away...\n\r", ch);
    act("$n looks drained and weak.", ch, NULL, NULL, TO_ROOM);
}
		else if (chance >= 17){ //slow
		   af.where = TO_AFFECTS;
    af.type = skill_lookup("slow");
    af.level = 1;
    af.duration = 2;
    af.location = APPLY_DEX;
    af.modifier = -3;
    af.bitvector = AFF_SLOW;
    affect_to_char(ch, &af);
    send_to_char("You feel yourself slowing d o w n...\n\r", ch);
    act("$n starts to move in slow motion.", ch, NULL, NULL, TO_ROOM);
}
		else if (chance >= 14){ // blind
		   af.where = TO_AFFECTS;
    af.type = skill_lookup("blindness");
    af.level = 1;
    af.location = APPLY_HITROLL;
    af.modifier = -4;
    af.duration = 0;
    af.bitvector = AFF_BLIND;
    affect_to_char(ch, &af);
    send_to_char("You are blinded!\n\r", ch);
    act("$n appears to be blinded.", ch, NULL, NULL, TO_ROOM);}
		else if (chance >= 11){ // weaken
		    af.where = TO_AFFECTS;
    af.type = skill_lookup("weaken");
    af.level = 1;
    af.duration = 1 / 2;
    af.location = APPLY_STR;
    af.modifier = -1;
    af.bitvector = AFF_WEAKEN;
    affect_to_char(ch, &af);}
		else if (chance >= 8){ // curse
		af.where = TO_AFFECTS;
    af.type = skill_lookup("curse");
    af.level = 1;
    af.duration = 3;
    af.location = APPLY_SAVING_SPELL;
    af.modifier = 5;
    af.bitvector = AFF_CURSE;
    affect_to_char(ch, &af);
    send_to_char("You feel uncomfortable.\n\r", ch);}
		else if (chance >= 6){ // death touch
		 af.where = TO_AFFECTS2;
    af.type = skill_lookup("death touch");
    af.level = 1;
    af.duration = 0;
    af.bitvector = AFF_UNDEAD;
    affect_to_char(ch, &af);
    send_to_char("Your soul is overridden with darkness!\n\r", ch);
    act("$n turns deathly white.", ch, NULL, NULL, TO_ROOM);}
		else if (chance >= 4){ // sleep
		  af.where = TO_AFFECTS;
    af.type = skill_lookup("sleep");
    af.level = 1;
    af.duration = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_SLEEP;
    affect_join(ch, &af);}
		else if (chance >= 2){ // entangle
		    af.where = TO_AFFECTS;
    af.type = skill_lookup("entangle");
    af.level = 1;
    af.duration = 0;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = AFF_WEB;
    affect_to_char(ch, &af);
    send_to_char("You are covered in sticky webs!!\n\r",ch);
}
}
	if (ch->hit < ch->max_hit / 4)
	{
	OBJ_DATA *puddle;
	puddle = create_object(get_obj_index(OBJ_VNUM_BLOOD_PUDDLE), 0);
	puddle->timer = number_range(2,4);
	obj_to_room(puddle, ch->in_room);
	}

    return TRUE;
}

void do_north(CHAR_DATA * ch, char *argument)
{
    move_char(ch, DIR_NORTH, FALSE);
    return;
}

void do_east(CHAR_DATA * ch, char *argument)
{
    move_char(ch, DIR_EAST, FALSE);
    return;
}

void do_south(CHAR_DATA * ch, char *argument)
{
    move_char(ch, DIR_SOUTH, FALSE);
    return;
}

void do_west(CHAR_DATA * ch, char *argument)
{
    move_char(ch, DIR_WEST, FALSE);
    return;
}

void do_up(CHAR_DATA * ch, char *argument)
{
    move_char(ch, DIR_UP, FALSE);
    return;
}

void do_down(CHAR_DATA * ch, char *argument)
{
    move_char(ch, DIR_DOWN, FALSE);
    return;
}

int find_door(CHAR_DATA * ch, char *arg)
{
    EXIT_DATA *pexit;
    int door;

    if (!str_cmp(arg, "n") || !str_cmp(arg, "north"))
	door = 0;
    else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))
	door = 1;
    else if (!str_cmp(arg, "s") || !str_cmp(arg, "south"))
	door = 2;
    else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))
	door = 3;
    else if (!str_cmp(arg, "u") || !str_cmp(arg, "up"))
	door = 4;
    else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))
	door = 5;
    else {
	for (door = 0; door <= 5; door++) {
	    if ((pexit = ch->in_room->exit[door]) != NULL
		&& IS_SET(pexit->exit_info, EX_ISDOOR)
		&& pexit->keyword != NULL
		&& is_name(arg, pexit->keyword))
		return door;
	}
	act("I see no $T here.", ch, NULL, arg, TO_CHAR);
	return -1;
    }

    if ((pexit = ch->in_room->exit[door]) == NULL) {
	act("I see no door $T here.", ch, NULL, arg, TO_CHAR);
	return -1;
    }
    if (!IS_SET(pexit->exit_info, EX_ISDOOR)) {
	send_to_char("You can't do that.\n\r", ch);
	return -1;
    }
    return door;
}

void do_open(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Open what?\n\r", ch);
	return;
    }
    if ((obj = get_obj_here(ch, arg)) != NULL) {
	/* open portal */
	if (obj->item_type == ITEM_PORTAL) {
	    if (!IS_SET(obj->value[1], EX_ISDOOR)) {
		send_to_char("You can't do that.\n\r", ch);
		return;
	    }
	    if (!IS_SET(obj->value[1], EX_CLOSED)) {
		send_to_char("It's already open.\n\r", ch);
		return;
	    }
	    if (IS_SET(obj->value[1], EX_LOCKED)) {
		send_to_char("It's locked.\n\r", ch);
		return;
	    }
	    REMOVE_BIT(obj->value[1], EX_CLOSED);
	    act("You open $p.", ch, obj, NULL, TO_CHAR);
	    act("$n opens $p.", ch, obj, NULL, TO_ROOM);
	    return;
	}
	/* 'open object' */
	if (obj->item_type != ITEM_CONTAINER) {
	    send_to_char("That's not a container.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_CLOSED)) {
	    send_to_char("It's already open.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_CLOSEABLE)) {
	    send_to_char("You can't do that.\n\r", ch);
	    return;
	}
	if (IS_SET(obj->value[1], CONT_LOCKED)) {
	    send_to_char("It's locked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act("You open $p.", ch, obj, NULL, TO_CHAR);
	act("$n opens $p.", ch, obj, NULL, TO_ROOM);
	return;
    }
    if ((door = find_door(ch, arg)) >= 0) {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
	    send_to_char("It's already open.\n\r", ch);
	    return;
	}
	if (IS_SET(pexit->exit_info, EX_LOCKED)) {
	    send_to_char("It's locked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act("$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM);
	send_to_char("You open it.\n\r", ch);

	/* open the other side */
	if ((to_room = pexit->u1.to_room) != NULL
	    && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
	    && pexit_rev->u1.to_room == ch->in_room) {
	    CHAR_DATA *rch;

	    REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
	    for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
		act("The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR);
	}
    }
    return;
}

void do_close(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Close what?\n\r", ch);
	return;
    }
    if ((obj = get_obj_here(ch, arg)) != NULL) {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL) {

	    if (!IS_SET(obj->value[1], EX_ISDOOR)
		|| IS_SET(obj->value[1], EX_NOCLOSE)) {
		send_to_char("You can't do that.\n\r", ch);
		return;
	    }
	    if (IS_SET(obj->value[1], EX_CLOSED)) {
		send_to_char("It's already closed.\n\r", ch);
		return;
	    }
	    SET_BIT(obj->value[1], EX_CLOSED);
	    act("You close $p.", ch, obj, NULL, TO_CHAR);
	    act("$n closes $p.", ch, obj, NULL, TO_ROOM);
	    return;
	}
	/* 'close object' */
	if (obj->item_type != ITEM_CONTAINER) {
	    send_to_char("That's not a container.\n\r", ch);
	    return;
	}
	if (IS_SET(obj->value[1], CONT_CLOSED)) {
	    send_to_char("It's already closed.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_CLOSEABLE)) {
	    send_to_char("You can't do that.\n\r", ch);
	    return;
	}
	SET_BIT(obj->value[1], CONT_CLOSED);
	act("You close $p.", ch, obj, NULL, TO_CHAR);
	act("$n closes $p.", ch, obj, NULL, TO_ROOM);
	return;
    }
    if ((door = find_door(ch, arg)) >= 0) {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if (IS_SET(pexit->exit_info, EX_CLOSED)) {
	    send_to_char("It's already closed.\n\r", ch);
	    return;
	}
	SET_BIT(pexit->exit_info, EX_CLOSED);
	act("$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM);
	send_to_char("You close it.\n\r", ch);

	/* close the other side */
	if ((to_room = pexit->u1.to_room) != NULL
	    && (pexit_rev = to_room->exit[rev_dir[door]]) != 0
	    && pexit_rev->u1.to_room == ch->in_room) {
	    CHAR_DATA *rch;

	    SET_BIT(pexit_rev->exit_info, EX_CLOSED);
	    for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
		act("The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR);
	}
    }
    return;
}

bool has_key(CHAR_DATA * ch, int key)
{
    OBJ_DATA *obj;

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content) {
	if (obj->pIndexData->vnum == key)
	    return TRUE;
    }

    return FALSE;
}

void do_lock(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Lock what?\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't reach the lock from your mount.\n\r", ch);
	return;
    }
    if ((obj = get_obj_here(ch, arg)) != NULL) {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL) {
	    if (!IS_SET(obj->value[1], EX_ISDOOR)
		|| IS_SET(obj->value[1], EX_NOCLOSE)) {
		send_to_char("You can't do that.\n\r", ch);
		return;
	    }
	    if (!IS_SET(obj->value[1], EX_CLOSED)) {
		send_to_char("It's not closed.\n\r", ch);
		return;
	    }
	    if (obj->value[4] < 0 || IS_SET(obj->value[1], EX_NOLOCK)) {
		send_to_char("It can't be locked.\n\r", ch);
		return;
	    }
	    if (!has_key(ch, obj->value[4])) {
		send_to_char("You lack the key.\n\r", ch);
		return;
	    }
	    if (IS_SET(obj->value[1], EX_LOCKED)) {
		send_to_char("It's already locked.\n\r", ch);
		return;
	    }
	    SET_BIT(obj->value[1], EX_LOCKED);
	    act("You lock $p.", ch, obj, NULL, TO_CHAR);
	    act("$n locks $p.", ch, obj, NULL, TO_ROOM);
	    return;
	}
	/* 'lock object' */
	if (obj->item_type != ITEM_CONTAINER) {
	    send_to_char("That's not a container.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_CLOSED)) {
	    send_to_char("It's not closed.\n\r", ch);
	    return;
	}
	if (obj->value[2] < 0) {
	    send_to_char("It can't be locked.\n\r", ch);
	    return;
	}
	if (!has_key(ch, obj->value[2])) {
	    send_to_char("You lack the key.\n\r", ch);
	    return;
	}
	if (IS_SET(obj->value[1], CONT_LOCKED)) {
	    send_to_char("It's already locked.\n\r", ch);
	    return;
	}
	SET_BIT(obj->value[1], CONT_LOCKED);
	act("You lock $p.", ch, obj, NULL, TO_CHAR);
	act("$n locks $p.", ch, obj, NULL, TO_ROOM);
	return;
    }
    if ((door = find_door(ch, arg)) >= 0) {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
	    send_to_char("It's not closed.\n\r", ch);
	    return;
	}
	if (pexit->key < 0) {
	    send_to_char("It can't be locked.\n\r", ch);
	    return;
	}
	if (!has_key(ch, pexit->key)) {
	    send_to_char("You lack the key.\n\r", ch);
	    return;
	}
	if (IS_SET(pexit->exit_info, EX_LOCKED)) {
	    send_to_char("It's already locked.\n\r", ch);
	    return;
	}
	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char("*Click*\n\r", ch);
	act("$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

	/* lock the other side */
	if ((to_room = pexit->u1.to_room) != NULL
	    && (pexit_rev = to_room->exit[rev_dir[door]]) != 0
	    && pexit_rev->u1.to_room == ch->in_room) {
	    SET_BIT(pexit_rev->exit_info, EX_LOCKED);
	}
    }
    return;
}

void do_unlock(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument(argument, arg);

    if (arg[0] == '\0') {
	send_to_char("Unlock what?\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't reach the lock from your mount.\n\r", ch);
	return;
    }
    if ((obj = get_obj_here(ch, arg)) != NULL) {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL) {
	    if (!IS_SET(obj->value[1], EX_ISDOOR)) {
		send_to_char("You can't do that.\n\r", ch);
		return;
	    }
	    if (!IS_SET(obj->value[1], EX_CLOSED)) {
		send_to_char("It's not closed.\n\r", ch);
		return;
	    }
	    if (obj->value[4] < 0) {
		send_to_char("It can't be unlocked.\n\r", ch);
		return;
	    }
	    if (!has_key(ch, obj->value[4])) {
		send_to_char("You lack the key.\n\r", ch);
		return;
	    }
	    if (!IS_SET(obj->value[1], EX_LOCKED)) {
		send_to_char("It's already unlocked.\n\r", ch);
		return;
	    }
	    REMOVE_BIT(obj->value[1], EX_LOCKED);
	    act("You unlock $p.", ch, obj, NULL, TO_CHAR);
	    act("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
	    return;
	}
	/* 'unlock object' */
	if (obj->item_type != ITEM_CONTAINER) {
	    send_to_char("That's not a container.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_CLOSED)) {
	    send_to_char("It's not closed.\n\r", ch);
	    return;
	}
	if (obj->value[2] < 0) {
	    send_to_char("It can't be unlocked.\n\r", ch);
	    return;
	}
	if (!has_key(ch, obj->value[2])) {
	    send_to_char("You lack the key.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_LOCKED)) {
	    send_to_char("It's already unlocked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	act("You unlock $p.", ch, obj, NULL, TO_CHAR);
	act("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
	return;
    }
    if ((door = find_door(ch, arg)) >= 0) {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if (!IS_SET(pexit->exit_info, EX_CLOSED)) {
	    send_to_char("It's not closed.\n\r", ch);
	    return;
	}
	if (pexit->key < 0) {
	    send_to_char("It can't be unlocked.\n\r", ch);
	    return;
	}
	if (!has_key(ch, pexit->key)) {
	    send_to_char("You lack the key.\n\r", ch);
	    return;
	}
	if (!IS_SET(pexit->exit_info, EX_LOCKED)) {
	    send_to_char("It's already unlocked.\n\r", ch);
	    return;
	}
	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char("*Click*\n\r", ch);
	act("$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

	/* unlock the other side */
	if ((to_room = pexit->u1.to_room) != NULL
	    && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
	    && pexit_rev->u1.to_room == ch->in_room) {
	    REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
	}
    }
    return;
}

void do_pick(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument(argument, arg);

if ((get_skill(ch, gsn_pick_lock) == 0
	|| ch->level < skill_table[gsn_pick_lock].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (arg[0] == '\0') {
	send_to_char("Pick what?\n\r", ch);
	return;
    }
    if (MOUNTED(ch)) {
	send_to_char("You can't pick locks while mounted.\n\r", ch);
	return;
    }
	if (!IS_IMMORTAL(ch))

   WAIT_STATE(ch, skill_table[gsn_pick_lock].beats);


    /* look for guards */
    for (gch = ch->in_room->people; gch; gch = gch->next_in_room) {
	if (IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level) {
	    act("$N is standing too close to the lock.",
		ch, NULL, gch, TO_CHAR);
	    return;
	}
    }

    if (!IS_NPC(ch) && number_percent() > get_skill(ch, gsn_pick_lock)) {
	send_to_char("You failed.\n\r", ch);
	check_improve(ch, gsn_pick_lock, FALSE, 2);
	return;
    }
    if ((obj = get_obj_here(ch, arg)) != NULL) {
	/* portal stuff */
	if (obj->item_type == ITEM_PORTAL) {
	    if (!IS_SET(obj->value[1], EX_ISDOOR)) {
		send_to_char("You can't do that.\n\r", ch);
		return;
	    }
	    if (!IS_SET(obj->value[1], EX_CLOSED)) {
		send_to_char("It's not closed.\n\r", ch);
		return;
	    }
	    if (obj->value[4] < 0) {
		send_to_char("It can't be unlocked.\n\r", ch);
		return;
	    }
	    if (IS_SET(obj->value[1], EX_PICKPROOF)) {
		send_to_char("You failed.\n\r", ch);
		return;
	    }
	    REMOVE_BIT(obj->value[1], EX_LOCKED);
	    act("You pick the lock on $p.", ch, obj, NULL, TO_CHAR);
	    act("$n picks the lock on $p.", ch, obj, NULL, TO_ROOM);
	    check_improve(ch, gsn_pick_lock, TRUE, 2);
	    return;
	}
	/* 'pick object' */
	if (obj->item_type != ITEM_CONTAINER) {
	    send_to_char("That's not a container.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_CLOSED)) {
	    send_to_char("It's not closed.\n\r", ch);
	    return;
	}
	if (obj->value[2] < 0) {
	    send_to_char("It can't be unlocked.\n\r", ch);
	    return;
	}
	if (!IS_SET(obj->value[1], CONT_LOCKED)) {
	    send_to_char("It's already unlocked.\n\r", ch);
	    return;
	}
	if (IS_SET(obj->value[1], CONT_PICKPROOF)) {
	    send_to_char("You failed.\n\r", ch);
	    return;
	}
	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	act("You pick the lock on $p.", ch, obj, NULL, TO_CHAR);
	act("$n picks the lock on $p.", ch, obj, NULL, TO_ROOM);
	check_improve(ch, gsn_pick_lock, TRUE, 2);
	return;
    }
    if ((door = find_door(ch, arg)) >= 0) {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if (!IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch)) {
	    send_to_char("It's not closed.\n\r", ch);
	    return;
	}
	if (pexit->key < 0 && !IS_IMMORTAL(ch)) {
	    send_to_char("It can't be picked.\n\r", ch);
	    return;
	}
	if (!IS_SET(pexit->exit_info, EX_LOCKED)) {
	    send_to_char("It's already unlocked.\n\r", ch);
	    return;
	}
	if (IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch)) {
	    send_to_char("You failed.\n\r", ch);
	    return;
	}
	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char("*Click*\n\r", ch);
	act("$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM);
	check_improve(ch, gsn_pick_lock, TRUE, 2);

	/* pick the other side */
	if ((to_room = pexit->u1.to_room) != NULL
	    && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
	    && pexit_rev->u1.to_room == ch->in_room) {
	    REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
	}
    }
    return;
}

void do_stand(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    if (argument[0] != '\0') {
	if (ch->position == POS_FIGHTING) {
	    send_to_char("Maybe you should finish fighting first?\n\r", ch);
	    return;
	}
	obj = get_obj_list(ch, argument, ch->in_room->contents);
	if (obj == NULL) {
	    send_to_char("You don't see that here.\n\r", ch);
	    return;
	}
	if (obj->item_type != ITEM_FURNITURE
	    || (!IS_SET(obj->value[2], STAND_AT)
		&& !IS_SET(obj->value[2], STAND_ON)
		&& !IS_SET(obj->value[2], STAND_IN))) {
	    send_to_char("You can't seem to find a place to stand.\n\r", ch);
	    return;
	}
	if (ch->on != obj && count_users(obj) >= obj->value[0]) {
	    act_new("There's no room to stand on $p.",
		    ch, obj, NULL, TO_CHAR, POS_DEAD);
	    return;
	}
	ch->on = obj;
    }
    switch (ch->position) {
    case POS_SLEEPING:
	if (IS_AFFECTED(ch, AFF_SLEEP) 
      || IS_AFFECTED2(ch, AFF_SUSPEND) )
     {
	    send_to_char("You can't wake up!\n\r", ch);
	    return;
	}
	if (obj == NULL) {
	    send_to_char("You wake and stand up.\n\r", ch);
	    act("$n wakes and stands up.", ch, NULL, NULL, TO_ROOM);
	    ch->on = NULL;
	} else if (IS_SET(obj->value[2], STAND_AT)) {
	    act_new("You wake and stand at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    act("$n wakes and stands at $p.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], STAND_ON)) {
	    act_new("You wake and stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    act("$n wakes and stands on $p.", ch, obj, NULL, TO_ROOM);
	} else {
	    act_new("You wake and stand in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    act("$n wakes and stands in $p.", ch, obj, NULL, TO_ROOM);
	}
	ch->position = POS_STANDING;
	do_look(ch, "auto");
	break;

    case POS_RESTING:

if (ch->fighting != NULL) ch->position = POS_FIGHTING;
    case POS_SITTING:
	if (obj == NULL) {
	    send_to_char("You stand up.\n\r", ch);
	    act("$n stands up.", ch, NULL, NULL, TO_ROOM);
	    ch->on = NULL;
	} else if (IS_SET(obj->value[2], STAND_AT)) {
	    act("You stand at $p.", ch, obj, NULL, TO_CHAR);
	    act("$n stands at $p.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], STAND_ON)) {
	    act("You stand on $p.", ch, obj, NULL, TO_CHAR);
	    act("$n stands on $p.", ch, obj, NULL, TO_ROOM);
	} else {
	    act("You stand in $p.", ch, obj, NULL, TO_CHAR);
	    act("$n stands on $p.", ch, obj, NULL, TO_ROOM);
	}
	ch->position = POS_STANDING;
break;

    case POS_STANDING:
	send_to_char("You are already standing.\n\r", ch);
	break;

    case POS_FIGHTING:
	send_to_char("You are already fighting!\n\r", ch);
	break;
    }
   update_pkpos(ch);
   return;
}

void do_rest(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    if (MOUNTED(ch)) {
	send_to_char("You can't rest while mounted.\n\r", ch);
	return;
    }
    if (RIDDEN(ch)) {
	send_to_char("You can't rest while being ridden.\n\r", ch);
	return;
    }
    if (ch->position == POS_FIGHTING) {
	send_to_char("You are already fighting!\n\r", ch);
	return;
    }
    /* okay, now that we know we can rest, find an object to rest on */
    if (argument[0] != '\0') {
	obj = get_obj_list(ch, argument, ch->in_room->contents);
	if (obj == NULL) {
	    send_to_char("You don't see that here.\n\r", ch);
	    return;
	}
    } else
	obj = ch->on;

    if (obj != NULL) {
	if (obj->item_type != ITEM_FURNITURE
	    || (!IS_SET(obj->value[2], REST_ON)
		&& !IS_SET(obj->value[2], REST_IN)
		&& !IS_SET(obj->value[2], REST_AT))) {
	    send_to_char("You can't rest on that.\n\r", ch);
	    return;
	}
	if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0]) {
	    act_new("There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    return;
	}
	ch->on = obj;
    }
    switch (ch->position) {
    case POS_SLEEPING:
	if (IS_AFFECTED(ch, AFF_SLEEP)) {
	    send_to_char("You can't wake up!\n\r", ch);
	    return;
	}
	if (obj == NULL) {
	    send_to_char("You wake up and start resting.\n\r", ch);
	    act("$n wakes up and starts resting.", ch, NULL, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], REST_AT)) {
	    act_new("You wake up and rest at $p.",
		    ch, obj, NULL, TO_CHAR, POS_SLEEPING);
	    act("$n wakes up and rests at $p.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], REST_ON)) {
	    act_new("You wake up and rest on $p.",
		    ch, obj, NULL, TO_CHAR, POS_SLEEPING);
	    act("$n wakes up and rests on $p.", ch, obj, NULL, TO_ROOM);
	} else {
	    act_new("You wake up and rest in $p.",
		    ch, obj, NULL, TO_CHAR, POS_SLEEPING);
	    act("$n wakes up and rests in $p.", ch, obj, NULL, TO_ROOM);
	}
	ch->position = POS_RESTING;
	break;

    case POS_RESTING:
	send_to_char("You are already resting.\n\r", ch);
	break;

    case POS_STANDING:
	if (obj == NULL) {
	    send_to_char("You rest.\n\r", ch);
	    act("$n sits down and rests.", ch, NULL, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], REST_AT)) {
	    act("You sit down at $p and rest.", ch, obj, NULL, TO_CHAR);
	    act("$n sits down at $p and rests.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], REST_ON)) {
	    act("You sit on $p and rest.", ch, obj, NULL, TO_CHAR);
	    act("$n sits on $p and rests.", ch, obj, NULL, TO_ROOM);
	} else {
	    act("You rest in $p.", ch, obj, NULL, TO_CHAR);
	    act("$n rests in $p.", ch, obj, NULL, TO_ROOM);
	}
	ch->position = POS_RESTING;
	break;

    case POS_SITTING:
	if (obj == NULL) {
	    send_to_char("You rest.\n\r", ch);
	    act("$n rests.", ch, NULL, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], REST_AT)) {
	    act("You rest at $p.", ch, obj, NULL, TO_CHAR);
	    act("$n rests at $p.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], REST_ON)) {
	    act("You rest on $p.", ch, obj, NULL, TO_CHAR);
	    act("$n rests on $p.", ch, obj, NULL, TO_ROOM);
	} else {
	    act("You rest in $p.", ch, obj, NULL, TO_CHAR);
	    act("$n rests in $p.", ch, obj, NULL, TO_ROOM);
	}
	ch->position = POS_RESTING;
	break;
    }

    return;
}

void do_sit(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    if (MOUNTED(ch)) {
	send_to_char("You can't sit while mounted.\n\r", ch);
	return;
    }
    if (RIDDEN(ch)) {
	send_to_char("You can't sit while being ridden.\n\r", ch);
	return;
    }
    if (ch->position == POS_FIGHTING) {
	send_to_char("Maybe you should finish this fight first?\n\r", ch);
	return;
    }
    /* okay, now that we know we can sit, find an object to sit on */
    if (argument[0] != '\0') {
	obj = get_obj_list(ch, argument, ch->in_room->contents);
	if (obj == NULL) {
	    send_to_char("You don't see that here.\n\r", ch);
	    return;
	}
    } else
	obj = ch->on;

    if (obj != NULL) {
	if (obj->item_type != ITEM_FURNITURE
	    || (!IS_SET(obj->value[2], SIT_ON)
		&& !IS_SET(obj->value[2], SIT_IN)
		&& !IS_SET(obj->value[2], SIT_AT))) {
	    send_to_char("You can't sit on that.\n\r", ch);
	    return;
	}
	if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0]) {
	    act_new("There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    return;
	}
	ch->on = obj;
    }
    switch (ch->position) {
    case POS_SLEEPING:
	if (IS_AFFECTED(ch, AFF_SLEEP)) {
	    send_to_char("You can't wake up!\n\r", ch);
	    return;
	}
	if (obj == NULL) {
	    send_to_char("You wake and sit up.\n\r", ch);
	    act("$n wakes and sits up.", ch, NULL, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], SIT_AT)) {
	    act_new("You wake and sit at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    act("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], SIT_ON)) {
	    act_new("You wake and sit on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    act("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
	} else {
	    act_new("You wake and sit in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	    act("$n wakes and sits in $p.", ch, obj, NULL, TO_ROOM);
	}

	ch->position = POS_SITTING;
	break;
    case POS_RESTING:
	if (obj == NULL)
	    send_to_char("You stop resting.\n\r", ch);
	else if (IS_SET(obj->value[2], SIT_AT)) {
	    act("You sit at $p.", ch, obj, NULL, TO_CHAR);
	    act("$n sits at $p.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], SIT_ON)) {
	    act("You sit on $p.", ch, obj, NULL, TO_CHAR);
	    act("$n sits on $p.", ch, obj, NULL, TO_ROOM);
	}
	ch->position = POS_SITTING;
	break;
    case POS_SITTING:
	send_to_char("You are already sitting down.\n\r", ch);
	break;
    case POS_STANDING:
	if (obj == NULL) {
	    send_to_char("You sit down.\n\r", ch);
	    act("$n sits down on the ground.", ch, NULL, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], SIT_AT)) {
	    act("You sit down at $p.", ch, obj, NULL, TO_CHAR);
	    act("$n sits down at $p.", ch, obj, NULL, TO_ROOM);
	} else if (IS_SET(obj->value[2], SIT_ON)) {
	    act("You sit on $p.", ch, obj, NULL, TO_CHAR);
	    act("$n sits on $p.", ch, obj, NULL, TO_ROOM);
	} else {
	    act("You sit down in $p.", ch, obj, NULL, TO_CHAR);
	    act("$n sits down in $p.", ch, obj, NULL, TO_ROOM);
	}
	ch->position = POS_SITTING;
	break;
    }
    return;
}

void do_sleep(CHAR_DATA * ch, char *argument)
{
    OBJ_DATA *obj = NULL;

    if (MOUNTED(ch)) {
	send_to_char("You can't sleep while mounted.\n\r", ch);
	return;
    }
    if (RIDDEN(ch)) {
	send_to_char("You can't sleep while being ridden.\n\r", ch);
	return;
    }
    switch (ch->position) {
    case POS_SLEEPING:
	send_to_char("You are already sleeping.\n\r", ch);
	break;

    case POS_RESTING:
    case POS_SITTING:
    case POS_STANDING:
	if (argument[0] == '\0' && ch->on == NULL) {
	    send_to_char("You go to sleep.\n\r", ch);
	    act("$n goes to sleep.", ch, NULL, NULL, TO_ROOM);
	    ch->position = POS_SLEEPING;
	} else {		/* find an object and sleep on it */
	    if (argument[0] == '\0')
		obj = ch->on;
	    else
		obj = get_obj_list(ch, argument, ch->in_room->contents);

	    if (obj == NULL) {
		send_to_char("You don't see that here.\n\r", ch);
		return;
	    }
	    if (obj->item_type != ITEM_FURNITURE
		|| (!IS_SET(obj->value[2], SLEEP_ON)
		    && !IS_SET(obj->value[2], SLEEP_IN)
		    && !IS_SET(obj->value[2], SLEEP_AT))) {
		send_to_char("You can't sleep on that!\n\r", ch);
		return;
	    }
	    if (ch->on != obj && count_users(obj) >= obj->value[0]) {
		act_new("There is no room on $p for you.",
			ch, obj, NULL, TO_CHAR, POS_DEAD);
		return;
	    }
	    ch->on = obj;
	    if (IS_SET(obj->value[2], SLEEP_AT)) {
		act("You go to sleep at $p.", ch, obj, NULL, TO_CHAR);
		act("$n goes to sleep at $p.", ch, obj, NULL, TO_ROOM);
	    } else if (IS_SET(obj->value[2], SLEEP_ON)) {
		act("You go to sleep on $p.", ch, obj, NULL, TO_CHAR);
		act("$n goes to sleep on $p.", ch, obj, NULL, TO_ROOM);
	    } else {
		act("You go to sleep in $p.", ch, obj, NULL, TO_CHAR);
		act("$n goes to sleep in $p.", ch, obj, NULL, TO_ROOM);
	    }
	    ch->position = POS_SLEEPING;
	}
	break;

    case POS_FIGHTING:
	send_to_char("You are already fighting!\n\r", ch);
	break;
    }

    return;
}

void do_wake(CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument(argument, arg);
    if (arg[0] == '\0') {
	do_stand(ch, argument);
	return;
    }
    if (!IS_AWAKE(ch)) {
	send_to_char("You are asleep yourself!\n\r", ch);
	return;
    }
    if ((victim = get_char_room(ch, arg)) == NULL) {
	send_to_char("They aren't here.\n\r", ch);
	return;
    }
    if (IS_AWAKE(victim)) {
	act("$N is already awake.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (IS_AFFECTED(victim, AFF_SLEEP)) {
	act("You can't wake $M!", ch, NULL, victim, TO_CHAR);
	return;
    }
    act_new("$n wakes you.", ch, NULL, victim, TO_VICT, POS_SLEEPING);
    do_stand(victim, "");
    return;
}

void do_sneak(CHAR_DATA * ch, char *argument)
{
    /* AFFECT_DATA af;
     */

if ((get_skill(ch, gsn_sneak) == 0
	|| ch->level < skill_table[gsn_sneak].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    if (MOUNTED(ch)) {
	send_to_char("You can't sneak while riding.\n\r", ch);
	return;
    }
    if (!IS_AFFECTED(ch, AFF_SNEAK)) {
	send_to_char("You will now sneak everywhere you go.\n\r", ch);
	SET_BIT(ch->affected_by, AFF_SNEAK);
    } else if (IS_AFFECTED(ch, AFF_SNEAK)) {
	send_to_char("You will no longer try to sneak.\n\r", ch);
	REMOVE_BIT(ch->affected_by, AFF_SNEAK);
    }
    return;

/*    affect_strip( ch, gsn_sneak ); */

/*
   if ( number_percent( ) < get_skill(ch,gsn_sneak))
   {
 */
    /*
       check_improve(ch,gsn_sneak,TRUE,3);
       af.where     = TO_AFFECTS;
       af.type      = gsn_sneak;
       af.level     = ch->level; 
       af.duration  = ch->level;
       af.location  = APPLY_NONE;
       af.modifier  = 0;
       af.bitvector = AFF_SNEAK;
       affect_to_char( ch, &af );
       else
       check_improve(ch,gsn_sneak,FALSE,3);
     */

    return;
}

void do_hide(CHAR_DATA * ch, char *argument)
{
    if (MOUNTED(ch)) {
	send_to_char("You can't hide while riding.\n\r", ch);
	return;
    }
if ((get_skill(ch, gsn_hide) == 0
	|| ch->level < skill_table[gsn_hide].skill_level[ch->class])) {
	send_to_char("Huh?\n\r", ch);
	return;
    }

    send_to_char("You attempt to hide.\n\r", ch);

    if (IS_AFFECTED(ch, AFF_HIDE))
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if (number_percent() < get_skill(ch, gsn_hide)) {
	SET_BIT(ch->affected_by, AFF_HIDE);
	check_improve(ch, gsn_hide, TRUE, 3);
    } else
	check_improve(ch, gsn_hide, FALSE, 3);

    return;
}

/*
 * Contributed by Alander.
 */
void do_visible(CHAR_DATA * ch, char *argument)
{
   affect_strip(ch, gsn_invis);
   affect_strip(ch, gsn_mass_invis);
   affect_strip(ch, gsn_sneak);
   REMOVE_BIT(ch->affected_by, AFF_HIDE);
   REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
   REMOVE_BIT(ch->affected_by, AFF_SNEAK);
   send_to_char("You are now completly visible.\n\r", ch);
   return;
}



void do_memorize(CHAR_DATA * ch, char *argument)
{
	   
    if (IS_NPC(ch))
	return;
	
	if ( (ch->side ==1) &&
	( (ch->in_room->vnum/100 == 34) || 
	  (ch->in_room->vnum/100 == 158) || 
	  (ch->in_room->vnum/100 == 159) || 
	  (ch->in_room->vnum/100 == 152) ||  
	  (ch->in_room->vnum/100 == 153) ||
	  (ch->in_room->vnum/100 == 3) ||
	  (ch->in_room->vnum/100 == 600) ||
	  (ch->in_room->vnum/100 == 264) ||
	  (ch->in_room->vnum/100 == 262) ||
	  (ch->in_room->vnum/100 == 259) ||
	  (ch->in_room->vnum/100 == 258) ||
	  (ch->in_room->vnum/100 == 261) ||
	  (ch->in_room->vnum/100 == 206) ||
	  (ch->in_room->vnum/100 == 150) ||
	  (ch->in_room->vnum/100 == 100) ||
	  (ch->in_room->vnum/100 == 61) ||
	  (ch->in_room->vnum/100 == 60) ||
	  (ch->in_room->vnum/100 == 271) ))
	{
		
		ch->pcdata->research = ch->in_room->vnum;
		send_to_char("You have memorized this location to recall to.\n\r",ch);
		return;
	}
		
	
	if ( (ch->side ==2) &&
	( (ch->in_room->vnum/100 == 202) || 
	  (ch->in_room->vnum/100 == 542) || 
	  (ch->in_room->vnum/100 == 203) || 
	  (ch->in_room->vnum/100 == 229) ||  
	  (ch->in_room->vnum/100 == 565) ||
	  (ch->in_room->vnum/100 == 227) ||
	  (ch->in_room->vnum/100 == 228) ||
	  (ch->in_room->vnum/100 == 226) ||
	  (ch->in_room->vnum/100 == 225) ||
	  (ch->in_room->vnum/100 == 224) ||
	  (ch->in_room->vnum/100 == 223) ||
	  (ch->in_room->vnum/100 == 220) ||
	  (ch->in_room->vnum/100 == 221) ||
	  (ch->in_room->vnum/100 == 208) ||
	  (ch->in_room->vnum/100 == 270) ||
	  (ch->in_room->vnum/100 == 579) ||
	  (ch->in_room->vnum/100 == 408) ||
	  (ch->in_room->vnum/100 == 209) ||
	  (ch->in_room->vnum/100 == 222) ||
	  (ch->in_room->vnum/100 == 4) ||
	  (ch->in_room->vnum/100 == 78) ))
	{
		ch->pcdata->research = ch->in_room->vnum;
		send_to_char("You have memorized this location to recall to.\n\r",ch);
		return;
	}	
	if ( (ch->side ==3) &&
	( (ch->in_room->vnum/100 == 453) || 
	  (ch->in_room->vnum/100 == 454) || 
	  (ch->in_room->vnum/100 == 455) || 
	  (ch->in_room->vnum/100 == 450) ||  
	  (ch->in_room->vnum/100 == 452) ||
	  (ch->in_room->vnum/100 == 451) ||
	  (ch->in_room->vnum/100 == 260) ||
	  (ch->in_room->vnum/100 == 457) ||
	  (ch->in_room->vnum/100 == 458) ||
	  (ch->in_room->vnum/100 == 459) ||
	  (ch->in_room->vnum/100 == 65) ||
	  (ch->in_room->vnum/100 == 301) ||
	  (ch->in_room->vnum/100 == 25) ||
	  (ch->in_room->vnum/100 == 23) ||
	  (ch->in_room->vnum/100 == 204) ||
	  (ch->in_room->vnum/100 == 300) ||
	  (ch->in_room->vnum/100 == 400) ||
	  (ch->in_room->vnum/100 == 402) ||
	  (ch->in_room->vnum/100 == 507) ||
	  (ch->in_room->vnum/100 == 508) ||
	  (ch->in_room->vnum/100 == 509) ))
	{
		ch->pcdata->research = ch->in_room->vnum;
		send_to_char("You have memorized this location to recall to.\n\r",ch);
		return;
	}	
	if ( (ch->side == 4) &&
	( (ch->in_room->vnum/100 == 281) || 
	  (ch->in_room->vnum/100 == 282) || 
	  (ch->in_room->vnum/100 == 283) || 
	  (ch->in_room->vnum/100 == 136) ||  
	  (ch->in_room->vnum/100 == 137) ||
	  (ch->in_room->vnum/100 == 138) ||
	  (ch->in_room->vnum/100 == 139) ||
	  (ch->in_room->vnum/100 == 135) ||
	  (ch->in_room->vnum/100 == 134) ||
	  (ch->in_room->vnum/100 == 133) ||
	  (ch->in_room->vnum/100 == 132) ||
	  (ch->in_room->vnum/100 == 131) ||
	  (ch->in_room->vnum/100 == 130) ||
	  (ch->in_room->vnum/100 == 189) ||
	  (ch->in_room->vnum/100 == 180) ||
	  (ch->in_room->vnum/100 == 188) ||
	  (ch->in_room->vnum/100 == 187) ||
	  (ch->in_room->vnum/100 == 186) ||
	  (ch->in_room->vnum/100 == 185) ||
	  (ch->in_room->vnum/100 == 181) ||
	  (ch->in_room->vnum/100 == 182) ||
	  (ch->in_room->vnum/100 == 183) ||
	  (ch->in_room->vnum/100 == 184) ))
	{
		ch->pcdata->research = ch->in_room->vnum;
		send_to_char("You have memorized this location to recall to.\n\r",ch);
		return;
	}
	send_to_char("This location is not inside your kingdom boundaries.\n\r",ch);
	return;    
}

void do_precall(CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *och, *och_next;
    ROOM_INDEX_DATA *location;
	extern bool saltrecall;

    if (IS_NPC(ch))
	return;

if (!IS_IMMORTAL(ch))
{
    if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALLING))
	{
	send_to_char("Something about this room prevents you from remembering where you are from!\n\r", ch);
	return;
    }
}
    if (IS_NPC(ch) && !IS_SET(ch->act, ACT_PET) && !ch->mount) 
	{
		send_to_char("Only players can recall.\n\r", ch);
		return;
    }
if (!IS_IMMORTAL(ch))
{
    if (ch->fighting != NULL && !(armageddon)) 
	{
		send_to_char("You can't recall in the middle of combat.\n\r", ch);
		return;
    }
}
if (!IS_IMMORTAL(ch))
{
    if (ch->level > 20 && ch->move <= 100 && !(armageddon)) 
	{
		send_to_char("You don't have enough moves to recall. You need 100.\n\r", ch);
		return;
    }
}
    act("$n prays for transportation!", ch, 0, 0, TO_ROOM);

    if ((location = get_room_index(ch->pcdata->research)) == NULL ||
        ch->pcdata->research == 0) 
	{
		send_to_char("You are completely lost.\n\r", ch);
		return;
    }


//quest code
	if(saltrecall)
		location = get_room_index(kingdom_table[ch->side].salt_recall);

    if (ch->in_room == location)
	return;
if (!IS_IMMORTAL(ch))
{
    if (IS_AFFECTED(ch, AFF_CURSE) && !(armageddon)) 
	{
	send_to_char("You cannot recall, you must be CURSED!\n\r", ch);
	return;
    }
}

if (!IS_IMMORTAL(ch))
{
    if (IS_AFFECTED(ch, AFF_NOQUIT) && !(armageddon)) 
	{
	send_to_char("You cannot recall until some more time passes.\n\r", ch);
	return;
    }
}


    for (och = ch->in_room->people; och != NULL; och = och_next) 
	{
		och_next = och->next_in_room;

		if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch) 
		{
		    stop_follower(och);
		}
    }

    ch->move -= number_fuzzy(50);

    act("$n disappears.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_recall, TRUE, 4);
    char_from_room(ch);
    char_to_room(ch, location);

    act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
    send_to_char("The gods say,'Recalling is a MIGHTY way to return home.'\n\r", ch);
    do_look(ch, "auto");

    if (ch->pet != NULL)
	do_recall(ch->pet, "");

    /*
       if (ch->mount != NULL)
       do_recall( ch->mount, "" );
     */
     
    return;
}

void do_recall(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim, *och, *och_next;
    ROOM_INDEX_DATA *location;
	extern bool saltrecall;

    if (IS_NPC(ch))
	return;

if (!IS_IMMORTAL(ch))
{
    if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALLING))
	{
	send_to_char("Something about this room prevents you from remembering where you are from!\n\r", ch);
	return;
    }
}
    if (IS_NPC(ch) && !IS_SET(ch->act, ACT_PET) && !ch->mount) 
	{
		send_to_char("Only players can recall.\n\r", ch);
		return;
    }
if (!IS_IMMORTAL(ch))
{
    if (ch->fighting != NULL && !(armageddon)) 
	{
		send_to_char("You can't recall in the middle of combat.\n\r", ch);
		return;
    }
}
if (!IS_IMMORTAL(ch))
{
    if (ch->level > 20 && ch->move <= 100 && !(armageddon)) 
	{
		send_to_char("You don't have enough moves to recall. You need 100.\n\r", ch);
		return;
    }
}
    act("$n prays for transportation!", ch, 0, 0, TO_ROOM);

    if ((location = get_room_index(kingdom_table[ch->side].recall_room_num)) == NULL) 
	{
		send_to_char("You are completely lost.\n\r", ch);
		return;
    }


//quest code
	if(saltrecall)
		location = get_room_index(kingdom_table[ch->side].salt_recall);

    if (ch->in_room == location)
	return;
if (!IS_IMMORTAL(ch))
{
    if (IS_AFFECTED(ch, AFF_CURSE) && !(armageddon)) 
	{
	send_to_char("You cannot recall, you must be CURSED!\n\r", ch);
	return;
    }
}

if (!IS_IMMORTAL(ch))
{
    if (IS_AFFECTED(ch, AFF_NOQUIT) && !(armageddon)) 
	{
	send_to_char("You cannot recall until some more time passes.\n\r", ch);
	return;
    }
}

    if ((victim = ch->fighting) != NULL && !(armageddon)) 
	{
		int lose, skill;
		
		skill = get_skill(ch, gsn_recall);

		if (number_percent() < 80 * skill / 100) 
		{
		    check_improve(ch, gsn_recall, FALSE, 6);
			if (!IS_IMMORTAL(ch))
			{		    WAIT_STATE(ch, 4);			}
		    sprintf(buf, "You failed!.\n\r");
		    send_to_char(buf, ch);
		    return;
		}
		lose = (ch->desc != NULL) ? 25 : 50;
		gain_exp(ch, 0 - lose);
		check_improve(ch, gsn_recall, TRUE, 4);
		sprintf(buf, "You recall from combat!  You lose %d exps.\n\r", lose);
		send_to_char(buf, ch);
		stop_fighting(ch, TRUE);

	}
    for (och = ch->in_room->people; och != NULL; och = och_next) 
	{
		och_next = och->next_in_room;

		if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch) 
		{
		    stop_follower(och);
		}
    }

    ch->move /= 4;

    act("$n disappears.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_recall, TRUE, 4);
    char_from_room(ch);
    char_to_room(ch, location);

    act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
    send_to_char("The gods say,'Recalling is a MIGHTY lame way to return home.'\n\r", ch);
    do_look(ch, "auto");

    if (ch->pet != NULL)
	do_recall(ch->pet, "");

    /*
       if (ch->mount != NULL)
       do_recall( ch->mount, "" );
     */
    return;
}

void do_train(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *mob;
    sh_int stat = -1;
    char *pOutput = NULL;
    int cost;

    if (IS_NPC(ch))
	return;

    /*
     * Check for trainer.
     */
    for (mob = ch->in_room->people; mob; mob = mob->next_in_room) {
	if (IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN))
	    break;
    }

    if (mob == NULL) {
	send_to_char("You can't do that here.\n\r", ch);
	return;
    }
    if (argument[0] == '\0') {
	sprintf(buf, "You have %d training sessions.\n\r", ch->train);
	send_to_char(buf, ch);
	argument = "foo";
    }
    cost = 1;

    if (!str_cmp(argument, "str")) {
	if (class_table[ch->class].attr_prime == STAT_STR)
	    cost = 1;
	stat = STAT_STR;
	pOutput = "strength";
    } else if (!str_cmp(argument, "int")) {
	if (class_table[ch->class].attr_prime == STAT_INT)
	    cost = 1;
	stat = STAT_INT;
	pOutput = "intelligence";
    } else if (!str_cmp(argument, "wis")) {
	if (class_table[ch->class].attr_prime == STAT_WIS)
	    cost = 1;
	stat = STAT_WIS;
	pOutput = "wisdom";
    } else if (!str_cmp(argument, "dex")) {
	if (class_table[ch->class].attr_prime == STAT_DEX)
	    cost = 1;
	stat = STAT_DEX;
	pOutput = "dexterity";
    } else if (!str_cmp(argument, "con")) {
	if (class_table[ch->class].attr_prime == STAT_CON)
	    cost = 1;
	stat = STAT_CON;
	pOutput = "constitution";
    } else if (!str_cmp(argument, "hp"))
	cost = 1;

    else if (!str_cmp(argument, "mana"))
	cost = 1;

    else {
	strcpy(buf, "You can train:");
	if (ch->perm_stat[STAT_STR] < get_max_train(ch, STAT_STR))
	    strcat(buf, " str");
	if (ch->perm_stat[STAT_INT] < get_max_train(ch, STAT_INT))
	    strcat(buf, " int");
	if (ch->perm_stat[STAT_WIS] < get_max_train(ch, STAT_WIS))
	    strcat(buf, " wis");
	if (ch->perm_stat[STAT_DEX] < get_max_train(ch, STAT_DEX))
	    strcat(buf, " dex");
	if (ch->perm_stat[STAT_CON] < get_max_train(ch, STAT_CON))
	    strcat(buf, " con");
	strcat(buf, " hp mana");

	if (buf[strlen(buf) - 1] != ':') {
	    strcat(buf, ".\n\r");
	    send_to_char(buf, ch);
	} else {
	    /*
	     * This message dedicated to Jordan ... you big stud!
	     */
	    act("You have nothing left to train, you $T!",
		ch, NULL,
		ch->sex == SEX_MALE ? "big stud" :
		ch->sex == SEX_FEMALE ? "hot babe" :
		"wild thing",
		TO_CHAR);
	}

	return;
    }

    if (!str_cmp("hp", argument)) {
	if (cost > ch->train) {
	    send_to_char("You don't have enough training sessions.\n\r", ch);
	    return;
	}
	ch->train -= cost;
	ch->pcdata->perm_hit += 10;
	ch->max_hit += 10;
	ch->hit += 10;
	ch->spentOnHps += 10;
	act("Your durability increases!", ch, NULL, NULL, TO_CHAR);
	act("$n's durability increases!", ch, NULL, NULL, TO_ROOM);
	return;
    }
    if (!str_cmp("mana", argument)) {
	if (cost > ch->train) {
	    send_to_char("You don't have enough training sessions.\n\r", ch);
	    return;
	}
	ch->train -= cost;
	ch->pcdata->perm_mana += 10;
	ch->max_mana += 10;
	ch->mana += 10;
	ch->spentOnMana += 10;
	act("Your power increases!", ch, NULL, NULL, TO_CHAR);
	act("$n's power increases!", ch, NULL, NULL, TO_ROOM);
	return;
    }
    if (ch->perm_stat[stat] >= get_max_train(ch, stat)) {
	act("Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR);
	return;
    }
    if (cost > ch->train) {
	send_to_char("You don't have enough training sessions.\n\r", ch);
	return;
    }
    ch->train -= cost;

    ch->perm_stat[stat] += 1;
    act("Your $T increases!", ch, NULL, pOutput, TO_CHAR);
    act("$n's $T increases!", ch, NULL, pOutput, TO_ROOM);
    return;
}

void trap_trigger(CHAR_DATA * ch, ROOM_INDEX_DATA * tRoom)
{
    int chance, level, dam;
    AFFECT_DATA af;
    AFFECT_DATA *paf;
    DESCRIPTOR_DATA *d;

    if (IS_NPC(ch))
	return;

    if (tRoom->trap == NULL)
	return;
	
   if (IS_IMMORTAL(ch) || (ch->side == tRoom->trap->kingdom && get_skill(ch, gsn_detect_trap) >= 86)) {
	send_to_char("{cThere is a", ch);
	switch (tRoom->trap->type) {
	case 0:
	    send_to_char(" fire", ch);
	    break;
	case 1:
	    send_to_char(" net", ch);
	    break;
	case 2:
	    send_to_char(" stun", ch);
	    break;
	case 3:
	    send_to_char(" blind", ch);
	    break;
   case 4:
	    send_to_char(" snare", ch);
		break; //<-- that break wasn't there for some reason... so a trap would look funky potentially - Akuji
	case 5:
	    send_to_char("n alarm", ch);
	    break;
	}
	send_to_char(" trap here, placed by ", ch);
	send_to_char(kingdom_table[tRoom->trap->kingdom].name, ch);
	send_to_char(".{x\n\r", ch);
	return;
    }
    if (ch->side == tRoom->trap->kingdom)
	return;

    chance = get_skill(ch, gsn_detect_trap);
    chance = chance * 2/3;
    chance += (ch->level - tRoom->trap->level) / 2;

    level = tRoom->trap->level;

    if (number_percent() < chance) {
	send_to_char("You have disarmed a trap!\n\r", ch);
	act("$n disarmed a trap.", ch, NULL, NULL, TO_ROOM);
	tRoom->trap->is_set = FALSE;
	check_improve(ch, gsn_detect_trap, TRUE, 1);
	return;
    }
    switch (tRoom->trap->type) {
    case 0:
	dam = number_range(level * 3, level * 6);
	damage_old(ch, ch, dam, gsn_trap, DAM_FIRE, FALSE);
	send_to_char("You have been caught in a massive {rexplosion{x!\n\r", ch);
	act("{rBOOM!!{x $n sets off a trap.", ch, NULL, NULL, TO_ROOM);
	for (d = descriptor_list; d != NULL; d = d->next) {
	    if (d->connected == CON_PLAYING &&
		d->character != ch &&
		d->character->in_room != NULL &&
		d->character->in_room->area == ch->in_room->area &&
		d->character->in_room != ch->in_room) {
		send_to_char("You see a flash of light off in the distance.\n\r", d->character);
	    }
	}
	break;
    case 1:
	af.where = TO_AFFECTS;
	af.type = gsn_trap;
	af.level = level;
	af.duration = 4;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_WEB;

	affect_to_char(ch, &af);
	send_to_char("A sticky net covers your body!!\n\r", ch);
	act("$n gets trapped in a NET!", ch, NULL, NULL, TO_ROOM);
	break;
    case 2:
	af.where = TO_AFFECTS;
	af.type = gsn_stun;
	af.level = level;
	af.duration = 1;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_SLEEP;

	affect_to_char(ch, &af);
	if (IS_AWAKE(ch))
	    ch->position = POS_SLEEPING;
	send_to_char("An insidious trap knocks you out!\n\r", ch);
	act("$n springs a trap and gets knocked out.", ch, NULL, NULL, TO_ROOM);
	break;
    case 3:
	af.where = TO_AFFECTS;
	af.type = gsn_blindness;
	af.level = level;
	af.location = APPLY_HITROLL;
	af.modifier = -4;
	af.duration = level / 8;
	af.bitvector = AFF_BLIND;

	affect_to_char(ch, &af);
	send_to_char("You have been blinded by a trap!\n\r", ch);
	act("$n is blinded by a trap.", ch, NULL, NULL, TO_ROOM);
	break;
    case 4:
	af.where = TO_AFFECTS;
	af.type = gsn_trap;
	af.level = level;
	af.duration = 2;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = AFF_WEB;
	
   case 5:
	ch->move /= 2;
if (!IS_IMMORTAL(ch))
{
	WAIT_STATE(ch, 5);
}
	if (MOUNTED(ch)) {
	    ch->hit -= 10;
	    ch->mount->riding = FALSE;
	    ch->riding = FALSE;
	    send_to_char("You've been thrown from your mount!\n\r", ch);
	}
	ch->position = POS_RESTING;
	send_to_char("You've fallen, and you can't get up! A trap snags your leg.\n\r", ch);
	act("$n is knocked down by a trap.", ch, NULL, NULL, TO_ROOM);
	break;
    }
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
    tRoom->trap->is_set = FALSE;
    return;
}
