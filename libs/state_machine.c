// State machine worker implementation for weather client-server application
#include "state_machine.h"
#include <stdio.h>      // For printf, snprintf
#include <stdlib.h>     // For malloc, free 
#include <string.h>     // For string operations
#include <unistd.h>     // For close()

// Forward declarations of state functions
static event_t state_init(state_context_t *ctx);
static event_t state_error(state_context_t *ctx);
static event_t state_cleanup(state_context_t *ctx);

// Server state functions
static event_t state_load_cities(state_context_t *ctx);
static event_t state_tcp_listen(state_context_t *ctx);
static event_t state_wait_connection(state_context_t *ctx);
static event_t state_parse_request(state_context_t *ctx);
static event_t state_process_weather(state_context_t *ctx);
static event_t state_send_response(state_context_t *ctx);

// Client state functions
static event_t state_load_config(state_context_t *ctx);
static event_t state_connect_server(state_context_t *ctx);
static event_t state_get_cities(state_context_t *ctx);
static event_t state_wait_user_input(state_context_t *ctx);
static event_t state_request_weather(state_context_t *ctx);
static event_t state_receive_response(state_context_t *ctx);
static event_t state_display_weather(state_context_t *ctx);

// State function lookup table
static state_func_t state_functions[STATE_MAX] = {
    [STATE_INIT] = state_init,
    [STATE_ERROR] = state_error,
    [STATE_CLEANUP] = state_cleanup,
    [STATE_LOAD_CITIES] = state_load_cities,
    [STATE_TCP_LISTEN] = state_tcp_listen,
    [STATE_WAIT_CONNECTION] = state_wait_connection,
    [STATE_PARSE_REQUEST] = state_parse_request,
    [STATE_PROCESS_WEATHER] = state_process_weather,
    [STATE_SEND_RESPONSE] = state_send_response,
    [STATE_LOAD_CONFIG] = state_load_config,
    [STATE_CONNECT_SERVER] = state_connect_server,
    [STATE_GET_CITIES] = state_get_cities,
    [STATE_WAIT_USER_INPUT] = state_wait_user_input,
    [STATE_REQUEST_WEATHER] = state_request_weather,
    [STATE_RECEIVE_RESPONSE] = state_receive_response,
    [STATE_DISPLAY_WEATHER] = state_display_weather
};

