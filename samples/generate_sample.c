
#include <stdio.h>

#include <libteletone.h>

int store_result(teletone_generation_session_t *ts) {
    FILE *f;

    // Show message
    fprintf(stderr, "Session contain %d samples, %d channel(s) and %d datalen\n", ts->samples, ts->channels, ts->datalen);

    // Write result
    f = fopen("tone.raw", "wb");
    fwrite(ts->buffer, 1, ts->datalen, f);
    fclose(f);

    return 0;
}

int main() {
    teletone_generation_session_t ts;
    teletone_tone_map_t tone_map;
    int buflen = 44100;
    void *user_data = NULL;
    int rc;

    // Create generator
    rc = teletone_init_session(&ts, buflen, NULL, user_data);
    if (rc < 0) {
        fprintf(stderr, "error: can not init (rc = %d)\n", rc);
        return 1;
    }

    // Set map
    rc = teletone_set_map(&tone_map, 200, 300);
    if (rc < 0) {
        fprintf(stderr, "error: can not set map (rc = %d)\n", rc);
        return 1;
    }

    // Create tones
    ts.duration = 2000; // TODO - research what is it ...
    rc = teletone_mux_tones(&ts, &tone_map);
    if (rc < 0) {
        fprintf(stderr, "error: can not generate (rc = %d)\n", rc);
        return 1;
    }

    // Store result
    rc = store_result(&ts);
    if (rc < 0) {
        fprintf(stderr, "error: can not store result (rc = %d)\n", rc);
        return 1;
    }

    // Destroy
    rc = teletone_destroy_session(&ts);
    if (rc < 0) {
        fprintf(stderr, "error: can not dispose (rc = %d)\n", rc);
        return 1;
    }

    return 0;
}

