// yaml_parser.c

#include <stdio.h>
#include <string.h>
#include "yaml_parser.h"

int parse_practice_yaml(const char *file_path, Practice *practice) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open practice file: %s\n", file_path);
        return 1;
    }

    yaml_parser_t parser;
    yaml_document_t document;

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Failed to initialize parser\n");
        fclose(file);
        return 1;
    }

    yaml_parser_set_input_file(&parser, file);

    if (!yaml_parser_load(&parser, &document)) {
        fprintf(stderr, "Failed to parse YAML document\n");
        yaml_parser_delete(&parser);
        fclose(file);
        return 1;
    }

    yaml_node_t *root = yaml_document_get_root_node(&document);
    if (root->type != YAML_MAPPING_NODE) {
        fprintf(stderr, "Root is not a mapping node\n");
        yaml_document_delete(&document);
        yaml_parser_delete(&parser);
        fclose(file);
        return 1;
    }

    for (yaml_node_pair_t *pair = root->data.mapping.pairs.start; pair < root->data.mapping.pairs.top; pair++) {
        yaml_node_t *key = yaml_document_get_node(&document, pair->key);
        yaml_node_t *value = yaml_document_get_node(&document, pair->value);

        if (key->type == YAML_SCALAR_NODE) {
            const char *key_str = (const char *)key->data.scalar.value;

            if (strcmp(key_str, "short_name") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->short_name, (const char *)value->data.scalar.value, sizeof(practice->short_name) - 1);
            } else if (strcmp(key_str, "name") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->name, (const char *)value->data.scalar.value, sizeof(practice->name) - 1);
            } else if (strcmp(key_str, "description") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->description, (const char *)value->data.scalar.value, sizeof(practice->description) - 1);
            } else if (strcmp(key_str, "version") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->version, (const char *)value->data.scalar.value, sizeof(practice->version) - 1);
            } else if (strcmp(key_str, "schematic_version") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->schematic_version, (const char *)value->data.scalar.value, sizeof(practice->schematic_version) - 1);
            } else if (strcmp(key_str, "category") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->category, (const char *)value->data.scalar.value, sizeof(practice->category) - 1);
            } else if (strcmp(key_str, "origin") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->origin, (const char *)value->data.scalar.value, sizeof(practice->origin) - 1);
            } else if (strcmp(key_str, "difficulty") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(practice->difficulty, (const char *)value->data.scalar.value, sizeof(practice->difficulty) - 1);
            } else if (strcmp(key_str, "exercise_structure") == 0 && value->type == YAML_MAPPING_NODE) {
                parse_exercise_structure(&document, value, practice);
            }
        }
    }

    yaml_document_delete(&document);
    yaml_parser_delete(&parser);
    fclose(file);

    return 0;
}

void parse_exercise_structure(yaml_document_t *document, yaml_node_t *node, Practice *practice) {
    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
        yaml_node_t *key = yaml_document_get_node(document, pair->key);
        yaml_node_t *value = yaml_document_get_node(document, pair->value);

        if (key->type == YAML_SCALAR_NODE) {
            const char *key_str = (const char *)key->data.scalar.value;

            if (strcmp(key_str, "rounds") == 0 && value->type == YAML_MAPPING_NODE) {
                parse_rounds(document, value, practice);
            } else if (strcmp(key_str, "phases") == 0 && value->type == YAML_SEQUENCE_NODE) {
                parse_phases(document, value, practice);
            }
        }
    }
}

void parse_rounds(yaml_document_t *document, yaml_node_t *node, Practice *practice) {
    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
        yaml_node_t *key = yaml_document_get_node(document, pair->key);
        yaml_node_t *value = yaml_document_get_node(document, pair->value);

        if (key->type == YAML_SCALAR_NODE && value->type == YAML_SCALAR_NODE) {
            const char *key_str = (const char *)key->data.scalar.value;
            if (strcmp(key_str, "default") == 0) {
                practice->total_rounds = atoi((const char *)value->data.scalar.value);
            }
        }
    }
}

void parse_phases(yaml_document_t *document, yaml_node_t *node, Practice *practice) {
    practice->phase_count = 0;
    for (yaml_node_item_t *item = node->data.sequence.items.start; item < node->data.sequence.items.top; item++) {
        yaml_node_t *phase_node = yaml_document_get_node(document, *item);
        if (phase_node->type == YAML_MAPPING_NODE && practice->phase_count < MAX_PHASES) {
            parse_phase(document, phase_node, &practice->phases[practice->phase_count]);
            practice->phase_count++;
        }
    }
}

