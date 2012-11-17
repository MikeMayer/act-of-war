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
#include <string.h>
#include <time.h>
#include "merc.h"
#include "music.h"

/* command procedures needed */
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_stand);

/*
 * Local functions.
 */
int hit_gain args((CHAR_DATA * ch));
int mana_gain args((CHAR_DATA * ch));
int move_gain args((CHAR_DATA * ch));
void mobile_update args((void));
void weather_update args((void));
void char_update args((void));
void kingdom_update args((void));
void obj_update args((void));
void aggr_update args((void));
void quest_update args((void));	/* Vassago - quest.c */

extern int do_advice args((char *arg));
extern void room_aff_update args((ROOM_INDEX_DATA * room));

/* used for saving */
int save_number = 0;
int advice_number = 0;

void check_move(CHAR_DATA * ch)
{
    int loc, mod;
    OBJ_DATA *obj;
    AFFECT_DATA *af;

    if (IS_NPC(ch))
	return;

    if (IS_IMMORTAL(ch))
	return;

    if (ch == NULL)
	return;

/* Lets remove the move modifiers first. */

    for (loc = 0; loc < MAX_WEAR; loc++) {
	obj = get_eq_char(ch, loc);
	if (obj == NULL)
	    continue;
	if (!obj->enchanted)
	    for (af = obj->pIndexData->affected; af != NULL; af = af->next) {
		mod = af->modifier;
		switch (af->location) {
		case APPLY_MOVE:
		    ch->max_move -= mod;
		    break;
		}
	    }
	for (af = obj->affected; af != NULL; af = af->next) {
	    mod = af->modifier;
	    switch (af->location) {
	    case APPLY_MOVE:
		ch->max_move -= mod;
		break;
	    }
	}
    }

/* Check the moves */

    if (ch->pcdata->perm_move > class_table[ch->class].maxmove) {
	ch->pcdata->perm_move = class_table[ch->class].maxmove;
    }
    if (ch->max_move > class_table[ch->class].maxmove) {
	ch->max_move = class_table[ch->class].maxmove;
    }
    if (ch->move > class_table[ch->class].maxmove)
	ch->move = class_table[ch->class].maxmove;

/* Lets put the move modifiers back. */

    for (loc = 0; loc < MAX_WEAR; loc++) {
	obj = get_eq_char(ch, loc);
	if (obj == NULL)
	    continue;
	if (!obj->enchanted)
	    for (af = obj->pIndexData->affected; af != NULL; af = af->next) {
		mod = af->modifier;
		switch (af->location) {
		case APPLY_MOVE:
		    ch->max_move += mod;
		    break;
		}
	    }
	for (af = obj->affected; af != NULL; af = af->next) {
	    mod = af->modifier;
	    switch (af->location) {
	    case APPLY_MOVE:
		ch->max_move += mod;
		break;
	    }
	}
    }
}

/*
 * Advancement stuff.
 */
void advance_level(CHAR_DATA * ch, bool hide)
{
    char buf[MAX_STRING_LENGTH];
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;
    int add_raises;

    add_hp = con_app[ch->perm_stat[STAT_STR]].hitp +
	con_app[ch->perm_stat[STAT_CON]].hitp +
	number_range(class_table[ch->class].hp_min, class_table[ch->class].hp_max);

    add_mana = con_app[ch->perm_stat[STAT_INT]].hitp +
	con_app[ch->perm_stat[STAT_WIS]].hitp +
	number_range(class_table[ch->class].mana_min, class_table[ch->class].mana_max);

    add_prac = wis_app[get_curr_stat(ch, STAT_WIS)].practice;

    add_move = number_range(get_curr_stat(ch, STAT_DEX) / 6,
			    get_curr_stat(ch, STAT_DEX) / 2);

    add_move = UMAX(1, add_move);
    add_mana = UMAX(1, add_mana);

    add_raises = prime_app[ch->perm_stat[class_table[ch->class].attr_prime]].raise_bonus;
    
    if (!strcmp(class_table[ch->class].name, "warrior"))
    add_hp += number_range(15,20);


if (!pc_race_table[ch->race].isRemort && ch->level >= LEGEND_LEVEL ) 
	add_raises = 0;
else
    ch->train += 1;
	
    ch->max_hit += add_hp;
    ch->max_mana += add_mana;
    ch->max_move += add_move;
    ch->practice += add_prac;
    ch->levelpoints += add_raises;

    ch->pcdata->perm_hit += add_hp;
    ch->pcdata->perm_mana += add_mana;
    ch->pcdata->perm_move += add_move;

    check_move(ch);

    if (!hide) {
	sprintf(buf,
		"You gain %d hit point%s, %d mana, %d move%s, %d practice%s, and %d raise point%s.\n\r",
		add_hp, add_hp == 1 ? "" : "s",
		add_mana,
		add_move, add_move == 1 ? "" : "s",
		add_prac, add_prac == 1 ? "" : "s",
		add_raises, add_raises == 1 ? "" : "s");
	send_to_char(buf, ch);
    }
    return;
}

void gain_exp(CHAR_DATA * ch, int gain)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch) || ch->level >= LEVEL_HERO)
	return;

ch->exp = UMAX(exp_per_level(ch, ch->pcdata->points), ch->exp + gain);

    while (ch->level < LEVEL_HERO && ch->exp >=
	   exp_per_level(ch, ch->pcdata->points) * (ch->level + 1)) {
	send_to_char("{YYou raise a level!!{x  ", ch);
	ch->level += 1;
	//set_prename(ch);
	sprintf(buf, "%s gained level %d", ch->name, ch->level);
	log_string(buf);
	sprintf(buf, "$N has attained level %d!", ch->level);
	wiznet(buf, ch, NULL, WIZ_LEVELS, 0, 0);
	advance_level(ch, FALSE);
	save_char_obj(ch);
    }

    return;
}

/*
 * Regeneration stuff.
 */
