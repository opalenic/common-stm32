/*
 * music.c
 *
 *  Created on: 5. 11. 2013
 *      Author: ondra
 */


#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>

#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include "music.h"
#include "bsp.h"


#define C_0 61162
#define C_SHARP_0 57737
#define D_FLAT_0 57737
#define D_0 54496
#define D_SHARP_0 51414
#define E_FLAT_0 51414
#define E_0 48544
#define F_0 45809
#define F_SHARP_0 43253
#define G_FLAT_0 43253
#define G_0 40816
#define G_SHARP_0 38521
#define A_FLAT_0 38521
#define A_0 36364
#define A_SHARP_0 34317
#define B_FLAT_0 34317
#define B_0 32394
#define C_1 30581
#define C_SHARP_1 28860
#define D_FLAT_1 28860
#define D_1 27241
#define D_SHARP_1 25714
#define E_FLAT_1 25714
#define E_1 24272
#define F_1 22910
#define F_SHARP_1 21622
#define G_FLAT_1 21622
#define G_1 20408
#define G_SHARP_1 19264
#define A_FLAT_1 19264
#define A_1 18182
#define A_SHARP_1 17161
#define B_FLAT_1 17161
#define B_1 16197
#define C_2 15288
#define C_SHARP_2 14430
#define D_FLAT_2 14430
#define D_2 13620
#define D_SHARP_2 12857
#define E_FLAT_2 12857
#define E_2 12134
#define F_2 11453
#define F_SHARP_2 10811
#define G_FLAT_2 10811
#define G_2 10204
#define G_SHARP_2 9631
#define A_FLAT_2 9631
#define A_2 9091
#define A_SHARP_2 8581
#define B_FLAT_2 8581
#define B_2 8099
#define C_3 7645
#define C_SHARP_3 7216
#define D_FLAT_3 7216
#define D_3 6811
#define D_SHARP_3 6428
#define E_FLAT_3 6428
#define E_3 6068
#define F_3 5727
#define F_SHARP_3 5405
#define G_FLAT_3 5405
#define G_3 5102
#define G_SHARP_3 4816
#define A_FLAT_3 4816
#define A_3 4545
#define A_SHARP_3 4290
#define B_FLAT_3 4290
#define B_3 4050
#define C_4 3822
#define C_SHARP_4 3608
#define D_FLAT_4 3608
#define D_4 3405
#define D_SHARP_4 3214
#define E_FLAT_4 3214
#define E_4 3034
#define F_4 2863
#define F_SHARP_4 2703
#define G_FLAT_4 2703
#define G_4 2551
#define G_SHARP_4 2408
#define A_FLAT_4 2408
#define A_4 2273
#define A_SHARP_4 2145
#define B_FLAT_4 2145
#define B_4 2025
#define C_5 1911
#define C_SHARP_5 1804
#define D_FLAT_5 1804
#define D_5 1703
#define D_SHARP_5 1607
#define E_FLAT_5 1607
#define E_5 1517
#define F_5 1432
#define F_SHARP_5 1351
#define G_FLAT_5 1351
#define G_5 1276
#define G_SHARP_5 1204
#define A_FLAT_5 1204
#define A_5 1136
#define A_SHARP_5 1073
#define B_FLAT_5 1073
#define B_5 1012
#define C_6 956
#define C_SHARP_6 902
#define D_FLAT_6 902
#define D_6 851
#define D_SHARP_6 804
#define E_FLAT_6 804
#define E_6 758
#define F_6 716
#define F_SHARP_6 676
#define G_FLAT_6 676
#define G_6 638
#define G_SHARP_6 602
#define A_FLAT_6 602
#define A_6 568
#define A_SHARP_6 536
#define B_FLAT_6 536
#define B_6 506
#define C_7 478
#define C_SHARP_7 451
#define D_FLAT_7 451
#define D_7 426
#define D_SHARP_7 402
#define E_FLAT_7 402
#define E_7 379
#define F_7 358
#define F_SHARP_7 338
#define G_FLAT_7 338
#define G_7 319
#define G_SHARP_7 301
#define A_FLAT_7 301
#define A_7 284
#define A_SHARP_7 268
#define B_FLAT_7 268
#define B_7 253

#define REST 0


