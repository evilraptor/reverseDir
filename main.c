#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//TODO добавь копирование прав а не по дефолту фул допуск всем на все (вроде готово)
//TODO добавь чтобы редачились и файлы в поддиректориях

void create_dir(char *target_dir_name, char *original_dir_name) {
    mkdir(target_dir_name, 0700);
    //printf("%s\n",target_dir_name);

    struct stat st;
    if (stat(original_dir_name, &st) == -1) {
        printf("cant get permissions, error!");
        return;
    }
    if (chmod(target_dir_name, st.st_mode) == -1) {
        printf("cant set permissions, error!");
        return;
    }
}

void create_reverse_file(char *target_file_name, char *original_file_name) {
    //printf("target_file_name - %s\n", target_file_name);
    int fd = open(target_file_name, O_RDWR | O_CREAT, 0777);
    if (fd == -1) {
        printf("cant create file, error!");
        return;
    }
    close(fd);

    struct stat st;
    if (stat(original_file_name, &st) == -1) {
        printf("cant get permissions, error!");
        return;
    }
    if (chmod(target_file_name, st.st_mode) == -1) {
        printf("cant set permissions, error!");
        return;
    }

    FILE *f1 = fopen(original_file_name, "rb");
    FILE *f2 = fopen(target_file_name, "wb");

    if (f1 == NULL) {
        printf("permission of this file -%s is denied\n", original_file_name);
        return;
    }

    char input;
    fseek(f1, -1, SEEK_END);

    while (ftell(f1) > 0) {
        fread(&input, 1, 1, f1);
        fwrite(&input, 1, 1, f2);
        fseek(f1, -2, SEEK_CUR);
    }
    fread(&input, 1, 1, f1);
    fwrite(&input, 1, 1, f2);

    fclose(f1);
    fclose(f2);

    return;
}

void create_reverse_dir(char *target_dir_name, char *original_dir_name) {

    mkdir(target_dir_name, 0700);
    //FIXME тут права (хотя ладно наверное это же уже не файлы а каталожек который я создаю т.е. конечный результат кода а его можно и показать миру!)

    struct stat st;
    if (stat(original_dir_name, &st) == -1) {
        printf("cant get permissions, error!");
        return;
    }
    if (chmod(target_dir_name, st.st_mode) == -1) {
        printf("cant set permissions, error!");
        return;
    }

    return;
}

char *get_full_name_target_file(char *first_part, char *second_part) {
    char *result = malloc(strlen(first_part) + strlen(second_part) + 1);

    int second_part_len = strlen(second_part);

    for (int i = 0; i < second_part_len / 2; i++) {
        char tmp = second_part[i];
        second_part[i] = second_part[second_part_len - 1 - i];
        second_part[second_part_len - 1 - i] = tmp;
    }

    strcpy(result, first_part);
    strcat(result, second_part);

    return result;
}

char *get_full_name_original_file(char *first_part, char *second_part) {
    char *result = malloc(strlen(first_part) + strlen(second_part) + 1);

    strcpy(result, first_part);
    strcat(result, second_part);

    return result;
}

char *get_first_part(char *first_part) {
    int len_of_first_part = strlen(first_part);

    int len_of_rest = 0;

    char *result = (char *) calloc(len_of_first_part + 1, sizeof(char));

    for (int i = len_of_first_part - 2; i >= 0; i--) {
        if (first_part[i] == '/') {
            break;
        }
        len_of_rest++;// /proc/dir1/
        // /proc/1rid/
    }

    for (int i = 0; i < len_of_first_part; i++) {
        result[i] = first_part[i];
    }
    result[len_of_first_part] = '\0';


    for (int i = 0; i < len_of_rest; i++) {
        result[len_of_first_part - 1 - len_of_rest + i] = first_part[len_of_first_part - 2 - i];
    }

    return result;
}

char *get_original_dir_name(char *dir_name) {
    int len = strlen(dir_name);
    if (dir_name[len - 1] == '/') {
        return dir_name;
    } else {
        char *result = (char *) calloc(len + 2, sizeof(char));
        for (int i = 0; i < len; ++i) {
            result[i] = dir_name[i];
        }
        result[len] = '/';
        result[len + 1] = '\0';
        return result;
    }
}

void reverse_dir(/*char *target_dir_name, */char *first_part, char *original_dir_name) {
    //printf("%s\n", input_original_dir_name);

    /*char *original_dir_name = get_original_dir_name(input_original_dir_name);

    char *first_part = get_first_part(original_dir_name);
create_reverse_dir(first_part, original_dir_name);*/
    char *target_file_name;
    char *original_file_name;
    //printf("%s\n",first_part);
    //create_reverse_dir(first_part, original_dir_name);


    DIR *d;

    struct dirent *dir;
    d = opendir(original_dir_name);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            short dir_flag = 0;

            original_file_name = get_full_name_original_file(original_dir_name, dir->d_name);
            target_file_name = get_full_name_target_file(first_part, dir->d_name);
            if (dir->d_name[0] != 46)
                dir_flag = 1;
            if (dir_flag == 1) {
                if (dir->d_type == DT_REG) {
                    //ТУТ ИДЕТ ПРО ПАПКИ


                    //printf("%s      %s\n",target_file_name,original_file_name);
                    create_reverse_file(target_file_name, original_file_name);
                } else if (dir->d_type == DT_DIR) {
                    //ТУТ ИДЕТ ПРО ПОДПАПКИ



                    char *tmp_dir = (char *) calloc(strlen(first_part) + strlen(dir->d_name) + 2, sizeof(char));

                    for (int i = 0; i < strlen(first_part) - 1; i++) {
                        tmp_dir[i] = first_part[i];
                    }
                    strcat(tmp_dir, "/");
                    strcat(tmp_dir, dir->d_name);
                    strcat(tmp_dir, "/");

                    create_dir(tmp_dir, original_file_name);
                    reverse_dir(tmp_dir, /*first_part, */strcat(original_file_name, "/"));
                }
            }
        }
        closedir(d);
    }
    printf("done.");
    //free(target_file_name);
    //free(original_file_name);
    //free(first_part);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("you have to write directory name, error!");
        return 0;
    }
    //char *original_dir_name = get_original_dir_name(argv[1]);

    char *original_dir_name = get_original_dir_name(argv[1]);

    char *first_part = get_first_part(original_dir_name);


    //printf("%s\n",first_part);
    create_reverse_dir(first_part, original_dir_name);

    reverse_dir(/*first_part, */first_part, original_dir_name);

    //free(target_file_name);
    //free(original_file_name);
    free(first_part);

    return 0;
}