int hit_gain(CHAR_DATA * ch)
{
    int gain;
    int number;

    if (ch->in_room == NULL)
	return 0;

    if (IS_NPC(ch)) {
	gain = 5 + ch->level;
	if (IS_AFFECTED(ch, AFF_REGENERATION))
	    gain *= 2;

	switch (ch->position) {
	default:
	    gain /= 2;
	    break;
	case POS_SLEEPING:
	    gain = 3 * gain / 2;
	    break;
	case POS_RESTING:
	    break;
	case POS_FIGHTING:
	    gain /= 3;
	    break;
	}

    } else {
	gain = UMAX(3, get_curr_stat(ch, STAT_CON) - 3 + ch->level / 2);
	gain += class_table[ch->class].hp_max - 10;
	number = number_percent();
	if (number < get_skill(ch, gsn_fast_healing)) {
	    gain += number * gain / 100;
	    if (ch->hit < ch->max_hit)
		check_improve(ch, gsn_fast_healing, TRUE, 8);
	}
	switch (ch->position) {
	default:
	    gain /= 4;
	    break;
	case POS_SLEEPING:
	    break;
	case POS_RESTING:
	    gain /= 2;
	    break;
	case POS_FIGHTING:
	    gain /= 6;
	    break;
	}

	//if (ch->pcdata->condition[COND_HUNGER] == 0)
	//    gain /= 2;

	//if (ch->pcdata->condition[COND_THIRST] == 0)
	//    gain /= 2;

    }

    gain = gain * ch->in_room->heal_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[3] / 100;

    if(IS_AFFECTED2(ch, AFF_SUSPEND))
      gain *= 10;

    if (IS_AFFECTED(ch, AFF_POISON))
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
	gain /= 8;

    if (IS_AFFECTED(ch, AFF_HASTE) || IS_AFFECTED(ch, AFF_SLOW))
	gain /= 2;

    return UMIN(gain, ch->max_hit - ch->hit);
}

int mana_gain(CHAR_DATA * ch)
{
    int gain;
    int number;

    if (ch->in_room == NULL)
	return 0;

    if (IS_NPC(ch)) {
	gain = 5 + ch->level;
	switch (ch->position) {
	default:
	    gain /= 2;
	    break;
	case POS_SLEEPING:
	    gain = 3 * gain / 2;
	    break;
	case POS_RESTING:
	    break;
	case POS_FIGHTING:
	    gain /= 3;
	    break;
	}
    } else {
	gain = (get_curr_stat(ch, STAT_WIS)
		+ get_curr_stat(ch, STAT_INT) + ch->level) / 2;
	number = number_percent();
	if (number < get_skill(ch, gsn_meditation)) {
	    gain += number * gain / 100;
	    if (ch->mana < ch->max_mana)
		check_improve(ch, gsn_meditation, TRUE, 8);
	}
	if (!class_table[ch->class].fMana)
	    gain /= 2;

	switch (ch->position) {
	default:
	    gain /= 4;
	    break;
	case POS_SLEEPING:
	    break;
	case POS_RESTING:
	    gain /= 2;
	    break;
	case POS_FIGHTING:
	    gain /= 6;
	    break;
	}

//	if (ch->pcdata->condition[COND_HUNGER] == 0)
//	    gain /= 2;
//
//	if (ch->pcdata->condition[COND_THIRST] == 0)
//	    gain /= 2;

    }

    gain = gain * ch->in_room->mana_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[4] / 100;

    if(IS_AFFECTED2(ch, AFF_SUSPEND))
      gain *= 10;

    if (IS_AFFECTED(ch, AFF_POISON))
	gain /= 4;
   
    if (IS_AFFECTED(ch, AFF_PLAGUE))
	gain /= 8;

    if (IS_AFFECTED(ch, AFF_HASTE) || IS_AFFECTED(ch, AFF_SLOW))
	gain /= 2;

    return UMIN(gain, ch->max_mana - ch->mana);
}

int move_gain(CHAR_DATA * ch)
{
    int gain;

    if (ch->in_room == NULL)
	return 0;

    if (IS_NPC(ch)) {
	gain = ch->level;
    } else {
	gain = UMAX(15, ch->level);

	switch (ch->position) {
	case POS_SLEEPING:
	    gain += get_curr_stat(ch, STAT_DEX);
	    break;
	case POS_RESTING:
	    gain += get_curr_stat(ch, STAT_DEX) / 2;
	    break;
	}

//	if (ch->pcdata->condition[COND_HUNGER] == 0)
//	    gain /= 2;
//
//	if (ch->pcdata->condition[COND_THIRST] == 0)
//	    gain /= 2;
    }

    gain = gain * ch->in_room->heal_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
	gain = gain * ch->on->value[3] / 100;

    if (IS_AFFECTED(ch, AFF_POISON))
	gain /= 4;

    if (IS_AFFECTED(ch, AFF_PLAGUE))
	gain /= 8;

    if (IS_AFFECTED(ch, AFF_HASTE) || IS_AFFECTED(ch, AFF_SLOW))
	gain /= 2;

    return UMIN(gain, ch->max_move - ch->move);
}

void gain_condition(CHAR_DATA * ch, int iCond, int value)
{
    int condition;

    if (value == 0 || IS_NPC(ch) || ch->level >= LEVEL_IMMORTAL)
	return;

    condition = ch->pcdata->condition[iCond];
    if (condition == -1)
	return;
    ch->pcdata->condition[iCond] = URANGE(0, condition + value, 48);

    if (ch->pcdata->condition[iCond] == 0) {
	switch (iCond) {
//	case COND_HUNGER:
//	    send_to_char("You are hungry.\n\r", ch);
//	    break;
//
//	case COND_THIRST:
//	    send_to_char("You are thirsty.\n\r", ch);
//	    break;

	case COND_DRUNK:
	    if (condition != 0)
		send_to_char("You are sober.\n\r", ch);
	    break;
	}
    }
    return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update(void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for (ch = char_list; ch != NULL; ch = ch_next) {
	ch_next = ch->next;

	if (!IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch, AFF_CHARM))
	    continue;

	if (ch->in_room->area->empty && !IS_SET(ch->act, ACT_UPDATE_ALWAYS))
	    continue;

	if (ch->position != POS_STANDING && ch->fighting != NULL)
	    do_stand(ch, "");

	/* Examine call for special procedure */
	if (ch->spec_fun != 0) {
	    if ((*ch->spec_fun) (ch))
		continue;
	}
	if (ch->pIndexData->pShop != NULL)	/* give him some gold */
	    if ((ch->gold * 100 + ch->silver) < ch->pIndexData->wealth) {
		ch->gold += ch->pIndexData->wealth * number_range(1, 20) / 5000000;
		ch->silver += ch->pIndexData->wealth * number_range(1, 20) / 50000;
	    }
	/*
	 * Check triggers only if mobile still in default position
	 */
	if (ch->position == ch->pIndexData->default_pos) {
	    /* Delay */
	    if (HAS_TRIGGER(ch, TRIG_DELAY)
		&& ch->mprog_delay > 0) {
		if (--ch->mprog_delay <= 0) {
		    mp_percent_trigger(ch, NULL, NULL, NULL, TRIG_DELAY);
		    continue;
		}
	    }
	    if (HAS_TRIGGER(ch, TRIG_RANDOM)) {
		if (mp_percent_trigger(ch, NULL, NULL, NULL, TRIG_RANDOM))
		    continue;
	    }
	}
	/* That's all for sleeping / busy monster, and empty zones */
	if (ch->position != POS_STANDING)
	    continue;

	/* Scavenge */
	if (IS_SET(ch->act, ACT_SCAVENGER)
	    && ch->in_room->contents != NULL
	    && number_bits(6) == 0) {
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max = 1;
	    obj_best = 0;
	    for (obj = ch->in_room->contents; obj; obj = obj->next_content) {
		if (CAN_WEAR(obj, ITEM_TAKE) && can_loot(ch, obj)
		    && obj->cost > max && obj->cost > 0) {
		    obj_best = obj;
		    max = obj->cost;
		}
	    }

	    if (obj_best) {
		obj_from_room(obj_best);
		obj_to_char(obj_best, ch);
		act("$n gets $p.", ch, obj_best, NULL, TO_ROOM);
	    }
	} {
	    CHAR_DATA *rch;

   	    if (is_hating(ch, ch)) stop_hating(ch);      //  no mobs hating themselves

	    for (rch = ch->in_room->people; rch; rch = rch->next_in_room) {
		if (is_hating(ch, rch)) {
		    do_emote(ch, "realizes you are a threat and attacks.");
		    multi_hit(ch, rch, TYPE_UNDEFINED);
		    break;
		}
	    }
	}

	/* Wander */
	if (!IS_SET(ch->act, ACT_SENTINEL)
	    && number_bits(3) == 0
	    && (door = number_bits(5)) <= 5
	    && (pexit = ch->in_room->exit[door]) != NULL
	    && pexit->u1.to_room != NULL
	    && !IS_SET(pexit->exit_info, EX_CLOSED)
	    && !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)
	    && (!IS_SET(ch->act, ACT_STAY_AREA)
		|| pexit->u1.to_room->area == ch->in_room->area))
//      && ( !IS_SET(ch->act, ACT_OUTDOORS)
	    //      ||   !IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)) 
	    //      && ( !IS_SET(ch->act, ACT_INDOORS)
	    //      ||   IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)))
	{
	    move_char(ch, door, FALSE);
	}
    }

    return;
}

