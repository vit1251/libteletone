/* 
 * libteletone
 * Copyright (C) 2005-2014, Anthony Minessale II <anthm@freeswitch.org>
 *
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is libteletone
 *
 * The Initial Developer of the Original Code is
 * Anthony Minessale II <anthm@freeswitch.org>
 * Portions created by the Initial Developer are Copyright (C)
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * 
 * Anthony Minessale II <anthm@freeswitch.org>
 *
 *
 * libteletone.c -- Tone Generator
 *
 *
 *
 * Exception:
 * The author hereby grants the use of this source code under the 
 * following license if and only if the source code is distributed
 * as part of the OpenZAP or FreeTDM library.	Any use or distribution of this
 * source code outside the scope of the OpenZAP or FreeTDM library will nullify the
 * following license and reinact the MPL 1.1 as stated above.
 *
 * Copyright (c) 2007, Anthony Minessale II
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * * Neither the name of the original author; nor the names of any contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.	 IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <libteletone.h>

/* don't ask */
static char *my_strdup (const char *s)
{
	size_t len = strlen (s) + 1;
	void *new = malloc (len);
	
	if (new == NULL) {
		return NULL;
	}

	return (char *) memcpy (new, s, len);
}

TELETONE_API(int) teletone_run(teletone_generation_session_t *ts, const char *cmd)
{
	char *data = NULL, *cur = NULL, *end = NULL;
	int LOOPING = 0;
	
	if (!cmd) {
		return -1;
	}

	do {
		if (!(data = my_strdup(cmd))) {
			return -1;
		}

		cur = data;

		while (*cur) {
			if (*cur == ' ' || *cur == '\r' || *cur == '\n') {
				cur++;
				continue;
			}

			if ((end = strchr(cur, ';')) != 0) {
				*end++ = '\0';
			}
			
			if (*(cur + 1) == '=') {
				switch(*cur) {
				case 'c':
					ts->channels = atoi(cur + 2);
					break;
				case 'r':
					ts->rate = atoi(cur + 2);
					break;
				case 'd':
					ts->duration = atoi(cur + 2) * (ts->rate / 1000);
					break;
				case 'v':
					{
						float vol = (float)atof(cur + 2);
						if (vol <= TELETONE_VOL_DB_MAX && vol >= TELETONE_VOL_DB_MIN) {
							ts->volume = vol;
						}
					}
					break;
				case '>':
					ts->decay_step = atoi(cur + 2) * (ts->rate / 1000);
					ts->decay_direction = -1;
					break;
				case '<':
					ts->decay_step = atoi(cur + 2) * (ts->rate / 1000);
					ts->decay_direction = 1;
					break;
				case '+':
					ts->decay_factor = (float)atof(cur + 2);
					break;
				case 'w':
					ts->wait = atoi(cur + 2) * (ts->rate / 1000);
					break;
				case 'l':
					ts->loops = atoi(cur + 2); 
					break;
				case 'L':
					if (!LOOPING) {
						int L;
						if ((L = atoi(cur + 2)) > 0) {
							ts->LOOPS = L;
							LOOPING++;
						}
					}
					break;
				}
			} else {
				while (*cur) {
					char *p = NULL, *e = NULL;
					teletone_tone_map_t mymap, *mapp = NULL;

					if (*cur == ' ' || *cur == '\r' || *cur == '\n') {
						cur++;
						continue;
					}
					
					ts->tmp_duration = -1;
					ts->tmp_wait = -1;

					memset(&mymap, 0, sizeof(mymap));

					if (*(cur + 1) == '(') {
						p = cur + 2;
						if (*cur) {
							char *next;
							int i = 0;
							if ((e = strchr(p, ')')) != 0) {
								*e++ = '\0';
							}
							do {
#if (_MSC_VER == 1600)
								if (!p) {
									break;
								}
#endif
								if ((next = strchr(p, ',')) != 0) {
									*next++ = '\0';
								}
								if (i == 0) {
									ts->tmp_duration = atoi(p) * (ts->rate / 1000);
									i++;
								} else if (i == 1) {
									ts->tmp_wait = atoi(p) * (ts->rate / 1000);
									i++;
								} else {
									mymap.freqs[i++ - 2] = atof(p);
								}
								p = next;

							} while (next && (i-2) < TELETONE_MAX_TONES);
							if (i > 2 && *cur == '%') {
								mapp = &mymap;
							} else if ((i != 2 || *cur == '%')) { 
								if (ts->debug && ts->debug_stream) {
									fprintf(ts->debug_stream, "Syntax Error!\n");
								}
								goto bottom;
							}
						} 
					}

					if (*cur && !mapp) {
						if (*cur > 0 && *cur < TELETONE_TONE_RANGE) { 
							mapp = &ts->TONES[(int)*cur];
						} else if (ts->debug && ts->debug_stream) {
							fprintf(ts->debug_stream, "Map [%c] Out Of Range!\n", *cur);
						}
					}

					if (mapp) {
						if (mapp->freqs[0]) {
							if (ts->handler) {
								do {
									ts->handler(ts, mapp);
									if (ts->loops > 0) {
										ts->loops--;
									}
								} while (ts->loops);
							}
						} else if (ts->debug && ts->debug_stream) {
							fprintf(ts->debug_stream, "Ignoring Empty Map [%c]!\n", *cur);
						}
					}
					
					if (e) {
						cur = e;
					} else {
						cur++;
					}
				}
			}

			if (end) {
				cur = end;
			} else if (*cur){
				cur++;
			}
		}
	bottom:
		free(data);
		data = NULL;

		if (ts->LOOPS > 0) {
			ts->LOOPS--;
		}

	} while (ts->LOOPS);

	return 0;
}
