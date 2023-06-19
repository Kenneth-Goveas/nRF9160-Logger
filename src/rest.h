/** @defgroup   rest REST
 *
 *  @brief      REST requests.
 *
 *  This module makes REST requests and receives responses from the configured
 *  server. The basic requests, GET, PUT, POST, and DELETE, can be made by
 *  calling rest_get(), rest_put(), rest_post(), and rest_delete() respectively.
 */

#ifndef __REST_H__
#define __REST_H__

/** @ingroup    rest
 *
 *  @brief      Make GET request.
 *
 *  Makes a GET request to the configured server, and writes the response
 *  payload into the provided buffer.
 *
 *  @param      url     URL of the requested resource.
 *  @param      payload Pointer to buffer into which the response payload and a
 *                      terminating null byte must be written.
 *
 *  @retval     0       Success.
 *  @retval     -1      Failure.
 */

int rest_get (const char * url, char * payload);

/** @ingroup    rest
 *
 *  @brief      Make PUT request.
 *
 *  Makes a PUT request to the configured server, and sends the payload
 *  contained in the provided buffer.
 *
 *  @param      url     URL of the requested resource.
 *  @param      payload Pointer to buffer containing null-terminated payload
 *                      that must be sent.
 *
 *  @retval     0       Success.
 *  @retval     -1      Failure.
 */

int rest_put (const char * url, const char * payload);

/** @ingroup    rest
 *
 *  @brief      Make POST request.
 *
 *  Makes a POST request to the configured server, and sends the payload
 *  contained in the provided buffer.
 *
 *  @param      url     URL of the requested resource.
 *  @param      payload Pointer to buffer containing null-terminated payload
 *                      that must be sent.
 *
 *  @retval     0       Success.
 *  @retval     -1      Failure.
 */

int rest_post (const char * url, const char * payload);

/** @ingroup    rest
 *
 *  @brief      Make DELETE request.
 *
 *  Makes a DELETE request to the configured server.
 *
 *  @param      url URL of the requested resource.
 */

int rest_delete (const char * url);

#endif
