//
//  http_api.c
//  dspsys_xcode_dev
//
//  Created by Matt Webb on 1/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "http_api.h"

void http_api_info_ctor(HttpApiInfo* info) {
    info->params.num_values = 0;
}

KeyValueMap* http_api_info_get_params(HttpApiInfo* info) {
    return &(info->params);
}

Io_enum http_api_info_get_io(HttpApiInfo* info) {
    return info->io_type;
}

uint8_t http_api_info_get_chan_num(HttpApiInfo* info) {
    return info->chan_num;
}

Io_enum http_api_info_set_io(HttpApiInfo* info, Io_enum io) {
    info->io_type = io;
    return io;
}

uint8_t http_api_info_set_chan_num(HttpApiInfo* info, uint8_t cnum) {
    info->chan_num = cnum;
    return cnum;
}



uint8_t http_process_url(char* first_char, uint8_t length) {
    printf("value: %d\n",strncmp(first_char,"/a/",3));
    char temp[80];
    uint8_t temp_len = length;
    
    HttpApiInfo api_info;
    http_api_info_ctor(&api_info);
    
    strncpy(temp, first_char, 80);
    
    if(!strncmp(temp, "/a/", 3)) {
        //ok, we have an api command
        advance_string(temp, &temp_len, 3);
        printf("we have a web API command: leftover %s\n", temp);
        
        if( ((temp[0] == 'o') || (temp[0] == 'i')) && (temp[1] == '/') ) {
            //we have an input or an output being specified
            printf("input/output status: %c\n", temp[0]);
            http_api_info_set_io(&api_info, ( (temp[0] == 'i') ? INPUT : OUTPUT ));
            
            advance_string(temp, &temp_len, 2);
            printf("%s\n",temp);
            //now we need to find the channel number
            
            if ( (http_api_info_set_chan_num(&api_info, get_channel_number(temp, &temp_len))) > 0) {
                
                if (!strncmp(temp, "eqparams", 7)) {
                    printf("eqparams!!\n");
                } else if (!strncmp(temp, "compparams", 9)) {
                    printf("compparams!!\n");
                } else if (!strncmp(temp,"rename",6)) {
                    printf("rename channel!!!");
                } else {
                    //ERROR
                    printf("ERROR: not eqparams, compparams, or rename\n");
                }
                
            } else if (!strncmp(temp, "chanlist", 8)) {
                //return a list of channel data
                printf("return list of channel data\n");
                
            } else {
                //ERROR
                printf("ERROR: what error is this?\n");
            }
            
        } else if (temp[0] == 's') {
            printf("we are requesting a status\n");
            advance_string(temp, &temp_len, 2);
            if(!strncmp(temp,"bob",3)) {
                printf("return the bob status info\n");
            } else if(!strncmp(temp,"clip",4)) {
                printf("return clip information\n");
            } else {
                printf("no matching status api command\n");
            }
            
            
        } else if(temp[0] == 'm' && temp[1] == '/') {
            advance_string(temp, &temp_len, 2);
            printf("we are requesting routing matrix\n");
            if ( (http_api_info_set_chan_num(&api_info, get_channel_number(temp, &temp_len))) > 0) {
                printf("matrix output chan number: %d \n", http_api_info_get_chan_num(&api_info));
                
                if(!strncmp(temp, "modroute", 8)) {
                    printf("now we're modifying a route\n");
                    advance_string(temp, &temp_len, 8);
                    extract_query_params(temp, &temp_len, http_api_info_get_params(&api_info));
                    map_get_value_by_key(http_api_info_get_params(&api_info),"i");
                    char test[5];
                    strncpy(test, "blah",4);
                    map_get_value_by_key(http_api_info_get_params(&api_info), test);
                }
                
            }
            
        } else {
            //ERROR
            printf("ERROR: incorrect api command identifier\n");
        }
    } else {
        printf("we don't have a web API command\n");
        //we don't have an API command... send it to the filesystem
    }
    printf("channel number: %d\n\n\n", http_api_info_get_chan_num(&api_info));
    return 1;
}

void advance_string(char* string, uint8_t* length, uint8_t num_chars) {
    strncpy(string,string + num_chars*sizeof(char), *length-num_chars);
    
    for(int i=1; i <= num_chars; i++) {
        string[*length-i] = '\0';    
    }
    
    *length = *length - num_chars;
}


uint8_t get_channel_number(char* string, uint8_t* length ) {
    //now we need to find the channel number
    
    uint8_t chan_num = 0;  //if function returns 0 there is an error
    
    if( (string[0] <= '9') && (string[0] >= '0') ) {
        if(string[1] == '/') {
            chan_num = (string[0] - '0');
            advance_string(string, length, 2);
        } else if( (string[1] <= '9') && (string[1] >= '0') && (string[2] == '/') ) {
            uint8_t msb = string[0] - '0';
            uint8_t lsb = string[1] - '0';
            chan_num = (msb*10 + lsb);
            advance_string(string, length, 3);
            
            if( chan_num > 16 ) {
                //ERROR
                printf("ERROR: channel_number is larger than max number of channels\n");
            }
        } else {
            //ERROR
            printf("ERROR: wrong channel number format\n");
        }
    } else {
        //ERROR
        printf("not a number to start channel!");
    }
    return chan_num;
}

uint8_t extract_query_params(char* string, uint8_t* length, KeyValueMap* map) {
    uint8_t kv_counter = 0;
    
    if ( string[0] == '?' ) {
        advance_string(string, length, 1); //get past the questionmark
        
        while ( (kv_counter != 99) && (string[0] != '\0') ) {
            uint8_t equals_position = strcspn(string, "="); //figure out where the equal sign is in the string
            string[equals_position] = '\0'; //set the equals to the string termination character
            char key[20];
            strncpy(key, string, equals_position); //copy key into new memory location before string gets incremented by get_value_from_string
            key[equals_position] = '\0';
            
            map_add_key_value_pair(map, key, get_value_from_string(string, length, equals_position), equals_position+1);
            
            //if there is another parameter, dictated by a &, then increment the counter and move it past there.
            if ( string[0] == '&' ) {
                kv_counter++;
                advance_string(string, length, 1);
            } else kv_counter = 99;
        }
    }
    return 1;
}

float get_value_from_string(char* string, uint8_t* length, uint8_t offset) {
    advance_string(string, length, offset+1);
    
    uint8_t num_digits_front_decimal = strcspn(string, "./&");
    uint8_t j = num_digits_front_decimal-1;
    float mult = 1;
    float temp_value;
    uint8_t dec_flag = 0;
    
    if (string[num_digits_front_decimal] == '.') {
        dec_flag = 1;
        
    }
    
    //create a multiplier according to the number of digits left of the decimal
    while( j > 0) {
        mult *= 10;
        j--;
    }
    //loop through the digits in front of the decimal
    for (uint8_t k = num_digits_front_decimal; k > 0; k--) {
        //if it's a number then add it's integer value to temp_value multiplied by the multiplier
        if( (string[0] <= '9') && (string[0] >= '0') ) {
            temp_value += (string[0] - '0') * mult;
        }
        mult /= 10; // shift 
        advance_string(string, length, 1);
    }
    
    //if there is a decimal do the same thing as above
    if(dec_flag) {
        advance_string(string, length, 1);
        uint8_t num_digits_post_decimal = strcspn(string, "/&");
        for (uint8_t w = num_digits_post_decimal; w >0; w--) {
            temp_value += (string[0] -'0') * mult;
            mult /= 10;
            advance_string(string, length, 1);
        }
    }
    return temp_value;
}