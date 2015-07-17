// Found on hackchina.com
// Tidied up a little by David Bolton, http://cplus.about.com
// Tidied up even more by Timm Knape, http://www.knp.de
// Adapted to work with AVR by Matthias L. Jugel, http://thinkberg.com
// This is in the public domain under MIT license

#include "eliza.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

#include <avr/pgmspace.h>

// read an array entry from progmem (for printf)
#define P(v) (wchar_t *) pgm_read_word(&(v))
// read an array entru from progmem (for other uses)
#define S(v) (char *) pgm_read_word(&(v))


// generic responses
const char response_01[] PROGMEM = "How are you this beautiful day?";
const char response_02[] PROGMEM = "Did you have a happy childhood?";
const char response_03[] PROGMEM = "Did you hate your father?";
const char response_04[] PROGMEM = "Did you have a good friend?";
const char response_05[] PROGMEM = "Did you like your friend?";
const char response_06[] PROGMEM = "What do you think about your friendship?";
const char response_07[] PROGMEM = "I'm not sure I understand.";

// fabricated answers
const char answer_08[] PROGMEM = "Tell me more about the detail.";
const char answer_09[] PROGMEM = "Don't be so rude again!";
const char answer_10[] PROGMEM = "Let't not talk about me.";
const char answer_11[] PROGMEM = "Why do you think that?";
const char answer_12[] PROGMEM = "So you hate something -- tell me more.";
const char answer_13[] PROGMEM = "Why do you ask?";
const char answer_14[] PROGMEM = "Why do you want that?";
const char answer_15[] PROGMEM = "We all need many things -- is this special?";
const char answer_16[] PROGMEM = "Remember,therapy is good for you.";
const char answer_17[] PROGMEM = "How do you know that?";
const char answer_18[] PROGMEM = "Your bill will be mailed to you.";
const char answer_19[] PROGMEM = "I don't like killing.";
const char answer_20[] PROGMEM = "It is wrong to kill.";
const char answer_21[] PROGMEM = "Don't ever call me a jerk!";
const char answer_22[] PROGMEM = "Don't be negative -- be positive.";
const char answer_23[] PROGMEM = "Strive for success.";
const char answer_24[] PROGMEM = "Why are you unhappy?";
const char answer_25[] PROGMEM = "I'm sure you like her,don't you?";
const char answer_26[] PROGMEM = "Why do you like that?";
const char answer_27[] PROGMEM = "Remember,love everthing what you love.";

// trigger words
const char no[] PROGMEM = "no";
const char yes[] PROGMEM = "yes";
const char fuck[] PROGMEM = "fuck";
const char shet[] PROGMEM = "shet";
const char you[] PROGMEM = "you";
const char think[] PROGMEM = "think";
const char hate[] PROGMEM = "hate";
const char what[] PROGMEM = "what";
const char want[] PROGMEM = "want";
const char need[] PROGMEM = "need";
const char why[] PROGMEM = "why";
const char know[] PROGMEM = "know";
const char bye[] PROGMEM = "bye";
const char murder[] PROGMEM = "murder";
const char kill[] PROGMEM = "kill";
const char jerk[] PROGMEM = "jerk";
const char cant[] PROGMEM = "can't";
const char failure[] PROGMEM = "failure";
const char never[] PROGMEM = "never";
const char unhappy[] PROGMEM = "unhappy";
const char beautiful[] PROGMEM = "beautiful";
const char like[] PROGMEM = "like";
const char love[] PROGMEM = "love";

const char *const responses[] PROGMEM = {
        response_01, response_02, response_03, response_04, response_05, response_06, response_07, NULL
};

static const char *const answers[] PROGMEM = {
        no, answer_08,
        yes, answer_08,
        fuck, answer_09,
        shet, answer_09,
        you, answer_10,
        think, answer_11,
        hate, answer_12,
        what, answer_13,
        want, answer_14,
        need, answer_15,
        why, answer_16,
        know, answer_17,
        bye, answer_18,
        murder, answer_19,
        kill, answer_20,
        jerk, answer_21,
        cant, answer_22,
        failure, answer_23,
        never, answer_22,
        unhappy, answer_24,
        beautiful, answer_25,
        like, answer_26,
        love, answer_27,
        NULL
};