/*
 * Update the weather.
 */
void weather_update(void)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;

    buf[0] = '\0';

    switch (++time_info.hour) {
    case 5:
	if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	    weather_info.sunlight = SUN_SUMMER_RISE;
	    strcat(buf, "Summer night begins to fade away as the sun rises.\n\r");
	}
	break;

    case 6:
	if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
	    weather_info.sunlight = SUN_WINTER_RISE;
	    strcat(buf, "The winter night gives way as the sun peaks over the mountains.\n\r");
	} else if (time_info.month >= MARCH && time_info.month <= MAY) {
	    weather_info.sunlight = SUN_SPRING_RISE;
	    strcat(buf, "A spring day begins a new as the sun rises.\n\r");
	} else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	    weather_info.sunlight = SUN_SUMMER_LIGHT;
	    strcat(buf, "The hot summer day begins.\n\r");
	} else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
	    weather_info.sunlight = SUN_FALL_RISE;
	    strcat(buf, "A cold fall night begins to fade away as dawn approaches.\n\r");
	} else if (time_info.month == DECEMBER) {
	    weather_info.sunlight = SUN_WINTER_RISE;
	    strcat(buf, "The winter night gives way as the sun peaks over the mountains.\n\r");
	}
	break;

    case 7:
	if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
	    weather_info.sunlight = SUN_WINTER_LIGHT;
	    strcat(buf, "The sun peaks in the east bringing warmth to the frozen land.\n\r");
	} else if (time_info.month >= MARCH && time_info.month <= MAY) {
	    weather_info.sunlight = SUN_SPRING_LIGHT;
	    strcat(buf, "Snow begins to melt from the heat of the rising spring sun.\n\r");
	} else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	} else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
	    weather_info.sunlight = SUN_FALL_LIGHT;
	    strcat(buf, "The sky is filled with color as the fall sun rises.\n\r");
	} else if (time_info.month == DECEMBER) {
	    weather_info.sunlight = SUN_WINTER_LIGHT;
	    strcat(buf, "The sun peaks in the east bringing warmth to the frozen land.\n\r");
	}
	break;

    case 17:
	if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
	    weather_info.sunlight = SUN_WINTER_SET;
	    strcat(buf, "The warmth of the winter sun fades as it sets in the west.\n\r");
	} else if (time_info.month >= MARCH && time_info.month <= MAY) {
	} else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	} else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
	} else if (time_info.month == DECEMBER) {
	    weather_info.sunlight = SUN_WINTER_SET;
	    strcat(buf, "The warmth of the winter sun fades as it sets in the west.\n\r");
	}
	break;

    case 18:
	if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
	    weather_info.sunlight = SUN_WINTER_DARK;
	    strcat(buf, "A cold winter night has begun.\n\r");
	} else if (time_info.month >= MARCH && time_info.month <= MAY) {
	    weather_info.sunlight = SUN_SPRING_SET;
	    strcat(buf, "Long shadows form on the land as the spring sun begins to set.\n\r");
	} else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	} else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
	    weather_info.sunlight = SUN_FALL_SET;
	    strcat(buf, "The fall sun sets in the west.\n\r");
	} else if (time_info.month == DECEMBER) {
	    weather_info.sunlight = SUN_WINTER_DARK;
	    strcat(buf, "A cold winter night has begun.\n\r");
	}
	break;

    case 19:
	if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
	} else if (time_info.month >= MARCH && time_info.month <= MAY) {
	    weather_info.sunlight = SUN_SPRING_DARK;
	    strcat(buf, "The light fades as a spring night begins.\n\r");
	} else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	    weather_info.sunlight = SUN_SUMMER_SET;
	    strcat(buf, "The hot summer sun sets in the west.\n\r");
	} else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
	    weather_info.sunlight = SUN_FALL_DARK;
	    strcat(buf, "Fall darkness engulfs the land.\n\r");
	} else if (time_info.month == DECEMBER) {
	}
	break;
    case 20:
	if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
	} else if (time_info.month >= MARCH && time_info.month <= MAY) {
	} else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
	    weather_info.sunlight = SUN_SUMMER_DARK;
	    strcat(buf, "Stars appear as a warm summer night begins.\n\r");
	} else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
	} else if (time_info.month == DECEMBER) {
	}
	break;
    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;
    }

    if (time_info.day >= 35) {
	time_info.day = 0;
	time_info.month++;
    }
    if (time_info.month >= 12) {
	time_info.month = 0;
	time_info.year++;
    }
    /*
     * Weather change.
     */
    if (time_info.month >= 9 && time_info.month <= 12)
	diff = weather_info.mmhg > 985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change = UMAX(weather_info.change, -12);
    weather_info.change = UMIN(weather_info.change, 12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg = UMAX(weather_info.mmhg, 960);
    weather_info.mmhg = UMIN(weather_info.mmhg, 1040);

    switch (weather_info.sky) {
    default:
	bug("Weather_update: bad sky %d.", weather_info.sky);
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if (weather_info.mmhg < 990
	    || (weather_info.mmhg < 1010 && number_bits(2) == 0)) {
	    if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "Clouds begin to form in the winter sky.\n\r");
	    } else if (time_info.month >= MARCH && time_info.month <= MAY) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "Billowing clouds have started to take shape.\n\r");
	    } else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "Dark clouds roll in from the east.\n\r");
	    } else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "Leaves fall from the trees as clouds move overhead.\n\r");
	    } else if (time_info.month == DECEMBER) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "Clouds begin to form in the winter sky.\n\r");
	    }
	}
	break;

    case SKY_CLOUDY:
	if (weather_info.mmhg < 970
	    || (weather_info.mmhg < 990 && number_bits(2) == 0)) {
	    if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "Snow begins to silently fall from the sky.\n\r");
	    } else if (time_info.month >= MARCH && time_info.month <= MAY) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "A light spring rain begins.\n\r");
	    } else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "A cool summer rain starts to pour from the sky.\n\r");
	    } else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "A cold rain begins to fall.\n\r");
	    } else if (time_info.month == DECEMBER) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "Snow begins to silently fall from the sky.\n\r");
	    }
	}
	if (weather_info.mmhg > 1030 && number_bits(2) == 0) {
	    if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
		weather_info.sky = SKY_CLOUDLESS;
		strcat(buf, "The clouds head eastward in the winter sky.\n\r");
	    } else if (time_info.month >= MARCH && time_info.month <= MAY) {
		weather_info.sky = SKY_CLOUDLESS;
		strcat(buf, "The clouds roll away to the north in the sky.\n\r");
	    } else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
		weather_info.sky = SKY_CLOUDLESS;
		strcat(buf, "Strange shaped clouds float away high above.\n\r");
	    } else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
		weather_info.sky = SKY_CLOUDLESS;
		strcat(buf, "A cool fall wind blows the clouds to the west.\n\r");
	    } else if (time_info.month == DECEMBER) {
		weather_info.sky = SKY_CLOUDLESS;
		strcat(buf, "The sky clears up.\n\r");
	    }
	}
	break;

    case SKY_RAINING:
	if (weather_info.mmhg < 970 && number_bits(2) == 0) {
	    if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
		weather_info.sky = SKY_LIGHTNING;
		strcat(buf, "The temperature drops as it starts to snow heavily, a blizzard is forming.\n\r");
	    } else if (time_info.month >= MARCH && time_info.month <= MAY) {
		weather_info.sky = SKY_LIGHTNING;
		strcat(buf, "Lightning can be seen in the sky as it starts to hail.\n\r");
	    } else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
		weather_info.sky = SKY_LIGHTNING;
		strcat(buf, "Lightning appears on the horizon as the summer rain pounds the earth.\n\r");
	    } else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
		weather_info.sky = SKY_LIGHTNING;
		strcat(buf, "It starts to rain hard as lightning strikes at the lands.\n\r");
	    } else if (time_info.month == DECEMBER) {
		weather_info.sky = SKY_LIGHTNING;
		strcat(buf, "The temperature drops as it starts to snow heavily, a blizzard is forming.\n\r");
	    }
	}
	if (weather_info.mmhg > 1030
	    || (weather_info.mmhg > 1010 && number_bits(2) == 0)) {
	    if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "Light breaks in the sky as it stops snowing.\n\r");
	    } else if (time_info.month >= MARCH && time_info.month <= MAY) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "A light spring rain stops.\n\r");
	    } else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "The cool summer rain stops.\n\r");
	    } else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "The icy raining subsides.\n\r");
	    } else if (time_info.month == DECEMBER) {
		weather_info.sky = SKY_CLOUDY;
		strcat(buf, "It stops snowing.\n\r");
	    }
	}
	break;

    case SKY_LIGHTNING:
	if (weather_info.mmhg > 1010
	    || (weather_info.mmhg > 990 && number_bits(2) == 0)) {
	    if (time_info.month >= JANUARY && time_info.month <= FEBUARY) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "The blizzard passes as the snow continues to fall.\n\r");
	    } else if (time_info.month >= MARCH && time_info.month <= MAY) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "The hail turns into softer rain.\n\r");
	    } else if (time_info.month >= JUNE && time_info.month <= AUGUST) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "The lightning dissappears from the sky.\n\r");
	    } else if (time_info.month >= SEPTEMBER && time_info.month <= NOVEMBER) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "The lightning has stopped but the rain continues to pour.\n\r");
	    } else if (time_info.month == DECEMBER) {
		weather_info.sky = SKY_RAINING;
		strcat(buf, "The blizzard passes as the snow continues to fall.\n\r");
	    }
	}
	break;
    }

    if (buf[0] != '\0') {
	for (d = descriptor_list; d != NULL; d = d->next) {
	    if (d->connected == CON_PLAYING
		&& IS_OUTSIDE(d->character)
		&& IS_AWAKE(d->character))
		send_to_char(buf, d->character);
	}
    }
    return;
}

