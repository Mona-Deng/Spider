#include "cs136.h"
#include "spider.h"

static const int TABLEAU_SIZE = 10;
static const int DECK_SIZE = 13;
static const int NUM_DECK = 8;

static void turn_up(struct spider *s);
static void complete(struct spider *s);

// spider_init_stacks(s) initializes all
// the stacks in the spider struct. 
// requires: s is a valid pointer.
// effects: mutates *s
static void spider_init_stacks(struct spider *s) {
  stack_init(&s->stock);
  for (int i = 0; i < TABLEAU_SIZE; ++i) {
    stack_init(&s->tableau_up[i]);
    stack_init(&s->tableau_down[i]);
  }
}

// get_random_card(card_count) repeatedly generates
// a random card value from 1 to 13 until 
// the number of cards left for that value is 
// positive according to card_count.
// requires: card_count is a valid pointer.
//           length of card_count is 13.
static int get_random_card(int card_count[]) {
  assert(card_count);

  int card_random = rand() % DECK_SIZE + 1;
  while (card_count[card_random - 1] == 0) {
    card_random = rand() % DECK_SIZE + 1;
  }
  return card_random;
}

// init_tableau(s,index,num_down,num_up,card_count)
// initialize the index-th tableau randomly by 
// putting num_down cards in the face-down pile and
// num_up cards in the face-up pile.
// requires: s and card_count are valid pointers.
//           0 <= index <= 9
//           4 <= num_down <= 5
//           num_up == 1
//           length of card_count is 13.
// effects: mutates *s
static void init_tableau(struct spider *s, 
                         int index, 
                         int num_down, 
                         int num_up, 
                         int card_count[]) {
  assert(s);
  assert(card_count);
  assert(0 <= index);
  assert(index <= 9);
  assert(num_down == 4 || num_down == 5);
  assert(num_up == 1);

  for (int j = 0; j <= num_down - 1; ++j) {
    int card_random = get_random_card(card_count);
    stack_push(card_random, &s->tableau_down[index]);
    --card_count[card_random - 1];
  }
  for (int j = 0; j <= num_up - 1; ++j) {
    int card_random = get_random_card(card_count);
    stack_push(card_random, &s->tableau_up[index]);
    --card_count[card_random - 1];
  }
}

void spider_init_random(struct spider *s, 
                        int seed) {
  assert(s);

  const int STOCK_SIZE_INITIAL = 50;

  int card_count[13];
  for (int i = 0; i < DECK_SIZE; ++i) {
    card_count[i] = 8;
  }

  srand(seed);

  // initialize the stacks
  spider_init_stacks(s); 

  // init stock pile
  s->stock_size = STOCK_SIZE_INITIAL;
  for (int i = 0; i < STOCK_SIZE_INITIAL; ++i) {
    int card_random = get_random_card(card_count);
    stack_push(card_random, &s->stock);
    --card_count[card_random - 1];
  }

  // init the tableaus
  for (int i = 0; i < 4; ++i) {
    init_tableau(s, i, 5, 1, card_count);
  }
  for (int i = 4; i <= TABLEAU_SIZE - 1; ++i) {
    init_tableau(s, i, 4, 1, card_count);
  }

  s->comp_decks = 0;
}

void spider_init_custom(struct spider *s) {
  assert(s);

  int card_read = -1;

  // initialize the stacks
  spider_init_stacks(s); 

  // init stock pile
  s->stock_size = 0;
  int r = read_card(&card_read);
  int count = 0;
  while (!r) {
    ++count;
    ++(s->stock_size);
    stack_push(card_read, &s->stock);
    r = read_card(&card_read);
  }

  // init the tableaus
  for (int i = 0; i <= TABLEAU_SIZE - 1; ++i) {

    int r = read_card(&card_read);
    while (!r) {
      stack_push(card_read, &s->tableau_down[i]);
      r = read_card(&card_read);
    }

    r = read_card(&card_read);
    while (!r) {
      stack_push(card_read, &s->tableau_up[i]);
      r = read_card(&card_read);
    }

  }

  scanf("%d", &s->comp_decks);
}

bool spider_has_won(const struct spider *s) {
  assert(s);

  return (s->comp_decks == NUM_DECK);
}