// State transition table [current_state][event] = next_state
static state_t transition_table[STATE_MAX][EVENT_MAX] = {
    // STATE_INIT transitions
    [STATE_INIT] = {
        [EVENT_SUCCESS] = STATE_LOAD_CITIES,    // Server: load cities first
        [EVENT_FAILURE] = STATE_ERROR
    },
    
    // STATE_LOAD_CITIES transitions (server)
    [STATE_LOAD_CITIES] = {
        [EVENT_SUCCESS] = STATE_TCP_LISTEN,
        [EVENT_FAILURE] = STATE_ERROR
    },
    
    // STATE_TCP_LISTEN transitions (server)
    [STATE_TCP_LISTEN] = {
        [EVENT_SUCCESS] = STATE_WAIT_CONNECTION,
        [EVENT_FAILURE] = STATE_ERROR
    },
    
    // STATE_WAIT_CONNECTION transitions (server)
    [STATE_WAIT_CONNECTION] = {
        [EVENT_CONNECTION_READY] = STATE_PARSE_REQUEST,
        [EVENT_FAILURE] = STATE_ERROR,
        [EVENT_SHUTDOWN] = STATE_CLEANUP
    },
    
    // STATE_PARSE_REQUEST transitions (server)
    [STATE_PARSE_REQUEST] = {
        [EVENT_SUCCESS] = STATE_PROCESS_WEATHER,
        [EVENT_FAILURE] = STATE_SEND_RESPONSE  // Send error response
    },
    
    // STATE_PROCESS_WEATHER transitions (server)
    [STATE_PROCESS_WEATHER] = {
        [EVENT_SUCCESS] = STATE_SEND_RESPONSE,
        [EVENT_FAILURE] = STATE_SEND_RESPONSE  // Send error response
    },
    
    // STATE_SEND_RESPONSE transitions (server)
    [STATE_SEND_RESPONSE] = {
        [EVENT_SUCCESS] = STATE_WAIT_CONNECTION,  // Handle next client
        [EVENT_FAILURE] = STATE_WAIT_CONNECTION   // Try to continue anyway
    },
    
    // STATE_LOAD_CONFIG transitions (client)
    [STATE_LOAD_CONFIG] = {
        [EVENT_SUCCESS] = STATE_CONNECT_SERVER,
        [EVENT_FAILURE] = STATE_ERROR
    },
    
    // STATE_CONNECT_SERVER transitions (client)
    [STATE_CONNECT_SERVER] = {
        [EVENT_SUCCESS] = STATE_GET_CITIES,
        [EVENT_FAILURE] = STATE_ERROR,
        [EVENT_RETRY] = STATE_CONNECT_SERVER
    },
    
    // STATE_GET_CITIES transitions (client)
    [STATE_GET_CITIES] = {
        [EVENT_SUCCESS] = STATE_WAIT_USER_INPUT,
        [EVENT_FAILURE] = STATE_ERROR
    },
    
    // STATE_WAIT_USER_INPUT transitions (client)
    [STATE_WAIT_USER_INPUT] = {
        [EVENT_USER_INPUT] = STATE_REQUEST_WEATHER,
        [EVENT_SHUTDOWN] = STATE_CLEANUP
    },
    
    // STATE_REQUEST_WEATHER transitions (client)
    [STATE_REQUEST_WEATHER] = {
        [EVENT_SUCCESS] = STATE_RECEIVE_RESPONSE,
        [EVENT_FAILURE] = STATE_ERROR
    },
    
    // STATE_RECEIVE_RESPONSE transitions (client)
    [STATE_RECEIVE_RESPONSE] = {
        [EVENT_SUCCESS] = STATE_DISPLAY_WEATHER,
        [EVENT_FAILURE] = STATE_ERROR,
        [EVENT_TIMEOUT] = STATE_ERROR
    },
    
    // STATE_DISPLAY_WEATHER transitions (client)
    [STATE_DISPLAY_WEATHER] = {
        [EVENT_SUCCESS] = STATE_WAIT_USER_INPUT,  // Loop back for next request
        [EVENT_FAILURE] = STATE_WAIT_USER_INPUT
    },
    
    // STATE_ERROR transitions
    [STATE_ERROR] = {
        [EVENT_RETRY] = STATE_INIT,         // Retry from beginning
        [EVENT_SHUTDOWN] = STATE_CLEANUP    // Give up and cleanup
    },
    
    // STATE_CLEANUP transitions (terminal state)
    [STATE_CLEANUP] = {
        [EVENT_SUCCESS] = STATE_CLEANUP,    // Stay in cleanup
        [EVENT_FAILURE] = STATE_CLEANUP     // Stay in cleanup
    }
};

// Create new state machine instance
state_machine_t* sm_create(bool is_server_mode) {
    state_machine_t *sm = malloc(sizeof(state_machine_t));
    if (!sm) return NULL;
    
    // Initialize state machine
    sm->current_state = STATE_INIT;
    sm->previous_state = STATE_INIT;
    sm->is_running = false;
    sm->is_server_mode = is_server_mode;
    
    // Initialize context
    memset(&sm->context, 0, sizeof(state_context_t));
    sm->context.socket_fd = -1;
    sm->context.server_port = 8080;  // Default port
    strcpy(sm->context.server_ip, "127.0.0.1");  // Default IP
    
    return sm;
}

// Destroy state machine and cleanup resources
void sm_destroy(state_machine_t *sm) {
    if (!sm) return;
    
    // Cleanup any open connections
    if (sm->context.socket_fd >= 0) {
        close(sm->context.socket_fd);
    }
    
    free(sm);
}

// Reset state machine to initial state
void sm_reset(state_machine_t *sm) {
    if (!sm) return;
    
    sm->current_state = STATE_INIT;
    sm->previous_state = STATE_INIT;
    sm->is_running = false;
    
    // Clear context but preserve configuration
    char ip_backup[16];
    int port_backup = sm->context.server_port;
    strcpy(ip_backup, sm->context.server_ip);
    
    memset(&sm->context, 0, sizeof(state_context_t));
    sm->context.socket_fd = -1;
    sm->context.server_port = port_backup;
    strcpy(sm->context.server_ip, ip_backup);
}

