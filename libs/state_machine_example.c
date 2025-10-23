// Example usage of the state machine worker
// Compile with: gcc -o example state_machine_example.c ../libs/state_machine.c
#include <stdio.h>
#include <unistd.h>  // For sleep
#include "../libs/state_machine.h"

void run_server_example() {
    printf("=== SERVER STATE MACHINE EXAMPLE ===\n");
    
    // Create server state machine
    state_machine_t *server_sm = sm_create(true);  // true = server mode
    if (!server_sm) {
        printf("Failed to create server state machine\n");
        return;
    }
    
    // Start the state machine
    server_sm->is_running = true;
    
    // Run state machine for a few cycles
    int cycles = 0;
    while (server_sm->is_running && cycles < 10) {
        printf("\n--- Server Cycle %d ---\n", cycles + 1);
        
        if (!sm_run(server_sm)) {
            printf("Server state machine stopped\n");
            break;
        }
        
        // Add small delay to see transitions
        sleep(1);
        cycles++;
        
        // Stop after a few cycles for demo
        if (cycles >= 8) {
            sm_stop(server_sm);
        }
    }
    
    // Cleanup
    sm_destroy(server_sm);
    printf("Server state machine destroyed\n");
}

void run_client_example() {
    printf("\n=== CLIENT STATE MACHINE EXAMPLE ===\n");
    
    // Create client state machine
    state_machine_t *client_sm = sm_create(false);  // false = client mode
    if (!client_sm) {
        printf("Failed to create client state machine\n");
        return;
    }
    
    // Modify initial state for client
    sm_transition_to(client_sm, STATE_LOAD_CONFIG);
    
    // Start the state machine
    client_sm->is_running = true;
    
    // Run state machine for a few cycles
    int cycles = 0;
    while (client_sm->is_running && cycles < 15) {
        printf("\n--- Client Cycle %d ---\n", cycles + 1);
        
        if (!sm_run(client_sm)) {
            printf("Client state machine stopped\n");
            break;
        }
        
        // Add small delay to see transitions
        sleep(1);
        cycles++;
        
        // Stop after demonstrating a full cycle
        if (cycles >= 12) {
            sm_stop(client_sm);
        }
    }
    
    // Cleanup
    sm_destroy(client_sm);
    printf("Client state machine destroyed\n");
}

void demonstrate_error_handling() {
    printf("\n=== ERROR HANDLING EXAMPLE ===\n");
    
    state_machine_t *sm = sm_create(true);
    if (!sm) return;
    
    // Set an error to demonstrate error handling
    sm_set_error(sm, 404, "Resource not found");
    
    // Transition to error state
    sm_transition_to(sm, STATE_ERROR);
    sm->is_running = true;
    
    // Run a few cycles to see retry logic
    for (int i = 0; i < 5 && sm->is_running; i++) {
        printf("\n--- Error Handling Cycle %d ---\n", i + 1);
        
        if (!sm_run(sm)) {
            printf("State machine terminated\n");
            break;
        }
        
        sleep(1);
    }
    
    sm_destroy(sm);
}

int main() {
    printf("Weather Application State Machine Demo\n");
    printf("=====================================\n");
    
    // Run server example
    run_server_example();
    
    // Run client example  
    run_client_example();
    
    // Demonstrate error handling
    demonstrate_error_handling();
    
    printf("\n=== DEMO COMPLETE ===\n");
    return 0;
}