#define SIXTY_FOURTH_NOTE 2
#define SIXTY_FOURTH_NOTE_DOTTED 3
#define THIRTY_SECOND_NOTE 4
#define THIRTY_SECOND_NOTE_DOTTED 6
#define SIXTEENTH_NOTE 8
#define SIXTEENTH_NOTE_DOTTED 12
#define EIGHTH_NOTE 16
#define EIGHTH_NOTE_DOTTED 24
#define QUARTER_NOTE 32
#define QUARTER_NOTE_DOTTED 48
#define HALF_NOTE 64
#define HALF_NOTE_DOTTED 96
#define WHOLE_NOTE 128
#define WHOLE_NOTE_DOTTED 192
#define DOUBLE_WHOLE_NOTE 256
#define DOUBLE_WHOLE_NOTE_DOTTED 384
#define LONG_NOTE_NOTE 512
#define LONG_NOTE_NOTE_DOTTED 768

#define END_OF_SONG { .pitch = 0, .length = 0 }



track_t morrowind_simple = {
	.name = "Morrowind theme - simple version",
	.prefered_tempo = 120,
	.notes = {
		{ .pitch = REST, .length = HALF_NOTE_DOTTED },
		{ .pitch = C_4, .length = QUARTER_NOTE },
		{ .pitch = D_4, .length = QUARTER_NOTE },
		{ .pitch = E_FLAT_4, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = F_4, .length = QUARTER_NOTE },
		{ .pitch = G_4, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = G_4, .length = QUARTER_NOTE },
		{ .pitch = B_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = F_4, .length = WHOLE_NOTE_DOTTED },
		{ .pitch = G_4, .length = EIGHTH_NOTE },
		{ .pitch = F_4, .length = EIGHTH_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = D_4, .length = QUARTER_NOTE },
		{ .pitch = C_4, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = C_4, .length = QUARTER_NOTE },
		{ .pitch = D_4, .length = QUARTER_NOTE },
		{ .pitch = E_FLAT_4, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = F_4, .length = QUARTER_NOTE },
		{ .pitch = G_4, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = G_4, .length = QUARTER_NOTE },
		{ .pitch = B_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = C_5, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = B_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = D_5, .length = QUARTER_NOTE },
		{ .pitch = C_5, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = C_5, .length = QUARTER_NOTE },
		{ .pitch = D_5, .length = QUARTER_NOTE },
		{ .pitch = E_FLAT_5, .length = WHOLE_NOTE },
		{ .pitch = D_5, .length = WHOLE_NOTE },
		{ .pitch = C_5, .length = WHOLE_NOTE },
		{ .pitch = B_FLAT_4, .length = WHOLE_NOTE },
		{ .pitch = A_FLAT_4, .length = WHOLE_NOTE },
		{ .pitch = G_4, .length = WHOLE_NOTE },
		{ .pitch = F_4, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = G_4, .length = QUARTER_NOTE },
		{ .pitch = F_4, .length = DOUBLE_WHOLE_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE },
		{ .pitch = D_4, .length = QUARTER_NOTE },
		{ .pitch = C_4, .length = DOUBLE_WHOLE_NOTE_DOTTED },
		END_OF_SONG
	}
};


