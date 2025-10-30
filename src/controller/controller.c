#include "../../include/constants.h"
#include "../../include/controller.h"
#include "../../include/requests.h"
#include "../../include/responses.h"
#include "../../include/helpers.h"
#include "../../include/http_status.h"
#include "../../include/files_utils.h"

/**
 * Handles GET requests.
 * @param client_socket The socket descriptor for the client connection.
 * @param request The parsed http_request struct.
 * @return TRUE (1) on success, FALSE (0) on failure.
 */
x_bool get_controller(int client_socket, http_request *request)
{
    if (request == NULL || client_socket < 0)
    {
        if (LOGGING)
            perror(RED "invalid arguments (get_controller)" RESET);

        return FALSE;
    }

    char file_path[PATH_MAX];

    if (strcmp(request->url, "/") == 0)
        snprintf(file_path, sizeof(file_path), "%s/index.html", STATIC_FILES_PATH);
    else
        snprintf(file_path, sizeof(file_path), "%s%s", STATIC_FILES_PATH, request->url);

    printf("Requested file path: %s\n", file_path);

    char *content_type = extract_content_type_from_file(file_path);

    if (content_type == NULL)
    {
        if (LOGGING)
            perror(RED "Failed to extract content type in get_controller" RESET);

        send_error_response(client_socket, HTTP_NOT_FOUND, NULL);

        return TRUE;
    }

    http_response *response = (http_response *)malloc(sizeof(http_response));

    response->status_code = 0;
    response->status_message = NULL;
    response->http_response_headers = NULL;
    response->body = (char *)malloc(INITIAL_BODY_SIZE * sizeof(char));

    if (response->body == NULL)
    {
        if (LOGGING)
            perror(RED "Failed to allocate memory for response body in get_controller" RESET);

        send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);
        free_http_response(response);

        return TRUE;
    }

    if (strstr(content_type, "text") != NULL || strcasecmp(content_type, "application/json") == 0 ||
        strcasecmp(content_type, "application/javascript") == 0)
    {

        if (read_text_file(file_path, &response->body, INITIAL_BODY_SIZE) == NULL)
        {
            if (LOGGING)
                perror(RED "Failed to read requested file in get_controller" RESET);

            send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);
            free_http_response(response);

            return TRUE;
        }
    }
    else
    {
        // TODO: Handle binary files

        if (LOGGING)
            perror(RED "Binary file serving not implemented yet in get_controller" RESET);

        send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);

        free_http_response(response);

        return TRUE;
    }

    response->status_code = HTTP_OK;
    response->status_message = get_status_message(HTTP_OK);

    char content_length[SIZE_T_STRING_SIZE];

    sprintf(content_length, "%lu", strlen(response->body));

    response->http_response_headers = create_basic_response_headers(content_type, content_length);

    char *final_response = (char *)malloc(INITIAL_RESPONSE_SIZE);

    if (final_response == NULL)
    {
        if (LOGGING)
            perror(RED "Failed to allocate memory for final response in get_controller" RESET);

        free_http_response(response);

        send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);

        return TRUE;
    }

    if (create_final_response(response, &final_response, INITIAL_RESPONSE_SIZE) == NULL)
    {
        if (LOGGING)
            perror(RED "Failed to create final response in get_controller" RESET);

        free(final_response);
        free_http_response(response);

        send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);

        return TRUE;
    }

    send_tcp_response(client_socket, final_response);

    x_free(final_response);
    free_http_response(response);

    return TRUE;
}

/**
 * Handles an incoming HTTP request.
 * @param client_socket The socket descriptor for the client connection.
 * @param request The parsed http_request struct.
 * @return 0 on success, -1 on failure.
 */
x_bool handle_request(int client_socket, http_request *request)
{
    if (request == NULL || client_socket < 0)
    {
        if (LOGGING)
            perror(RED "invalid arguments (handle_request)" RESET);

        return FALSE;
    }

    if (request->method != NULL && strcasecmp(request->method, "GET") == 0)
    {
        return get_controller(client_socket, request);
    }

    if (LOGGING)
        fprintf(stderr, YELLOW "Unsupported HTTP method: %s\n" RESET, request->method);

    send_error_response(client_socket, HTTP_NOT_IMPLEMENTED, NULL);

    return TRUE;
}