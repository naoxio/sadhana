// core.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <yaml.h>

#include "miniz.h"
#include "core.h"
#include "yaml_parser.h"

#define SADHANA_HUB_URL "https://github.com/naoxio/sadhana/archive/refs/heads/main.zip"
#define SADHANA_FILE "sadhana-main.zip"
#define SADHANA_DIR "~/.local/share/sadhana"
#define PRACTICES_DIR SADHANA_DIR "/practices"
#define ASSETS_DIR SADHANA_DIR "/assets"
#define I18N_DIR SADHANA_DIR "/i18n"

void get_full_sadhana_dir(char *full_path, size_t size) {
    snprintf(full_path, size, "%s/%s", getenv("HOME"), &SADHANA_DIR[2]);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int download_sadhana_hub(void) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(SADHANA_FILE, "wb");
        if (fp == NULL) {
            fprintf(stderr, "Failed to open file for writing: %s\n", SADHANA_FILE);
            return 1;
        }

        curl_easy_setopt(curl, CURLOPT_URL, SADHANA_HUB_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

        res = curl_easy_perform(curl);
        
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res != CURLE_OK) {
            fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
        return 1;
    }

    return 0;
}

int extract_sadhana_hub(void) {
    mz_zip_archive zip_archive;
    char sadhana_dir[MAX_STRING_LENGTH];
    get_full_sadhana_dir(sadhana_dir, sizeof(sadhana_dir));

    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, SADHANA_FILE, 0)) {
        fprintf(stderr, "Failed to open zip file: %s\n", SADHANA_FILE);
        return 1;
    }

    for (unsigned int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            fprintf(stderr, "Failed to get file stat for file %u\n", i);
            mz_zip_reader_end(&zip_archive);
            return 1;
        }

        char output_path[MAX_STRING_LENGTH];
        snprintf(output_path, sizeof(output_path), "%s/%s", sadhana_dir, file_stat.m_filename + 13); // Skip "sadhana-main/"

        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            mkdir(output_path, 0755);
        } else {
            char *last_slash = strrchr(output_path, '/');
            if (last_slash) {
                *last_slash = '\0';
                mkdir(output_path, 0755);
                *last_slash = '/';
            }

            if (!mz_zip_reader_extract_to_file(&zip_archive, i, output_path, 0)) {
                mz_zip_reader_end(&zip_archive);
                return 1;
            }
        }
    }

    mz_zip_reader_end(&zip_archive);
    return 0;
}

int initialize_or_update_sadhana_hub(void) {
    if (download_sadhana_hub() != 0) {
        return 1;
    }

    if (extract_sadhana_hub() != 0) {
        return 1;
    }

    remove(SADHANA_FILE);
    return 0;
}

int practices_initialized(void) {
    struct stat st = {0};
    char practices_dir[MAX_STRING_LENGTH];
    get_full_sadhana_dir(practices_dir, sizeof(practices_dir));
    strncat(practices_dir, "/practices", sizeof(practices_dir) - strlen(practices_dir) - 1);
    return (stat(practices_dir, &st) != -1);
}

int get_practices(char practices[][MAX_PRACTICE_NAME], int max_practices) {
    char practices_dir[MAX_STRING_LENGTH];
    get_full_sadhana_dir(practices_dir, sizeof(practices_dir));
    strncat(practices_dir, "/practices", sizeof(practices_dir) - strlen(practices_dir) - 1);

    int count = 0;
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    dir = opendir(practices_dir);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open practices directory: %s\n", practices_dir);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL && count < max_practices) {
        char full_path[MAX_STRING_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", practices_dir, entry->d_name);

        if (stat(full_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".yaml") == 0) {
                strncpy(practices[count], entry->d_name, MAX_PRACTICE_NAME - 1);
                practices[count][MAX_PRACTICE_NAME - 1] = '\0';
                char *yaml_ext = strrchr(practices[count], '.');
                if (yaml_ext) *yaml_ext = '\0'; // Remove .yaml extension
                count++;
            }
        }
    }
    closedir(dir);

    return count;
}

