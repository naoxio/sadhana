#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "miniz.h"
#include "practice_manager.h"

#define SADHANA_HUB_URL "https://github.com/naoxio/sadhana/archive/refs/heads/main.zip"
#define SADHANA_FILE "sadhana-main.zip"
#define PRACTICES_DIR "~/.local/share/sadhana/practices"

const char* CATEGORY_DIRS[] = {
    "breathing",
    "meditation",
    "yoga",
    "movement",
    "mindfulness"
};

#define NUM_CATEGORIES (sizeof(CATEGORY_DIRS) / sizeof(CATEGORY_DIRS[0]))

void get_full_practices_dir(char *full_path, size_t size) {
    snprintf(full_path, size, "%s/%s", getenv("HOME"), &PRACTICES_DIR[2]);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int download_practices(void) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    long http_code = 0;
    curl_off_t dl_size = 0;

    printf("Debug: Starting download of practices\n");

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(SADHANA_FILE, "wb");
        if (fp == NULL) {
            fprintf(stderr, "Debug: Failed to open file for writing: %s\n", SADHANA_FILE);
            return 1;
        }

        printf("Debug: File opened for writing: %s\n", SADHANA_FILE);

        curl_easy_setopt(curl, CURLOPT_URL, SADHANA_HUB_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); // Fail on HTTP errors

        printf("Debug: Starting curl_easy_perform\n");
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Debug: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &dl_size);
            printf("Debug: Download completed. HTTP code: %ld, Downloaded size: %ld bytes\n", http_code, dl_size);
        }

        curl_easy_cleanup(curl);
        fclose(fp);

        // Check file size after download
        struct stat st;
        if (stat(SADHANA_FILE, &st) == 0) {
            printf("Debug: File size after download: %ld bytes\n", st.st_size);
            if (st.st_size == 0) {
                fprintf(stderr, "Debug: Downloaded file is empty\n");
                return 1;
            }
        } else {
            fprintf(stderr, "Debug: Unable to get file size\n");
            return 1;
        }

        return (res == CURLE_OK) ? 0 : 1;
    }

    fprintf(stderr, "Debug: curl_easy_init() failed\n");
    return 1;
}
int extract_practices(void) {
    mz_zip_archive zip_archive;
    char practices_dir[256];
    get_full_practices_dir(practices_dir, sizeof(practices_dir));

    printf("Debug: Starting extraction of practices\n");
    printf("Debug: Practices directory: %s\n", practices_dir);

    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, SADHANA_FILE, 0)) {
        fprintf(stderr, "Debug: Failed to open zip file: %s\n", SADHANA_FILE);
        return 1;
    }

    printf("Debug: Zip file opened successfully\n");

    for (unsigned int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            fprintf(stderr, "Debug: Failed to get file stat for file %u\n", i);
            mz_zip_reader_end(&zip_archive);
            return 1;
        }

        // Check if the file is in the "practices" directory
        if (strncmp(file_stat.m_filename, "sadhana-main/practices/", 23) == 0) {
            const char *relative_path = file_stat.m_filename + 23;  // Skip "sadhana-main/practices/"
            char output_path[1024];
            snprintf(output_path, sizeof(output_path), "%s/%s", practices_dir, relative_path);

            printf("Debug: Extracting file: %s\n", output_path);

            if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
                mkdir(output_path, 0755);
            } else {
                // Ensure the directory exists
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
    }

    mz_zip_reader_end(&zip_archive);
    return 0;
}

int practices_initialized(void) {
    struct stat st = {0};
    char practices_dir[256];
    get_full_practices_dir(practices_dir, sizeof(practices_dir));
    return (stat(practices_dir, &st) != -1);
}

int initialize_practices(void) {
    char practices_dir[256];
    get_full_practices_dir(practices_dir, sizeof(practices_dir));

    if (mkdir(practices_dir, 0755) != 0 && errno != EEXIST) {
        return 1;
    }

    if (download_practices() != 0) {
        return 1;
    }
    if (extract_practices() != 0) {
        return 1;
    }
    remove(SADHANA_FILE);
    return 0;
}

int update_practices(void) {
    return initialize_practices();
}

int get_practices(char practices[][MAX_PRACTICE_NAME], int max_practices) {
    char practices_dir[256];
    get_full_practices_dir(practices_dir, sizeof(practices_dir));

    int count = 0;
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    for (int i = 0; i < NUM_CATEGORIES && count < max_practices; i++) {
        char category_path[512];
        snprintf(category_path, sizeof(category_path), "%s/%s", practices_dir, CATEGORY_DIRS[i]);

        dir = opendir(category_path);
        if (dir == NULL) {
            continue;
        }

        while ((entry = readdir(dir)) != NULL && count < max_practices) {
            char full_path[768];
            snprintf(full_path, sizeof(full_path), "%s/%s", category_path, entry->d_name);

            if (stat(full_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
                char *ext = strrchr(entry->d_name, '.');
                if (ext && strcmp(ext, ".yaml") == 0) {
                    snprintf(practices[count], MAX_PRACTICE_NAME, "%s/%s", CATEGORY_DIRS[i], entry->d_name);
                    char *yaml_ext = strrchr(practices[count], '.');
                    if (yaml_ext) *yaml_ext = '\0'; // Remove .yaml extension
                    count++;
                }
            }
        }
        closedir(dir);
    }

    return count;
}

int run_practice(const char *practice_name) {
    printf("Running practice: %s\n", practice_name);
    return 0;
}

int configure_practice(const char *practice_name) {
    printf("Configuring practice: %s\n", practice_name);
    return 0;
}
