//
//  json.h
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/21/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef dspsys_xcode_dev_json_h
#define dspsys_xcode_dev_json_h

#include <stdint.h>
#include "libs/dspsys_lib_channel/channel.h"
#include "hashmap.h"
#include "http_api.h"
#include <stdlib.h>
#include "string.h"
#include "libs/dspsys_lib_channel/matrix.h"

typedef struct Json {
    char string[4][256];
    uint16_t total_length;
} Json;

//Convert eqbands in a channel to json
Json* json_encode_eq(Channel* channel);

//Covert comp in a channel to json
Json* json_encode_comp(Channel* channel);

//Convert channel names and active/inactive status to json
Json* json_encode_channels(Channel* channels[], uint8_t length);

//Convert matrix IO to json
Json* json_encode_matrix(Matrix* matrix);

//Convert breakout box status to json
Json* json_encode_status_bob();

//Convert channel clip/overflow status to json
Json* json_encode_status_clip();


//json to send when a change went through OK
Json* json_encode_ok(); 

//Convert a hashmap to JSON object
Json* json_encode_hashmap(Json* json, KeyValueMap* map);

//Concatenate two JSON objects
Json* json_concatenate(Json* first, Json* second);

//make a JSON array out of an array of JSON objects
Json* json_encode_array(Json* objects[], uint8_t num_objects);

uint16_t json_inc_length(Json* json, uint8_t length);

char* inc_char_pointer(char* chr);

void reverse2(char s[]);
void itoa(int n, char s[]);
#endif
