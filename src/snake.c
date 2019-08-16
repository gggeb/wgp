#include "display.h"
#include "input.h"

#include <stdlib.h>

#define ALLOC_CHUNK 16

#define RAND_SEED 1

#define DELAY 100

#define BLOCK_SIZE 8

#define MAX_X (DISPLAY_WIDTH / BLOCK_SIZE)
#define MAX_Y (DISPLAY_HEIGHT / BLOCK_SIZE)

#define INITIAL_X      5
#define INITIAL_Y      0
#define INITIAL_LENGTH 3
#define INITIAL_DIR    SOUTH

#define CLEAR_COLOUR   COLOUR(0,   0,   0)
#define SEGMENT_COLOUR COLOUR(0,   255, 0)
#define FRUIT_COLOUR   COLOUR(255, 0,   0)

#define HEAD(g) (g->segments[0])

#define GAMEOVER_BLINKS 3
#define ERROR_BLINKS 3

typedef enum {
    CONTINUE,
    GAMEOVER,
    ERROR
} exit_value;

typedef struct {
    uint8_t x;
    uint8_t y;
} point;

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST
} direction;

typedef struct {
    size_t length, max;
    point *segments;

    direction dir;

    point fruit;
} game;

uint8_t contains(game *g, point *p, size_t b) {
    for (; b < g->length; b++)
        if (g->segments[b].x == p->x && g->segments[b].y == p->y)
            return 1;
    return 0;
}

void randomize_fruit(game *g) {
    g->fruit.x = rand() % MAX_X;
    g->fruit.y = rand() % MAX_Y;

    if (contains(g, &g->fruit, 0))
        randomize_fruit(g);
}

uint8_t alloc_game(game *g) {
    uint8_t i;

    g->length = INITIAL_LENGTH;
    g->max = ALLOC_CHUNK;

    g->segments = malloc(ALLOC_CHUNK * sizeof(point));

    if (g->segments == NULL)
        return 0;

    for (i = 0; i < INITIAL_LENGTH; i++) {
        g->segments[i].x = INITIAL_X + INITIAL_LENGTH - i - 1;
        g->segments[i].y = INITIAL_Y;
    }

    g->dir = INITIAL_DIR;

    randomize_fruit(g);

    return 1;
}

uint8_t realloc_segments(game *g) {
    point *tmp;
    if ((tmp = realloc(g->segments, g->max += ALLOC_CHUNK)) == NULL)
        return 0;

    g->segments = tmp;
    return 1;
}

void free_game(game *g) {
    free(g->segments);
    g->segments = NULL;
}

void draw_block(point *p, uint16_t colour) {
    fill_rect(p->x * BLOCK_SIZE, p->y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, colour);
}

void initial_render(game *g) {
    size_t i;

    clear_with(CLEAR_COLOUR);
    for (i = 0; i < g->length; i++)
        draw_block(&g->segments[i], SEGMENT_COLOUR);

    draw_block(&g->fruit, FRUIT_COLOUR);
}

void turn(game *g, direction nd) {
    switch (g->dir) {
    case NORTH:
        if (nd != SOUTH)
            g->dir = nd;
        break;
    case EAST:
        if (nd != WEST)
            g->dir = nd;
        break;
    case SOUTH:
        if (nd != NORTH)
            g->dir = nd;
        break;
    case WEST:
        if (nd != EAST)
            g->dir = nd;
        break;
    }
}

void move(game *g) {
    size_t i;
    point p, t;

    p.x = HEAD(g).x;
    p.y = HEAD(g).y;

    switch (g->dir) {
    case NORTH:
        if (HEAD(g).y == 0)
            HEAD(g).y = MAX_Y - 1;
        else
            HEAD(g).y--;
        break;
    case EAST:
        if (HEAD(g).x == MAX_X - 1)
            HEAD(g).x = 0;
        else
            HEAD(g).x++;
        break;
    case SOUTH:
        if (HEAD(g).y == MAX_Y - 1)
            HEAD(g).y = 0;
        else
            HEAD(g).y++;
        break;
    case WEST:
        if (HEAD(g).x == 0)
            HEAD(g).x = MAX_X - 1;
        else
            HEAD(g).x--;
        break;
    }

    for (i = 1; i < g->length; i++) {
        t.x = g->segments[i].x;
        t.y = g->segments[i].y;

        g->segments[i].x = p.x;
        g->segments[i].y = p.y;
        
        p.x = t.x;
        p.y = t.y;
    }

    draw_block(&HEAD(g), SEGMENT_COLOUR);
    draw_block(&p, CLEAR_COLOUR);
}

uint8_t extend(game *g) {
    if (g->length == g->max)
        if (!realloc_segments(g))
            return 0;

    g->segments[g->length].x = g->segments[g->length - 1].x;
    g->segments[g->length].y = g->segments[g->length - 1].y;
    
    g->length++;

    return 1;
}

void begin_anim(void) {
    uint8_t i;

    for (i = 0; i < MAX_Y; i++) {
        fill_rect(0, (MAX_Y * BLOCK_SIZE) - (i + 1) * BLOCK_SIZE,
                  DISPLAY_WIDTH, BLOCK_SIZE, CLEAR_COLOUR);
        _delay_ms(DELAY / 4);
    }
}

void gameover_anim(void) {
    uint8_t i;
    for (i = 0; i < MAX_Y; i++) {
        fill_rect(0, i * BLOCK_SIZE, DISPLAY_WIDTH, BLOCK_SIZE, FRUIT_COLOUR);
        _delay_ms(DELAY / 4);
    }
}

exit_value tick(game *g) {
    if (GET_BUTTON(UP))
        turn(g, NORTH);
    if (GET_BUTTON(RIGHT))
        turn(g, EAST);
    if (GET_BUTTON(DOWN))
        turn(g, SOUTH);
    if (GET_BUTTON(LEFT))
        turn(g, WEST);

    move(g);

    if (HEAD(g).x == g->fruit.x && HEAD(g).y == g->fruit.y) {
        if (!extend(g))
            return ERROR;
        
        randomize_fruit(g);
        draw_block(&g->fruit, FRUIT_COLOUR);
    }

    if (g->length > 1)
        if (contains(g, &HEAD(g), 1))
            return GAMEOVER;

    return CONTINUE;
}

int main(void) {
    exit_value ev;
    game g;
    if (!alloc_game(&g))
        return 1;

    srand(RAND_SEED);

    init_leds();

    init_spi();
    init_display();

    clear_with(SEGMENT_COLOUR);
    begin_anim();
    initial_render(&g);
    
    while (1) {
        if ((ev = tick(&g)) != CONTINUE)
            break;
        _delay_ms(DELAY);
    }

    switch (ev) {
    case GAMEOVER:
        gameover_anim();
        break;
    case ERROR:
        blinkn(ERROR_BLINKS, HALTING_LED);
    default:
        break; // should never be reached
    }

    free_game(&g);

    end_display();
    end_spi();

    return 0;
}