// Run one state machine cycle
bool sm_run(state_machine_t *sm) {
    if (!sm || !sm->is_running) return false;
    
    // Get current state function
    state_func_t state_func = state_functions[sm->current_state];
    if (!state_func) {
        sm_set_error(sm, -1, "Invalid state function");
        return false;
    }
    
    // Execute current state and get next event
    event_t event = state_func(&sm->context);
    
    // Look up next state based on current state and event
    state_t next_state = transition_table[sm->current_state][event];
    
    // Perform state transition if valid
    if (next_state != 0 || (sm->current_state == STATE_INIT && event == EVENT_SUCCESS)) {
        sm->previous_state = sm->current_state;
        sm->current_state = next_state;
        
        printf("State transition: %s -> %s (event: %s)\n",
               sm_state_name(sm->previous_state),
               sm_state_name(sm->current_state),
               sm_event_name(event));
    }
    
    // Check if we've reached a terminal state
    return !sm_is_terminal_state(sm->current_state);
}

// Stop state machine
void sm_stop(state_machine_t *sm) {
    if (sm) {
        sm->is_running = false;
    }
}

// Get current state
state_t sm_get_current_state(const state_machine_t *sm) {
    return sm ? sm->current_state : STATE_INIT;
}

// Force state transition
bool sm_transition_to(state_machine_t *sm, state_t new_state) {
    if (!sm || new_state >= STATE_MAX) return false;
    
    sm->previous_state = sm->current_state;
    sm->current_state = new_state;
    return true;
}

// Get state name as string
const char* sm_state_name(state_t state) {
    static const char* state_names[STATE_MAX] = {
        "INIT", "ERROR", "CLEANUP",
        "LOAD_CITIES", "TCP_LISTEN", "WAIT_CONNECTION",
        "PARSE_REQUEST", "PROCESS_WEATHER", "SEND_RESPONSE",
        "LOAD_CONFIG", "CONNECT_SERVER", "GET_CITIES",
        "WAIT_USER_INPUT", "REQUEST_WEATHER", "RECEIVE_RESPONSE",
        "DISPLAY_WEATHER"
    };
    
    return (state < STATE_MAX) ? state_names[state] : "UNKNOWN";
}

// Get event name as string
const char* sm_event_name(event_t event) {
    static const char* event_names[EVENT_MAX] = {
        "NONE", "SUCCESS", "FAILURE", "CONNECTION_READY",
        "DATA_RECEIVED", "USER_INPUT", "TIMEOUT", "SHUTDOWN", "RETRY"
    };
    
    return (event < EVENT_MAX) ? event_names[event] : "UNKNOWN";
}

// Get context pointer
state_context_t* sm_get_context(state_machine_t *sm) {
    return sm ? &sm->context : NULL;
}

// Set error state
void sm_set_error(state_machine_t *sm, int code, const char *message) {
    if (!sm) return;
    
    sm->context.error_code = code;
    if (message) {
        strncpy(sm->context.error_message, message, sizeof(sm->context.error_message) - 1);
        sm->context.error_message[sizeof(sm->context.error_message) - 1] = '\0';
    }
    
    printf("Error set: %d - %s\n", code, message ? message : "Unknown error");
}

// Check for errors
bool sm_has_error(const state_machine_t *sm) {
    return sm && sm->context.error_code != 0;
}

// Clear error state
void sm_clear_error(state_machine_t *sm) {
    if (!sm) return;
    
    sm->context.error_code = 0;
    memset(sm->context.error_message, 0, sizeof(sm->context.error_message));
}

// Check if state is server-specific
bool sm_is_server_state(state_t state) {
    return (state >= STATE_LOAD_CITIES && state <= STATE_SEND_RESPONSE);
}

// Check if state is client-specific
bool sm_is_client_state(state_t state) {
    return (state >= STATE_LOAD_CONFIG && state <= STATE_DISPLAY_WEATHER);
}

// Check if state is terminal
bool sm_is_terminal_state(state_t state) {
    return (state == STATE_CLEANUP);
}