void kingdom_update(void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    DESCRIPTOR_DATA *d;

    int random;
    int percent;
    int excess_food;
    int excess_land;
    int vChange;
    int iChange;
    percent = 0;
    excess_food = 0;
    excess_land = 0;
    vChange = 0;

    ch_next = NULL;

    for (d = descriptor_list; d != NULL; d = d->next) {

	if (d->connected != CON_PLAYING)
	    continue;

	ch = d->character;

	if (ch->pcdata == NULL)
	    continue;

	if (ch->pcdata->vassals > 0) {
	    if (ch->pcdata->vassals >= 150000)
		continue;
		
	if ((ch->side == 1) && ((ch->in_room->vnum / 100 == 158) 
	|| (ch->in_room->vnum / 100 == 200) 
	|| (ch->in_room->vnum / 100 == 159) 
	|| (ch->in_room->vnum == 20514) ))
	continue;
	
	if ((ch->side == 2) && ((ch->in_room->vnum / 100 == 202) 
	|| (ch->in_room->vnum / 100 == 542) 
	|| (ch->in_room->vnum / 100 == 203)
	|| (ch->in_room->vnum == 20974) ))
	continue;
	
	if ((ch->side == 3) && ((ch->in_room->vnum / 100 == 204) || (ch->in_room->vnum / 100 == 538) 
	|| (ch->in_room->vnum / 100 == 542) //Undead clan hall
	|| (ch->in_room->vnum / 100 == 719) //Trunks clan hall
	|| (ch->in_room->vnum == 30061) ))
	continue;

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
	    vChange = (excess_food + excess_land) - (ch->pcdata->tax / 20);

	   /* if (!number_range(0, 1) == 0)*/
		ch->pcdata->morale = ch->pcdata->tax;

	    if (number_range(0, 1) == 0)
		ch->pcdata->morale += 1;
	    else
		ch->pcdata->morale += -1;

	    if (ch->pcdata->morale > 59) {
		send_to_char("Your VASSALS are in {RREVOLT{x against the TAXES!!\n\r", ch);
		send_to_char("Farms and Mines are destroyed!\n\r", ch);
		send_to_char("They demand you LOWER taxes!\n\r", ch);
		send_to_char("Vassals abandon your cause.\n\r", ch);

		if (ch->pcdata->farms > 0)
		    ch->pcdata->farms -= 1;
		if (ch->pcdata->mines > 0)
		    ch->pcdata->mines -= 1;
		vChange += -48;
	    } else if (ch->pcdata->morale > 49) {
		send_to_char("Your vassals threaten to revolt if taxes are not lowered!\n\r", ch);
		send_to_char("Vassals abandon your cause.\n\r", ch);
		vChange += -48;
	    } else if (ch->pcdata->morale > 36) {
		vChange += -8;
	    } else if (ch->pcdata->morale > 19) {
	    } else if (ch->pcdata->morale > 9) {
		vChange += 2;
	    } else if (ch->pcdata->morale > 0) {
		vChange += 4;
	    }
	    
	    if (ch->pcdata->vassals > ch->pcdata->farms * 20) {
		send_to_char("Your vassals demand more food! Purchase more farms.\n\r", ch);
		vChange += -25;
	    }
	    if ((ch->pcdata->vassals / 10) > ch->pcdata->mines) {
		send_to_char("Your vassals demand more resources! Purchase more mines.\n\r", ch);
		vChange += -25;
	    }
	    if (ch->pcdata->vassals > (ch->pcdata->parcels * 10)) {
		send_to_char("Your out of land space. Buy more parcels of land.\n\r", ch);
		vChange += -25;
	    }
	    if (vChange >= 12)
		vChange = 12;

	    ch->pcdata->growth += vChange;
	    ch->pcdata->vassals += (ch->pcdata->growth / 24);

	    if (ch->pcdata->growth <= -24)
		ch->pcdata->growth /= 24;
	    if (ch->pcdata->growth >= 24)
		ch->pcdata->growth /= 24;

	    /* add some random events */
	    random = number_range(1, 10000);

	    switch (random) {
	    case 1:
		break;
	    case 12:
		send_to_char("Heavy rains have flooded your lands.\n\r", ch);
		send_to_char("You suffer the loss of agriculture.\n\r", ch);
		if (ch->pcdata->farms > 0)
		    ch->pcdata->farms = (ch->pcdata->farms * 2) / 3;
		break;
	    case 25:
		send_to_char("You hear word that someone has been bad mouthing your leadership.\n\r", ch);
		send_to_char("Some vassals are plotting against you be carefull.\n\r", ch);
		ch->pcdata->morale += +15;
		break;
	    case 55:
		send_to_char("One of your vassals report that a mine has collapsed.\n\r", ch);
		if (ch->pcdata->mines > 0)
		    ch->pcdata->mines -= 1;
		break;
	    case 98:
		send_to_char("One of your farms is ravaged by locusts.\n\r", ch);
		if (ch->pcdata->farms > 0)
		    ch->pcdata->farms -= 1;
		break;
	    }

	    if (ch->pcdata->vassals < 0)
		ch->pcdata->vassals = 0;

	}
	percent = ch->pcdata->tax;

	iChange =
	    ((ch->pcdata->mines) * 3)
	    + (ch->pcdata->farms * 4);

	ch->banked_gold += ((6 * ch->pcdata->vassals * ch->pcdata->tax / 100) - iChange) / 2;

	if (ch->banked_gold < 0) {
	    ch->banked_gold = 0;

	    if (ch->pcdata->mines > 0) {
		send_to_char("Your out of money! Mine sold to try to compensate.\n\r", ch);
		ch->pcdata->mines -= 1;
	    } else if (ch->pcdata->farms > 0) {
		send_to_char("Your out of money! Farm sold to try to compensate.\n\r", ch);
		ch->pcdata->farms -= 1;
	    } else
		send_to_char("Your treasury is EMPTY!! Get some MONEY!\n\r", ch);

	    
	    continue;
	}
    }
}

