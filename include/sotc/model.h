#ifndef INCLUDE_SOTC_MODEL_H_
#define INCLUDE_SOTC_MODEL_H_

#include <stdint.h>
#include <uuid/uuid.h>
#include <stdbool.h>
#include <json.h>

enum sotc_transition_kind
{
    SOTC_TRANSITION_EXTERNAL,
    SOTC_TRANSITION_INTERNAL,
    SOTC_TRANSITION_LOCAL,
};

enum sotc_state_kind
{
    SOTC_STATE_SIMPLE,
    SOTC_STATE_INIT,
    SOTC_STATE_FINAL,
    SOTC_STATE_SHALLOW_HISTORY,
    SOTC_STATE_DEEP_HISTORY,
    SOTC_STATE_EXIT_POINT,
    SOTC_STATE_ENTRY_POINT,
    SOTC_STATE_JOIN,
    SOTC_STATE_FORK,
    SOTC_STATE_CHOICE,
    SOTC_STATE_JUNCTION,
    SOTC_STATE_TERMINATE,
};

enum sotc_action_kind
{
    SOTC_ACTION_ACTION,
    SOTC_ACTION_GUARD,
    SOTC_ACTION_ENTRY,
    SOTC_ACTION_EXIT,
};

struct sotc_action
{
    uuid_t id;
    const char *name;
    enum sotc_action_kind kind;
    struct sotc_action *next;
};

struct sotc_action_ref
{
    struct sotc_action *act;
    struct sotc_action_ref *next;
};

struct sotc_transition
{
    uuid_t id;
    const char *name;
    const char *trigger_name;
    int trigger;
    enum sotc_transition_kind kind;
    struct sotc_action_ref *action;
    struct sotc_action_ref *guard;
    struct sotc_state *source;
    struct sotc_state *dest;
    struct sotc_transition *next;
};

struct sotc_region
{
    uuid_t id;
    const char *name;
    bool off_page;
    double h;
    bool focus;
    struct sotc_state *state;
    struct sotc_transition *transition;
    struct sotc_state *parent_state;
    struct sotc_state *last_state;
    struct sotc_region *next;
};

struct sotc_state
{
    uuid_t id;
    const char *name;
    double x;
    double y;
    double w;
    double h;
    double region_y_offset;
    bool focus;
    enum sotc_state_kind kind;
    struct sotc_action_ref *entries;
    struct sotc_action_ref *exits;
    struct sotc_region *regions;
    struct sotc_region *parent_region;
    struct sotc_region *last_region;
    struct sotc_state *next;
};

struct sotc_model
{
    json_object *jroot;
    struct sotc_region *root;
    struct sotc_action *entries; /* Global list of entry functions */
    struct sotc_action *exits;   /* Global list of exit functions  */
    struct sotc_action *guards;  /* Global list of guard functions */
    struct sotc_action *actions; /* Global list of action functions */
    const char *name;
    int version;
};

int sotc_model_load(const char *filename, struct sotc_model **model);
int sotc_model_create(struct sotc_model **model, const char *name);
int sotc_model_write(const char *filename, struct sotc_model *model);
int sotc_model_free(struct sotc_model *model);
int sotc_model_add_action(struct sotc_model *model,
                          enum sotc_action_kind kind,
                          const char *name,
                          struct sotc_action **act);
int sotc_model_delete_action(struct sotc_model *model, uuid_t id);
int sotc_model_get_action(struct sotc_model *model, uuid_t id,
                          enum sotc_action_kind kind,
                          struct sotc_action **result);

struct sotc_action* sotc_model_get_entries(struct sotc_model *model);
struct sotc_action* sotc_model_get_exits(struct sotc_model *model);
struct sotc_action* sotc_model_get_guards(struct sotc_model *model);
struct sotc_action* sotc_model_get_actions(struct sotc_model *model);

/* Region api */
int sotc_add_region(struct sotc_state *state, bool off_page,
                     struct sotc_region **out);
int sotc_set_region_name(struct sotc_region *region, const char *name);

int sotc_region_append_state(struct sotc_region *r, struct sotc_state *state);

int sotc_region_serialize(struct sotc_region *region, json_object *state,
                         json_object **out);

int sotc_region_deserialize(json_object *j_r, struct sotc_state *state,
                            struct sotc_region **out);

int sotc_region_set_height(struct sotc_region *r, double h);
int sotc_region_get_height(struct sotc_region *r, double *h);

/* State api */

int sotc_add_state(struct sotc_region *region, const char *name,
                    struct sotc_state **out);

int sotc_state_add_entry(struct sotc_model *model,
                         struct sotc_state *state,
                         uuid_t id);

int sotc_state_add_exit(struct sotc_model *model,
                        struct sotc_state *state,
                        uuid_t id);

int sotc_state_delete_entry(struct sotc_state *state, uuid_t id);
int sotc_state_delete_exit(struct sotc_state *state, uuid_t id);

int sotc_state_get_entries(struct sotc_state *state,
                           struct sotc_action_ref **list);
int sotc_state_get_exits(struct sotc_state *state,
                         struct sotc_action_ref **list);

int sotc_add_transition(struct sotc_transition **transition,
                       struct sotc_state *source,
                       struct sotc_state *dest);

int sotc_state_append_region(struct sotc_state *state, struct sotc_region *r);

int sotc_state_serialize(struct sotc_state *state, json_object *region,
                        json_object **out);

int sotc_state_deserialize(struct sotc_model *model,
                           json_object *j_state,
                           struct sotc_region *region,
                           struct sotc_state **out);

int sotc_state_set_size(struct sotc_state *s, double x, double y);
int sotc_state_set_xy(struct sotc_state *s, double x, double y);

int sotc_state_get_size(struct sotc_state *s, double *x, double *y);
int sotc_state_get_xy(struct sotc_state *s, double *x, double *y);

const char * sotc_model_name(struct sotc_model *model);

#endif  // INCLUDE_SOTC_MODEL_H_