//=========================================================
// The function implemnetations above are provided.  
// Do not modify them.
//=========================================================

//=========================================================
// Implement the functions below.
//=========================================================

//see spider.h for details
bool read_card(int *card_read) {
  assert(card_read);
  char c = 0;
  scanf(" %c", &c);
  if (c == 'E') return true;
  if (c == '1') {
    scanf("%c", &c);
    if (c == '0') {
      *card_read = 10;
      return false;
    }
  }
  if ((c >= '2') && (c <= '9')) {
    *card_read = c - '2' + 2;
    return false;
  }
  if (c == 'A') {
    *card_read = 1;
    return false;
  }
  if (c == 'J') {
    *card_read = 11;
    return false;
  }
  if (c == 'Q') {
    *card_read = 12;
    return false;
  }
  if (c == 'K') {
    *card_read = 13;
    return false;
  }
  printf("Error reading in a card.\n");
  exit(EXIT_FAILURE);
}


//see spider.h for details
void spider_print(struct spider *s) {
  assert(s);
  struct stack temp = {};
  stack_init(&temp);
  printf("The number of cards left in the stock pile: %d\n", s->stock_size);
  printf("The number of completed decks: %d\n", s->comp_decks);
  for (int i = 0; i < TABLEAU_SIZE; i++) {
    printf("%d:", i);
    while (!stack_is_empty(&((s->tableau_down)[i]))) {
      printf(" _");
      stack_push(stack_pop(&((s->tableau_down)[i])), &temp);
    }
    while (!stack_is_empty(&temp)) {
      stack_push(stack_pop(&temp), &((s->tableau_down)[i]));
    }
    while (!stack_is_empty(&((s->tableau_up)[i]))) {
      stack_push(stack_pop(&((s->tableau_up)[i])), &temp);
    }
    while (!stack_is_empty(&temp)) {
      int a = stack_top(&temp);
      if (a == 1) printf(" %c", 'A');
      else if (a == 11) printf(" %c", 'J');
        else if (a == 12) printf(" %c", 'Q');
        else if (a == 13) printf(" %c", 'K');
        else printf(" %d", a);
        stack_push(stack_pop(&temp), &((s->tableau_up)[i]));
        }
    printf("\n");
  }
}


void spider_print_faceup(struct spider *s) {
  // Replace the code below with your implementation.
  printf("spider_print_faceup has not been implemented yet.\n");
}



//see spider.h for details
void spider_get_stock(struct spider *s) {
  assert(s);
  if (stack_is_empty(&(s->stock))) {
    printf("Error getting cards from the stock.\n");
    return;
  }
  for (int i = 0; i < TABLEAU_SIZE; i++) {
    if (stack_is_empty(&((s->tableau_up)[i]))) {
      printf("Error getting cards from the stock.\n");
      return;
    }
  }
  for (int i = 0; i < TABLEAU_SIZE; i++) {
    stack_push(stack_pop(&(s->stock)), &((s->tableau_up)[i]));
    s->stock_size -= 1;
  }
  complete(s);
  turn_up(s);
}

// spider_find_seq(s_src_up,temp,card) attempts
// to find a descending sequence of cards starting 
// with the provided card on top of s_src_up.  
// If such a sequence does not exist, 
// this function finds the longest descending sequence
// of cards on top of s_src_up.  After finding
// such a sequence, the function moves this sequence
// to temp in reverse order.  Then, the function
// returns true if the descending sequence ends with 
// the provided card.  It returns false otherwise.
// requires: s_src_up is non-empty.
//           s_src_up and temp are valid pointers.
//           1 <= card <= 13
//effects: may mutate *s_src_up and *temp
static bool spider_find_seq(struct stack *s_src_up, 
                            struct stack *temp,
                            int card) {
  assert(s_src_up);
  assert(temp);
  assert(card >= 1);
  assert(card <= 13);
  assert(!stack_is_empty(s_src_up));
  if (stack_top(s_src_up) <= card) {
    stack_push(stack_pop(s_src_up), temp);
    while ((!stack_is_empty(s_src_up)) 
           && ((stack_top(s_src_up) - stack_top(temp)) == 1) 
           && (stack_top(s_src_up) <= card)) {
      stack_push(stack_pop(s_src_up), temp);
    }
    if (card == stack_top(temp)) return true;
    return false;
  } else {
    stack_push(stack_pop(s_src_up), temp);
    while ((!stack_is_empty(s_src_up))
           && ((stack_top(s_src_up) - stack_top(temp)) == 1)) {
      stack_push(stack_pop(s_src_up), temp);
    }
    return false;
  }
}

