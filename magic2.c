
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
#include "interp.h"

DECLARE_DO_FUN(do_scan);

extern char *target_name;

void spell_farsight(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *gch;

    send_to_char("You can momentarily see in all directions..\n\r", ch);

    for (d = descriptor_list; d; d = d->next) {
	if ((d->connected == CON_PLAYING)
	    && (victim = d->character) != NULL
	    && (!IS_NPC(victim)                    )
	    && victim->in_room != NULL
	    && !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE)
	    && !(IS_SET(victim->in_room->affected_by, AFF_FS_PROOF))
	    && (is_room_owner(ch, victim->in_room)
		|| !room_is_private(victim->in_room))
	    && victim->in_room->area == ch->in_room->area
	    && can_see(ch, victim)
	    ) {




	    if (GET_SIDE(victim) == GET_SIDE(ch)) 
	{
		sprintf(buf, "{W%-28s %s\n\r{x",
			victim->name, victim->in_room->name);
//		send_to_char(buf, ch);

for (gch = char_list; gch != NULL; gch = gch->next) {
	if (is_same_group(gch, ch)) {
		send_to_char(buf, gch);}}



	    } else 
	{
		PERS(victim, ch, pers_buf);
		sprintf(buf2, "%s", pers_buf);

		sprintf(buf, "{R%-28s %s\n\r{x",
			buf2, victim->in_room->name);
//		send_to_char(buf, ch);

    for (gch = char_list; gch != NULL; gch = gch->next) {
	if (is_same_group(gch, ch)) {
		send_to_char(buf, gch);

	}
    }


	    }
	}
    }
}

void spell_eagle_eye(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char pers_buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *gch;


    send_to_char("You can momentarily see in all directions..\n\r", ch);

    for (d = descriptor_list; d; d = d->next) {
	if ((d->connected == CON_PLAYING)
	    && (victim = d->character) != NULL
	    && !IS_NPC(victim)
	    && victim->in_room != NULL
	    && !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE)
	    && !(victim->in_room->sector_type == SECT_INSIDE)
	    && !(IS_SET(victim->in_room->affected_by, AFF_FS_PROOF))
	    && (is_room_owner(ch, victim->in_room)
		|| !room_is_private(victim->in_room))
	    && victim->in_room->area == ch->in_room->area
	    && can_see(ch, victim)
	    ) {

	    if (GET_SIDE(victim) == GET_SIDE(ch)) {
		sprintf(buf, "{W%-28s %s\n\r{x",
			victim->name, victim->in_room->name);
//		send_to_char(buf, ch);

for (gch = char_list; gch != NULL; gch = gch->next) {
	if (is_same_group(gch, ch)) {
		send_to_char(buf, gch); }}

	    } else {
		PERS(victim, ch, pers_buf);
		sprintf(buf2, "%s", pers_buf);

		sprintf(buf, "{W%-28s %s\n\r{x",
			buf2, victim->in_room->name);
//		send_to_char(buf, ch);

for (gch = char_list; gch != NULL; gch = gch->next) {
	if (is_same_group(gch, ch)) {
		send_to_char(buf, gch);



	    }    

	}
    }




	}
    }
}

