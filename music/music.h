/*
 * music.h
 *
 *  Created on: 5. 11. 2013
 *      Author: ondra
 */

#ifndef __MUSIC_H
#define __MUSIC_H

#include <stdint.h>
#include <stdbool.h>

typedef struct note {
	uint16_t pitch;
	uint16_t length;
} note_t;

typedef struct song {
	const char *name;
	uint16_t prefered_tempo;
	struct note notes[];
} track_t;


extern track_t morrowind;
extern track_t morrowind_simple;
extern track_t morrowind_bass;

void music_init(void);

bool music_play(track_t *track, uint16_t tempo);
void music_statemachine_step(void);


#endif /* __MUSIC_H */
