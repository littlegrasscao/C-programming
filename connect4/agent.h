#pragma once

// Do not modify this file

/*
 * This file implements an agent 'class'
 *
 * It defines the methods each agent has to have.
 * (An agent is a player of a connect 4 game)
 *
 * Look at player_agent.h and player_agent.c for an example on how to write an
 * agent.
 *
 * We need 3 agents:
 *    - player: ask for the move from the keyboard
 *    - random: make a random but valid move
 *    - computer: uses minimax to determine which move to make next
 */

#include "board.h"

#include <stdlib.h>
#include <stdbool.h>

struct agent_t;

typedef struct agent_t agent_t;

struct agent_t 
{

    /// Given board and color, set move to the next move for this agent
    int (*play) (agent_t * this,
                 const board_t * b, player_t color, unsigned int * move);

    /// Cleanup data
    /// Returns true on success, false otherwise
    /// This function should not free the agent_t structure itself;
    bool (*destroy) (agent_t * this);

    /// Describe the agent
    /// Returns a string WHICH HAS TO REMAIN VALID UNTIL THE AGENT IS
    /// DESTROYED.
    const char * (*describe) (agent_t * this);

    // private data;
    // Can be used by the agent implementation
    void * data;
};



// Returns 0 if all OK (and move is set to the next move), or -1 if no move
// can be made. You can assume that the input board is not in a final (i.e.
// win/loss/draw state)
static inline
int agent_play (agent_t * this, const board_t * b, player_t color, 
        unsigned int * move)
{
    return this->play (this, b, color, move);
}

/// Destroy the agent
/// Returns true on success, false otherwise
static inline
bool agent_destroy (agent_t * this)
{
    bool ret = this->destroy(this);
    free (this);
    return ret;
}

/// Return a pointer to a string (which must remain valid as long as the agent
/// is not destroyed!) describing the agent.
static inline
const char * agent_describe (agent_t * this)
{
   return this->describe (this);
}