track_t morrowind = {
	.name = "Morrowind theme",
	.prefered_tempo = 60,
	.notes = {
		{ .pitch = C_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = E_FLAT_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = F_5, .length = EIGHTH_NOTE },
		{ .pitch = G_5, .length = HALF_NOTE },
		{ .pitch = G_5, .length = EIGHTH_NOTE },
		{ .pitch = B_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = F_5, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = G_5, .length = SIXTEENTH_NOTE },
		{ .pitch = F_5, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = C_5, .length = HALF_NOTE },
		{ .pitch = C_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = E_FLAT_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = F_5, .length = EIGHTH_NOTE },
		{ .pitch = G_5, .length = HALF_NOTE },
		{ .pitch = G_5, .length = EIGHTH_NOTE },
		{ .pitch = B_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = C_6, .length = HALF_NOTE },
		{ .pitch = B_FLAT_5, .length = EIGHTH_NOTE_DOTTED },
		{ .pitch = D_6, .length = SIXTEENTH_NOTE },
		{ .pitch = C_6, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = REST, .length = EIGHTH_NOTE },
		{ .pitch = C_6, .length = EIGHTH_NOTE_DOTTED },
		{ .pitch = D_6, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_6, .length = QUARTER_NOTE },
		{ .pitch = D_6, .length = QUARTER_NOTE },
		{ .pitch = C_6, .length = QUARTER_NOTE },
		{ .pitch = B_FLAT_5, .length = QUARTER_NOTE },
		{ .pitch = A_FLAT_5, .length = QUARTER_NOTE },
		{ .pitch = G_5, .length = QUARTER_NOTE },
		{ .pitch = F_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = G_5, .length = EIGHTH_NOTE },
		{ .pitch = F_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = C_5, .length = WHOLE_NOTE },
		{ .pitch = REST, .length = QUARTER_NOTE },
		{ .pitch = C_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = E_FLAT_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = F_5, .length = EIGHTH_NOTE },
		{ .pitch = G_5, .length = HALF_NOTE },
		{ .pitch = G_5, .length = EIGHTH_NOTE },
		{ .pitch = B_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = F_5, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = G_5, .length = SIXTEENTH_NOTE },
		{ .pitch = F_5, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = C_5, .length = HALF_NOTE },
		{ .pitch = C_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = E_FLAT_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = F_5, .length = EIGHTH_NOTE },
		{ .pitch = G_5, .length = HALF_NOTE },
		{ .pitch = G_5, .length = EIGHTH_NOTE },
		{ .pitch = B_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = C_6, .length = HALF_NOTE },
		{ .pitch = B_FLAT_5, .length = EIGHTH_NOTE_DOTTED },
		{ .pitch = D_6, .length = SIXTEENTH_NOTE },
		{ .pitch = C_6, .length = HALF_NOTE },
		{ .pitch = C_6, .length = EIGHTH_NOTE_DOTTED },
		{ .pitch = D_6, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_6, .length = QUARTER_NOTE },
		{ .pitch = D_6, .length = QUARTER_NOTE },
		{ .pitch = C_6, .length = QUARTER_NOTE },
		{ .pitch = B_FLAT_5, .length = QUARTER_NOTE },
		{ .pitch = A_FLAT_5, .length = QUARTER_NOTE },
		{ .pitch = G_5, .length = QUARTER_NOTE },
		{ .pitch = F_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE_DOTTED },
		{ .pitch = G_5, .length = SIXTEENTH_NOTE },
		{ .pitch = F_5, .length = HALF_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = C_5, .length = WHOLE_NOTE },
		{ .pitch = REST, .length = QUARTER_NOTE },
		{ .pitch = E_FLAT_5, .length = EIGHTH_NOTE },
		{ .pitch = D_5, .length = EIGHTH_NOTE },
		{ .pitch = C_5, .length = WHOLE_NOTE_DOTTED },
		END_OF_SONG
	}
};

track_t morrowind_bass = {
	.name = "Morrowind theme - bass track",
	.prefered_tempo = 60,
	.notes = {
		{ .pitch = REST, .length = QUARTER_NOTE },
		{ .pitch = C_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = G_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = B_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_4, .length = SIXTEENTH_NOTE },
		{ .pitch = D_4, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = EIGHTH_NOTE },
		{ .pitch = B_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = A_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = A_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = C_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = G_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = B_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = A_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = F_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = F_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = G_3, .length = EIGHTH_NOTE },
		{ .pitch = C_4, .length = SIXTEENTH_NOTE },
		{ .pitch = D_4, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = B_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = E_FLAT_4, .length = SIXTEENTH_NOTE },
		{ .pitch = F_4, .length = SIXTEENTH_NOTE },
		{ .pitch = G_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = A_FLAT_2, .length = EIGHTH_NOTE },
		{ .pitch = E_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = A_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = B_FLAT_2, .length = EIGHTH_NOTE },
		{ .pitch = F_3, .length = EIGHTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = SIXTEENTH_NOTE },
		{ .pitch = D_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = F_2, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = G_3, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = F_2, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = C_2, .length = SIXTEENTH_NOTE },
		{ .pitch = G_2, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = B_FLAT_1, .length = SIXTEENTH_NOTE },
		{ .pitch = F_2, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = A_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_4, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = C_2, .length = SIXTEENTH_NOTE },
		{ .pitch = G_2, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = E_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = A_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_4, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = F_2, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = SIXTEENTH_NOTE },
		{ .pitch = F_4, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = C_2, .length = SIXTEENTH_NOTE },
		{ .pitch = G_2, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_4, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_2, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_4, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_1, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_1, .length = SIXTEENTH_NOTE },
		{ .pitch = F_2, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_2, .length = SIXTEENTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = B_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = F_2, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = G_3, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = F_2, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = F_3, .length = SIXTEENTH_NOTE },
		{ .pitch = G_3, .length = SIXTEENTH_NOTE },
		{ .pitch = A_FLAT_3, .length = QUARTER_NOTE_DOTTED },
		{ .pitch = C_2, .length = EIGHTH_NOTE },
		{ .pitch = G_2, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = D_3, .length = EIGHTH_NOTE },
		{ .pitch = G_2, .length = EIGHTH_NOTE },
		{ .pitch = C_2, .length = EIGHTH_NOTE },
		{ .pitch = G_2, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = SIXTEENTH_NOTE },
		{ .pitch = D_3, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_3, .length = EIGHTH_NOTE },
		{ .pitch = D_3, .length = EIGHTH_NOTE },
		{ .pitch = G_2, .length = EIGHTH_NOTE },
		{ .pitch = C_3, .length = EIGHTH_NOTE },
		{ .pitch = G_3, .length = EIGHTH_NOTE },
		{ .pitch = C_4, .length = SIXTEENTH_NOTE },
		{ .pitch = D_4, .length = SIXTEENTH_NOTE },
		{ .pitch = E_FLAT_4, .length = EIGHTH_NOTE },
		{ .pitch = D_4, .length = EIGHTH_NOTE },
		{ .pitch = G_3, .length = EIGHTH_NOTE },
		{ .pitch = C_4, .length = HALF_NOTE_DOTTED },
		END_OF_SONG
	}
};

