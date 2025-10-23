# State Machine Guide

A beginner-friendly, detailed guide to the state machine added to this project. It explains what each line in `libs/state_machine.h` and `libs/state_machine.c` does, the flow of states and events, and how to write and integrate a similar state machine yourself.

---

## What is a State Machine (mental model)

- You define a set of States (INIT, LOAD_CITIES, TCP_LISTEN, etc.).
- Each state is a small function that does one task and returns an Event (SUCCESS, FAILURE, etc.).
- A Transition Table maps [current state + event] → next state.
- An engine (sm_run) runs the current state, gets an event, and moves to the next state.

This keeps your program’s flow simple, debuggable, and easy to extend.

---

## File-by-file walkthrough

### libs/state_machine.h (Header)

- File purpose: declares the state machine API, state/event enums, and the context struct shared across states.

Key parts:

1) Include guard and includes
- `#ifndef STATE_MACHINE_H` / `#define` / `#endif`: prevents duplicate inclusion.
- `#include <stddef.h>`: for size_t.
- `#include <stdbool.h>`: for bool.

2) Forward declarations
- `typedef struct state_machine state_machine_t;`
- `typedef struct state_context state_context_t;`
These let us refer to the types before the full struct definitions appear (helps break circular dependencies and keep the header tidy).

3) States (state_t)
- An enum listing all the states:
  - Common: `STATE_INIT`, `STATE_ERROR`, `STATE_CLEANUP`.
  - Server: `STATE_LOAD_CITIES`, `STATE_TCP_LISTEN`, `STATE_WAIT_CONNECTION`, `STATE_PARSE_REQUEST`, `STATE_PROCESS_WEATHER`, `STATE_SEND_RESPONSE`.
  - Client: `STATE_LOAD_CONFIG`, `STATE_CONNECT_SERVER`, `STATE_GET_CITIES`, `STATE_WAIT_USER_INPUT`, `STATE_REQUEST_WEATHER`, `STATE_RECEIVE_RESPONSE`, `STATE_DISPLAY_WEATHER`.
- `STATE_MAX` is a sentinel used for array sizes and bounds checking.

4) Events (event_t)
- An enum listing all the events a state can return:
  - `EVENT_SUCCESS`, `EVENT_FAILURE` (very common)
  - `EVENT_CONNECTION_READY`, `EVENT_DATA_RECEIVED`
  - `EVENT_USER_INPUT`, `EVENT_TIMEOUT`, `EVENT_SHUTDOWN`, `EVENT_RETRY`
- `EVENT_MAX` is a sentinel.

5) Context struct (state_context)
- A single struct that carries all data between states:
  - Network: `socket_fd`, `server_ip`, `server_port`
  - Buffers: `request_buffer`, `response_buffer`, and `buffer_len`
  - App data: `selected_city`, `weather_data`, `connection_active`
  - Error handling: `error_message`, `error_code`, `retry_count`

6) State function type
- `typedef event_t (*state_func_t)(state_context_t *ctx);`
- Every state is a function that takes the context and returns an event.

7) State machine struct (state_machine)
- Holds the current state, previous state, the shared `context`, and flags like `is_running` and `is_server_mode`.

8) Public API
- Lifecycle: `sm_create`, `sm_destroy`, `sm_reset`, `sm_stop`
- Execution: `sm_run` (executes one cycle)
- State mgmt: `sm_get_current_state`, `sm_transition_to`
- Debug helpers: `sm_state_name`, `sm_event_name`
- Context & error helpers: `sm_get_context`, `sm_set_error`, `sm_has_error`, `sm_clear_error`
- Classification: `sm_is_server_state`, `sm_is_client_state`, `sm_is_terminal_state`

---

### libs/state_machine.c (Implementation)

- File purpose: implements the engine (run loop), transition table, and state functions.

Key parts:

1) Includes
- Adds the header and standard libraries (`stdio.h`, `stdlib.h`, `string.h`, `unistd.h`).

2) Forward declarations for each state function
- `static event_t state_init(state_context_t *ctx);` etc.
- Marked `static` so they’re only visible inside this .c file.

3) State function lookup table
```c
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
```
- Maps a state enum to the function that implements it.

4) Transition table
```c
static state_t transition_table[STATE_MAX][EVENT_MAX] = {
  [STATE_INIT] = {
    [EVENT_SUCCESS] = STATE_LOAD_CITIES,
    [EVENT_FAILURE] = STATE_ERROR
  },
  ...
};
```
- Defines the flow for each state based on returned events.
- Example (server flow):
  INIT → LOAD_CITIES → TCP_LISTEN → WAIT_CONNECTION → PARSE_REQUEST → PROCESS_WEATHER → SEND_RESPONSE → WAIT_CONNECTION (loop)

5) sm_create
- Allocates and initializes a state machine.
- Sets defaults: `STATE_INIT`, `is_running=false`, `server_port=8080`, `server_ip="127.0.0.1"`, `socket_fd=-1`.

