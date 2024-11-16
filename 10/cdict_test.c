/*
 * dict_test.c
 * 
 * Test code for ISSE dictionary
 *
 * Author: John Bosco Thuo
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "cdict.h"

// Checks that value is true; if not, prints a failure message and
// returns 0 from this function
#define test_assert(value) {                                            \
    if (!(value)) {                                                     \
      printf("FAIL %s[%d]: %s\n", __FUNCTION__, __LINE__, #value);      \
      goto test_error;                                                  \
    }                                                                   \
  }

typedef struct {
  const char *city;
  const char *team;
} team_data_t;

team_data_t team_data[] = { {"Atlanta", "Hawks"}, {"Boston", "Celtics"},
                            {"Brooklyn", "Nets"}, {"Charlotte", "Hornets"},
                            {"Chicago", "Bulls"}, {"Cleveland", "Cavaliers"},
                            {"Dallas", "Mavericks"}, {"Denver", "Nuggets"},
                            {"Detroit", "Pistons"}, {"Golden State", "Warriors"},
                            {"Houston", "Rockets"}, {"Indiana", "Pacers"},
                            {"Los Angeles", "Lakers"}, {"Memphis", "Grizzlies"},
                            {"Miami", "Heat"}, {"Milwaukee", "Bucks"},
                            {"Minnesota", "Timberwolves"}, {"New Orleans", "Pelicans"},
                            {"New York", "Knicks"}, {"Oklahoma City", "Thunder"},
                            {"Orlando", "Magic"}, {"Philadelphia", "76ers"},
                            {"Phoenix", "Suns"}, {"Portland", "Trail Blazers"},
                            {"Sacramento", "Kings"}, {"San Antonio", "Spurs"},
                            {"Toronto", "Raptors"}, {"Utah", "Jazz"},
                            {"Washington", "Wizards"} };

const int team_data_len = sizeof (team_data) / sizeof(team_data[0]);


/*
 * Demonstrates basic operation of the dictionary. Returns 1 if all
 * tests, pass, 0 otherwise.
 */
int demonstrate_dict()
{
  CDict dict = CD_new();

  test_assert( CD_size(dict) == 0 );
  test_assert( CD_load_factor(dict) == 0.0 );

  CD_store(dict, "Atlanta", "Hawks");
  CD_store(dict, "Boston", "Celtics");
  CD_store(dict, "Los Angeles", "Lakers");
  CD_store(dict, "Denver", "Nuggets");
    
  printf("The Denver team is called the %s\n", CD_retrieve(dict, "Denver"));
  printf("  current load factor: %.2f\n", CD_load_factor(dict) );

  CD_delete(dict, "Boston");
  printf("  current size: %d\n", CD_size(dict));

  printf("Here is the entire dictionary:\n");
  CD_print(dict);

  // overwrite an existing value
  CD_store(dict, "Denver", "Broncos");
  printf("After updating Denver to 'Broncos':\n");
  CD_print(dict);
  
  // add two more elements to force a rehash
  CD_store(dict, "New York", "Knicks");
  CD_store(dict, "Miami", "Head");
  
  printf("After adding New York and Miami:\n");
  CD_print(dict);

  CD_free(dict);
  
  return 1;

 test_error:
  CD_free(dict);
  return 0;
}

// Test extensive insertion and retrieval
int test_all_teams() {
  CDict dict = CD_new();

  // Insert all teams
  for (int i = 0; i < team_data_len; i++) {
    CD_store(dict, team_data[i].city, team_data[i].team);
    test_assert(CD_size(dict) == i + 1);
  }

  // Verify all teams are present
  for (int i = 0; i < team_data_len; i++) {
    test_assert(CD_contains(dict, team_data[i].city));
    test_assert(strcmp(CD_retrieve(dict, team_data[i].city), team_data[i].team) == 0);
  }

  CD_free(dict);
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

// Test delete operations
int test_delete_operations() {
  CDict dict = CD_new();
  
  // Insert some teams
  CD_store(dict, team_data[0].city, team_data[0].team);
  CD_store(dict, team_data[1].city, team_data[1].team);
  CD_store(dict, team_data[2].city, team_data[2].team);
  
  // Test size
  test_assert(CD_size(dict) == 3);
  
  // Delete middle element
  CD_delete(dict, team_data[1].city);
  test_assert(CD_size(dict) == 2);
  test_assert(!CD_contains(dict, team_data[1].city));
  test_assert(CD_contains(dict, team_data[0].city));
  test_assert(CD_contains(dict, team_data[2].city));
  
  // Try to delete non-existent element
  CD_delete(dict, "NonExistentCity");
  test_assert(CD_size(dict) == 2);
  
  // Delete remaining elements
  CD_delete(dict, team_data[0].city);
  CD_delete(dict, team_data[2].city);
  test_assert(CD_size(dict) == 0);
  
  CD_free(dict);
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

// Test overwrite behavior
int test_overwrite() {
  CDict dict = CD_new();
  
  CD_store(dict, "Denver", "Nuggets");
  test_assert(CD_size(dict) == 1);
  test_assert(strcmp(CD_retrieve(dict, "Denver"), "Nuggets") == 0);
  
  CD_store(dict, "Denver", "Broncos");
  test_assert(CD_size(dict) == 1);
  test_assert(strcmp(CD_retrieve(dict, "Denver"), "Broncos") == 0);
  
  CD_free(dict);
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

// Test foreach functionality
typedef struct {
  int count;
  bool found_target;
  const char* target_city;
} foreach_data_t;

static void find_city_callback(CDictKeyType key, CDictValueType value, void *data) {
  foreach_data_t *cb_data = (foreach_data_t*)data;
  cb_data->count++;
  if (strcmp(key, cb_data->target_city) == 0) {
    cb_data->found_target = true;
  }
}

int test_foreach() {
  CDict dict = CD_new();
  
  // Add some teams
  for (int i = 0; i < 5; i++) {
    CD_store(dict, team_data[i].city, team_data[i].team);
  }
  
  foreach_data_t cb_data = {0, false, "Boston"};
  CD_foreach(dict, find_city_callback, &cb_data);
  
  test_assert(cb_data.count == 5);
  test_assert(cb_data.found_target == true);
  
  CD_free(dict);
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

// Test edge cases
int test_edge_cases() {
  CDict dict = CD_new();
  
  // Test retrieving from empty dictionary
  test_assert(CD_retrieve(dict, "NonExistent") == INVALID_VALUE);
  
  // Test deleting from empty dictionary
  CD_delete(dict, "NonExistent");
  test_assert(CD_size(dict) == 0);
  
  // Test empty string as key
  CD_store(dict, "", "EmptyKey");
  test_assert(CD_contains(dict, ""));
  test_assert(strcmp(CD_retrieve(dict, ""), "EmptyKey") == 0);
  
  CD_free(dict);
  return 1;

test_error:
  CD_free(dict);
  return 0;
}

int main()
{
  int passed = 0;
  int num_tests = 0;

  num_tests++; passed += demonstrate_dict();
  num_tests++; passed += test_all_teams();
  num_tests++; passed += test_delete_operations();
  num_tests++; passed += test_overwrite();
  num_tests++; passed += test_foreach();
  num_tests++; passed += test_edge_cases();

  printf("Passed %d/%d test cases\n", passed, num_tests);
  fflush(stdout);
  return 0;
}