void spell_portal(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;

    if ((victim = get_char_world(ch, target_name)) == NULL
	|| victim == ch
	|| victim->in_room == NULL
	|| !can_see_room(ch, victim->in_room)
	|| IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	|| IS_AFFECTED(victim, AFF_NOQUIT)
	|| IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	|| IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	|| victim->level >= level + 3
	|| (!IS_NPC(victim) && victim->level >= LEVEL_HERO)	/* NOT trust */
	|| (!IS_NPC(victim) && victim->side != ch->side)
	|| (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||(IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	|| (IS_NPC(victim) && saves_spell(level, victim, DAM_NONE,0))) {
	send_to_char("You failed.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_NOQUIT)) {
	send_to_char("You can't do that now!", ch);
	return;
    }
    if (IS_NPC(victim)) {
	send_to_char("You can only portal to people in your kingdom.\n\r", ch);
	return;
    }
    stone = get_eq_char(ch, WEAR_HOLD);
    /*   if (!IS_IMMORTAL(ch) 
       &&  (stone == NULL || stone->item_type != ITEM_WARP_STONE))
       {
       send_to_char("You lack the proper component for this spell.\n\r",ch);
       return;
       }

       if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
       {
       act("You draw upon the power of $p.",ch,stone,NULL,TO_CHAR);
       act("It flares brightly and vanishes!",ch,stone,NULL,TO_CHAR);
       extract_obj(stone);
       }
     */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
    portal->timer = 2 + level / 25;
    portal->value[3] = victim->in_room->vnum;

    obj_to_room(portal, ch->in_room);

    act("$p rises up from the ground.", ch, portal, NULL, TO_ROOM);
    act("$p rises up before you.", ch, portal, NULL, TO_CHAR);
}

void spell_nexus(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;
    ROOM_INDEX_DATA *to_room, *from_room;

    from_room = ch->in_room;

    if ((victim = get_char_world(ch, target_name)) == NULL
	|| victim == ch
	|| (to_room = victim->in_room) == NULL
	|| !can_see_room(ch, to_room) || !can_see_room(ch, from_room)
	|| IS_SET(to_room->room_flags, ROOM_SAFE)
	|| IS_SET(from_room->room_flags, ROOM_SAFE)
	|| IS_AFFECTED(victim, AFF_NOQUIT)
	|| IS_SET(to_room->room_flags, ROOM_PRIVATE)
	|| IS_SET(to_room->room_flags, ROOM_SOLITARY)
	|| IS_SET(to_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(to_room->room_flags, ROOM_NEWBIES_ONLY)
	|| IS_SET(from_room->room_flags, ROOM_NO_RECALL)
	|| victim->level >= level + 3
	|| (!IS_NPC(victim) && victim->level >= LEVEL_HERO)	/* NOT trust */
	|| (!IS_NPC(victim) && victim->side != ch->side)
	|| (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOSUMMON))
	||(IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
	|| (IS_NPC(victim) && saves_spell(level, victim, DAM_NONE,0))
	|| (is_clan(victim) && !is_same_clan(ch, victim))
	|| (!IS_NPC(victim) && (GET_SIDE(victim) != GET_SIDE(ch)))
	) {
	send_to_char("You failed.\n\r", ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_NOQUIT)) {
	send_to_char("You can't do that now!", ch);
	return;
    }
    if (IS_NPC(victim)) {
	send_to_char("You can only nexus to players in your kingdom.\n\r", ch);
	return;
    }
    stone = get_eq_char(ch, WEAR_HOLD);
    if (!IS_IMMORTAL(ch)
	&& (stone == NULL || stone->item_type != ITEM_WARP_STONE)) {
	send_to_char("You lack the proper component for this spell.\n\r", ch);
	return;
    }
    if (stone != NULL && stone->item_type == ITEM_WARP_STONE) {
	act("You draw upon the power of $p.", ch, stone, NULL, TO_CHAR);
	act("It flares brightly and vanishes!", ch, stone, NULL, TO_CHAR);
	extract_obj(stone);
    }
    /* portal one */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
    portal->timer = 1 + level / 10;
    portal->value[3] = to_room->vnum;

    obj_to_room(portal, from_room);

    act("$p rises up from the ground.", ch, portal, NULL, TO_ROOM);
    act("$p rises up before you.", ch, portal, NULL, TO_CHAR);

    /* no second portal if rooms are the same */
    if (to_room == from_room)
	return;

    /* portal two */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
    portal->timer = 1 + level / 10;
    portal->value[3] = from_room->vnum;

    obj_to_room(portal, to_room);

    if (to_room->people != NULL) {
	act("$p rises up from the ground.", to_room->people, portal, NULL, TO_ROOM);
	act("$p rises up from the ground.", to_room->people, portal, NULL, TO_CHAR);
    }
}

void spell_agony(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
CHAR_DATA *vch = (CHAR_DATA *) vo;
int curse_num, plague_num, poison_num, slow_num,
         weaken_num, blind_num, faerie_fire_num;

    plague_num = skill_lookup("plague");
    curse_num = skill_lookup("curse");
    poison_num = skill_lookup("poison");
    weaken_num = skill_lookup("weaken");
    blind_num = skill_lookup("blindness");
slow_num = skill_lookup("slow");
faerie_fire_num = skill_lookup("faerie fire");

    act("$n cackles wildly!", ch, NULL, NULL, TO_ROOM);
    send_to_char("You cackle and unleash agony upon your foe.\n\r", ch);

	spell_curse(curse_num, level, ch, (void *) vch, TARGET_CHAR);
	spell_plague(plague_num, level, ch, (void *) vch, TARGET_CHAR);
	spell_poison(poison_num, level, ch, (void *) vch, TARGET_CHAR);
	spell_weaken(weaken_num, level, ch, (void *) vch, TARGET_CHAR);
	spell_slow(slow_num, level, ch, (void *) vch, TARGET_CHAR);
	spell_blindness(blind_num, level, ch, (void *) vch, TARGET_CHAR);
	spell_faerie_fire(faerie_fire_num, level, ch, (void *) vch, TARGET_CHAR);

    send_to_char("You feel drained.\n\r", ch);

 return;
}

void spell_aaaa_song(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
	OBJ_DATA *song;

	//    act("$n has created a beautiful red rose.", ch, rose, NULL, TO_ROOM);
    send_to_char("aaaa.\n\r", ch);
    song = create_object(get_obj_index(OBJ_VNUM_SONG), 0);

    obj_to_room(song, ch->in_room);   //this is a hack
	do_play(ch, "sweet");
    obj_to_char(song, ch);

	song->timer = 2;
    return;
	

}
	
void spell_bbbb_song(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    send_to_char("bbbb.\n\r", ch);
}
	
void spell_cccc_song(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    send_to_char("cccc.\n\r", ch);

}
	
void spell_dddd_song(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    send_to_char("dddd.\n\r", ch);

}
	
	
	
void spell_call_animal(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{

    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *mob;
    bool isAnimal;
    int count = 0;

    send_to_char("You call upon the powers of nature.\n\r", ch);
    act("$n calls upon the powers of nature.\n\r", ch, NULL, NULL, TO_ROOM);

    if(victim->level + 9 < ch->level)  // no help is needed, no message
	return;

    for (mob = char_list; mob != NULL; mob = mob->next) 
    {
    isAnimal = FALSE;

    if (count > 20) break;

    if(!IS_NPC(mob))
	continue;
    if(victim->in_room->area != mob->in_room->area)
	continue;
    if( mob->side != SIDE_MOB && mob->side != ch->side)
	continue;
    if( mob->level > ch->level - 2)
	continue;
    if( IS_SET(mob->imm_flags, IMM_CHARM))
	continue;

    if (IS_SET(mob->form, FORM_ANIMAL)) 
	isAnimal = TRUE;
    else if (IS_SET(mob->form, FORM_INSECT)) 
	isAnimal = TRUE;
    else if (IS_SET(mob->form, FORM_SPIDER)) 
	isAnimal = TRUE;
    else if (IS_SET(mob->form, FORM_MAMMAL)) 
//	    if (!IS_SET(mob->form, FORM_SENTIENT)) 
		{isAnimal = TRUE;}
    else if (IS_SET(mob->form, FORM_BIRD)) 
	isAnimal = TRUE;
    else if (IS_SET(mob->form, FORM_REPTILE)) 
	isAnimal = TRUE;
    else if (IS_SET(mob->form, FORM_SNAKE)) 
	isAnimal = TRUE;
    else if (IS_SET(mob->form, FORM_FISH)) 
	isAnimal = TRUE;
    else if( mob->race == race_lookup("bat"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("bear"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("cat"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("centipede"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("dog"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("fido"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("fox"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("lizard"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("pig"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("rabbit"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("snake"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("song bird"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("water fowl"))
	isAnimal = TRUE;
    else if( mob->race == race_lookup("wolf"))
	isAnimal = TRUE;
 
    if(isAnimal)
    {
	count++;
	if(!IS_SET(mob->act, ACT_HUNT))
		SET_BIT(mob->act, ACT_HUNT);
	start_hating(mob, victim);
	mob->side = ch->side;
	mob->hunting = victim;
    }
    }//end search world



 return;
}

void spell_animate(int sn, int level, CHAR_DATA * ch, void *vo, int target)
/*
Been foolin around with this spell a lot, gettin it to work right
		-Serek 7/2/01
*/
{
    OBJ_DATA *obj;
    CHAR_DATA *mob;
    CHAR_DATA *och;
    int num, i;
    int five=0;
	five = five;
    
    obj = get_obj_here(ch, target_name);

    if (obj == NULL) {
	send_to_char("Animate what?\n\r", ch);
	return;
    }
    if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type!=ITEM_CORPSE_PC) {
	    send_to_char("It would serve no purpose...\n\r", ch);
	return;
    }
    if (obj->level > (ch->level + 6)) {
	send_to_char("You could not reach a soul of its power.\n\r", ch);
	return;
    }
    num = 1;

    for (och = char_list; och != NULL; och = och->next) {

	if (IS_NPC(och))
	    if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch
		&& och->pIndexData->vnum == MOB_VNUM_ZOMBIE)
		num++;

    }
    if (num > (ch->level / 20)) {
	send_to_char("You can't have that many.\n\r", ch);
	return;
    }
    /* Chew on the zombie a little bit, recalculate level-dependant stats */

    mob = create_mobile(get_mob_index(MOB_VNUM_ZOMBIE));
    if (ch->level >= obj->level) {
	mob->level = obj->level * .825;
    } else {
	mob->level = ch->level *.825;
    }
    mob->max_hit = mob->level * 16 + number_range(
						     mob->level * mob->level * 2,
						     mob->level * mob->level * 4);

if(mob->max_hit < 1) mob->max_hit = 1;
if(mob->max_hit > 5000)
	{
	five = mob->max_hit - 5000;
	if(five > 1000)
	{
		five = five / 12 + 23;
		return;
	}	
	if(five > 900)
	{	
		five = five / 11;
		return;
	}
	if(five > 800)
	{	
		five = five / 7;
		return;
	}
	if(five > 500)
	{	
		five = five / 5;
		return;	
	}
	}
    if(mob->max_hit > 5000)
		mob->max_hit = 5000;

	mob->max_hit = mob->max_hit + five;
    mob->hit = mob->max_hit;
    mob->max_mana = 100 + dice(mob->level, 10);
    mob->mana = mob->max_mana;
    for (i = 0; i < 3; i++)
	mob->armor[i] = interpolate(mob->level * 2, 100, -100);
    mob->armor[3] = interpolate(mob->level * 2, 100, 0);
    mob->damage[DICE_NUMBER] = mob->level / 8;
    mob->damage[DICE_TYPE] = mob->level / 8;
    mob->damage[DICE_BONUS] = mob->level + interpolate(mob->level * 2, 100, 0);
    mob->hitroll = mob->level;
    for (i = 0; i < MAX_STATS; i++)
	mob->perm_stat[i] = 11 + mob->level / 4;

    char_to_room(mob, ch->in_room);
    act("$p forms into a creature of the undead!", ch, obj, NULL, TO_ROOM);
    act("$p forms into a creature of the undead!", ch, obj, NULL, TO_CHAR);

    extract_obj(obj);

    SET_BIT(mob->affected_by, AFF_CHARM);
    add_follower(mob, ch);
    mob->leader = ch;
    mob->master = ch; 
   do_say(mob, "Death is only the beginning?");
    return;
   
}




void spell_blink(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *rch;

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room) {

	if (rch->fighting != NULL)
	    stop_fighting(rch, TRUE);

	if (IS_NPC(rch) && IS_SET(rch->act, ACT_AGGRESSIVE))
         
	    REMOVE_BIT(rch->act, ACT_AGGRESSIVE);

	    stop_hating(rch);
        
    }
    send_to_char("WHOAH! You blink!\n\r", ch);
    act("You blink!\n\r", ch, NULL, NULL, TO_ROOM);
    return;
}

void spell_bubble(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    AFFECT_DATA af;
    OBJ_DATA *spring;
    OBJ_DATA *bed;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 3;
    af.bitvector = AFF_FS_PROOF;
    affect_to_room(ch->in_room, &af);

/* Wee, air bed */

    bed = create_object(get_obj_index(OBJ_VNUM_AIRBED), 0);
    bed->timer = 4;
    obj_to_room(bed, ch->in_room);

/* Purdy Spring */

    spring = create_object(get_obj_index(OBJ_VNUM_SPRING), 0);
    spring->timer = 4;
    obj_to_room(spring, ch->in_room);
    act("$n has created a temporal bubble!", ch, spring, NULL, TO_ROOM);
    act("You have created a temporal bubble!", ch, spring, NULL, TO_CHAR);
    return;

}

void spell_hold(int sn, int level, CHAR_DATA * ch, void *vo, int target)
 {
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;


  if( victim->fighting != NULL )
  {
   send_to_char("They are moving to fast to hold!\n\r", ch);
   return;
  }
    if (IS_AFFECTED(victim, AFF_HOLD) || saves_spell(level, victim, DAM_OTHER, SAVE_NORMAL))
	return;

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.duration = 0;
  af.bitvector = AFF_HOLD;

  affect_to_char(victim, &af);
  act("You stop dead in your tracks, a victim of time!", ch, NULL, victim, TO_VICT);
  act("$N stops dead in his tracks, a victim of time!", ch, NULL, victim, TO_CHAR);
  act("$N stops dead in his tracks, a victim of time!", ch, NULL, victim, TO_NOTVICT);
  return;
}

void spell_lapse(int sn, int level, CHAR_DATA * ch, void *vo, int target)
 {
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

    if (IS_AFFECTED2(victim, AFF_LAPSE) || saves_spell(level, victim, DAM_OTHER, SAVE_OFTEN))
	return;

  af.where = TO_AFFECTS2;
  af.type = sn;
  af.level = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.duration = level / 5;
  af.bitvector = AFF_LAPSE;
  affect_to_char(victim, &af);

  act("You feel time slipping away.", ch, NULL, victim, TO_VICT);
  act("$N slips into the time stream.", ch, NULL, victim, TO_CHAR);
  act("$N slips into the time stream.", ch, NULL, victim, TO_ROOM);

  return;
}

void spell_suspend(int sn, int level, CHAR_DATA * ch, void *vo, int target)
 {
  AFFECT_DATA af;


  af.where = TO_AFFECTS2;
  af.type = sn;
  af.level = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.duration = 1;
  af.bitvector = AFF_SUSPEND;
  affect_to_char(ch, &af);
  act("$n goes in to suspended animation.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You fall into suspended animation.\n\r", ch);
  ch->position = POS_SLEEPING;
  update_pos(ch);
  return;
}

void spell_temp_blade(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
 OBJ_DATA *tempb;
 if(get_eq_char(ch, WEAR_WIELD) != NULL)
  {
   send_to_char("Your can't be wielding something already!\n\r", ch);
   return;
  }
 tempb = create_object(get_obj_index(OBJ_VNUM_TEMPB), 0);
 tempb->timer = level / 2;
 obj_to_char(tempb, ch);
 equip_char(ch, tempb, WEAR_WIELD);
 act("$n has created a mystical blade.", ch, NULL, NULL, TO_ROOM);
 act("You create a blade of displacement.", ch, NULL, NULL, TO_CHAR);
 return;
}

void spell_timestop(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
 CHAR_DATA *ppl;
 AFFECT_DATA af;

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.duration = 0;
  af.bitvector = AFF_HOLD;
  affect_to_char(ch, &af);
  act("Everything around you stops dead in their tracks!", ch, NULL, NULL, TO_ROOM);
  send_to_char("Everything around you stops dead in their tracks INCLUDING YOU!\n\r", ch);

  for(ppl = ch->in_room->people; ppl != NULL; ppl = ppl->next_in_room)
    {
	if (ppl->fighting != NULL)
	    stop_fighting(ppl, TRUE);

	if (IS_NPC(ppl) && IS_SET(ppl->act, ACT_AGGRESSIVE))
	    REMOVE_BIT(ppl->act, ACT_AGGRESSIVE);

	    stop_hating(ppl);
          if (ch != ppl)
          affect_to_char(ppl, &af);
    }
  return;
}

void spell_rift(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *victim;
    bool gate_pet;

    if ((victim = get_char_worldf(ch, target_name)) == NULL
	|| victim == ch
	|| victim->in_room == NULL
	|| !can_see_room(ch, victim->in_room)
	|| IS_AFFECTED(victim, AFF_NOQUIT)
	|| IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	|| IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	|| IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(ch->in_room->room_flags, ROOM_NEWBIES_ONLY)
	|| victim->level >= level + 5
/*	|| (IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON)) */
	) {
	send_to_char("You failed.\n\r", ch);
	return;
    }

    if (!IS_NPC(victim)) {
	send_to_char("You failed.\n\r", ch);
	return;
    }

    if (victim->in_room->area != ch->in_room->area)
     {
      send_to_char("You failed.\n\r", ch);
      return;
     }

    if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
	gate_pet = TRUE;
    else
	gate_pet = FALSE;

    act("$n opens a rift in space and steps through.", ch, NULL, NULL, TO_ROOM);
    send_to_char("You open a rift in space and step through.\n\r", ch);
    char_from_room(ch);
    char_to_room(ch, victim->in_room);

    act("$n has arrived through a rift.", ch, NULL, NULL, TO_ROOM);
    do_look(ch, "auto");

    if (gate_pet) {
	act("$n steps through a rift.", ch->pet, NULL, NULL, TO_ROOM);
	send_to_char("You step through a rift and vanish.\n\r", ch->pet);
	char_from_room(ch->pet);
	char_to_room(ch->pet, victim->in_room);
	act("$n has arrived through a rift.", ch->pet, NULL, NULL, TO_ROOM);
	do_look(ch->pet, "auto");
    }
    if (MOUNTED(ch)) {
	act("$n steps through a rift.", MOUNTED(ch), NULL, NULL, TO_ROOM);
	send_to_char("You step through a rift and vanish.\n\r", MOUNTED(ch));
	char_from_room(MOUNTED(ch));
	char_to_room(MOUNTED(ch), victim->in_room);
	act("$n has arrived through a rift.", MOUNTED(ch), NULL, NULL, TO_ROOM);
	do_look(MOUNTED(ch), "auto");
    }
if (victim->in_room->area == ch->in_room->area)
	send_to_char("{RYou feel an odd distortion in the time space continuum.{x\r\n", victim);
}

void spell_summon_fgolem( int sn, int level, CHAR_DATA *ch, void *vo, int target ) 
{
  CHAR_DATA *gch;
  CHAR_DATA *golem;
  AFFECT_DATA af;
  int i = 0;

  if (is_affected(ch,sn))
    {
      send_to_char("You lack the power to summon another golem right now.\n\r",
		   ch);
      return;
    }

  send_to_char("You attempt to summon a fire golem.\n\r",ch);
  act("$n attempts to summon a fire golem.",ch,NULL,NULL,TO_ROOM);

  for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (IS_NPC(gch) && IS_AFFECTED(gch,AFF_CHARM) && gch->master == ch &&
          ( gch->pIndexData->vnum == MOB_VNUM_IGOLEM && MOB_VNUM_FGOLEM 
             && MOB_VNUM_ICEGOLEM && MOB_VNUM_LGOLEM && MOB_VNUM_AGOLEM ) )
	{
	    send_to_char("More golems are more than you can control!\n\r",ch);
	    return;
	}
    }

  golem = create_mobile( get_mob_index(MOB_VNUM_FGOLEM) );


  for (i = 0; i < MAX_STATS; i ++)
       golem->perm_stat[i] = UMIN(25,15 + ch->level/10);
            
  golem->perm_stat[STAT_STR] += 3;
  golem->perm_stat[STAT_INT] -= 1;
  golem->perm_stat[STAT_CON] += 2;

  golem->max_hit = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000)
		: UMIN( (10 * ch->pcdata->perm_hit) + 4000, 30000);
  golem->hit = golem->max_hit;
  golem->max_mana = IS_NPC(ch)? ch->max_mana : ch->pcdata->perm_mana;
  golem->mana = golem->max_mana;
  golem->level = ch->level;
  for (i=0; i < 3; i++)
    golem->armor[i] = interpolate(golem->level,100,-100);
  golem->armor[3] = interpolate(golem->level,100,0);
  golem->gold = 0;
  golem->timer = 0;
  golem->damage[DICE_NUMBER] = 13;   
  golem->damage[DICE_TYPE] = 9;
  golem->damage[DICE_BONUS] = ch->level / 2 + 10;

  char_to_room(golem,ch->in_room);
  send_to_char("You summoned a {Rf{rir{Re{x golem!\n\r",ch);
  act("$n summons a {Rf{rir{Re{x golem!",ch,NULL,NULL,TO_ROOM);

  af.where		= TO_AFFECTS;
  af.type               = sn;
  af.level              = level; 
  af.duration           = 30;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);  

  SET_BIT(golem->affected_by, AFF_CHARM);
  golem->master = golem->leader = ch;

}

void spell_summon_lgolem( int sn, int level, CHAR_DATA *ch, void *vo, int target ) 
{
  CHAR_DATA *gch;
  CHAR_DATA *golem;
  AFFECT_DATA af;
  int i = 0;

  if (is_affected(ch,sn))
    {
      send_to_char("You lack the power to summon another golem right now.\n\r",
		   ch);
      return;
    }

  send_to_char("You attempt to summon a lightning golem.\n\r",ch);
  act("$n attempts to summon a lightning golem.",ch,NULL,NULL,TO_ROOM);

  for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (IS_NPC(gch) && IS_AFFECTED(gch,AFF_CHARM) && gch->master == ch &&
          ( gch->pIndexData->vnum == MOB_VNUM_IGOLEM && MOB_VNUM_FGOLEM 
             && MOB_VNUM_ICEGOLEM && MOB_VNUM_LGOLEM && MOB_VNUM_AGOLEM ) )
	{
	    send_to_char("More golems are more than you can control!\n\r",ch);
	    return;
	}
    }

  golem = create_mobile( get_mob_index(MOB_VNUM_LGOLEM) );


  for (i = 0; i < MAX_STATS; i ++)
       golem->perm_stat[i] = UMIN(25,15 + ch->level/10);
            
  golem->perm_stat[STAT_STR] += 3;
  golem->perm_stat[STAT_INT] -= 1;
  golem->perm_stat[STAT_CON] += 2;

  golem->max_hit = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000)
		: UMIN( (10 * ch->pcdata->perm_hit) + 4000, 30000);
  golem->hit = golem->max_hit;
  golem->max_mana = IS_NPC(ch)? ch->max_mana : ch->pcdata->perm_mana;
  golem->mana = golem->max_mana;
  golem->level = ch->level;
  for (i=0; i < 3; i++)
    golem->armor[i] = interpolate(golem->level,100,-100);
  golem->armor[3] = interpolate(golem->level,100,0);
  golem->gold = 0;
  golem->timer = 0;
  golem->damage[DICE_NUMBER] = 13;   
  golem->damage[DICE_TYPE] = 9;
  golem->damage[DICE_BONUS] = ch->level / 2 + 10;

  char_to_room(golem,ch->in_room);
  send_to_char("You summoned a {Ylightning{x golem!\n\r",ch);
  act("$n summons a {Ylightning{x golem!",ch,NULL,NULL,TO_ROOM);

  af.where		= TO_AFFECTS;
  af.type               = sn;
  af.level              = level; 
  af.duration           = 30;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);  

  SET_BIT(golem->affected_by, AFF_CHARM);
  golem->master = golem->leader = ch;

}

void spell_summon_igolem( int sn, int level, CHAR_DATA *ch, void *vo, int target ) 
{
  CHAR_DATA *gch;
  CHAR_DATA *golem;
  AFFECT_DATA af;
  int i = 0;

  if (is_affected(ch,sn))
    {
      send_to_char("You lack the power to summon another golem right now.\n\r",
		   ch);
      return;
    }

  send_to_char("You attempt to summon an ice golem.\n\r",ch);
  act("$n attempts to summon an ice golem.",ch,NULL,NULL,TO_ROOM);

  for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (IS_NPC(gch) && IS_AFFECTED(gch,AFF_CHARM) && gch->master == ch &&
          ( gch->pIndexData->vnum == MOB_VNUM_IGOLEM && MOB_VNUM_FGOLEM 
             && MOB_VNUM_ICEGOLEM && MOB_VNUM_LGOLEM && MOB_VNUM_AGOLEM ) )
	{
	    send_to_char("More golems are more than you can control!\n\r",ch);
	    return;
	}
    }

  golem = create_mobile( get_mob_index(MOB_VNUM_ICEGOLEM) );


  for (i = 0; i < MAX_STATS; i ++)
       golem->perm_stat[i] = UMIN(25,15 + ch->level/10);
            
  golem->perm_stat[STAT_STR] += 3;
  golem->perm_stat[STAT_INT] -= 1;
  golem->perm_stat[STAT_CON] += 2;

  golem->max_hit = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000)
		: UMIN( (10 * ch->pcdata->perm_hit) + 4000, 30000);
  golem->hit = golem->max_hit;
  golem->max_mana = IS_NPC(ch)? ch->max_mana : ch->pcdata->perm_mana;
  golem->mana = golem->max_mana;
  golem->level = ch->level;
  for (i=0; i < 3; i++)
    golem->armor[i] = interpolate(golem->level,100,-100);
  golem->armor[3] = interpolate(golem->level,100,0);
  golem->gold = 0;
  golem->timer = 0;
  golem->damage[DICE_NUMBER] = 13;   
  golem->damage[DICE_TYPE] = 9;
  golem->damage[DICE_BONUS] = ch->level / 2 + 10;

  char_to_room(golem,ch->in_room);
  send_to_char("You summoned an {Bi{cc{be{x golem!\n\r",ch);
  act("$n summons an {Bi{cc{be{x golem!",ch,NULL,NULL,TO_ROOM);

  af.where		= TO_AFFECTS;
  af.type               = sn;
  af.level              = level; 
  af.duration           = 30;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);  

  SET_BIT(golem->affected_by, AFF_CHARM);
  golem->master = golem->leader = ch;

}

void spell_summon_Igolem( int sn, int level, CHAR_DATA *ch, void *vo, int target ) 
{
  CHAR_DATA *gch;
  CHAR_DATA *golem;
  AFFECT_DATA af;
  int i = 0;

  if (is_affected(ch,sn))
    {
      send_to_char("You lack the power to summon another golem right now.\n\r",
		   ch);
      return;
    }

  send_to_char("You attempt to summon an iron golem.\n\r",ch);
  act("$n attempts to summon an iron golem.",ch,NULL,NULL,TO_ROOM);

  for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (IS_NPC(gch) && IS_AFFECTED(gch,AFF_CHARM) && gch->master == ch &&
          ( gch->pIndexData->vnum == MOB_VNUM_IGOLEM && MOB_VNUM_FGOLEM 
             && MOB_VNUM_ICEGOLEM && MOB_VNUM_LGOLEM && MOB_VNUM_AGOLEM) )
	{
	    send_to_char("More golems are more than you can control!\n\r",ch);
	    return;
	}
    }

  golem = create_mobile( get_mob_index(MOB_VNUM_IGOLEM) );


  for (i = 0; i < MAX_STATS; i ++)
       golem->perm_stat[i] = UMIN(25,15 + ch->level/10);
            
  golem->perm_stat[STAT_STR] += 5;
  golem->perm_stat[STAT_INT] -= 5;
  golem->perm_stat[STAT_CON] += 4;

  golem->max_hit = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000)
		: UMIN( (10 * ch->pcdata->perm_hit) + 4000, 30000);
  golem->hit = golem->max_hit;
  golem->max_mana = IS_NPC(ch)? ch->max_mana : ch->pcdata->perm_mana;
  golem->mana = golem->max_mana;
  golem->level = ch->level;
  for (i=0; i < 3; i++)
    golem->armor[i] = interpolate(golem->level,100,-100);
  golem->armor[3] = interpolate(golem->level,100,0);
  golem->gold = 0;
  golem->timer = 0;
  golem->damage[DICE_NUMBER] = 13;   
  golem->damage[DICE_TYPE] = 9;
  golem->damage[DICE_BONUS] = ch->level / 2 + 10;

  char_to_room(golem,ch->in_room);
  send_to_char("You summoned an {Diron{x golem!\n\r",ch);
  act("$n summons an {Diron{x golem!",ch,NULL,NULL,TO_ROOM);

  af.where		= TO_AFFECTS;
  af.type               = sn;
  af.level              = level; 
  af.duration           = 30;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);  

  SET_BIT(golem->affected_by, AFF_CHARM);
  golem->master = golem->leader = ch;

}

void spell_summon_agolem( int sn, int level, CHAR_DATA *ch, void *vo, int target ) 
{
  CHAR_DATA *gch;
  CHAR_DATA *golem;
  AFFECT_DATA af;
  int i = 0;

  if (is_affected(ch,sn))
    {
      send_to_char("You lack the power to summon another golem right now.\n\r",
		   ch);
      return;
    }

  send_to_char("You attempt to summon an acid golem.\n\r",ch);
  act("$n attempts to summon an acid golem.",ch,NULL,NULL,TO_ROOM);

  for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (IS_NPC(gch) && IS_AFFECTED(gch,AFF_CHARM) && gch->master == ch &&
          ( gch->pIndexData->vnum == MOB_VNUM_IGOLEM && MOB_VNUM_FGOLEM 
             && MOB_VNUM_ICEGOLEM && MOB_VNUM_LGOLEM && MOB_VNUM_AGOLEM ) )
	{
	    send_to_char("More golems are more than you can control!\n\r",ch);
	    return;
	}
    }

  golem = create_mobile( get_mob_index(MOB_VNUM_AGOLEM) );


  for (i = 0; i < MAX_STATS; i ++)
       golem->perm_stat[i] = UMIN(25,15 + ch->level/10);
            
  golem->perm_stat[STAT_STR] += 3;
  golem->perm_stat[STAT_INT] -= 1;
  golem->perm_stat[STAT_CON] += 2;

  golem->max_hit = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000)
		: UMIN( (10 * ch->pcdata->perm_hit) + 4000, 30000);
  golem->hit = golem->max_hit;
  golem->max_mana = IS_NPC(ch)? ch->max_mana : ch->pcdata->perm_mana;
  golem->mana = golem->max_mana;
  golem->level = ch->level;
  for (i=0; i < 3; i++)
    golem->armor[i] = interpolate(golem->level,100,-100);
  golem->armor[3] = interpolate(golem->level,100,0);
  golem->gold = 0;
  golem->timer = 0;
  golem->damage[DICE_NUMBER] = 13;   
  golem->damage[DICE_TYPE] = 9;
  golem->damage[DICE_BONUS] = ch->level / 2 + 10;

  char_to_room(golem,ch->in_room);
  send_to_char("You summoned an {macid{x golem!\n\r",ch);
  act("$n summons an {macid{x golem!",ch,NULL,NULL,TO_ROOM);

  af.where		= TO_AFFECTS;
  af.type               = sn;
  af.level              = level; 
  af.duration           = 30;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);  

  SET_BIT(golem->affected_by, AFF_CHARM);
  golem->master = golem->leader = ch;

}

