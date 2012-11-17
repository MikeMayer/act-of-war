#ifndef cloud_h

#define cloud_h



struct cloud_source_data

{



	CLOUD_SOURCE * next;

	int src_rate;

	int src_decrement;

	int src_decrement_nature;

	ROOM_DATA * inroom;

	CLOUD_DATA * type;



	OBJ_DATA * obj_src;

	CHAR_DATA * char_src;

};



struct cloud_data

{



	CLOUD_DATA * next;

	int wander_percent;

	int wander_nature;

	int dissolve_rate;

	int dissolve_nature;

	int max_particles_per_room;

	int cloud_nature;



	CLOUD_ROOM_INSTANCE * instances;

};



struct cloud_instance_data

{

	CLOUD_ROOM_INSTANCE * next;

	int current_particles;

	int receiving_particles;

	ROOM_DATA * inroom;

	CLOUD_DATA * type;

};

#endif

