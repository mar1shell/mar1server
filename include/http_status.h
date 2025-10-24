#ifndef HTTP_STATUS_H
#define HTTP_STATUS_H

// --- 2xx Success ---
#define HTTP_OK 200
#define HTTP_CREATED 201

// --- 4xx Client Error ---
#define HTTP_BAD_REQUEST 400
#define HTTP_NOT_FOUND 404
#define HTTP_METHOD_NOT_ALLOWED 405
#define HTTP_PAYLOAD_TOO_LARGE 413

// --- 5xx Server Error ---
#define HTTP_INTERNAL_SERVER_ERROR 500

#endif // HTTP_STATUS_H