void spell_draconian_blessing(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int bless_num, frenzy_num;

    bless_num = skill_lookup("bless");
    frenzy_num = skill_lookup("frenzy");
    act("$n's eyes flash a {ggreen{x tint.", ch, NULL, NULL, TO_ROOM);
    send_to_char("Your eyes glow {ggreen{x.\n\r", ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (ch->side == vch->side) {
	    send_to_char("You feel protected by the draconian spirit.\n\r", vch);
	    spell_frenzy(frenzy_num, level, ch, (void *) vch, TARGET_CHAR);
	    spell_bless(bless_num, level, ch, (void *) vch, TARGET_CHAR);
		    }
		}
    

    send_to_char("Your eyes return to their normal color.\n\r", ch);
    //ch->move = 0;
    //ch->hit /= 2;
}

void spell_draconian_mist(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int cure_num, refresh_num;

    cure_num = skill_lookup("cure light");
    refresh_num = skill_lookup("refresh");
    act("$n's eyes flash a {bblue{x tint.", ch, NULL, NULL, TO_ROOM);
    send_to_char("Your eyes glow {bblue{x.\n\r", ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (ch->side == vch->side) {
	    send_to_char("You feel protected by the draconian mist.\n\r", vch);
	    spell_cure_light(cure_num, level, ch, (void *) vch, TARGET_CHAR);
	    spell_refresh(refresh_num, level, ch, (void *) vch, TARGET_CHAR);
		    }
		}
    

    send_to_char("Your eyes return to their normal color.\n\r", ch);
    //ch->move = 0;
    //ch->hit /= 2;
}

