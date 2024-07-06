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
#include "practice.h"

#define SADHANA_HUB_URL "https://github.com/naoxio/sadhana/archive/refs/heads/main.zip"
#define SADHANA_FILE "sadhana-main.zip"
#define PRACTICES_DIR "~/.local/share/sadhana/practices"

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
                printf("Debug: Creating directory: %s\n", output_path);
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
                    fprintf(stderr, "Debug: Failed to extract file: %s\n", output_path);
                    mz_zip_reader_end(&zip_archive);
                    return 1;
                }
                printf("Debug: File extracted successfully: %s\n", output_path);
            }
        }
    }

    mz_zip_reader_end(&zip_archive);
    printf("Debug: Extraction completed successfully\n");
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

    printf("Debug: Creating practices directory: %s\n", practices_dir);
    if (mkdir(practices_dir, 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "Debug: Failed to create practices directory: %s\n", strerror(errno));
        return 1;
    }

    if (download_practices() != 0) {
        fprintf(stderr, "Failed to download practices\n");
        return 1;
    }
    if (extract_practices() != 0) {
        fprintf(stderr, "Failed to extract practices\n");
        return 1;
    }
    remove(SADHANA_FILE);
    return 0;
}

int update_practices(void) {
    return initialize_practices();  // For simplicity, we're just re-initializing
}

int get_practices(char practices[MAX_PRACTICES][MAX_PRACTICE_NAME], int max_practices) {
    char practices_dir[256];
    get_full_practices_dir(practices_dir, sizeof(practices_dir));

    DIR *dir;
    struct dirent *ent;
    int count = 0;

    dir = opendir(practices_dir);
    if (dir == NULL) {
        fprintf(stderr, "Error: Unable to open practices directory\n");
        return -1;
    }

    while ((ent = readdir(dir)) != NULL && count < max_practices) {
        char full_path[512];
        struct stat st;

        snprintf(full_path, sizeof(full_path), "%s/%s", practices_dir, ent->d_name);

        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            char *ext = strrchr(ent->d_name, '.');
            if (ext && strcmp(ext, ".yaml") == 0) {
                strncpy(practices[count], ent->d_name, MAX_PRACTICE_NAME - 1);
                practices[count][MAX_PRACTICE_NAME - 1] = '\0'; // Ensure null-termination
                practices[count][strlen(practices[count]) - 5] = '\0'; // Remove .yaml extension
                count++;
            }
        }
    }

    closedir(dir);
    return count;
}

int list_practices(void) {
    char practices[MAX_PRACTICES][MAX_PRACTICE_NAME];
    int count = get_practices(practices, MAX_PRACTICES);

    if (count < 0) {
        fprintf(stderr, "Error: Unable to get practices\n");
        return 1;
    }

    printf("Available practices:\n");
    for (int i = 0; i < count; i++) {
        printf("- %s\n", practices[i]);
    }

    return 0;
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