/*
 * Update all chars, including mobs.
 */
void char_update(void)
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_quit;

    ch_quit = NULL;

    /* update save counter */
    save_number++;

    if (save_number > 29)
	save_number = 0;

    if (advice_number > 24)
	advice_number = 0;

    switch (advice_number) {
    case 0:
	do_advice("Newbies can get the first levels in training areas in the town.");
	break;
    case 1:
	do_advice("Use 'map' to find your current location in the world.");
	break;
    case 2:
	do_advice("You can store excess equipment and money in storage boxes.");
	break;
    case 3:
	do_advice("Use 'trophy' to show you what enemies you have killed.");
	break;
    case 4:
	do_advice("Type 'advice' to turn off advice channel.");
	break;
    case 5:
	do_advice("Chapter 5 already came out, we might make a return to it someday!");
	break;
    case 6:
	do_advice("All communication is separated based on kingdom.");
	break;
    case 7:
	do_advice("Enemy players appear to you as *Race*.");
	break;
    case 8:
	do_advice("AoW is a pk mud.  Newbies might be thought of as spies untill they prove themselves.");
	break;
    case 9:
	do_advice("There will be {RNO PWIPES!!!{x until at least {GJune 1st, 2006.{X");
	break;
    case 10:
	do_advice("Train your prime attribute first.");
	break;
    case 11:
	do_advice("Use 'bug' or 'typo' to report either.");
	break;
    case 12:
	do_advice("Constantly begging or whining for levels or equipment might get you killed by players from your own kingdom.");
	break;
    case 13:
	do_advice("Use 'consider' before you kill a creature.");
	break;
    case 14:
	do_advice("Use 'finger' to find out information about a player.");
	break;
    case 15:
	do_advice("At lvl 50 you can remort an become a more powerful race.  help 'remort'");
	break;
    case 16:
	do_advice("Turn color on or off by typing 'color'.");
	break;
    case 17:
	do_advice("Gossip is used in Tier, bellow is used in Kael, snarl is used in Undermountain, and inform is used in Cairn.");
	break;
    case 18:
	do_advice("{R{4VOTE---> {0{mhttp://www.mudconnect.com/mud-bin/vote_rank.cgi?mud=Act+of+War+Chapter+3.9.X.");
	break;
    case 19:
	do_advice("You can use your raises before remort, but it's advised that you save your trains.");
	break;
    case 20:
	do_advice("Use 'wizlist' to see who is on our immortal staff!");
	break;
    case 21:
	do_advice("We are very serious about picking proper names. Please choose an appropriate one, or an immortal will change it later on.");
	break;
    case 22:
	do_advice("Chapter 4 will combine the map of chapter 3 with the skills spells and classes of chapter 5.");
	break;
	case 23:
	do_advice("{4{GVOTE---> {0{mhttp://www.mudmagic.com/listings/game/1539/");
	break;
	case 24:
	do_advice("Type 'penter' to enable a new feature that shows when kingdommates login.");
	break;
	case 25:
	do_advice("UM is gone and all its kingdom has been transfered to Cairn.");
	break;
    }

    advice_number++;

    for (ch = char_list; ch != NULL; ch = ch_next) {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;
	int goldcounter = 1;
	
	ch_next = ch->next;

	if (ch->timer > 30)
	    ch_quit = ch;
	
	while (goldcounter != 0)	{
	if (ch->silver >= 100)	{
		ch->silver -= 100;
		ch->gold += 1;
		}
	else if (ch->silver < 100)
	 goldcounter = 0;
	 }

	/* banking interest and stuff */
	if (!IS_NPC(ch) && ch->timer < 5) {
	    long total_money_in_silver;
	    long silver_gain;

	    total_money_in_silver  = ch->banked_gold * 100 + ch->banked_silver;
	    silver_gain            = total_money_in_silver / 10000;

	    if (silver_gain > 50000)
		silver_gain = 50000;

	    total_money_in_silver += silver_gain;

	    if (total_money_in_silver > 1000000000)
		total_money_in_silver = 1000000000;

            ch->banked_gold        = total_money_in_silver / 100;
            ch->banked_silver      = total_money_in_silver % 100;
	}	

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SEVERED)) {
	    REMOVE_BIT(ch->act, PLR_SEVERED);
	    act("With a last gasp of breath, $n dies due to massive lower body trauma.",
		ch, NULL, NULL, TO_ROOM);
	    send_to_char("Your injuries prove too much, and you die.\n\r", ch);
	    raw_kill(ch, ch, 0);
	    continue;
	}

	if (ch->position >= POS_STUNNED) {
	    /* check to see if we need to go home */
	    if (IS_NPC(ch) && ch->zone != NULL && ch->zone != ch->in_room->area
		&& ch->desc == NULL && ch->fighting == NULL
		&& !IS_AFFECTED(ch, AFF_CHARM) && number_percent() < 5) {
		act("$n wanders on home.", ch, NULL, NULL, TO_ROOM);
		extract_char(ch, TRUE);
		continue;
	    }
	    if (ch->hit < ch->max_hit)
		ch->hit += hit_gain(ch);
	    else
		ch->hit = ch->max_hit;

	    if (ch->mana < ch->max_mana)
		ch->mana += mana_gain(ch);
	    else
		ch->mana = ch->max_mana;

	    if (ch->move < ch->max_move)
		ch->move += move_gain(ch);
	    else
		ch->move = ch->max_move;
	}

	if (ch->position == POS_STUNNED)
	    update_pos(ch);

	if (!IS_NPC(ch) && ch->level < LEVEL_IMMORTAL) {
	    OBJ_DATA *obj;

	    if ((obj = get_eq_char(ch, WEAR_LIGHT)) != NULL
		&& (obj->item_type == ITEM_LIGHT || obj->item_type == ITEM_DARK)
		&& obj->value[2] > 0) {
		if (--obj->value[2] == 0 && ch->in_room != NULL) {
                    if (obj->item_type == ITEM_LIGHT)
		    --ch->in_room->light;
                    else
                    ++ch->in_room->light;
		    act("$p goes out.", ch, obj, NULL, TO_ROOM);
		    act("$p flickers and goes out.", ch, obj, NULL, TO_CHAR);
		    extract_obj(obj);
		} else if (obj->value[2] <= 5 && ch->in_room != NULL)
		    act("$p flickers.", ch, obj, NULL, TO_CHAR);
	    }

	    if (++ch->timer >= 12) {
		if (ch->was_in_room == NULL && ch->in_room != NULL) {
		    ch->was_in_room = ch->in_room;
		    if (ch->fighting != NULL)
			stop_fighting(ch, TRUE);
		    act("$n disappears into the void.",
			ch, NULL, NULL, TO_ROOM);
		    send_to_char("You disappear into the void.\n\r", ch);
		    if (ch->level > 1)
			save_char_obj(ch);
		    char_from_room(ch);
		    char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
		}
	    }

	    gain_condition(ch, COND_DRUNK, -1);
	    gain_condition(ch, COND_FULL, ch->size > SIZE_MEDIUM ? -4 : -2);
	    //gain_condition(ch, COND_THIRST, -1);
	    //gain_condition(ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
	}

	for (paf = ch->affected; paf != NULL; paf = paf_next) {
	    paf_next = paf->next;
	    if (paf->duration > 0) {
		paf->duration--;
		if (number_range(0, 4) == 0 && paf->level > 0)
		    paf->level--;	/* spell strength fades with time */
	    } else if (paf->duration < 0);
	    else {
		if (paf_next == NULL
		    || paf_next->type != paf->type
		    || paf_next->duration > 0) { /* Need to fix */
		    if (paf->type > 0 && skill_table[paf->type].msg_off && paf->type != gsn_ki_power ) {
			send_to_char(skill_table[paf->type].msg_off, ch); 
			 send_to_char("\n\r", ch); 
		    }
		}
		if (paf->type == gsn_charm_person) {
		    stop_follower(ch);
		} else
		    affect_remove(ch, paf);
	    }
	}

	//do ki power
	if( !IS_NPC(ch))