void parse_phase(yaml_document_t *document, yaml_node_t *node, Phase *phase) {
    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
        yaml_node_t *key = yaml_document_get_node(document, pair->key);
        yaml_node_t *value = yaml_document_get_node(document, pair->value);

        if (key->type == YAML_SCALAR_NODE) {
            const char *key_str = (const char *)key->data.scalar.value;

            if (strcmp(key_str, "name") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(phase->name, (const char *)value->data.scalar.value, sizeof(phase->name) - 1);
            } else if (strcmp(key_str, "repeats") == 0 && value->type == YAML_MAPPING_NODE) {
                parse_repeats(document, value, phase);
            } else if (strcmp(key_str, "steps") == 0 && value->type == YAML_SEQUENCE_NODE) {
                parse_steps(document, value, phase);
            }
        }
    }
}

void parse_repeats(yaml_document_t *document, yaml_node_t *node, Phase *phase) {
    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
        yaml_node_t *key = yaml_document_get_node(document, pair->key);
        yaml_node_t *value = yaml_document_get_node(document, pair->value);

        if (key->type == YAML_SCALAR_NODE && value->type == YAML_SCALAR_NODE) {
            const char *key_str = (const char *)key->data.scalar.value;
            if (strcmp(key_str, "default") == 0) {
                phase->repeats = atoi((const char *)value->data.scalar.value);
            }
        }
    }
}

void parse_steps(yaml_document_t *document, yaml_node_t *node, Phase *phase) {
    phase->step_count = 0;
    for (yaml_node_item_t *item = node->data.sequence.items.start; item < node->data.sequence.items.top; item++) {
        yaml_node_t *step_node = yaml_document_get_node(document, *item);
        if (step_node->type == YAML_MAPPING_NODE && phase->step_count < MAX_STEPS) {
            parse_step(document, step_node, &phase->steps[phase->step_count]);
            phase->step_count++;
        }
    }
}

void parse_step(yaml_document_t *document, yaml_node_t *node, Step *step) {
    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
        yaml_node_t *key = yaml_document_get_node(document, pair->key);
        yaml_node_t *value = yaml_document_get_node(document, pair->value);

        if (key->type == YAML_SCALAR_NODE) {
            const char *key_str = (const char *)key->data.scalar.value;

            if (strcmp(key_str, "name") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(step->name, (const char *)value->data.scalar.value, sizeof(step->name) - 1);
            } else if (strcmp(key_str, "action") == 0 && value->type == YAML_SCALAR_NODE) {
                strncpy(step->action, (const char *)value->data.scalar.value, sizeof(step->action) - 1);
            } else if (strcmp(key_str, "duration") == 0 && value->type == YAML_MAPPING_NODE) {
                parse_duration(document, value, step);
            } else if (strcmp(key_str, "sounds") == 0 && value->type == YAML_SEQUENCE_NODE) {
                parse_sounds(document, value, step);
            }
        }
    }
}

void parse_duration(yaml_document_t *document, yaml_node_t *node, Step *step) {
    for (yaml_node_pair_t *pair = node->data.mapping.pairs.start; pair < node->data.mapping.pairs.top; pair++) {
        yaml_node_t *key = yaml_document_get_node(document, pair->key);
        yaml_node_t *value = yaml_document_get_node(document, pair->value);

        if (key->type == YAML_SCALAR_NODE) {
            const char *key_str = (const char *)key->data.scalar.value;

            if (strcmp(key_str, "type") == 0 && value->type == YAML_SCALAR_NODE) {
                const char *type_str = (const char *)value->data.scalar.value;
                if (strcmp(type_str, "fixed") == 0) {
                    step->is_variable = 0;
                } else if (strcmp(type_str, "variable") == 0) {
                    step->is_variable = 1;
                }
            } else if (strcmp(key_str, "value") == 0 && value->type == YAML_SCALAR_NODE) {
                step->duration = atoi((const char *)value->data.scalar.value);
            }
        }
    }
}

void parse_sounds(yaml_document_t *document, yaml_node_t *node, Step *step) {
    step->sound_count = 0;
    for (yaml_node_item_t *item = node->data.sequence.items.start; item < node->data.sequence.items.top; item++) {
        yaml_node_t *sound_node = yaml_document_get_node(document, *item);
        if (sound_node->type == YAML_SCALAR_NODE && step->sound_count < MAX_SOUNDS) {
            strncpy(step->sounds[step->sound_count], (const char *)sound_node->data.scalar.value, sizeof(step->sounds[0]) - 1);
            step->sound_count++;
        }
    }
}