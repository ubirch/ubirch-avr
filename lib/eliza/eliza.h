// Found on hackchina.com
// Tidied up a little by David Bolton, http://cplus.about.com
// Tidied up even more by Timm Knape, http://www.knp.de
// Adapted to work with AVR by Matthias L. Jugel, http://thinkberg.com
// This is in the public domain under MIT license

#ifndef ELIZA_H
#define ELIZA_H

#define MAX_INPUT_BUFFER_SIZE 30
#define MAX_REMEMBERED_LINES_COUNT 50
#define MAX_TOKEN_SIZE 20

#define SHORT_ANSWER_LENGTH 11
#define VERY_SHORT_ANSWER_LENGTH 3

// available functions
void print_generic_response(void);
void respond(char *input);

#endif //ELIZA_H