//see spider.h for details
void spider_test_find_seq(void) {
  struct stack src = {};
  stack_init(&src);
  struct stack temp = {};
  stack_init(&temp);
  int card = 0;

  // Marmoset public test
  stack_push(1, &src);
  card = 1;
  bool found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 1);
  assert(stack_is_empty(&src));
  assert(stack_is_empty(&temp));

  // The first example
  stack_push(5, &src);
  stack_push(4, &src);
  stack_push(3, &src);
  stack_push(2, &src);
  card = 2;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 2);
  assert(stack_is_empty(&temp));
  assert(stack_pop(&src) == 3);
  assert(stack_pop(&src) == 4);
  assert(stack_pop(&src) == 5);
  assert(stack_is_empty(&src));

  // The second example
  stack_push(5, &src);
  stack_push(4, &src);
  stack_push(3, &src);
  stack_push(2, &src);
  card = 5;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 5);
  assert(stack_pop(&temp) == 4);
  assert(stack_pop(&temp) == 3);
  assert(stack_pop(&temp) == 2);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  // Add your own tests below

  stack_push(4, &src);
  stack_push(3, &src);
  stack_push(2, &src);
  stack_push(1, &src);
  card = 7;
  found = spider_find_seq(&src, &temp, card);
  assert(!found);
  assert(stack_pop(&temp) == 4);
  assert(stack_pop(&temp) == 3);
  assert(stack_pop(&temp) == 2);
  assert(stack_pop(&temp) == 1);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(13, &src);
  stack_push(12, &src);
  stack_push(11, &src);
  card = 2;
  found = spider_find_seq(&src, &temp, card);
  assert(!found);
  assert(stack_pop(&temp) == 13);
  assert(stack_pop(&temp) == 12);
  assert(stack_pop(&temp) == 11);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(13, &src);
  stack_push(12, &src);
  stack_push(11, &src);
  stack_push(10, &src);
  card = 10;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 10);
  assert(stack_pop(&src) == 11);
  assert(stack_pop(&src) == 12);
  assert(stack_pop(&src) == 13);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(5, &src);
  stack_push(4, &src);
  stack_push(2, &src);
  stack_push(1, &src);
  card = 3;
  found = spider_find_seq(&src, &temp, card);
  assert(!found);
  assert(stack_pop(&temp) == 2);
  assert(stack_pop(&temp) == 1);
  assert(stack_pop(&src) == 4);
  assert(stack_pop(&src) == 5);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(1, &src);
  stack_push(3, &src);
  stack_push(2, &src);
  stack_push(1, &src);
  card = 3;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 3);
  assert(stack_pop(&temp) == 2);
  assert(stack_pop(&temp) == 1);
  assert(stack_pop(&src) == 1);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(1, &src);
  stack_push(3, &src);
  stack_push(2, &src);
  stack_push(1, &src);
  card = 4;
  found = spider_find_seq(&src, &temp, card);
  assert(!found);
  assert(stack_pop(&temp) == 3);
  assert(stack_pop(&temp) == 2);
  assert(stack_pop(&temp) == 1);
  assert(stack_pop(&src) == 1);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(13, &src);
  stack_push(12, &src);
  card = 13;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 13);
  assert(stack_pop(&temp) == 12);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(1, &src);
  stack_push(1, &src);
  card = 1;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 1);
  assert(stack_pop(&src) == 1);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(1, &src);
  stack_push(2, &src);
  stack_push(3, &src);
  card = 2;
  found = spider_find_seq(&src, &temp, card);
  assert(!found);
  assert(stack_pop(&temp) == 3);
  assert(stack_pop(&src) == 2);
  assert(stack_pop(&src) == 1);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(13, &src);
  card = 1;
  found = spider_find_seq(&src, &temp, card);
  assert(!found);
  assert(stack_pop(&temp) == 13);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(13, &src);
  card = 13;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 13);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

  stack_push(13, &src);
  stack_push(12, &src);
  stack_push(11, &src);
  stack_push(10, &src);
  stack_push(9, &src);
  stack_push(8, &src);
  stack_push(7, &src);
  stack_push(6, &src);
  stack_push(5, &src);
  stack_push(4, &src);
  stack_push(3, &src);
  stack_push(2, &src);
  stack_push(1, &src);
  card = 13;
  found = spider_find_seq(&src, &temp, card);
  assert(found);
  assert(stack_pop(&temp) == 13);
  assert(stack_pop(&temp) == 12);
  assert(stack_pop(&temp) == 11);
  assert(stack_pop(&temp) == 10);
  assert(stack_pop(&temp) == 9);
  assert(stack_pop(&temp) == 8);
  assert(stack_pop(&temp) == 7);
  assert(stack_pop(&temp) == 6);
  assert(stack_pop(&temp) == 5);
  assert(stack_pop(&temp) == 4);
  assert(stack_pop(&temp) == 3);
  assert(stack_pop(&temp) == 2);
  assert(stack_pop(&temp) == 1);
  assert(stack_is_empty(&temp));
  assert(stack_is_empty(&src));

}