typedef enum {
	STOPPED = 0,
	PLAYING_NOTE = 1,
	PAUSE_AFTER_NOTE = 2
} sm_state_t;

sm_state_t sm_state;

note_t *current_track_notes = 0;
uint16_t current_tempo = 0;
uint32_t pos_in_track = 0;

static void set_tone(uint16_t period)
{
	timer_set_period(TIM3, period);
	timer_set_oc_value(TIM3, TIM_OC1, period / 2);
	timer_set_counter(TIM3, 0);
}

static void set_length(uint16_t length)
{
	uint32_t actual_length = (60 * 2000 * length)
			/ (current_tempo * WHOLE_NOTE) - 10;

	timer_set_counter(TIM7, 0);
	timer_set_period(TIM7, actual_length);
}

static void stop(void)
{
	timer_set_counter(TIM7, 0);
	timer_set_period(TIM7, 0);
}

static void pause(void)
{
	timer_set_counter(TIM7, 0);
	timer_set_period(TIM7, 10);
}

void music_init(void)
{
	sm_state = STOPPED;

	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM7);

	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO4);
	gpio_set_af(GPIOB, GPIO_AF1, GPIO4);

	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	TIM_CR1_DIR_UP);

	timer_set_prescaler(TIM3, 48 - 1);
	timer_set_period(TIM3, 0);

	timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_value(TIM3, TIM_OC1, 0);

	timer_enable_oc_output(TIM3, TIM_OC1);

	timer_set_mode(TIM7, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	TIM_CR1_DIR_UP);
	timer_set_prescaler(TIM7, 48000 - 1);

	timer_set_counter(TIM7, 0);

	timer_update_on_overflow(TIM7);

	timer_enable_irq(TIM7, TIM_DIER_UIE);

	nvic_enable_irq(NVIC_TIM7_IRQ);

}

void tim7_isr(void)
{
	timer_clear_flag(TIM7, TIM_SR_UIF);
	music_statemachine_step();
}

bool music_play(track_t *track, uint16_t tempo)
{
	if (sm_state != STOPPED) {
		return false;
	}

	current_track_notes = track->notes;
	current_tempo = tempo;
	pos_in_track = 0;
	sm_state = PLAYING_NOTE;

	timer_enable_counter(TIM3);
	timer_enable_counter(TIM7);

	music_statemachine_step();

	return true;
}

void music_statemachine_step()
{

	switch (sm_state) {
	case PLAYING_NOTE:
		if (current_track_notes[pos_in_track].length == 0) {
			sm_state = STOPPED;
			stop();
			return;
		}

		bsp_led_toggle(LEDG);

		set_length(current_track_notes[pos_in_track].length);
		set_tone(current_track_notes[pos_in_track].pitch);

		sm_state = PAUSE_AFTER_NOTE;
		break;
	case PAUSE_AFTER_NOTE:
		set_tone(REST);
		pause();
		pos_in_track++;
		sm_state = PLAYING_NOTE;
		break;
	case STOPPED:
		timer_disable_counter(TIM3);
		timer_disable_counter(TIM7);
		break;
	}

}