//	if( get_skill(ch, gsn_ki) != 0)
	if( number_range(1, 120) < get_skill(ch, gsn_ki_power)	)
	{
	    AFFECT_DATA af;
	 af.where = TO_AFFECTS;
	 af.type = gsn_ki_power;
	 af.level = ch->level;
	 af.duration =2 * ch->level / 3;
	 af.bitvector = AFF_BERSERK;
	 af.location = APPLY_HITROLL;
	 af.modifier =1;
	affect_to_char(ch, &af);
	 af.location = APPLY_DAMROLL;
	 af.modifier = 1;
	affect_to_char(ch, &af);
	check_improve(ch, gsn_ki_power, TRUE, 8);

	}


	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if (is_affected(ch, gsn_decay) && ch != NULL) {
	    AFFECT_DATA *af, decay;
	    CHAR_DATA *vch;
	    int dam;

	    if (ch->in_room == NULL)
		return;

	    act("$n screams in agony as their skin begins to decay.",
		ch, NULL, NULL, TO_ROOM);
	    send_to_char("Your skin begins to rot.\n\r", ch);
	    for (af = ch->affected; af != NULL; af = af->next) {
		if (af->type == gsn_decay)
		    break;
	    }
/*
	    if (af == NULL) {
		REMOVE_BIT(ch->affected_by, AFF_DECAY);
		return;
	    } */
	    if (af->level == 1)
		return;

	    
	    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
		if (!saves_spell(decay.level - 2, vch, DAM_DISEASE,0)
		    && !IS_IMMORTAL(vch)
		/*    && !IS_AFFECTED(vch, AFF_DECAY) */
		 && number_bits(4) == 0) {
		    send_to_char("Chunks of your body fall off.\n\r", vch);
		    act("$n begins to fall apart.", vch, NULL, NULL, TO_ROOM);
		    affect_join(vch, &decay);
		}
	    }

	    dam = UMIN(ch->level, af->level / 3 + 1);
	    ch->mana -= dam;
	    ch->move -= dam;
	    damage_old(ch, ch, dam, gsn_decay, DAM_DISEASE, FALSE);
	}

	if (is_affected(ch, gsn_plague) && ch != NULL) {
	    AFFECT_DATA *af, plague;
	    CHAR_DATA *vch;
	    int dam;

	    if (ch->in_room == NULL)
		return;

	    act("$n writhes in agony as plague sores erupt from $s skin.",
		ch, NULL, NULL, TO_ROOM);
	    send_to_char("You writhe in agony from the plague.\n\r", ch);
	    for (af = ch->affected; af != NULL; af = af->next) {
		if (af->type == gsn_plague)
		    break;
	    }

	    if (af == NULL) {
		REMOVE_BIT(ch->affected_by, AFF_PLAGUE);
		return;
	    }
	    if (af->level == 1)
		return;

	    plague.where = TO_AFFECTS;
	    plague.type = gsn_plague;
	    plague.level = af->level - 1;
	    plague.duration = number_range(1, 2 * plague.level);
	    plague.location = APPLY_STR;
	    plague.modifier = -5;
	    plague.bitvector = AFF_PLAGUE;

	    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room) {
		if (!saves_spell(plague.level - 2, vch, DAM_DISEASE,0)
		    && !IS_IMMORTAL(vch)
		    && !IS_AFFECTED(vch, AFF_PLAGUE) && number_bits(4) == 0) {
		    send_to_char("You feel hot and feverish.\n\r", vch);
		    act("$n shivers and looks very ill.", vch, NULL, NULL, TO_ROOM);
		    affect_join(vch, &plague);
		}
	    }

	    dam = UMIN(ch->level, af->level / 5 + 1);
	    ch->mana -= dam;
	    ch->move -= dam;
	    damage_old(ch, ch, dam, gsn_plague, DAM_DISEASE, FALSE);
	} else if (IS_AFFECTED(ch, AFF_POISON) && ch != NULL
		   && !IS_AFFECTED(ch, AFF_SLOW)) {
	    AFFECT_DATA *poison;

	    poison = affect_find(ch->affected, gsn_poison);

	    if (poison != NULL) {
		act("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM);
		send_to_char("You shiver and suffer.\n\r", ch);
		damage_old(ch, ch, poison->level * 2/*/ 10 + 1*/, gsn_poison,
			   DAM_POISON, FALSE);
	    }
	} else if (ch->position == POS_INCAP && number_range(0, 1) == 0) {
	    damage(ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE);
	} else if (ch->position == POS_MORTAL) {
	    damage(ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE);
	}
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    for (ch = char_list; ch != NULL; ch = ch_next) {
	ch_next = ch->next;

	if (ch->desc != NULL && ch->desc->descriptor % 30 == save_number
	    && ch->level > 1)
	    save_char_obj(ch);

	if (ch == ch_quit)
	    do_quit(ch, "");
    }

    return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update(void)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;

    for (obj = object_list; obj != NULL; obj = obj_next) {
	CHAR_DATA *rch;
	char *message;

	obj_next = obj->next;

	/* go through affects and decrement */
	for (paf = obj->affected; paf != NULL; paf = paf_next) {
	    paf_next = paf->next;
	    if (paf->duration > 0) {
		paf->duration--;
		if (number_range(0, 4) == 0 && paf->level > 0)
		    paf->level--;	/* spell strength fades with time */
	    } else if (paf->duration < 0);
	    else {
		if (paf_next == NULL
		    || paf_next->type != paf->type
		    || paf_next->duration > 0) {
		    if (paf->type > 0 && skill_table[paf->type].msg_obj) {
			if (obj->carried_by != NULL) {
			    rch = obj->carried_by;
			    act(skill_table[paf->type].msg_obj,
				rch, obj, NULL, TO_CHAR);
			}
			if (obj->in_room != NULL
			    && obj->in_room->people != NULL) {
			    rch = obj->in_room->people;
			    act(skill_table[paf->type].msg_obj,
				rch, obj, NULL, TO_ALL);
			}
		    }
		}
		affect_remove_obj(obj, paf);
	    }
	}

	if (obj->timer <= 0 || --obj->timer > 0)
	    continue;

	switch (obj->item_type) {
	default:
	    message = "$p crumbles into dust.";
	    break;
	case ITEM_FOUNTAIN:
	    message = "$p dries up.";
	    break;
	case ITEM_CORPSE_NPC:
	    message = "$p is devoured by maggots.";
	    break;
	case ITEM_CORPSE_PC:
	    message = "$p decays into dust.";
	    break;
	case ITEM_FOOD:
	    message = "$p decomposes.";
	    break;
	case ITEM_POTION:
	    message = "$p has evaporated from disuse.";
	    break;
	case ITEM_PORTAL:
	    message = "$p fades out of existence.";
	    break;
	case ITEM_CONTAINER:
	    if (CAN_WEAR(obj, ITEM_WEAR_FLOAT))
		if (obj->contains)
		    message =
			"$p flickers and vanishes, spilling its contents on the floor.";
		else
		    message = "$p flickers and vanishes.";
	    else
		message = "$p crumbles into dust.";
	    break;
	}
	if(obj->pIndexData->vnum == OBJ_VNUM_BLOOD_PUDDLE)
		{} //do nothing
	else if (obj->carried_by != NULL) {
	    if (IS_NPC(obj->carried_by)
		&& obj->carried_by->pIndexData->pShop != NULL)
		obj->carried_by->silver += obj->cost / 5;
	    else {
		act(message, obj->carried_by, obj, NULL, TO_CHAR);
		if (obj->wear_loc == WEAR_FLOAT)
		    act(message, obj->carried_by, obj, NULL, TO_ROOM);
	    }
	} else if (obj->in_room != NULL
		   && (rch = obj->in_room->people) != NULL) {
	    if (!(obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
		  && !CAN_WEAR(obj->in_obj, ITEM_TAKE))) {
		act(message, rch, obj, NULL, TO_ROOM);
		act(message, rch, obj, NULL, TO_CHAR);
	    }
	}
	if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
	    && obj->contains) {	/* save the contents */
	    OBJ_DATA *t_obj, *next_obj;

	    for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj) {
		next_obj = t_obj->next_content;
		obj_from_obj(t_obj);

		if (obj->in_obj)	/* in another object */
		    obj_to_obj(t_obj, obj->in_obj);

		else if (obj->carried_by)	/* carried */
		    if (obj->wear_loc == WEAR_FLOAT)
			if (obj->carried_by->in_room == NULL)
			    extract_obj(t_obj);
			else
			    obj_to_room(t_obj, obj->carried_by->in_room);
		    else
			obj_to_char(t_obj, obj->carried_by);

		else if (obj->in_room == NULL)	/* destroy it */
		    extract_obj(t_obj);

		else		/* to a room */
		    obj_to_room(t_obj, obj->in_room);
	    }
	}
	extract_obj(obj);
    }

    return;
}

