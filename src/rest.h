#ifndef __REST_H__
#define __REST_H__

int rest_get (const char * url, char * payload);
int rest_put (const char * url, const char * payload);
int rest_post (const char * url, const char * payload);
int rest_delete (const char * url);

#endif