void spell_draconian_skin(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    //int dam;
    int armor_num, shield_num;

    armor_num = skill_lookup("armor");
    shield_num = skill_lookup("shield");
    act("$n's eyes flash a {yyellow{x tint.", ch, NULL, NULL, TO_ROOM);
    send_to_char("Your eyes glow {yyellow{x.\n\r", ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next) {
	vch_next = vch->next_in_room;

	if (ch->side == vch->side) {
	    send_to_char("You feel protected by the draconian skin.\n\r", vch);
	    spell_armor(armor_num, level, ch, (void *) vch, TARGET_CHAR);
	    spell_shield(shield_num, level, ch, (void *) vch, TARGET_CHAR);
		    }
		}
    

    send_to_char("Your eyes return to their normal color.\n\r", ch);
    //ch->move = 0;
    //ch->hit /= 2;
}

void spell_prismatic_spray( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
  int limit;
  int i;
  int dam;
  dam = 0;
  limit  = number_range(1,5);

  act("A spray of {Rm{Ga{Bn{My {Cc{Yo{wl{wo{Rr{Gs{x flies from $n's hands!{x", ch, victim, victim, TO_ROOM);
  act("A spray of {Rm{Ga{Bn{My {Cc{Yo{wl{Bo{Rr{Gs{x flies from your hands!{x", ch, victim, victim, TO_CHAR);
  act("$n calls forth a spray of {Rm{Ga{Bn{My {Cc{Yo{wl{Bo{Rr{Gs{x against you!{x", ch, victim, victim, TO_VICT);

  for( i = 0; i < limit; i++)
  {
   switch (number_range(DAM_FIRE, DAM_LIGHT))
   {
     case DAM_FIRE:
     {
  act("{rA {Rred spray{r burns $N!{x", ch, victim, victim, TO_ROOM);
  act("{rA {Rred spray{r burns $N!{x", ch, victim, victim, TO_CHAR);
  act("{rA {Rred spray{r burns you!{x", ch, victim, victim, TO_VICT);
      fire_effect(victim,level/2,dam/4,TARGET_CHAR);

       dam = dice( level/2, 10 );
       if ( saves_spell( level, victim, DAM_FIRE, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_FIRE, TRUE);
           break;
      }
     case DAM_COLD:
     {
  act("{cAn {Cblue spray{c freezes $N!{x", ch, victim, victim, TO_ROOM);
  act("{cA {Cblue spray{c freezes $N!{x", ch, victim, victim, TO_CHAR);
  act("{cA {Cblue spray{c burns you!{x", ch, victim, victim, TO_VICT);
      cold_effect(victim,level/2,dam/4,TARGET_CHAR);
       dam = dice( level/2, 10 );
       if ( saves_spell( level, victim, DAM_COLD, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_COLD, TRUE);
           break;
      }
     case DAM_LIGHTNING:
     {
  act("{WAn {wwhite spray{W shocks $N!{x", ch, victim, victim, TO_ROOM);
  act("{WAn {wwhite spray{W shocks $N!{x", ch, victim, victim, TO_CHAR);
  act("{WAn {wwhite spray{W shocks you!{x", ch, victim, victim, TO_VICT);
      shock_effect(victim,level/2,dam/4,TARGET_CHAR);

       dam = dice( level/2, 10 );
       if ( saves_spell( level, victim, DAM_LIGHTNING, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE);
           break;
      }
     case DAM_ACID:
     {
  act("{gAn {Ggreen spray{g corrodes $N!{x", ch, victim, victim, TO_ROOM);
  act("{gAn {Ggreen spray{g corrodes $N!{x", ch, victim, victim, TO_CHAR);
  act("{gAn {Ggreen spray{g corrodes you!{x", ch, victim, victim, TO_VICT);
      acid_effect(victim,level/2,dam/4,TARGET_CHAR);
       dam = dice( level/2, 10 );
       if ( saves_spell( level, victim, DAM_ACID, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_ACID, TRUE);
           break;
      }
     case DAM_POISON:
     {
  act("{gAn {Gg{gr{Ge{ge{Gn {gs{Gp{gr{Ga{gy{g sickens $N!{x", ch, victim, victim, TO_ROOM);
  act("{gAn {Gg{gr{Ge{ge{Gn {gs{Gp{gr{Ga{gy{g sickens $N!{x", ch, victim, victim, TO_CHAR);
  act("{gAn {Gg{gr{Ge{ge{Gn {gs{Gp{gr{Ga{gy{g sickens you!{x", ch, victim, victim, TO_VICT);
      poison_effect(victim,level/2,dam/4,TARGET_CHAR);
       dam = dice( level/2, 10 );
       if ( saves_spell( level, victim, DAM_POISON, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_POISON, TRUE);
           break;
      }
     case DAM_NEGATIVE:
     {
  act("{WA {Ddark spray{W corrupts $N!{x", ch, victim, victim, TO_ROOM);
  act("{WA {Ddark spray{W corrupts $N!{x", ch, victim, victim, TO_CHAR);
  act("{WA {Ddark spray{W corrupts you!{x", ch, victim, victim, TO_VICT);
  if ( saves_spell( level, victim, DAM_NEGATIVE, SAVE_NORMAL ) )
      break;
    if ( victim->level <= 2 )
    {
	dam		 = ch->hit + 1;
    }
    else
    {
	victim->mana	/= 3/2;
	victim->move	/= 3/2;
	dam		 = dice(1, level);
    }

    send_to_char("You feel your life slipping away!\n\r",victim);
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
    break;
    }
     case DAM_HOLY:
     {
      int heal;
  act("{WAn {wwhite spray{W heals $N!{x", ch, victim, victim, TO_ROOM);
  act("{WAn {wwhite spray{W heals $N!{x", ch, victim, victim, TO_CHAR);
  act("{WAn {wgreen spray{W heals you!{x", ch, victim, victim, TO_VICT);
    heal = dice(1, 8) + level / 3;
    victim->hit = UMIN( victim->hit + heal, victim->max_hit );
    update_pos( victim );
    send_to_char( "You feel better!\n\r", victim );
     break;
      }
     case DAM_ENERGY:
     {
  act("{yAn {Yenergy flare{y blasts $N!{x", ch, victim, victim, TO_ROOM);
  act("{yAn {Yenergy flare{y blasts $N!{x", ch, victim, victim, TO_CHAR);
  act("{yAn {Yenergy flare{y blasts you!{x", ch, victim, victim, TO_VICT);
       dam = dice( level, 10 );
       if ( saves_spell( level, victim, DAM_ENERGY, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_ENERGY, TRUE);
           break;
      }
    case DAM_MENTAL:
     {
  act("{mAn {Mmental blast{m explodes in $N's mind!{x", ch, victim, victim, TO_ROOM);
  act("{mAn {Mmental blast{m explodes in $N's mind!{x", ch, victim, victim, TO_CHAR);
  act("{mAn {Mmental blast{m explodes in your mind!{x", ch, victim, victim, TO_VICT);
       dam = dice( level/2, 20 );
       if ( saves_spell( level, victim, DAM_MENTAL, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_MENTAL, TRUE);
           break;
      }
    case DAM_DISEASE:
     {
  act("{gAn {Ggas cloud{g sickens $N!{x", ch, victim, victim, TO_ROOM);
  act("{gAn {Ggas cloud{g sickens $N!{x", ch, victim, victim, TO_CHAR);
  act("{gAn {Ggas cloud{g sickens you!{x", ch, victim, victim, TO_VICT);
    if (saves_spell(level - 2,victim,DAM_DISEASE, SAVE_NORMAL) ||
        (IS_NPC(victim) && IS_SET(victim->act,ACT_UNDEAD)))
    {
	if (ch == victim)
	  send_to_char("You feel momentarily ill, but it passes.\n\r",ch);
	else
	  act("$N seems to be unaffected.",ch,NULL,victim,TO_CHAR);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type 	  = skill_lookup("plague");
    af.level	  = level / 2;
    af.duration  = level/10;
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = AFF_PLAGUE;
    affect_join(victim,&af);

    send_to_char
      ("You scream in agony as plague sores erupt from your skin.\n\r",victim);
    act("$n screams in agony as plague sores erupt from $s skin.",
	victim,NULL,NULL,TO_ROOM);
    break;
      }
     case DAM_DROWNING:
     {
  act("{bAn {Bblue spray{b drenches $N!{x", ch, victim, victim, TO_ROOM);
  act("{bAn {Bblue spray{b drenches $N!{x", ch, victim, victim, TO_CHAR);
  act("{bAn {Bblue spray{b drenches you!{x", ch, victim, victim, TO_VICT);
       dam = dice( level/2, 10 );
       if ( saves_spell( level, victim, DAM_DROWNING, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_DROWNING, TRUE);
           break;
      }
     case DAM_LIGHT:
     {
  act("{yAn {Ylight flare{y blinds $N!{x", ch, victim, victim, TO_ROOM);
  act("{yAn {Ylight flare{y blinds $N!{x", ch, victim, victim, TO_CHAR);
  act("{yAn {Ylight flare{y blinds you!{x", ch, victim, victim, TO_VICT);
       dam = dice( level/2, 10 );
    if ( IS_AFFECTED(victim, AFF_BLIND) || saves_spell(level,victim,DAM_LIGHT, SAVE_NORMAL))
	break;
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup("blind");
      af.level     = level;
      af.location  = APPLY_HITROLL;
      af.modifier  = -4;
      af.duration  = 5;
      af.bitvector = AFF_BLIND;
      affect_to_char( victim, &af );
      send_to_char( "You are blinded!\n\r", victim );
      act("$n appears to be blinded.",victim,NULL,NULL,TO_ROOM);
      break;
       if ( saves_spell( level, victim, DAM_LIGHT, SAVE_NORMAL ) )
	      dam /= 2;
           damage( ch, victim, dam, sn, DAM_LIGHT, TRUE);
           break;
      }
   }
  }
  return;
}

void spell_zeal_blade(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
 OBJ_DATA *zealb;
 OBJ_DATA *wielded;

wielded = get_eq_char(ch, WEAR_WIELD);

	if (wielded == NULL)
{
	goto CREATE_Z_BLADE;

} else if (wielded->timer > 1)
{
	goto REPLACE_Z_BLADE;
} else {
	goto ERR_HAVE_OTHER;
} 

ERR_HAVE_OTHER:
   send_to_char("You can't be wielding something already!\n\r", ch);
   return;

REPLACE_Z_BLADE:
 obj_from_char(wielded);
 extract_obj(wielded);
 zealb = create_object(get_obj_index(OBJ_VNUM_ZEALB), 0);
 zealb->timer = level / 2;
 obj_to_char(zealb, ch);
 equip_char(ch, zealb, WEAR_WIELD);
 SET_BIT(zealb->extra_flags, ITEM_GLOW);
 act("$n's blade flashes.", ch, NULL, NULL, TO_ROOM);
 act("You replace your spell's edge.", ch, NULL, NULL, TO_CHAR);
 return;

CREATE_Z_BLADE:
 zealb = create_object(get_obj_index(OBJ_VNUM_ZEALB), 0);
 zealb->timer = level / 2;
 obj_to_char(zealb, ch);
 equip_char(ch, zealb, WEAR_WIELD);
 SET_BIT(zealb->extra_flags, ITEM_GLOW);
 act("$n has created a glowing blade.", ch, NULL, NULL, TO_ROOM);
 act("You create a magical edge.", ch, NULL, NULL, TO_CHAR);
 return;
}

void spell_warlock_blade(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
 OBJ_DATA *warlb;
 OBJ_DATA *warldual;
 if(get_eq_char(ch, WEAR_WIELD) == NULL)
 	{ 
 warlb = create_object(get_obj_index(OBJ_VNUM_WARLB), 0);
 warlb->timer = level / 2;
 obj_to_char(warlb, ch);
 equip_char(ch, warlb, WEAR_WIELD);
 act("$n has formed a small dagger out of magic!", ch, NULL, NULL, TO_ROOM);
 act("You summon a magical dagger.", ch, NULL, NULL, TO_CHAR);
	}
	
if(get_eq_char(ch, WEAR_SECONDARY) == NULL
&& get_eq_char(ch, WEAR_SHIELD)== NULL)
 	{ 
 warldual = create_object(get_obj_index(OBJ_VNUM_WARLDUAL), 0);
 warldual->timer = level / 2;
 obj_to_char(warldual, ch);
 equip_char(ch, warldual, WEAR_SECONDARY);
 act("$n summons a magical whip!", ch, NULL, NULL, TO_ROOM);
 act("You concentrate and form a magical whip!", ch, NULL, NULL, TO_CHAR);
	}
	
	if (get_eq_char(ch, WEAR_SHIELD) != NULL)
	send_to_char("You can't wear a shield AND a dual weapon at the same time!\n\r",ch);
return;
}

/*Zyloch Attempt to Manipulate a New Spell Blade, God Forbid It Works*/
/*
void spell_drag_blade(int sn, int level, CHAR_DATA * ch, void *vo, int target)
{
 OBJ_DATA *dragb;
 OBJ_DATA *wielded;

wielded = get_eq_char(ch, WEAR_WIELD);

	if (wielded == NULL)
{
	goto CREATE_D_BLADE;

} else if (wielded->timer > 1)
{
	goto REPLACE_D_BLADE;
} else {
	goto ERR_HAVE_OTHER;
} 

ERR_HAVE_OTHER:
   send_to_char("You can't be wielding something already!\n\r", ch);
   return;

REPLACE_D_BLADE:
 obj_from_char(wielded);
 extract_obj(wielded);
 dragb = create_object(get_obj_index(OBJ_VNUM_DRAGB), 0);
 dragb->timer = level / 2;
 obj_to_char(dragb, ch);
 equip_char(ch, dragb, WEAR_WIELD);
 SET_BIT(dragb->extra_flags, ITEM_GLOW);
 act("$n's blade flashes.", ch, NULL, NULL, TO_ROOM);
 act("You replace your Draconian Lance.", ch, NULL, NULL, TO_CHAR);
 return;

CREATE_D_BLADE:
 dragb = create_object(get_obj_index(OBJ_VNUM_DRAGB), 0);
 dragb->timer = level / 2;
 obj_to_char(dragb, ch);
 equip_char(ch, dragb, WEAR_WIELD);
 SET_BIT(dragb->extra_flags, ITEM_GLOW);
 act("$n has created a shining lance.", ch, NULL, NULL, TO_ROOM);
 act("You create a shining lance.", ch, NULL, NULL, TO_CHAR);
 return;
}
*/