// State function implementations
static event_t state_init(state_context_t *ctx) {
    printf("Initializing system...\n");
    
    // Basic initialization always succeeds
    ctx->connection_active = false;
    ctx->retry_count = 0;
    
    return EVENT_SUCCESS;
}

static event_t state_error(state_context_t *ctx) {
    printf("Error state: %s (code: %d)\n", ctx->error_message, ctx->error_code);
    
    // Increment retry counter
    ctx->retry_count++;
    
    // Give up after 3 retries
    if (ctx->retry_count >= 3) {
        printf("Maximum retries reached, shutting down\n");
        return EVENT_SHUTDOWN;
    }
    
    return EVENT_RETRY;
}

static event_t state_cleanup(state_context_t *ctx) {
    printf("Cleaning up resources...\n");
    
    // Close any open sockets
    if (ctx->socket_fd >= 0) {
        close(ctx->socket_fd);
        ctx->socket_fd = -1;
    }
    
    ctx->connection_active = false;
    return EVENT_SUCCESS;
}

// Server state implementations
static event_t state_load_cities(state_context_t *ctx) {
    printf("Loading cities database...\n");
    
    // TODO: Implement actual city loading
    // For now, just simulate success
    return EVENT_SUCCESS;
}

static event_t state_tcp_listen(state_context_t *ctx) {
    printf("Starting TCP listener on port %d...\n", ctx->server_port);
    
    // TODO: Implement actual TCP listener setup
    // For now, just simulate success
    return EVENT_SUCCESS;
}

static event_t state_wait_connection(state_context_t *ctx) {
    printf("Waiting for client connections...\n");
    
    // TODO: Implement actual connection waiting
    // For now, just simulate connection ready
    return EVENT_CONNECTION_READY;
}

static event_t state_parse_request(state_context_t *ctx) {
    printf("Parsing HTTP request...\n");
    
    // TODO: Implement actual request parsing
    // For now, just simulate success
    return EVENT_SUCCESS;
}

static event_t state_process_weather(state_context_t *ctx) {
    printf("Processing weather request for city: %s\n", ctx->selected_city);
    
    // TODO: Implement actual weather processing
    // For now, just simulate weather data
    strcpy(ctx->weather_data, "{\"weather\":\"sunny\",\"temp\":20}");
    
    return EVENT_SUCCESS;
}

static event_t state_send_response(state_context_t *ctx) {
    printf("Sending HTTP response...\n");
    
    // TODO: Implement actual response sending
    // For now, just simulate success
    return EVENT_SUCCESS;
}

// Client state implementations
static event_t state_load_config(state_context_t *ctx) {
    printf("Loading configuration (IP: %s, Port: %d)...\n", ctx->server_ip, ctx->server_port);
    
    // TODO: Implement actual config file loading
    // For now, use defaults
    return EVENT_SUCCESS;
}

static event_t state_connect_server(state_context_t *ctx) {
    printf("Connecting to server %s:%d...\n", ctx->server_ip, ctx->server_port);
    
    // TODO: Implement actual server connection
    // For now, just simulate success
    ctx->connection_active = true;
    return EVENT_SUCCESS;
}

static event_t state_get_cities(state_context_t *ctx) {
    printf("Fetching available cities from server...\n");
    
    // TODO: Implement actual cities retrieval
    // For now, just simulate success
    return EVENT_SUCCESS;
}

static event_t state_wait_user_input(state_context_t *ctx) {
    printf("Waiting for user to select city...\n");
    
    // TODO: Implement actual user input handling
    // For now, just simulate user input
    strcpy(ctx->selected_city, "Stockholm");
    return EVENT_USER_INPUT;
}

static event_t state_request_weather(state_context_t *ctx) {
    printf("Requesting weather for %s...\n", ctx->selected_city);
    
    // TODO: Implement actual weather request
    // For now, just simulate success
    return EVENT_SUCCESS;
}

static event_t state_receive_response(state_context_t *ctx) {
    printf("Receiving server response...\n");
    
    // TODO: Implement actual response receiving
    // For now, just simulate success
    return EVENT_SUCCESS;
}

static event_t state_display_weather(state_context_t *ctx) {
    printf("Displaying weather: %s\n", ctx->weather_data);
    
    // TODO: Implement actual weather display
    // For now, just simulate success
    return EVENT_SUCCESS;
}