void room_update(AREA_DATA * pArea)
{
    ROOM_INDEX_DATA *room;
    int vnum;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++) {
	if ((room = get_room_index(vnum)))
	    room_aff_update(room);
    }

    return;
}

void room_aff_update(ROOM_INDEX_DATA * room)
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for (paf = room->affected; paf != NULL; paf = paf_next) {
	paf_next = paf->next;
	if (paf->duration > 0) {
	    paf->duration--;
	    if (number_range(0, 4) == 0 && paf->level > 0)
		paf->level--;	/* spell strength fades with time */
	} else if (paf->duration < 0);
	else {
	    if (paf_next == NULL
		|| paf_next->type != paf->type
		|| paf_next->duration > 0) {
		if (paf->type > 0 && skill_table[paf->type].msg_off) {
		    /*      send_to_char( skill_table[paf->type].msg_off, ch);
		       send_to_char( "\n\r", ch );  */
		}
	    }
	    affect_remove_room(room, paf);
	}
    }

	//trap decay - 25 Ticks - Akuji
	if (room->trap != NULL)
	{
		if (room->trap->tick > 0)
		{
			room->trap->tick--;
		}
		else 
			if (room->trap->tick <= 0)
				room->trap->is_set = FALSE;
	}

}

/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update(void)
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;

    for (wch = char_list; wch != NULL; wch = wch_next) {
	wch_next = wch->next;
	if (IS_NPC(wch)
	    || wch->level >= LEVEL_IMMORTAL
	    || wch->in_room == NULL
	    || wch->in_room->area->empty)
	    continue;

	for (ch = wch->in_room->people; ch != NULL; ch = ch_next) {
	    int count;

	    ch_next = ch->next_in_room;

	    if (!IS_NPC(ch)
		|| !IS_SET(ch->act, ACT_AGGRESSIVE)
		|| IS_SET(ch->in_room->room_flags, ROOM_SAFE)
		|| ch->fighting != NULL
		|| IS_AFFECTED(ch, AFF_CHARM)
		|| !IS_AWAKE(ch)
		|| (IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch))
		|| !can_see(ch, wch)
		|| number_bits(1) == 0)
		continue;

	    /*
	     * Ok we have a 'wch' player character and a 'ch' npc aggressor.
	     * Now make the aggressor fight a RANDOM pc victim in the room,
	     *   giving each 'vch' an equal chance of selection.
	     */
	    count = 0;
	    victim = NULL;
	    for (vch = wch->in_room->people; vch != NULL; vch = vch_next) {
		vch_next = vch->next_in_room;

		if (!IS_NPC(vch)
		    && vch->level < LEVEL_IMMORTAL
		    && ch->level >= vch->level - 10
		    && (!IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch))
		    && can_see(ch, vch)) {
		    if (number_range(0, count) == 0)
			victim = vch;
		    count++;
		}
	    }

	    if (victim == NULL)
		continue;

	    if (RIDDEN(ch)) {
		if (!mount_success(RIDDEN(wch), ch, FALSE)) {
		    send_to_char("Your mount escapes your control!\n\r", RIDDEN(ch));
		    multi_hit(ch, victim, TYPE_UNDEFINED);
		} else {
		    send_to_char("You manage to keep your mount under control.\n\r", RIDDEN(ch));
		}
	    } else
		multi_hit(ch, victim, TYPE_UNDEFINED);
	}
    }

    return;
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void update_handler(void)
{
    static int pulse_area;
    static int pulse_mobile;
    static int pulse_violence;
    static int pulse_point;
    static int pulse_music;
	static int pulse_quest;
   //Vashna 07-13-01:Commented out because of unuse.
   //static int pulse_cloud;
    AREA_DATA *pArea;

	    if( copyover_engage )
    {
    	if( --copyover_engage <= 0 )
    		do_copyover2( );
    }

    if (--pulse_area <= 0) {
	pulse_area = PULSE_AREA;
	/* number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); */
	area_update();
    }
    if (--pulse_music <= 0) {
	pulse_music = PULSE_MUSIC;
	song_update();
    }

/*Vashna 03-07-01*/
/*Commented out until cloud_update() is implemented in cloud.c*/
/*
    if (--pulse_cloud <= 0) {
	pulse_cloud = PULSE_CLOUD;
	cloud_update();
    }
*/
    if (--pulse_mobile <= 0) {
	pulse_mobile = PULSE_MOBILE;
	mobile_update();
    }
	if (--pulse_quest <= 0) {
	pulse_quest = PULSE_QUEST;
    quest_update();
    }
    if (--pulse_violence <= 0) {
	pulse_violence = PULSE_VIOLENCE;
	violence_update();
    }
    if (--pulse_point <= 0) {
	wiznet("TICK!", NULL, NULL, WIZ_TICKS, 0, 0);
	pulse_point = PULSE_TICK;
	/* number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 ); */
	weather_update();
	char_update();
	obj_update();

	for (pArea = area_first; pArea != NULL; pArea = pArea->next)
	    room_update(pArea);

//      room_update2    ( );
	kingdom_update();
    }
    aggr_update();
    tail_chain();
    return;
}

void quest_update()
{
   extern bool auto_quest;
   extern bool quest;
   extern CHAR_DATA *quest_mob;
   extern OBJ_DATA *quest_object;
   extern int quest_timer;
   extern int quest_wait;
   
   if ( !quest && !auto_quest )
      return;
   
   if ( quest )
   {
      /* Make sure the mobile and obj still exist! */
      if ( quest_mob == NULL || quest_object == NULL )
      {
         quest_cancel();
         return;
      }
      
      quest_inform();
      if ( quest_timer > 15 )
         quest_cancel();
      return;
   }
   
   if (!quest)
   {
     if (quest_wait > 0)
     {
       quest_wait--;
       return;
     }

     if (auto_quest)
       generate_auto_quest();
   }
}