const char LAST_RESORT_SHORT_ANSWER[] PROGMEM = "It's seems interesting, tell me more ...";
const char LAST_RESORT_ANSWER[] PROGMEM = "Tell me more ...";
const char STOP_REPEATING_YOURSELF[] PROGMEM = "Stop repeating yourself!";
const char YOU_JUST_SAID[] PROGMEM = "You just said: %s\nTell me more.\n";


/* GENERIC RESPONSES
 ********************/

void print_generic_response(void) {
    static int next = 0;
    if (!S(responses[next])) next = 0;
    printf("%S\n", P(responses[next++]));
}

/* FABRICATED ANSWERS
 *********************/

bool print_fabricated_answer(const char *token) {
    int index = 0;
    while (S(answers[index])) {
        char *entry = S(answers[index]);
        if (!strcmp_P(token, entry)) {
            puts_P(S(answers[index + 1]));
            return true;
        }
        index += 2;
    }
    return false;
}

/* REMEMBERED LINES
 *******************/

static char remembered_lines[MAX_REMEMBERED_LINES_COUNT][MAX_INPUT_BUFFER_SIZE];
static int next_remembered_line_enter_index = 0;
static int next_remembered_line_return_index = 0;

void remember_line(const char *line) {
    if (next_remembered_line_enter_index == MAX_REMEMBERED_LINES_COUNT) {
        next_remembered_line_enter_index = 0;
    }
    strncpy(remembered_lines[next_remembered_line_enter_index], line, MAX_INPUT_BUFFER_SIZE);
    ++next_remembered_line_enter_index;
}

const char *get_old_remembered_line(void) {
    if (next_remembered_line_enter_index != next_remembered_line_return_index) {
        int result = next_remembered_line_return_index;
        ++next_remembered_line_return_index;
        if (next_remembered_line_return_index == MAX_REMEMBERED_LINES_COUNT) {
            next_remembered_line_return_index = 0;
        }
        return remembered_lines[result];
    }
    return NULL;
}

bool is_line_remembered(const char *line) {
    for (int index = 0; index < MAX_REMEMBERED_LINES_COUNT; ++index) {
        if (!strcmp(line, remembered_lines[index])) return true;
    }
    return false;
}


/*
 * TOKENIZER
 ************/

static const char *position_in_input;

const char *next_token(void) {

    static char token[MAX_TOKEN_SIZE];

    char *position_in_token = token;

    /* skip spaces */
    while (*position_in_input && *position_in_input <= ' ') ++position_in_input;

    if (!*position_in_input) { // end of input
        return NULL;
    }

    if (strchr(".,;!?", *position_in_input)) { // return punctuation
        *position_in_token++ = *position_in_input++;
        *position_in_token = 0;
        return token;
    }

    // read word
    while (*position_in_input && !strchr(" \n\r.,;?!", *position_in_input)) {
        if (position_in_token != token + MAX_TOKEN_SIZE - 2) {
            *position_in_token++ = (char) tolower(*position_in_input++);
        } else {
            ++position_in_input;
        }
    }
    *position_in_token = 0;
    return token;
}


/*
 * CREATE THE DOCTOR'S RESPONE
 ******************************/

void respond(char *input) {
    if (strlen(input) < VERY_SHORT_ANSWER_LENGTH) { // short answer
        const char *remembered = get_old_remembered_line();
        if (remembered) {
            printf_P(YOU_JUST_SAID, remembered);
        }
        else {
            print_generic_response();
        }
        return;
    }

    if (is_line_remembered(input)) {
        puts_P(STOP_REPEATING_YOURSELF);
        return;
    }

    if (strlen(input) > SHORT_ANSWER_LENGTH) { // remember line
        remember_line(input);
    }

    position_in_input = input;

    for (; ;) {
        const char *token = next_token();
        if (!token) break;
        if (print_fabricated_answer(token)) return;
    }

    // comment of last resort
    if (strlen(input) > SHORT_ANSWER_LENGTH) {
        puts_P(LAST_RESORT_SHORT_ANSWER);
    } else {
        puts_P(LAST_RESORT_ANSWER);
    }
}
