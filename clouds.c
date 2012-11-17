#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"

//	cloud_update()
//	cloud_instance_update()
//	cloud_infect_obj()
//	cloud_infect_char()
//	cloud_infect_room()
//	cloud_spread()
//	create_cloud_instance()
//	create_cloud()
//	create_cloud_source()

void cloud_update(void)
{

// walk thru all instances




// walk thru all instances again

// walk thru all sources

}



//void cloud_infect_char(CHAR_DATA * victim, CLOUD_INDEX_DATA * cloud)
//{
//	CLOUD_INSTANCE_DATA * cloud;
	
//	cloud = new_obj();


//}







CLOUD_INDEX_DATA *cloud_index_free;

CLOUD_INDEX_DATA *new_cloud_index(void)
{
    static CLOUD_INDEX_DATA cloud_index_zero;
    CLOUD_INDEX_DATA *cloud;

   if (cloud_index_free == NULL)
	cloud = alloc_perm(sizeof(*cloud));
   else {
	cloud = cloud_index_free;
	cloud_index_free = cloud_index_free->next;
    }
    *cloud = cloud_index_zero;
   VALIDATE(cloud);

    return cloud;

}

void free_cloud_index(CLOUD_INDEX_DATA * cloud)
{

    if (!IS_VALID(cloud))
	return;

    INVALIDATE(cloud);

    cloud->next = cloud_index_free;
    cloud_index_free = cloud;

}

CLOUD_INSTANCE_DATA *cloud_instance_free;

CLOUD_INSTANCE_DATA *new_cloud_instance(void)
{
    static CLOUD_INSTANCE_DATA cloud_instance_zero;
    CLOUD_INSTANCE_DATA *cloud_instance;

   if (cloud_instance_free == NULL)
	cloud_instance = alloc_perm(sizeof(*cloud_instance));
   else {
	cloud_instance = cloud_instance_free;
	cloud_instance_free = cloud_instance_free->next;
    }
    *cloud_instance = cloud_instance_zero;
   VALIDATE(cloud_instance);

    return cloud_instance;

}

void free_cloud_instance(CLOUD_INSTANCE_DATA * cloud_instance)
{

    if (!IS_VALID(cloud_instance))
	return;

    INVALIDATE(cloud_instance);

    cloud_instance->next = cloud_instance_free;
    cloud_instance_free = cloud_instance;

}

CLOUD_SOURCE_DATA *cloud_source_free;

CLOUD_SOURCE_DATA *new_cloud_source(void)
{
    static CLOUD_SOURCE_DATA cloud_source_zero;
    CLOUD_SOURCE_DATA *cloud_source;

   if (cloud_source_free == NULL)
	cloud_source = alloc_perm(sizeof(*cloud_source));
   else {
	cloud_source = cloud_source_free;
	cloud_source_free = cloud_source_free->next;
    }
    *cloud_source = cloud_source_zero;
   VALIDATE(cloud_source);

    return cloud_source;

}

void free_cloud_source(CLOUD_SOURCE_DATA * cloud_source)
{

    if (!IS_VALID(cloud_source))
	return;

    INVALIDATE(cloud_source);

    cloud_source->next = cloud_source_free;
    cloud_source_free = cloud_source;

}

CLOUD_INSTANCE_DATA * create_cloud_instance(CLOUD_INDEX_DATA * cloudIndex, int particles)
{
    CLOUD_INSTANCE_DATA *cloud;

    if (cloudIndex == NULL) {
	bug("Create_cloud: NULL cloudIndex.", 0);
	exit(1);
    }
    cloud = new_cloud_instance();

    cloud->type = cloudIndex;
    cloud->next = cloud_instance_list;
    cloud_instance_list = cloud;
    cloudIndex->count++;

    return cloud;
}


