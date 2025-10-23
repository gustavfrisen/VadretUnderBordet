// State machine worker for weather client-server application
// Handles different operational states and transitions
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stddef.h>      // For size_t type
#include <stdbool.h>     // For bool type

// Forward declarations
typedef struct state_machine state_machine_t;
typedef struct state_context state_context_t;

// State enumeration for different operational modes
typedef enum {
    // Common states
    STATE_INIT = 0,           // Initial startup state
    STATE_ERROR,              // Error handling state
    STATE_CLEANUP,            // Cleanup and shutdown state
    
    // Server-specific states
    STATE_LOAD_CITIES,        // Load city database
    STATE_TCP_LISTEN,         // Start TCP listener
    STATE_WAIT_CONNECTION,    // Wait for client connections
    STATE_PARSE_REQUEST,      // Parse incoming HTTP request
    STATE_PROCESS_WEATHER,    // Process weather request
    STATE_SEND_RESPONSE,      // Send HTTP response
    
    // Client-specific states
    STATE_LOAD_CONFIG,        // Load IP address and port from config
    STATE_CONNECT_SERVER,     // Establish connection to server
    STATE_GET_CITIES,         // Fetch available cities list
    STATE_WAIT_USER_INPUT,    // Wait for user city selection
    STATE_REQUEST_WEATHER,    // Send weather request to server
    STATE_RECEIVE_RESPONSE,   // Receive and process server response
    STATE_DISPLAY_WEATHER,    // Display weather information
    
    STATE_MAX                 // Total number of states
} state_t;

// State machine events that trigger transitions
typedef enum {
    EVENT_NONE = 0,           // No event
    EVENT_SUCCESS,            // Operation completed successfully
    EVENT_FAILURE,            // Operation failed
    EVENT_CONNECTION_READY,   // TCP connection established
    EVENT_DATA_RECEIVED,      // Data received from network
    EVENT_USER_INPUT,         // User provided input
    EVENT_TIMEOUT,            // Operation timed out
    EVENT_SHUTDOWN,           // Shutdown requested
    EVENT_RETRY,              // Retry operation
    EVENT_MAX                 // Total number of events
} event_t;

// Context structure to hold state-specific data
struct state_context {
    // Network related data
    int socket_fd;            // Socket file descriptor
    char server_ip[16];       // Server IP address (IPv4)
    int server_port;          // Server port number
    
    // Request/Response data
    char request_buffer[4096];   // Buffer for HTTP requests
    char response_buffer[4096];  // Buffer for HTTP responses
    size_t buffer_len;           // Current buffer length
    
    // Application data
    char selected_city[64];      // Currently selected city
    char weather_data[512];      // Weather information
    bool connection_active;      // Connection status flag
    
    // Error handling
    char error_message[256];     // Last error message
    int error_code;              // Last error code
    int retry_count;             // Number of retry attempts
};

// State function pointer type
// Returns the next event to process
typedef event_t (*state_func_t)(state_context_t *ctx);

// State machine structure
struct state_machine {
    state_t current_state;       // Current active state
    state_t previous_state;      // Previous state (for rollback)
    state_context_t context;     // State context data
    bool is_running;             // Machine running flag
    bool is_server_mode;         // Server vs client mode flag
};

// State Machine Core Functions
state_machine_t* sm_create(bool is_server_mode);     // Create new state machine
void sm_destroy(state_machine_t *sm);                // Destroy state machine
void sm_reset(state_machine_t *sm);                  // Reset to initial state
bool sm_run(state_machine_t *sm);                    // Run one state machine cycle
void sm_stop(state_machine_t *sm);                   // Stop state machine

// State Management Functions
state_t sm_get_current_state(const state_machine_t *sm);     // Get current state
bool sm_transition_to(state_machine_t *sm, state_t new_state); // Force state transition
const char* sm_state_name(state_t state);                   // Get state name string
const char* sm_event_name(event_t event);                   // Get event name string

// Context Management Functions
state_context_t* sm_get_context(state_machine_t *sm);       // Get context pointer
void sm_set_error(state_machine_t *sm, int code, const char *message); // Set error state
bool sm_has_error(const state_machine_t *sm);               // Check for errors
void sm_clear_error(state_machine_t *sm);                   // Clear error state

// Utility Functions
bool sm_is_server_state(state_t state);      // Check if state is server-specific
bool sm_is_client_state(state_t state);      // Check if state is client-specific
bool sm_is_terminal_state(state_t state);    // Check if state is terminal (end state)

#endif // STATE_MACHINE_H