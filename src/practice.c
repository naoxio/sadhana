#include "practice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <unistd.h>

#define RECIPES_URL "https://github.com/naoxio/sadhana/archive/refs/heads/main.zip"
#define RECIPES_FILE "sadhana_recipes.zip"
#define RECIPES_DIR "~/.local/share/sadhana/recipes"

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int download_recipes(void) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(RECIPES_FILE, "wb");
        if (fp == NULL) {
            fprintf(stderr, "Failed to open file for writing\n");
            return 1;
        }
        curl_easy_setopt(curl, CURLOPT_URL, RECIPES_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        return (res == CURLE_OK) ? 0 : 1;
    }
    return 1;
}

int extract_recipes(void) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "unzip -o %s -d %s", RECIPES_FILE, RECIPES_DIR);
    return system(cmd);
}

int initialize_recipes(void) {
    mkdir(RECIPES_DIR, 0755);
    if (download_recipes() != 0) {
        fprintf(stderr, "Failed to download recipes\n");
        return 1;
    }
    if (extract_recipes() != 0) {
        fprintf(stderr, "Failed to extract recipes\n");
        return 1;
    }
    remove(RECIPES_FILE);
    return 0;
}

int update_recipes(void) {
    return initialize_recipes();  // For simplicity, we're just re-initializing
}

int list_recipes(void) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ls %s/*.yaml", RECIPES_DIR);
    return system(cmd);
}

int run_practice(const char *practice_name) {
    // This is a placeholder. You'll need to implement the actual logic
    // to run a practice based on its YAML file.
    printf("Running practice: %s\n", practice_name);
    return 0;
}

int configure_practice(const char *practice_name) {
    // This is a placeholder. You'll need to implement the actual logic
    // to configure a practice.
    printf("Configuring practice: %s\n", practice_name);
    return 0;
}