//turn_up(s): if there is no face-up card in a tableau in 
//s, turns the top card in tableau_down face-up.
//requires: s is not null
//effects: may mutate *s
static void turn_up(struct spider *s) {
  assert(s);
  for (int i = 0; i < TABLEAU_SIZE; i++) {
    if (stack_is_empty(&((s->tableau_up)[i])) 
        && (!stack_is_empty(&((s->tableau_down)[i])))) {
      stack_push(stack_pop(&((s->tableau_down)[i])), 
                 &((s->tableau_up)[i]));
    }
  }
}

//complete(s): if a complete deck is face-up and on top of 
//a tableau pile, removes from the tableau_up and increases 
//the number of the completed decks.
//requires: s is not null
//effects: may mutate *s
static void complete(struct spider *s) {
  assert(s);
  bool is_comp = true;
  struct stack temp = {};
  stack_init(&temp);
  for (int i = 0; i < TABLEAU_SIZE; i++) {
    for (int j = 1; j <= DECK_SIZE; j++) {
      if (stack_is_empty(&((s->tableau_up)[i]))) {
        is_comp = false;
        break;
      }
      if (stack_top(&((s->tableau_up)[i])) != j) {
        is_comp = false;
        break;
      }
      stack_push(stack_pop(&((s->tableau_up)[i])), &temp);
    }
    if (is_comp) {
      s->comp_decks += 1;
      turn_up(s);
      while (!stack_is_empty(&temp)) {
        stack_pop(&temp);
      }
      continue;
    }
    while (!stack_is_empty(&temp)) {
      stack_push(stack_pop(&temp), &((s->tableau_up)[i]));
    }
    is_comp = true;
  }
}

//see spider.h for details
void spider_move(struct spider *s, 
                 int src, int card, int dest) {
  assert(s);
  assert(src >= 0);
  assert(src <= 9);
  assert(dest >= 0);
  assert(dest <= 9);
  assert(card >= 1);
  assert(card <= 13);
  struct stack temp = {};
  stack_init(&temp);
  if (src == dest) return;
  if (stack_is_empty(&((s->tableau_up)[src]))) {
    printf("Invalid move.\n");
    return;
  }
  if ((spider_find_seq(&((s->tableau_up)[src]), 
                       &temp, 
                       card)) 
      && (stack_is_empty(&((s->tableau_up)[dest]))
          || (stack_top(&((s->tableau_up)[dest])) - card == 1))) {
    while (!stack_is_empty(&temp)) {
      stack_push(stack_pop(&temp), &((s->tableau_up)[dest]));
    }
    complete(s);
    turn_up(s);
    return;
  }
  printf("Invalid move.\n");
  while (!stack_is_empty(&temp)) {
    stack_push(stack_pop(&temp), &((s->tableau_up)[src]));
  }
}


