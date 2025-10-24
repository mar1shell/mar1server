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
bool get_controller(int client_socket, http_request *request) {
    if (request == NULL) {
        if (LOGGING) perror(RED"Received NULL request in get_controller"RESET);

        return FALSE;
    }

    http_response *response = (http_response *)malloc(sizeof(http_response));

    if (response == NULL) {
        if (LOGGING) perror("error in malloc response (get_controller)");
        return FALSE;
    }

    // initialize response fields
    response->status_code = 0;
    response->status_message = NULL;
    response->http_response_headers = NULL;
    response->body = NULL;

    char *final_response;

    final_response = (char *)malloc(sizeof(char) * INITIAL_RESPONSE_SIZE);

    if (final_response == NULL) {
        if (LOGGING) perror(RED"error in malloc final_response (get_controller)"RESET);

        response = x_free(response);

        return FALSE;
    }

    char *file_path = request->url;
    char absolute_file_path[MAX_URL_LENGTH];

    // Construct the absolute file path
    strcpy(absolute_file_path, STATIC_FILES_PATH);

    if (strcmp(file_path, "/") == 0) {
        strcat(absolute_file_path, "/index.html");
    } else {
        strcat(absolute_file_path, file_path);
    }
    
    FILE *file;
    ssize_t bytes_sent;

    if (file_exists(absolute_file_path) == TRUE) {
        printf(MAGENTA"GET request for file: %s\n"RESET, absolute_file_path);
        
        file = fopen(absolute_file_path, "r");

        if (file == NULL) {
            if (LOGGING) fprintf(stderr, YELLOW"error in opening file %s (get_controller)\n"RESET, absolute_file_path);
            
            response->status_code = HTTP_INTERNAL_SERVER_ERROR;
            response->status_message = get_status_message(response->status_code);

            final_response = create_final_response_from_string(&response, final_response, INITIAL_RESPONSE_SIZE, "EROR OPENING FILE :(");

            bytes_sent = send_response(client_socket, final_response);

            if (bytes_sent < 0) {
                if (LOGGING) perror(RED"error in send_response (get_controller)"RESET);
                
                final_response = x_free(final_response);

                free_http_response(response);
                x_fclose(file);

                return FALSE;
            }

            printf(B_WHITE"Sent %zd bytes:\n%s\n"RESET, bytes_sent, final_response);

            final_response = x_free(final_response);

            free_http_response(response);
            x_fclose(file);

            return TRUE;
        }

        response->status_code = HTTP_OK;
        response->status_message = get_status_message(response->status_code);

        final_response = create_final_response_from_file(&response, final_response, INITIAL_RESPONSE_SIZE, file, absolute_file_path);

        bytes_sent = send_response(client_socket, final_response);

        if (bytes_sent < 0) {
            if (LOGGING) perror(RED"error in send_response (get_controller)"RESET);

            final_response = x_free(final_response);

            x_fclose(file);
            free_http_response(response);

            return FALSE;
        }

        printf(B_WHITE"Sent %zd bytes:\n%s\n"RESET, bytes_sent, final_response);

        final_response = x_free(final_response);
        
        x_fclose(file);
        free_http_response(response);

        return TRUE;
    }

    response->status_code = HTTP_NOT_FOUND;
    response->status_message = get_status_message(response->status_code);

    strcpy(absolute_file_path, STATIC_FILES_PATH);
    strcat(absolute_file_path, "/404.html");

    file = fopen(absolute_file_path, "r");

    if (file == NULL) {
        if (LOGGING) fprintf(stderr, "error in opening file %s (get_controller)\n", absolute_file_path);

        final_response = create_final_response_from_string(&response, final_response, INITIAL_RESPONSE_SIZE, "404 FILE NOT FOUND :(");

        if (final_response != NULL) {
            bytes_sent = send_response(client_socket, final_response);

            if (bytes_sent < 0) {
                if (LOGGING) perror(RED"error in send_response (get_controller)"RESET);

                final_response = x_free(final_response);
                free_http_response(response);

                return FALSE;
            }
            
            printf(B_WHITE"Sent %zd bytes:\n%s\n"RESET, bytes_sent, final_response);
            
            final_response = x_free(final_response);
            free_http_response(response);
            
            return TRUE;
        }


        final_response = x_free(final_response);
        free_http_response(response);

        return FALSE;
    }
    
    final_response = create_final_response_from_file(&response, final_response, INITIAL_RESPONSE_SIZE, file, absolute_file_path);

    bytes_sent = send_response(client_socket, final_response);

    if (bytes_sent < 0) {
        if (LOGGING) perror(RED"error in send_response (get_controller)"RESET);

        final_response = x_free(final_response);

        x_fclose(file);
        free_http_response(response);

        return FALSE;
    }

    printf(B_WHITE"Sent %zd bytes:\n%s\n"RESET, bytes_sent, final_response);

    final_response = x_free(final_response);

    x_fclose(file);
    free_http_response(response);

    return TRUE;
}

/**
 * Handles an incoming HTTP request.
 * @param client_socket The socket descriptor for the client connection.
 * @param request The parsed http_request struct.
 * @return 0 on success, -1 on failure.
 */
bool handle_request(int client_socket, http_request *request) {
    if (request == NULL) {
        if (LOGGING) perror(RED"Received NULL request"RESET);

        return FALSE;
    }

    int status;

    if (request->method != NULL && strcasecmp(request->method, "GET") == 0) {
        status = get_controller(client_socket, request);
        
        return status;
    }

    if (LOGGING) fprintf(stderr, YELLOW"Unsupported HTTP method: %s\n"RESET, request->method);

    http_response *response = (http_response *)malloc(sizeof(http_response));

    if (response == NULL) {
        if (LOGGING) perror(RED"error in malloc response (get_controller)"RESET);
        
        return FALSE;
    }

    response->http_response_headers = NULL;
    response->body = NULL;
    response->status_code = HTTP_METHOD_NOT_ALLOWED;
    response->status_message = get_status_message(response->status_code);

    char *final_response = (char *)malloc(sizeof(char) * INITIAL_RESPONSE_SIZE);

    if (final_response == NULL) {
        if (LOGGING) perror(RED"error in malloc final_response (get_controller)"RESET);
   
        response = x_free(response);
        
        return FALSE;
    }

    char *file_path = "/405.html";
    char absolute_file_path[MAX_URL_LENGTH];

    strcpy(absolute_file_path, STATIC_FILES_PATH);
    strcat(absolute_file_path, file_path);

    FILE *file = fopen(absolute_file_path, "r");

    final_response = create_final_response_from_file(&response, final_response, INITIAL_RESPONSE_SIZE, file, file_path);

    ssize_t bytes_sent = send_response(client_socket, final_response);

    status = TRUE;
    
    if (bytes_sent < 0) {
        if (LOGGING) perror(RED"error in send_response (get_controller)"RESET);

        status = FALSE;
    }

    printf(B_WHITE"Sent %zd bytes:\n%s\n"RESET, bytes_sent, final_response);
    
    free_http_response(response);
    x_free(final_response);

    return status;
}