6) sm_destroy
- Closes any open socket and frees the machine.

7) sm_reset
- Resets to INIT and clears the context while preserving IP/port.

8) sm_run
- If not running, returns false.
- Looks up and calls the current state function.
- Gets back an event.
- Uses the transition table to determine next state.
- Logs the transition using state/event names.
- Returns true if not in a terminal state (i.e., continue running).

9) sm_stop
- Sets `is_running=false`.

10) Helpers
- `sm_get_current_state`, `sm_transition_to`, `sm_state_name`, `sm_event_name`, `sm_get_context`
- Error helpers: `sm_set_error`, `sm_has_error`, `sm_clear_error`
- Classification helpers: `sm_is_server_state`, `sm_is_client_state`, `sm_is_terminal_state`

11) State function implementations
- Each prints what it’s doing and (for now) simulates success.
- TODOs indicate where to plug in your real code (TCP/HTTP/city loading):
  - Server:
    - `state_tcp_listen`: call `tcp_listen(...)`, store fd in `ctx->socket_fd`.
    - `state_wait_connection`: `tcp_accept(ctx->socket_fd)`, store client fd, or just move on if single request.
    - `state_parse_request`: read into `ctx->request_buffer`, parse with your HTTP functions.
    - `state_process_weather`: set `ctx->weather_data` to structured JSON.
    - `state_send_response`: build a response with `http_build_response` and write() it.
  - Client:
    - `state_load_config`: read server ip/port from a file.
    - `state_connect_server`: `tcp_connect(ip, port)`.
    - `state_request_weather`: send HTTP GET.
    - `state_receive_response`: read response into `ctx->response_buffer`.
    - `state_display_weather`: print/parse JSON.

---

## How to write this from scratch (order + why)

If you were authoring your own state machine from an empty project:

1) Design states and events
- Write down the minimal states you need (INIT, ERROR, CLEANUP + 3–5 real steps).
- Define the minimal events you need (SUCCESS/FAILURE, maybe SHUTDOWN/RETRY).

2) Create the header (.h)
- Include guard, includes.
- Forward declarations.
- state_t and event_t enums.
- context struct: start with only what you need (sockets, buffers, a few fields).
- function pointer typedef for states.
- state_machine struct.
- API declarations (create/destroy/run/transition/getters/helpers).

3) Create the implementation (.c)
- Includes.
- `static` forward declarations for each state function.
- `state_functions[]` table mapping.
- `transition_table[][]` mapping.
- Implement engine functions: `sm_create`, `sm_destroy`, `sm_reset`, `sm_run`, `sm_stop`, helpers.
- Implement the state functions; start with printing and returning `EVENT_SUCCESS` to watch the transitions, then plug in real code.

4) Integrate into your program
- In server `main.c`:
```c
state_machine_t *server_sm = sm_create(true);
server_sm->is_running = true;
while (server_sm->is_running) {
  if (!sm_run(server_sm)) break;
}
sm_destroy(server_sm);
```
- Do the same on the client side (start at `STATE_LOAD_CONFIG`).

5) Grow gradually
- Replace TODOs with real calls to your TCP/HTTP code.
- Add new states only when you need them.
- Use `sm_state_name` logs to debug and confirm the flow.

---

## Build and run (examples)

Build the standalone example (provided Makefile in `libs`):

```bash
cd libs
make -f Makefile_state_machine
./state_machine_example
```

Clean the build:

```bash
cd libs
make -f Makefile_state_machine clean
```

---

## Typical server flow (with events)

```
INIT --SUCCESS--> LOAD_CITIES --SUCCESS--> TCP_LISTEN --SUCCESS-->
WAIT_CONNECTION --CONNECTION_READY--> PARSE_REQUEST --SUCCESS-->
PROCESS_WEATHER --SUCCESS--> SEND_RESPONSE --SUCCESS-->
WAIT_CONNECTION (loop)
```

Any FAILURE can go to `STATE_ERROR`, which either `EVENT_RETRY` back to `STATE_INIT` or `EVENT_SHUTDOWN` to `STATE_CLEANUP`.

---

## Tips for success

- Keep states small: do one thing per state.
- Always return a clear event based on the outcome (SUCCESS/FAILURE/RETRY).
- Log transitions (already included) — they make debugging 10x easier.
- Only add fields to the context when you actually need them.
- Start with a minimal path working, then layer in more states.

---

## Where to plug in your project code

- TCP: `tcp_listen`, `tcp_accept`, `tcp_connect`, `read`, `write` from `libs/TCP.c`.
- HTTP: `http_parse_request`, `http_build_response` from `libs/HTTP.c`.
- Data: functions for loading cities, formatting responses, etc.

If you want, I can help wire the server states to your existing TCP/HTTP functions and test them in WSL.
