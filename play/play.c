/*
 * play.c
 *
 *  Created on: 13. 9. 2014
 *      Author: ondra
 */

#include <stdint.h>

#include "play.h"
#include "cli.h"
#include "music.h"
#include "comm.h"
#include "common.h"


struct known_track {
	const char *short_name;
	track_t *track;
};


static struct known_track known_tracks[] = {
	{
		.short_name = "morrowind",
		.track = &morrowind
	},
	{
		.short_name = "morrowind_bass",
		.track = &morrowind_bass
	},
	{
		.short_name = "morrowind_simple",
		.track = &morrowind_simple
	}
};


static void list_tracks(void) {
	comm_send_str("List of known tracks:\r\n");

	for (uint8_t i = 0; i < ARRAY_SIZE(known_tracks); i++) {
		comm_send_ch('\t');
		comm_send_str(known_tracks[i].short_name);
		comm_send_str(" - ");
		comm_send_str(known_tracks[i].track->name);
		comm_send_str("\r\n");
	}
}

static void play(struct character *first_tok)
{
	if (first_tok == NULL) {
		comm_send_str("Usage: play track [tempo]\r\n");
		list_tracks();
		return;
	}


	track_t *track = NULL;

	for (uint8_t i = 0; i < ARRAY_SIZE(known_tracks); i++) {

		if (cli_match_tok(first_tok, known_tracks[i].short_name) == 0) {
			track = known_tracks[i].track;
		}
	}

	if (track == NULL) {
		comm_send_str("Unknown track.\r\n");

		list_tracks();

		return;
	}

	uint16_t tempo;
	if (!cli_parse_tok_uint16(cli_next_tok(first_tok), &tempo)) {
		tempo = track->prefered_tempo;

		comm_send_str("Using prefered tempo: ");
		comm_send_num_u(tempo);
		comm_send_str(" bps\r\n");
	}

	if (!music_play(track, tempo)) {
		comm_send_str("Something is already playing.\r\n");
	}

}

bool play_register(void)
{
	bool ret = cli_register_command("play", play);
	if (!ret) {
		comm_send_str("Failed to register command 'play'.\r\n");
	}

	return ret;
}