int load_practice(const char *practice_name, Practice *practice) {
    char file_path[MAX_STRING_LENGTH];
    char practices_dir[MAX_STRING_LENGTH];
    get_full_sadhana_dir(practices_dir, sizeof(practices_dir));
    snprintf(file_path, sizeof(file_path), "%s/practices/%s.yaml", practices_dir, practice_name);

    return parse_practice_yaml(file_path, practice);
}

int execute_practice(Practice *practice, void (*step_callback)(const Step*, int, int, int, void*), void *user_data) {
    for (int round = 1; round <= practice->total_rounds; round++) {
        for (int phase = 0; phase < practice->phase_count; phase++) {
            Phase *current_phase = &practice->phases[phase];

            for (int rep = 1; rep <= current_phase->repeats; rep++) {
                for (int step = 0; step < current_phase->step_count; step++) {
                    Step *current_step = &current_phase->steps[step];
                    step_callback(current_step, round, phase, rep, user_data);
                }
            }
        }
    }
    return 0;
}

int configure_practice(const char *practice_name, const char *config_key, const char *config_value) {
    char config_path[MAX_STRING_LENGTH];
    char sadhana_dir[MAX_STRING_LENGTH];
    get_full_sadhana_dir(sadhana_dir, sizeof(sadhana_dir));
    snprintf(config_path, sizeof(config_path), "%s/config/%s_config.yaml", sadhana_dir, practice_name);

    yaml_parser_t parser;
    yaml_emitter_t emitter;
    yaml_document_t document;
    FILE *config_file;

    // Try to open existing config file
    config_file = fopen(config_path, "r");
    if (config_file == NULL) {
        // Config file doesn't exist, create a new one
        yaml_document_initialize(&document, NULL, NULL, NULL, 1, 1);
        int root = yaml_document_add_mapping(&document, NULL, YAML_BLOCK_MAPPING_STYLE);
        yaml_document_add_scalar(&document, NULL, (yaml_char_t*)config_key, -1, YAML_PLAIN_SCALAR_STYLE);
        yaml_document_add_scalar(&document, NULL, (yaml_char_t*)config_value, -1, YAML_PLAIN_SCALAR_STYLE);
    } else {
        // Config file exists, update it
        yaml_parser_initialize(&parser);
        yaml_parser_set_input_file(&parser, config_file);

        if (!yaml_parser_load(&parser, &document)) {
            yaml_parser_delete(&parser);
            fclose(config_file);
            return 1;
        }

        yaml_node_t *root = yaml_document_get_root_node(&document);
        if (root->type != YAML_MAPPING_NODE) {
            yaml_document_delete(&document);
            yaml_parser_delete(&parser);
            fclose(config_file);
            return 1;
        }

        // Update or add the config key-value pair
        int found = 0;
        for (yaml_node_pair_t *pair = root->data.mapping.pairs.start; pair < root->data.mapping.pairs.top; pair++) {
            yaml_node_t *key = yaml_document_get_node(&document, pair->key);
            if (key->type == YAML_SCALAR_NODE && strcmp((char*)key->data.scalar.value, config_key) == 0) {
                yaml_document_delete(&document);
                pair->value = yaml_document_add_scalar(&document, NULL, (yaml_char_t*)config_value, -1, YAML_PLAIN_SCALAR_STYLE);
                found = 1;
                break;
            }
        }

        if (!found) {
            yaml_document_append_mapping_pair(&document, 
                root->data.mapping.pairs.start - root->data.mapping.pairs.start, // This will be 0
                yaml_document_add_scalar(&document, NULL, (yaml_char_t*)config_key, -1, YAML_PLAIN_SCALAR_STYLE),
                yaml_document_add_scalar(&document, NULL, (yaml_char_t*)config_value, -1, YAML_PLAIN_SCALAR_STYLE));
        }

        yaml_parser_delete(&parser);
        fclose(config_file);
    }

    // Write the config to file
    config_file = fopen(config_path, "w");
    if (config_file == NULL) {
        yaml_document_delete(&document);
        return 1;
    }

    yaml_emitter_initialize(&emitter);
    yaml_emitter_set_output_file(&emitter, config_file);
    yaml_emitter_dump(&emitter, &document);

    yaml_emitter_delete(&emitter);
    yaml_document_delete(&document);
    fclose(config_file);

    return 0;
}
