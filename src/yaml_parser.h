// yaml_parser.h

#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include <yaml.h>
#include "core.h"

int parse_practice_yaml(const char *file_path, Practice *practice);
void parse_exercise_structure(yaml_document_t *document, yaml_node_t *node, Practice *practice);
void parse_rounds(yaml_document_t *document, yaml_node_t *node, Practice *practice);
void parse_phases(yaml_document_t *document, yaml_node_t *node, Practice *practice);
void parse_phase(yaml_document_t *document, yaml_node_t *node, Phase *phase);
void parse_repeats(yaml_document_t *document, yaml_node_t *node, Phase *phase);
void parse_steps(yaml_document_t *document, yaml_node_t *node, Phase *phase);
void parse_step(yaml_document_t *document, yaml_node_t *node, Step *step);
void parse_duration(yaml_document_t *document, yaml_node_t *node, Step *step);
void parse_sounds(yaml_document_t *document, yaml_node_t *node, Step *step);

#endif // YAML_PARSER_H
